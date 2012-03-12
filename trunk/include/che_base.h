#ifndef _CHE_BASE_H_
#define _CHE_BASE_H_

#include "che_define.h"

#include <new>
#include <cstdlib>
#include <malloc.h>
#include <windows.h>
#undef GetObject

class CHE_Allocator
{
public:
	virtual ~CHE_Allocator(){}
	virtual void* Alloc( size_t cb ) = 0;
    virtual void Free( void* data ) = 0;
	virtual size_t GetSize( void* data ) = 0;
	
	template <class Type>
	inline Type* New()
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type;
	}

	template <class Type>
	inline Type* NewArray( size_t count )
	{
		void* obj = Alloc( sizeof(Type) * count );
		Type * pT = (Type *)obj;
		for ( size_t i = 0; i < count; i++ )
		{
			new((void *)pT) Type;
			pT++;
		}
		return (Type *)obj;
	}

	template <class Type>
	inline void Delete( Type * p )
	{
		((Type*)p)->~Type();
		Free( p );
	}

	template <class Type>
	inline void DeleteArray( Type * p )
	{
		size_t count = GetSize( p ) / sizeof( Type ) ;
		Type * pTmp = p;
		for ( size_t i = 0; i < count; i++ )
		{
			((Type*)pTmp)->~Type();
			pTmp++;
		}
		Free( p ); 
	}

	template <class Type, class Arg1>
	inline Type* New( Arg1 arg1 )
	{
		void* obj =Alloc( sizeof(Type) );
		return new(obj) Type(arg1);
	}

	template <class Type, class Arg1, class Arg2>
	inline Type* New( Arg1 arg1, Arg2 arg2 )
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2);
	}

	template <class Type, class Arg1, class Arg2, class Arg3>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3 )
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4 )
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
	inline Type* New(  Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5 )
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6 )
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7 )
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8 )
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9 )
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9, Arg10 arg10 )
	{
		void* obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	}
};

class CHE_DefCrtAllocator : public CHE_Allocator
{
public:
	inline void* Alloc( size_t cb );

    inline void Free( void* data );

	inline size_t GetSize( void * data );
};

CHE_Allocator * GetDefaultAllocator();

class CHE_HeapAllocator : public CHE_Allocator
{
public:
	CHE_HeapAllocator( size_t initSize = 1 * 1024 * 1024 );
	
	~CHE_HeapAllocator();
		
	inline void* Alloc( size_t cb );
	
	inline void Free( void* data );

	inline size_t GetSize( void * data );
private:
	HANDLE m_Heap;
};

class CHE_Object
{
public:
	CHE_Object( CHE_Allocator * pAllocator );

	CHE_Allocator * GetAllocator() const { return m_pAllocator; }

private:
	CHE_Allocator * m_pAllocator;
};
 
class IHE_Pause
{
public:
	virtual HE_BOOL	NeedToPauseNow() = 0;
};

class IHE_Write : public CHE_Object
{
public:
	IHE_Write( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {};

	virtual ~IHE_Write() {};

	virtual HE_DWORD	GetSize() = 0;

	virtual HE_DWORD	GetCurOffset() = 0;

	virtual HE_DWORD	Flush() = 0;

	virtual	HE_BOOL		WriteBlock( const HE_LPVOID pData, HE_DWORD offset, HE_DWORD size ) = 0;
	
	virtual	HE_BOOL		WriteBlock( const HE_LPVOID pData, HE_DWORD size )
	{
		return WriteBlock( pData, GetSize(), size );
	}

	virtual HE_BOOL		WriteByte( HE_BYTE byte )
	{
		return WriteBlock( &byte, 1 );
	}

	virtual HE_VOID		Release() = 0;
};

#define FILEWRITE_MODE_NEW			0
#define FILEWRITE_MODE_OPEN			1

IHE_Write*	HE_CreateFileWrite( HE_LPCSTR filename, HE_BYTE mode = FILEWRITE_MODE_NEW, CHE_Allocator * pAllocator = NULL );

HE_VOID		HE_DestoryIHEWrite( IHE_Write * pIHEWrite );

class IHE_Read : public CHE_Object
{
public:
	IHE_Read( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {}

	virtual ~IHE_Read() {};

	virtual HE_DWORD	GetSize() = 0;
	
	virtual HE_DWORD	ReadBlock( HE_LPVOID buffer, HE_DWORD offset, HE_DWORD size ) = 0;

	virtual HE_BYTE		ReadByte( HE_DWORD offset ) = 0;
	
	virtual HE_VOID		Release() = 0;
};

#define FILEREAD_MODE_DEFAULT		0
#define FILEREAD_MODE_MEMCOPY		1
#define FILEREAD_MODE_BUFFER		2
#define FILEREAD_MODE_BLOCKLINK		3

IHE_Read*	HE_CreateFileRead( HE_LPCSTR filename, HE_BYTE mode = FILEREAD_MODE_DEFAULT, HE_DWORD param = 0, CHE_Allocator * pAllocator = NULL );

IHE_Read*	HE_CreateMemBufRead( HE_LPCBYTE pBuf, HE_DWORD lSize, CHE_Allocator * pAllocator = NULL );

HE_VOID		HE_DestoryIHERead( IHE_Read * pIHERead );

#endif
