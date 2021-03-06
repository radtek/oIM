﻿/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SSingleton.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI系统中使用的单件模块
*/

#pragma once

#include <assert.h>

namespace SOUI
{
#define SINGLETON_GET		(0)	// 获取
#define SINGLETON_INIT		(1)	// 初始化
#define SINGLETON_RESET		(2)	// 复位
    /**
    * @class      SSingleton
    * @brief      单件模板
    * 
    * Describe    
    */
    template <typename T> 
    class SSingleton
    {
	public:
		static T* GetObject(int nType = SINGLETON_GET, T* pThis=NULL)
		{
			static T* s_Singleton = NULL;
			if ( nType == SINGLETON_GET )
			{
			//	assert( s_Singleton );
				return s_Singleton;
			}
			else if ( nType == SINGLETON_INIT )
			{
				assert( !s_Singleton );
				s_Singleton = pThis;
			}
			else if ( nType == SINGLETON_RESET )
			{
				assert( s_Singleton );
				s_Singleton = NULL;
			}
			else 
				assert(0);

			return NULL;
		}

        SSingleton( void )
        {
           SSingleton::GetObject(SINGLETON_INIT, static_cast<T*>(this));
        }
        virtual ~SSingleton( void )
        {
           SSingleton::GetObject(SINGLETON_RESET);
        }
        static T& getSingleton( void )
        {
			return *GetObject();
        }
        static T* getSingletonPtr( void )
        {
            return GetObject();
        }

    private:
        SSingleton& operator=(const SSingleton&)
        {
            return *this;
        }
        SSingleton(const SSingleton&) {}
    };

}//namespace SOUI