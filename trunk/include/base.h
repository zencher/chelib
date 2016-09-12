#ifndef _CHELIB_BASE_H_
#define _CHELIB_BASE_H_

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

#include "define.h"

namespace chelib {

class CAllocator
{
public:
	virtual ~CAllocator(){}
	virtual void * Alloc( size_t size ) = 0;
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


class CDefCrtAllocator : public CAllocator
{
public:
	inline void * Alloc( size_t size );

    inline void Free( void * data );

	inline size_t GetSize( void * data );
};


CAllocator * GetDefaultAllocator();


#ifdef WIN32
class CHeapAllocator : public CAllocator
{
public:
	CHeapAllocator( size_t initSize = 1 * 1024 * 1024 );
	
	~CHeapAllocator();
		
	inline void * Alloc( size_t size );
	
	inline void Free( void * data );

	inline size_t GetSize( void * data );
private:
	HANDLE mHeap;
};
#endif


class CObject
{
public:
	CObject( CAllocator * pAllocator );

	CAllocator * GetAllocator() const { return mpAllocator; }

private:
	CAllocator * mpAllocator;
};


class IWrite : public CObject
{
public:
	IWrite( CAllocator * pAllocator = nullptr ) : CObject( pAllocator ) {};

	virtual ~IWrite() {};

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

IWrite* CreateFileIWrite( char const * filename, BYTE mode = FILEWRITE_MODE_NEW, CAllocator * pAllocator = nullptr );

void DestoryIWrite( IWrite * pIWrite );

class IRead : public CObject
{
public:
	IRead( CAllocator * pAllocator = nullptr ) : CObject( pAllocator ) {}

	virtual ~IRead() {};

	virtual size_t GetSize() = 0;
	
	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size ) = 0;

	virtual BYTE ReadByte( size_t offset ) = 0;
	
	virtual void Release() = 0;
};

#define FILEREAD_MODE_DEFAULT		0
#define FILEREAD_MODE_MEMCOPY		1
#define FILEREAD_MODE_BUFFER		2
#define FILEREAD_MODE_BLOCKLINK		3

IRead * CreateFileIRead( char const * filename, BYTE mode = FILEREAD_MODE_DEFAULT,
                        size_t param = 0, CAllocator * pAllocator = nullptr );

IRead * CreateMemBufIRead( PCBYTE pBuf, size_t lSize, CAllocator * pAllocator = nullptr );

void DestoryIRead( IRead * pIRead );


class CDynBuffer : public CObject
{
public:
	CDynBuffer( size_t capacity = 1024, size_t increament = 1024, CAllocator * pAllocator = nullptr );
	CDynBuffer( const CDynBuffer & buf );
	~CDynBuffer();

	const CDynBuffer & operator = ( const CDynBuffer & buf );

	PBYTE GetData() { return mpData; }

	size_t GetCapacity() { return mCapacity; }

	size_t GetSize() { return mSize; }

	size_t Write( PCBYTE pBuffer, size_t offset, size_t size );

	size_t Write( PCBYTE pBuffer, size_t size );

	size_t Read( PBYTE pBuffer, size_t size );

	bool ReadByte( size_t offset, PBYTE pByte );

	size_t Write( const CDynBuffer & dynBuffer );

	void Clear() { mSize = 0; }

	void Alloc( size_t size );

private:
	size_t mCapacity;
	size_t mIncreament;
    size_t mSize;
	PBYTE mpData;
};


IWrite * CreateDynBufferIWrite( CDynBuffer * pBuffer, CAllocator * pAllocator = nullptr );


class CRefCount
{
public:
	CRefCount() : mRefCount(0) {}

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


class CLock
{
public:
	CLock();
	~CLock();

	void Lock();
	void UnLock();

#ifdef WIN32
	HANDLE	mMutex;
#endif
    
#ifdef _MAC_OS_X_
    pthread_mutex_t mMutex;
#endif
};

}//namespace

#endif
