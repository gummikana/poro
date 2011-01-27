/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ***************************************************************************/


//
// +----------------------------------------------------------------------+
// | CDirList 0.8                                                         |
// +----------------------------------------------------------------------+
// | You are free to use this as you wish.								  |
// +----------------------------------------------------------------------+
// | Based on code by Daniel Dean < ddd196 @ hotmail.com >                |
// |                                                                      |
// | New version edited by Petri Purho < pete @ markkupurho.fi >          |
// +----------------------------------------------------------------------+
//
//

#include "cdirlist.h"
#include "../string/string.h"
#include <vector>



#if defined (__MSDOS__)
#include <stdlib.h>
#include <dir.h>
#include <dos.h>
#endif

#if defined (unix)
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#if defined (_MSC_VER)
#include <io.h>
#endif

namespace ceng {

using namespace std;


CDirList::CDirList() { Init(); }
CDirList::CDirList( string directory ) { ReadDir( directory ); }
CDirList::~CDirList() { ClearData(); }

void CDirList::ReadDirsAndFiles( string directory, bool files ) {


	string pattern;

	pattern	  = GetPattern( directory );
	directory = ParsePattern( directory );


	string return_value = "";

	// This part here isn't my code
	// It is coded by Danien Dean < ddd196 @ hotmail.com >
	// http://www.dma.org/~deand/potm.html
	// This is a bit edited by me, but most the stuff is / the core
	// of it is coded by Danien.
	/////////////////////////////////////////////////////////////////////////////////////////////
	// MS_DOS
	#if defined (__MSDOS__)                  /* BORLAND C DOS */

	int i = 0, done;
	struct ffblk dta;

	done = findfirst(directory.c_str(), &dta, FA_DIREC);

	while (!done){
		if ( files ) {
			if ( !((dta.ff_attrib & FA_DIREC) == FA_DIREC) && (dta.ff_name[0] != '.')) {
				i++;
				return_value = dta.ff_name;
				if ( return_value != "" && return_value != ".." ) PushToList( return_value, pattern );
				// myData.push_back( dta.ff_name );

			}
		} else {
			if (((dta.ff_attrib & FA_DIREC) == FA_DIREC) && (dta.ff_name[0] != '.')) {
				i++;
				return_value = dta.ff_name;
				if ( return_value != "" && return_value != ".." ) PushToList( return_value, pattern, true );
				// myData.push_back( dta.ff_name );

			}
		}

		done = findnext(&dta);
    }


	#endif
	/////////////////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////////////////
	// UNIX
	#if defined (unix)                       /* UNIX */

	DIR *dirp;
    struct dirent *dp;
    struct stat buf;

    dirp = opendir( directory.c_str() );
    while (dp = readdir(dirp)) {
        if (stat(dp->d_name,&buf) == 0)
			if ( files ) {
				if( !(buf.st_mode & S_IFDIR))
					return_value = dp->d_name;
					if ( return_value != "" && return_value != ".." ) PushToList( return_value, pattern );

			} else {
				if(buf.st_mode & S_IFDIR)
					return_value = dp->d_name;
					if ( return_value != "" && return_value != ".." ) PushToList( return_value, pattern, true );

			}

              // myData.push_bakc( dp->d_name );
    }

	#endif
	/////////////////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////////////////
	// MS Visual C++
	#if defined (_MSC_VER)                   /* MS Visual C++ */

	struct _finddata_t c_file;
	long hFile;


	/* Find first .c file in current directory */
	if((hFile = _findfirst(directory.c_str(), &c_file)) == -1L) {



	} else {

		/* Find the rest of the *.* files */
		while(_findnext(hFile, &c_file) == 0) {
			if ( files ) {

				if(!(c_file.attrib & _A_SUBDIR )) {
					return_value = c_file.name;
					if ( return_value != "" && return_value != ".." ) PushToList( return_value, pattern );
					// myData.push_back( c_file.name );

				}
			} else {

				if((c_file.attrib & _A_SUBDIR )) {
					return_value = c_file.name;
					if ( return_value != "" && return_value != ".." ) PushToList( return_value, pattern, true );
					// myData.push_back( c_file.name );

				}
			}

		}

		_findclose(hFile);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////

	#endif
	// Thats it thats was the brialiant code of Danien Dean, which the core of this class
	//


	// if ( return_value != "" || return_value != ".." ) myData.push_back( return_value );

	Init();
}

void CDirList::ReadDir( std::string directory )	  {	ClearData(); ReadDirsAndFiles( directory, false ); }
void CDirList::ReadFiles( std::string directory ) {	ClearData(); ReadDirsAndFiles( directory, true  ); }
void CDirList::Read( std::string directory )	  { ClearData(); ReadDirsAndFiles( directory, false );
													ReadDirsAndFiles( directory, true  ); }

void CDirList::Init() { myBrowser = myData.begin(); }
void CDirList::ClearData() { while ( !myData.empty() ) myData.pop_front(); }

void CDirList::PushToList( std::string filename, std::string pattern, bool isdir )
{
	if ( isdir )
	{
		if ( IsValidFile( filename + ".", pattern ) )
			myData.push_back( filename );
	} else {
		if ( IsValidFile( filename, pattern ) )
			myData.push_back( filename );
	}


}

string CDirList::Top() {

	string return_value;

	myBrowser = myData.begin();

	if ( myBrowser != myData.end() )
		return_value = ( *myBrowser );
	else
		return_value  = "";

	++myBrowser;

	return return_value;

}

string CDirList::Next() {

	string return_value;

	if ( myBrowser != myData.end() ) return_value = ( *myBrowser ); else return_value = "";

	++myBrowser;

	return return_value;

}



bool CDirList::End() {

	if ( myBrowser == myData.end() ) return true; else return false;
}


bool CDirList::IsValidFile( const string& filename, const string& pattern )
{
	vector<string> Array;

	Array = Split( "*", pattern );



	//if ( Array.empty() )
	//	return true;





	unsigned int i = 0;


	size_t prev_pos = 0;
	size_t current_pos = 0;


	while ( i < Array.size() )
	{




		current_pos = filename.find( Array[ i ], prev_pos );

		// if it is the first
		if ( i == 0 &&
			( char )pattern[ 0 ] != '*' &&
			current_pos != 0 )

			return false;


		// if it is the last
		if ( i == Array.size() - 1 &&
			( char )pattern[ pattern.size() - 1 ] != '*' &&
			current_pos != ( filename.size() - Array[ i ].size() ) )
			return false;

		if ( current_pos == filename.npos )
		{
			//cout << "Could not find " << Array[ i ] << " in string " << filename << endl;
			return false;
		}
			//cout << "Found " << Array[ i ] << " in string " << filename << endl;

		prev_pos = current_pos + Array[ i ].size();

		i++;

	}

	return true;
}

std::string CDirList::ParsePattern( std::string directory )
{
	size_t position;

	directory = ConvertDosPath( directory );

	// remove the whitespace
	position = directory.find_first_not_of(" \t");
	directory.erase( 0,  position );

	position = directory.find_last_not_of(" \t");
	directory.erase( position+1 );


	position = directory.find_last_of("/");

	if ( position == directory.npos )
		return "./*.*";

	directory.erase( position );

	/*if (  ( directory.find(".") == directory.npos ) ||
	   ( directory.find_last_of(".") <= directory.find_last_of("/") ) )
	{*/
		if ( (char)directory[ directory.length() - 1 ] == '/')
			directory += "*.*";
	    else
			directory += "/*.*";
     //}

	 return directory;
}


std::string CDirList::GetPattern( std::string directory )
{

	size_t position;

	directory = ConvertDosPath( directory );

	// remove the whitespace
	position = directory.find_first_not_of(" \t");
	directory.erase( 0,  position );

	position = directory.find_last_not_of(" \t");
	directory.erase( position+1 );

	position = directory.find_last_of("/");

	if ( position == directory.npos ) return directory;

	string return_value;

	return_value = directory.substr( position + 1 );

	if ( return_value.find(".") == return_value.npos ) return "*.*";
	else return return_value;


}

std::string CDirList::ConvertDosPath( std::string directory )
{

	if ( directory == "." ) directory = "./*.*";


	while ( directory.find("\\") != directory.npos) {
                directory.replace( directory.find("\\"), 1, "/");
        }

	return directory;
}

} //end o namespace
