/***************************************************************************
 *
 * Copyright (c) 2010 - 2011 Petri Purho, Dennis Belfrage
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


/*
 *  rest_interface.h
 *  cpt-farbs
 *
 *  Created by Dennis Belfrage on 13.10.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_REST_INTERFACE_H
#define INC_REST_INTERFACE_H 

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>

#include "TCPInterface.h"
#include "extended_http_connection.h"
#include "utils/xml/cxml.h"


namespace ceng {
	class CXmlFileSys;
}



class IRestResource
{
public:
	IRestResource() : mId(0)
	{}
	
	virtual void SetId(int id){ mId=id; }
	virtual int GetId(){ return mId; }
	
	//Should return the path to the resource ie. for the users table this should return "/cpt_data/users"
	virtual std::string GetResourceTypePath()=0;
	
	//Returns the actuall url used in the server call, appending the resourse unique id and mime type when needed ie. "/cpt_data/users/123.xml"
	virtual std::string GetUrl();
	
	virtual void Serialize( ceng::CXmlFileSys* filesys ){};
protected:
	int mId;

};

/**
 *	RestResourceSelection is an interface for fetching many resources from the database based on a criteria
 *  It can also be used for checking if a resource exists or counting resources.
 */
class RestInterface;

template <class T>
class RestResourceSelection : public IRestResource
{
public:
	RestResourceSelection() : IRestResource()
	{}
	
	//RestResource Interface
	virtual void SetId(int id){}
	virtual int GetId(){return 0;}
	virtual std::string GetResourceTypePath();
	virtual std::string GetUrl();
	virtual void Serialize( ceng::CXmlFileSys* filesys );
	
	/**
	 *	Select criteria functions.
	 */
	void SetCriteria(std::map<std::string,std::string> criteria);
	std::map<std::string,std::string> GetCriteria();
	void AddCriteria(std::string field, std::string criteria);
	void AddCriteria(std::string field, int criteria);
	void ClearCriteria();
	
	/**
	 *	Assert that the result table contains only one resource id and returns it.
	 *  If there are no match or more than one match this function returns 0.
	 */
	int GetSingleResourceId();
	
	/**
	 *	Returns the resources by id. Searches and asserts that resources with id exists.
	 */
	const T& GetResource(int id);
	
	/**
	 *	Returns the resources map.
	 */
	const std::map<int,T>& GetResources();
	
private:
	std::map<std::string,std::string> mCriteria;
	std::map<int,T> mResources;
	
};


/**
 *	RestInterface is an interface for fetching many resources from the database based on a criteria
 *  It can also be used for checking if a resource exists or counting resources.
 */
class RestInterface
{
public:
	RestInterface();
	~RestInterface();
	
	void SetAuthentication(std::string user, std::string password);
	void RemoveAuthentication();
	bool ReadResult();
	
	//template <typename T>
	//RestResourceSelector<T>* GetResourceSelector(T const &d);
	//RestResourceSelector* GetResourceSelector();
	
	bool Get(IRestResource& resource);
	bool Post(IRestResource& resource);
	bool Put(IRestResource& resource);
	bool Delete(IRestResource& resource);
	
private:
	RakNet::TCPInterface *tcp;
	RakNet::ExtendedHTTPConnection *httpConnection;
	RakNet::RakString httpResult;
	std::string authHeader;
};




//-----------------------------------------------------
//   RestResourceSelector
//-----------------------------------------------------

template <class T>
inline std::string RestResourceSelection<T>::GetResourceTypePath(){
	T tmp;
	return tmp.GetResourceTypePath();
}
	
template <class T>
inline std::string RestResourceSelection<T>::GetUrl(){
	std::string url = GetResourceTypePath();
	std::ostringstream buffer;
	buffer << url << ".xml?";
	if(!mCriteria.empty()){
		for( std::map<std::string, std::string>::iterator ii=mCriteria.begin(); ii!=mCriteria.end(); ++ii)
		{
			RakNet::RakString value = (*ii).second.c_str();
			value.URLEncode();
			buffer << (*ii).first << "=" << value << "&";
		}
	}
	url = buffer.str();
	url = url.substr(0,url.length()-1);
	
	return url;
}

template <class T>
inline void RestResourceSelection<T>::Serialize( ceng::CXmlFileSys* filesys ){
	
	//Selectors can only GET data
	if(filesys->IsReading()){
		mResources.clear();
		
		int i = 0;
		for( i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
		{
			if( filesys->GetNode()->GetChild(i)->GetName() == "row" ) 
			{
				T resource;
				XmlConvertTo( filesys->GetNode()->GetChild(i), resource );
				assert(resource.GetId());
				mResources[resource.GetId()] = resource;
			}
		}
	}
	
}

template <class T>
inline void RestResourceSelection<T>::AddCriteria(std::string field, std::string criteria)
{
	mCriteria[field] = criteria;
}

template <class T>
inline void RestResourceSelection<T>::AddCriteria(std::string field, int criteria)
{
	std::ostringstream buffer;
	buffer << criteria;
	mCriteria[field] = buffer.str();
}

template <class T>
inline void RestResourceSelection<T>::ClearCriteria()
{
	mCriteria.clear();
}

template <class T>
inline int RestResourceSelection<T>::GetSingleResourceId()
{
	if(mResources.empty() || mResources.size()>1){
		return 0;
	}
	return mResources.begin()->first;
}

template <class T>
inline const T& RestResourceSelection<T>::GetResource(int id)
{
	assert(mResources.find(id)!=mResources.end());
	return mResources.find(id)->second;
}

template <class T>
inline const std::map<int,T>& RestResourceSelection<T>::GetResources()
{
	return mResources;
}


#endif
