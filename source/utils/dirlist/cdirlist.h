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

#ifndef INC_CDIRLIST_H
#define INC_CDIRLIST_H


#pragma warning(disable:4786) // Poistaa ilkeet warningit mit‰ stl antaa jos k‰ytt‰‰ stringi‰

#include <string>
#include <list>

namespace ceng {
// This is a class that will read files and directories from sertain path
// 
class CDirList {

public:

	CDirList();
	CDirList( std::string directory );
	~CDirList();

	// This function will files and dirs. Input is basicly just a path.
	// like c:/windows/*.dll this will read all files and dirs with .dll 
	// extension in c:/windows folder
	void			Read( std::string directory = "." );

	// This will only read directories
	void			ReadDir( std::string directory = "." );

	// And thiss will only read files
	void			ReadFiles( std::string directory = "." );

	// This clears the list
	void			Clear() { ClearData(); };

	// This sets the iteration to the top of the list
	std::string		Top();
	

	// Using these two funtions below ( next and end )
	// you can read all the files from CDirList
	// Works something like this: 
	/*
		CDirList dirlist(".");
		while ( dirlist.End() != false )
			cout << dirlist.Next() << endl;

	*/
	

	// This returns the next file. 
	std::string		Next();
	
	// And this tells if we are at the end yet.
	bool			End();

	
	// This returns the files a std::list < std::string > type
	// thingie
	std::list< std::string > GetTheList() { return myData; }



private:

	// This sets mybrowser to begining of mydata, called by constructors
	void			Init();

	// This clears the list
	void			ClearData();

	// This reads files and folders, it is used by 
	// Read, ReadFiles, ReadDirs functions.
	void			ReadDirsAndFiles( std::string directory, bool files );
	
	// This checks if the filename is valid, and pushes it then to the list
	void			PushToList( std::string filename, std::string pattern, bool isdir = false );
	
	// This tells PushToList if a certain filename is valid.
	// For example	IsValidFile( "CDirList.h", "*.*" )		returns true
	// But			IsValidFile( "CDirList.h", "*.cpp" )	returns false
	bool			IsValidFile( const std::string& filename, const std::string& pattern );

	// This returns a Pattern from user input, which is used by ReadDirsAndFiles
	// For example  GetPattern( "c:\\mp3\\*.mp3" ) would return *.mp3
	std::string		GetPattern( std::string directory );

	// This parses user pattern so it can be used by ReadDirsAndFiles
	std::string		ParsePattern( std::string directory );

	// This converts a dos path ( c:\games\ ) to unix path type ( c:/games/ )
	std::string		ConvertDosPath( std::string directory );
	

	// This is the list where the files are stored
	std::list< std::string >			myData;
	
	// This is the iterator which keeps track where Next() is going
	std::list< std::string >::iterator	myBrowser;
	/*
	std::list < std::string >			 myData;
	std::list < std::string >::iterator  myBrowser;
	*/

};

} //end o namespace ceng
#endif