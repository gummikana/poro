#include "cxmlhandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include "../../../../Source/misc_utils/simple_profiler.h"

namespace ceng { 
#if 0 
static char* ReadEntireFileIntoMemoryAndNullTerminate(const char *FileName)
{
    char *Result = 0;
    FILE *File = fopen(FileName, "r");
    if(File)
    {
		// TODO( Petri ): fseek doesn't give accurate filesize
        fseek(File, 0, SEEK_END);
        size_t FileSize = ftell(File);
        fseek(File, 0, SEEK_SET);

		size_t ResultSize = FileSize + 1;
        Result = (char *)malloc(ResultSize);
		// size_t len = fread(Result, 1, FileSize, File);
		fread(Result, FileSize, 1, File);

		/*
		char symbol = 0;
		size_t i = 0;
		while((symbol = getc(File)) != EOF && i < FileSize )
		{
			Result[ i++ ] = symbol;
		}*/
        Result[FileSize] = 0;
		if( false )
		{
			
			for( int i = FileSize; i > 0; --i )
			{
				if( Result[i] == '>' ) 
					break;
				else
					Result[i] = '\0';
			}
		}
        
        fclose(File);
    }

    return(Result);
}
#endif

static char* ReadWholeFile( const char* filename )
{
	using namespace poro;

	FileSystem* file_system = Poro()->GetFileSystem();
	bool delete_file_system = false;
	if ( file_system == NULL )
	{
		file_system = new FileSystem;
		delete_file_system = true;
	}
	
	char* result = NULL;
	size_t result_size = 0;
	file_system->ReadWholeFileAndNullTerminate( filename, result, &result_size );

	if ( delete_file_system )
		delete file_system;

	return result;
}

// --- 

struct XmlHandlerImpl
{
	struct String
	{
		String() : text( NULL ), length( 0 ) { }
		String( const char* t, size_t l ) : text( t ), length( l ) { }

		std::string AsString() 
		{
			std::string result;
			result.resize( length );
			for( size_t i = 0; i < length; ++i ) 
				result[i] = text[i];
			return result;
 		}
		const char* text;
		size_t length;
	};

	CXmlHandler* mHandler;
	String mName;
	std::vector< String > mContent;
	std::vector< std::pair< String, String > > mTags;

	//! Called when a document it started
	void StartDocument() 
	{ 
		mHandler->StartDocument();
	}

	//! Called when the document is ended
	void EndDocument() 
	{ 
		mHandler->EndDocument();
	}

	void Flush()
	{
		if( mName.text == 0 )
			return;

		// Flush...
		cassert( mHandler );
		CXmlHandler::attributes tags;
		for( size_t i = 0; i < mTags.size(); ++i )
		{
			tags.insert( std::make_pair( 
				mTags[i].first.AsString(), mTags[i].second.AsString()
				) );
		}

		const std::string str_name = mName.AsString();
		mHandler->StartElement( str_name, tags );

		if( mContent.empty() == false )
		{
			std::stringstream ss;
			for( std::size_t i = 0; i < mContent.size(); ++i )
			{
				ss << mContent[i].AsString();
			}
			mHandler->Characters( ss.str() );
		}

		mName.text = 0;
		mName.length = 0;
		mContent.clear();
		mTags.clear();
	}

	//! When we hit some characters this gets called,
	void AddContent( const char* content, int content_length ) 
	{ 
		mContent.push_back( String( content, content_length ) );
	}

	//! When a element gets startted this gets called
	void StartElement( const char* name, int name_length ) 
	{ 
		mName = String( name, name_length );
	}

	void AddTag( const char* tag, int tag_length, const char* value, int value_length ) 
	{ 
		mTags.push_back( std::make_pair( String( tag, tag_length ), String( value, value_length ) ) );
	}

	//! And when it ends we call him
	void EndElement( const char* name, int name_length ) 
	{ 
		Flush();
		String s( name, name_length );
		mHandler->EndElement( s.AsString() );
	}
	// virtual void EndElement( /*const std::string& name*/ ) { }

};


//--------------------------------

enum TokenType
{
	TOKEN_Unknown,

	TOKEN_OpenLess,
	TOKEN_CloseGreater,
	TOKEN_Slash,
	TOKEN_Equal,

	TOKEN_String,

	TOKEN_EndOfStream
};

struct Token
{
	char* text;
	TokenType type;	
	size_t length;
};

struct Tokenizer
{
	char* at;
	bool end_of_stream;
};

bool TextCompare( const Token& a, const Token& b )
{
	if( a.type != TOKEN_String || b.type != TOKEN_String )
		return false;
	
	if( a.length != b.length )
		return false;

	for( size_t i = 0; i < a.length; ++i )
	{
		if( a.text[i] != b.text[i] )
			return false;
	}

	return true;
}

bool StringMatch( Tokenizer* tokenizer, const char* match_me )
{
	for( size_t i = 0; match_me[i]; ++i )
	{
		if( tokenizer->at[i] == 0 ) 
			return false;
		
		if( tokenizer->at[i] != match_me[i] )
			return false;
	}
	return true;
}

bool IsWhitespace( char c )
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

bool NonStringCharacter( char c )
{
	return ( IsWhitespace( c ) || c == '<' || c == '>' || c == '=' || c == '/' );
}

static void SkipWhiteSpace( Tokenizer* tokenizer )
{
	while( tokenizer->at[0] )
	{
		if( IsWhitespace( tokenizer->at[0] ) )
		{
			++tokenizer->at;
		}
		else if( tokenizer->at[0] == '<' && tokenizer->at[1] == '!' )
		{
			tokenizer->at += 2;
			while( tokenizer->at[0] && tokenizer->at[0] != '>')
			{
				++tokenizer->at;
			}
			
			if( tokenizer->at[0] == '>')
				++tokenizer->at;
		}
		else if( StringMatch( tokenizer, "<?" ) )
		{
			tokenizer->at += 2;
			while( tokenizer->at[0] && StringMatch( tokenizer, "?>" ) == false )
			{
				++tokenizer->at;
			}

			if( StringMatch( tokenizer, "?>") )
				tokenizer->at += 2;
		}
		else if( StringMatch( tokenizer, "<!--" ) )
		{
			tokenizer->at += 4;
			while( tokenizer->at[0] && StringMatch( tokenizer, "-->" ) == false )
			{
				++tokenizer->at;
			}

			if( StringMatch( tokenizer, "-->") )
				tokenizer->at += 3;

		}
		else
		{
			break;
		}
	}
}

static Token GetToken( Tokenizer* tokenizer )
{

	SkipWhiteSpace( tokenizer );

	Token result = {};
	result.text = tokenizer->at;
	result.length = 1;

	if( tokenizer->end_of_stream )
	{
		result.type = TOKEN_EndOfStream;
		return result;
	}

	char c = tokenizer->at[0];
	++tokenizer->at;
	switch( c )
	{
	case '\0': { result.type = TOKEN_EndOfStream; tokenizer->end_of_stream = true; } break;
	case '<': { result.type = TOKEN_OpenLess; } break;
	case '>': { result.type = TOKEN_CloseGreater; } break;
	case '/': { result.type = TOKEN_Slash; } break;
	case '=': { result.type = TOKEN_Equal; } break;

	case '"': 
	{ 
		result.type = TOKEN_String;
		result.text = tokenizer->at;
		while( tokenizer->at[0] && tokenizer->at[0] != '"' ) {
			++tokenizer->at;
		}
		result.length = ( tokenizer->at - result.text );
		++tokenizer->at;
	}
	break;

	default:
		{
			// string, read till next white space
			result.type = TOKEN_String;
			while( tokenizer->at[0] && NonStringCharacter( tokenizer->at[0] ) == false )
			{
				++tokenizer->at;
			}
			result.length = ( tokenizer->at - result.text );
			
			// result.type = TOKEN_Unknown;
		}
		break;
	}
	return result;
}

void ParseTag( Tokenizer* tokenizer, XmlHandlerImpl* handler, Token tag_name )
{
	// should we reset the position? 
	Token t = GetToken( tokenizer );
	if( t.type == TOKEN_Equal )
	{
		Token tag_value = GetToken( tokenizer);
		if( tag_value.type == TOKEN_String )
		{
			if( handler )
				handler->AddTag( tag_name.text, tag_name.length, tag_value.text, tag_value.length );
		}
		else
		{
			std::cout << "Error at - expected string" << std::endl;
			return;
		}
	}
	else
	{
		std::cout << "Error at - expected = " << std::endl;
		return;
	}
}

void ParseElement( Tokenizer* tokenizer, XmlHandlerImpl* handler )
{
	// name of this element 
	Token name = GetToken( tokenizer );
	if( name.type != TOKEN_String )
	{
		// error
		std::cout << "Error at: " << std::endl;
		return;
	}

	if( handler )
	{
		handler->Flush();
		handler->StartElement( name.text, name.length );
	}

	// read till the end of the element
	bool parsing_element_start = true;
	bool element_ended = false;
	while(parsing_element_start)
	{
		Token t = GetToken( tokenizer );
		switch(t.type)
		{
			case TOKEN_EndOfStream: { return; } break;
			case TOKEN_Slash:
			{
				if( tokenizer->at[0] == '>')
				{
					element_ended = true;
					parsing_element_start = false;
				}
			}
			break;
			case TOKEN_CloseGreater:
			{
				parsing_element_start = false;
			}
			break;
			case TOKEN_String:
			{
				ParseTag( tokenizer, handler, t );
			}
			break;
		}
	}

	if( element_ended )
	{
		if( handler )
			handler->EndElement( name.text, name.length );
		return;
	}

	// read contents
	bool parsing_contents = true;
	while( parsing_contents )
	{
		Token t = GetToken( tokenizer );
		switch( t.type )
		{
			case TOKEN_EndOfStream:
			{ 
				parsing_contents = false; 
				return; 
			}
			break;

			case TOKEN_OpenLess:
			{
				if( tokenizer->at[0] == '/' )
				{
					++tokenizer->at;

					// check if the rest is correct
					Token end_name = GetToken( tokenizer );
					if( end_name.type == TOKEN_String && TextCompare( name, end_name ) )
					{
						Token close_greater = GetToken( tokenizer );
						if( close_greater.type == TOKEN_CloseGreater )
						{
							if( handler )
								handler->EndElement( name.text, name.length );

						}
						else if( close_greater.type != TOKEN_CloseGreater )
						{
							std::cout << "Error at - expected >" << std::endl;
						}
					}
					else
					{
						std::cout << "Error at - expected an ending element " << std::endl;
					}

					// and we're done with parsing element
					return;
				}
				else /*if( c == '<' )*/
				{
					// new element
					ParseElement( tokenizer, handler );
				}
			}
			break;

			default:
			{
				if( handler ) 
					handler->AddContent( t.text, t.length );
			}
			break;
 		}
	}
}



void CFasterParserHandler::ParseFile( const char* filename )
{
	using namespace poro;
	SPROFILE( "XML::ParseFile" );

	if( filename == NULL ) return;

	char* contents = ReadWholeFile( filename );
	if( contents == NULL ) 
		return;

	XmlHandlerImpl handler;
	handler.mHandler = &mHandler;

	handler.StartDocument();

	Tokenizer tokenizer = {};
	tokenizer.at = contents;

	Token t = GetToken( &tokenizer );
	if( t.type == TOKEN_OpenLess )
	{
		ParseElement( &tokenizer, &handler );
	}
	else
	{
		std::cout << "Error at - expected < to start at least 1 element in this document" << std::endl;
	}

	handler.EndDocument();
	free( contents );
}

} // end of namespace ceng