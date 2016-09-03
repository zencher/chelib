#ifndef _CHE_BASE_H_
#define _CHE_BASE_H_

#ifdef _MAC_OS_X_
#include <malloc/malloc.h>
#include <pthread.h>
#else
#include <malloc.h>
#endif

#ifdef WIN32
#include <windows.h>
#undef GetObject
#endif

#include <new>

#include "che_define.h"


class CHE_Allocator
{
public:
	virtual ~CHE_Allocator(){}
	virtual void * Alloc( size_t cb ) = 0;
    virtual void Free( void * data ) = 0;
	virtual size_t GetSize( void * data ) = 0;
	
	template <class Type>
	inline Type * New()
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type;
	}

	template <class Type>
	inline Type* NewArray( size_t count )
	{
		void * obj = Alloc( sizeof(Type) * count );
        size_t relCount = GetSize( obj ) / sizeof( Type ) ;
		Type * pTmp = (Type *)obj;
		for ( size_t i = 0; i < relCount; i++ )
		{
			new((void *)pTmp) Type;
			pTmp++;
		}
		return (Type *)obj;
	}

	template <class Type>
	inline void Delete( Type * p )
	{
		((Type *)p)->~Type();
		Free( p );
	}

	template <class Type>
	inline void DeleteArray( Type * p )
	{
		size_t count = GetSize( p ) / sizeof( Type ) ;
		Type * pTmp = p;
		for ( size_t i = 0; i < count; i++ )
		{
			((Type *)pTmp)->~Type();
			pTmp++;
		}
		Free( p ); 
	}
    
#define CPP_VARIADIC_TEMPLATE
#ifdef  CPP_VARIADIC_TEMPLATE
    template <class Type, typename... Args>
    inline Type * New(Args... args )
    {
        void * obj = Alloc( sizeof(Type) );
        return new(obj) Type(args...);
    }
#else
	template <class Type, class Arg1>
	inline Type * New( Arg1 arg1 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1);
	}

	template <class Type, class Arg1, class Arg2>
	inline Type* New( Arg1 arg1, Arg2 arg2 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2);
	}

	template <class Type, class Arg1, class Arg2, class Arg3>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
	inline Type* New(  Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}

	template <class Type, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10>
	inline Type* New( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9, Arg10 arg10 )
	{
		void * obj = Alloc( sizeof(Type) );
		return new(obj) Type(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	}
#endif
};


class CHE_DefCrtAllocator : public CHE_Allocator
{
public:
	inline void * Alloc( size_t cb );

    inline void Free( void * data );

	inline size_t GetSize( void * data );
};


CHE_Allocator * GetDefaultAllocator();


#ifdef WIN32
class CHE_HeapAllocator : public CHE_Allocator
{
public:
	CHE_HeapAllocator( size_t initSize = 1 * 1024 * 1024 );
	
	~CHE_HeapAllocator();
		
	inline void * Alloc( size_t cb );
	
	inline void Free( void * data );

	inline size_t GetSize( void * data );
private:
	HANDLE mHeap;
};
#endif


class CHE_Object
{
public:
	CHE_Object( CHE_Allocator * pAllocator );

	CHE_Allocator * GetAllocator() const { return m_pAllocator; }

private:
	CHE_Allocator * m_pAllocator;
};


class IHE_Write : public CHE_Object
{
public:
	IHE_Write( CHE_Allocator * pAllocator = nullptr ) : CHE_Object( pAllocator ) {};

	virtual ~IHE_Write() {};

	virtual size_t GetSize() = 0;

	virtual size_t GetCurOffset() = 0;

	virtual size_t Flush() = 0;

	virtual	bool WriteBlock( const void * pData, size_t offset, size_t size ) = 0;
	
	virtual	bool WriteBlock( const void * pData, size_t size )
	{
		return WriteBlock( pData, GetSize(), size );
	}

	virtual bool WriteByte( BYTE byte )
	{
		return WriteBlock( &byte, 1 );
	}

	virtual void Release() = 0;
};


#define FILEWRITE_MODE_NEW  0
#define FILEWRITE_MODE_OPEN 1

IHE_Write* HE_CreateFileWrite( char const * filename, BYTE mode = FILEWRITE_MODE_NEW, CHE_Allocator * pAllocator = nullptr );

void HE_DestoryIHEWrite( IHE_Write * pIHEWrite );

class IHE_Read : public CHE_Object
{
public:
	IHE_Read( CHE_Allocator * pAllocator = nullptr ) : CHE_Object( pAllocator ) {}

	virtual ~IHE_Read() {};

	virtual size_t GetSize() = 0;
	
	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size ) = 0;

	virtual BYTE ReadByte( size_t offset ) = 0;
	
	virtual void Release() = 0;
};

#define FILEREAD_MODE_DEFAULT		0
#define FILEREAD_MODE_MEMCOPY		1
#define FILEREAD_MODE_BUFFER		2
#define FILEREAD_MODE_BLOCKLINK		3

IHE_Read * HE_CreateFileRead( char const * filename, BYTE mode = FILEREAD_MODE_DEFAULT, size_t param = 0, CHE_Allocator * pAllocator = nullptr );

IHE_Read * HE_CreateMemBufRead( PCBYTE pBuf, size_t lSize, CHE_Allocator * pAllocator = nullptr );

void HE_DestoryIHERead( IHE_Read * pIHERead );


class CHE_DynBuffer : public CHE_Object
{
public:
	CHE_DynBuffer( size_t capacity = 1024, size_t increament = 1024, CHE_Allocator * pAllocator = nullptr );
	CHE_DynBuffer( const CHE_DynBuffer & buf );
	~CHE_DynBuffer();

	const CHE_DynBuffer & operator = ( const CHE_DynBuffer & buf );

	PBYTE GetData() { return mpData; }

	size_t GetCapacity() { return mCapacity; }

	size_t GetSize() { return mSize; }

	size_t Write( PCBYTE pBuffer, size_t offset, size_t size );

	size_t Write( PCBYTE pBuffer, size_t size );

	size_t Read( PBYTE pBuffer, size_t size );

	bool ReadByte( size_t offset, PBYTE pByte );

	size_t Write( const CHE_DynBuffer & dynBuffer );

	void Clear() { mSize = 0; }

	void Alloc( size_t size );

private:
	size_t mCapacity;
	size_t mIncreament;
    size_t mSize;
	PBYTE mpData;
};


IHE_Write * HE_CreateDynBufferWrite( CHE_DynBuffer * pBuffer, CHE_Allocator * pAllocator = nullptr );


class CHE_RefCount
{
public:
	CHE_RefCount() : mRefCount(0) {}

	inline operator size_t() { return mRefCount; }

	void AddRef();
	void DecRef();

private:
#ifdef _MAC_OS_X_
    int32 mRefCount;
#else
	long mRefCount;
#endif
};


class CHE_Lock
{
public:
	CHE_Lock();
	~CHE_Lock();

	void Lock();
	void UnLock();

#ifdef WIN32
	HANDLE	mMutex;
#endif
    
#ifdef _MAC_OS_X_
    pthread_mutex_t mMutex;
#endif
};

#endif
