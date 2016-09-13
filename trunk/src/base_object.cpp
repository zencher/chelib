#include "../include/base_object.h"

#include <cstdlib>
#include <cstdio>
#include <memory>

#ifdef WIN32
#include <windows.h>
#include <intrin.h>
#endif

#ifdef _MAC_OS_X_
#include <libkern/OSAtomic.h>
#endif

namespace chelib {

CDefCrtAllocator gDefCrtAllocator;

CObject::CObject( CAllocator * pAllocator )
{
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	mpAllocator = pAllocator;
}


inline void * CDefCrtAllocator::Alloc( size_t cb )
{
	return malloc( cb );
}

inline void CDefCrtAllocator::Free( void * data )
{
	free( data );
}

inline size_t CDefCrtAllocator::GetSize( void * data )
{
#ifdef WIN32
	return _msize( data );
#endif
#ifdef _LINUX_
	return malloc_usable_size( data );
#endif
#ifdef _MAC_OS_X_
    return malloc_size( data );
#endif
}

CAllocator * GetDefaultAllocator()
{
	return & gDefCrtAllocator;
}

#ifdef WIN32

CHeapAllocator::CHeapAllocator( size_t initSize )
{
	mHeap = HeapCreate( 0, initSize, 0 );
}

CHeapAllocator::~CHeapAllocator()
{
	if ( mHeap )
	{
		HeapDestroy( m_Heap );
	}
}

inline void * CHeapAllocator::Alloc( size_t cb )
{
	return HeapAlloc( mHeap, 0, cb );
}

inline void CHeapAllocator::Free( void * data )
{
	HeapFree( mHeap, 0, data );
}

inline size_t CHeapAllocator::GetSize( void * data )
{
	return HeapSize( mHeap, 0, data );
}

#endif

class ICrtFileWrite: public IWrite
{
public:
	ICrtFileWrite( char const * filename,  BYTE mode, CAllocator * pAllocator = nullptr );

	virtual ~ICrtFileWrite();

	virtual size_t GetSize();

	virtual size_t GetCurOffset();

	virtual size_t Flush();

	virtual	bool WriteBlock( const void * pData, size_t offset, size_t size);

	virtual void Release();

private:
	FILE * m_pFile;
};

ICrtFileWrite::ICrtFileWrite( char const * filename, BYTE mode, CAllocator * pAllocator ) : IWrite( pAllocator )
{
	if ( filename == nullptr )
	{
		m_pFile = nullptr;
	}else{
		if ( mode == FILEWRITE_MODE_NEW )
		{
			m_pFile = fopen( filename, "wb+" );
		}else{
			m_pFile = fopen( filename, "rb+" );
		}
	}
}

ICrtFileWrite::~ICrtFileWrite()
{
	if ( m_pFile != nullptr )
	{
		fclose( m_pFile );
		m_pFile = nullptr;
	}
}

size_t ICrtFileWrite::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

size_t ICrtFileWrite::GetCurOffset()
{
	return GetSize();
}

size_t ICrtFileWrite::Flush()
{
	if ( m_pFile )
	{
		return fflush( m_pFile );
	}else{
		return 0;
	}
}

void ICrtFileWrite::Release()
{
	if ( m_pFile )
	{
		fflush( m_pFile );
		fclose( m_pFile );
		m_pFile = nullptr;
	}
}

bool ICrtFileWrite::WriteBlock( const void * pData, size_t offset, size_t size )
{
	if ( pData == nullptr || size == 0 )
	{
		return false;
	}
	if ( m_pFile )
	{
		fseek( m_pFile, offset, SEEK_SET );
		size_t dwRet = fwrite( pData, 1, size, m_pFile );
		if ( dwRet > 0 )
		{
			return TRUE;
		}else{
			return false;
		}
	}else{
		return false;
	}
}

IWrite* CreateFileIWrite( char const * filename, BYTE mode, CAllocator * pAllocator )
{
	if ( filename != nullptr )
	{
		IWrite * pTmp = nullptr;
		if ( pAllocator == nullptr )
		{
			pTmp = new ICrtFileWrite( filename, mode, nullptr );
		}else{
			pTmp = pAllocator->New<ICrtFileWrite>( filename, mode, pAllocator );
		}
		return pTmp;
	}else{
		return nullptr;
	}
}

void DestoryIWrite( IWrite * pIHEWrite )
{
	if ( pIHEWrite != nullptr )
	{
		pIHEWrite->GetAllocator()->Delete<IWrite>( pIHEWrite );
	}
}

class IMemBufRead : public IRead
{
public:
	IMemBufRead( PCBYTE pBuf, size_t lSize , CAllocator * pAllocator = nullptr ) : IRead( pAllocator )
 	{ m_lSize = lSize; m_pBuf = pBuf; }
	
	virtual ~IMemBufRead() {};
	
	virtual size_t GetSize() { return m_lSize; }
	
	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size );
	
	virtual BYTE ReadByte( size_t offset );
	
	virtual void Release() { m_pBuf = nullptr; m_lSize = 0; }
	
private:
	PCBYTE m_pBuf;
    size_t m_lSize;
};

size_t IMemBufRead::ReadBlock( void * buffer, size_t offset, size_t size )
{
	if ( buffer == nullptr || m_pBuf == nullptr || size == 0 )
	{
		return 0;
	}
	if ( offset >= m_lSize )
	{
		return 0;
	}else{
		if ( offset + size > m_lSize )
		{
			size = m_lSize - offset;
		}
		memcpy( buffer, m_pBuf + offset, size );
		return size;
	}
}

BYTE IMemBufRead::ReadByte( size_t offset )
{
	if ( offset >= m_lSize )
	{
		return 0;
	}else{
		return m_pBuf[offset];
	}
}

IRead * CreateMemBufIRead( PCBYTE pBuf, size_t lSize, CAllocator * pAllocator )
{
	if ( pBuf == nullptr || lSize == 0 )
	{
		return nullptr;
	}
	if ( pAllocator == nullptr )
	{
		return new IMemBufRead( pBuf, lSize, nullptr );
	}else{
		return pAllocator->New<IMemBufRead>( pBuf, lSize, pAllocator );
	}
}

class ICrtFileReadDefault: public IRead
{
public:
	ICrtFileReadDefault( char const * filename, CAllocator * pAllocator = nullptr );

	virtual ~ICrtFileReadDefault();

	virtual size_t GetSize();

	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size );

	virtual BYTE ReadByte( size_t offset );

	virtual void Release();

private:
	FILE * m_pFile;
};

ICrtFileReadDefault::ICrtFileReadDefault( char const * filename, CAllocator * pAllocator ) : IRead( pAllocator )
{
	if ( filename == nullptr )
	{
		m_pFile = nullptr;
	}else{
		m_pFile = fopen( filename, "rb" );
	}
}

ICrtFileReadDefault::~ICrtFileReadDefault()
{
	if ( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = nullptr;
	}
}

size_t ICrtFileReadDefault::GetSize()
{
	if( m_pFile )
	{
		fseek( m_pFile, 0, SEEK_END );
		return ftell( m_pFile );
	}else{
		return 0;
	}
}

size_t ICrtFileReadDefault::ReadBlock( void * buffer, size_t offset, size_t size )
{
	if ( buffer == nullptr )
	{
		return 0;
	}
	if ( m_pFile )
	{
		fseek( m_pFile, offset, SEEK_SET );
		size_t dwRet = fread( buffer, 1, size, m_pFile );
		return dwRet;
	}else{
		return 0;
	}
}

BYTE ICrtFileReadDefault::ReadByte( size_t offset )
{
	if ( m_pFile )
	{
		if ( fseek( m_pFile, offset, SEEK_SET ) == -1 )
		{
			return 0;
		}else{
			BYTE byte;
			size_t dwRet = fread( &byte, 1, 1, m_pFile );
			if ( dwRet == 1 )
			{
				return byte;
			}else{
				return 0;
			}
		}
	}else{
		return 0;
	}
}

void ICrtFileReadDefault::Release()
{
	if ( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = nullptr;
	}
}

class ICrtFileReadMemcopy: public IRead
{
public:
	ICrtFileReadMemcopy( char const * filename, CAllocator * pAllocator = nullptr );
	virtual ~ICrtFileReadMemcopy();
	
	virtual size_t GetSize() { return m_lSize; }
	
	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size );
	
	virtual BYTE ReadByte( size_t offset );
	
	virtual void Release() {}
	
private:
	PBYTE m_pByte;
    size_t m_lSize;
};

ICrtFileReadMemcopy::ICrtFileReadMemcopy( char const * filename, CAllocator * pAllocator ) : IRead( pAllocator )
{
	FILE * pFile = fopen( filename, "rb" );
	if ( pFile )
	{
		fseek( pFile, 0, SEEK_END );
		m_lSize = ftell( pFile );
		fseek( pFile, 0, SEEK_SET );
		m_pByte = GetAllocator()->NewArray<BYTE>( m_lSize );
		fread( m_pByte, 1, m_lSize, pFile );
		fclose( pFile );
	}else{
		m_pByte = nullptr;
		m_lSize = 0;
	}
}

ICrtFileReadMemcopy::~ICrtFileReadMemcopy()
{
	if ( m_pByte )
	{
		GetAllocator()->DeleteArray<BYTE>(m_pByte);
	}
}

size_t ICrtFileReadMemcopy::ReadBlock( void * buffer, size_t offset, size_t size )
{
	if ( offset + size < m_lSize )
	{
		memcpy( buffer, m_pByte+offset, size );
		return size;
	}else{
		size = m_lSize - offset;
		memcpy( buffer, m_pByte+offset, size );
		return size;
	}
	return 0;
}

BYTE ICrtFileReadMemcopy::ReadByte( size_t offset )
{
	if ( offset < m_lSize )
	{
		return m_pByte[offset];
	}else{
		return 0;
	}
}

IRead * CreateFileIRead( char const * filename, BYTE mode, size_t param, CAllocator * pAllocator )
{
	CDefCrtAllocator defcrtAllocator;
	CAllocator * pTmpAllocator = nullptr;
	if ( pAllocator == nullptr )
	{
		pTmpAllocator = &defcrtAllocator;
	}else{
		pTmpAllocator = pAllocator;
	}
	if ( filename != nullptr )
	{
		switch ( mode )
		{
		case FILEREAD_MODE_DEFAULT:
			return pTmpAllocator->New<ICrtFileReadDefault>( filename, pAllocator );
		case FILEREAD_MODE_MEMCOPY:
			return pTmpAllocator->New<ICrtFileReadMemcopy>( filename, pAllocator );
		case FILEREAD_MODE_BUFFER:
			return nullptr;
		case FILEREAD_MODE_BLOCKLINK:
			return nullptr;
		default:
			return nullptr;
		}
	}else{
		return nullptr;
	}
}

void DestoryIRead( IRead * pIHERead )
{
	if ( pIHERead != nullptr )
	{
		pIHERead->GetAllocator()->Delete<IRead>( pIHERead );
	}
}

class IFile : public CObject 
{
	IFile( CAllocator * pAllocator ) : CObject( pAllocator ) {}

	virtual ~IFile() {};

	virtual size_t GetSize() = 0;

	virtual size_t GetCurOffset() = 0;

	virtual size_t ReadBlock( void * buffer, size_t offset, size_t size ) = 0;

	virtual BYTE ReadByte( size_t offset ) = 0;

	virtual	bool WriteBlock( const void * pData, size_t offset, size_t size ) = 0;

	virtual	bool WriteBlock( const void * pData, size_t size )
	{
		return WriteBlock( pData, GetSize(), size );
	}

	virtual bool WriteByte( BYTE byte )
	{
		return WriteBlock( &byte, 1 );
	}

	virtual size_t Flush() = 0;

	virtual void Release() = 0;
};

CDynBuffer::CDynBuffer( size_t capacity /*= 1024*/, size_t increament /*= 1024*/, CAllocator * pAllocator /*= nullptr*/ )
	: CObject( pAllocator )
{
	mCapacity = capacity;
	mIncreament = increament;

	mpData = GetAllocator()->NewArray<BYTE>( mCapacity );
	memset( mpData, 0, mCapacity );
	mSize = 0;
}

CDynBuffer::CDynBuffer( const CDynBuffer & buf )
	: CObject( buf.GetAllocator() )
{
	mCapacity = buf.mCapacity;
	mIncreament = buf.mIncreament;
	mSize = buf.mSize;
	mpData = GetAllocator()->NewArray<BYTE>( mCapacity );
	memset( mpData, 0, mCapacity );
	if ( mSize > 0 )
	{
		memcpy( mpData, buf.mpData, mSize );
	}
}

CDynBuffer::~CDynBuffer()
{
	if ( mpData )
	{
		GetAllocator()->DeleteArray<BYTE>( mpData );
		mpData = nullptr;
	}
}

const CDynBuffer & CDynBuffer::operator = ( const CDynBuffer & buf )
{
	if ( this != &buf )
	{
		if ( mpData )
		{
			GetAllocator()->DeleteArray<BYTE>( mpData );
		}
		mCapacity = buf.mCapacity;
		mIncreament = buf.mIncreament;
		mSize = buf.mSize;
		mpData = GetAllocator()->NewArray<BYTE>( mCapacity );
		memset( mpData, 0, mCapacity );
		if ( mSize > 0 )
		{
			memcpy( mpData, buf.mpData, mSize );
		}
	}
	return *this;
}

size_t CDynBuffer::Write( PCBYTE pBuffer, size_t offset, size_t size )
{
	if ( pBuffer == nullptr || size == 0 || offset > mSize )
	{
		return 0;
	}

	if ( size + offset > mCapacity )
	{
		size_t lNeed = size + offset - mCapacity;
		if ( lNeed <= mIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (size_t)( lNeed / mIncreament ) + 1;
		}
		PBYTE tmp = GetAllocator()->NewArray<BYTE>( mCapacity + lNeed * mIncreament );
		memset( tmp, 0, mCapacity + lNeed * mIncreament );
		memcpy( tmp, mpData, mSize );
		memcpy( tmp + offset, pBuffer, size );
		GetAllocator()->DeleteArray<BYTE>( mpData );
		mpData = tmp;
		mSize = offset + size;
		mCapacity += lNeed * mIncreament;
		return size;
	}else{
		memcpy( mpData + offset, pBuffer, size );
		mSize = offset + size;
		return size;
	}
}

void CDynBuffer::Alloc( size_t size )
{
	if ( size <= mCapacity )
	{
		mSize = size;
		return;
	}
	if ( mpData )
	{
		GetAllocator()->DeleteArray( mpData );
	}
	mpData = GetAllocator()->NewArray<BYTE>( size );
	mSize = size;
	mCapacity = size;
	mIncreament = size;
}

size_t CDynBuffer::Write( PCBYTE pBuffer, size_t size )
{
	return Write( pBuffer, mSize, size );
}

size_t CDynBuffer::Write( const CDynBuffer & dynBuffer )
{
	if ( dynBuffer.mpData == nullptr || dynBuffer.mSize == 0 )
	{
		return 0;
	}

	if ( mSize + dynBuffer.mSize > mCapacity )
	{
		size_t lNeed = mSize + dynBuffer.mSize - mCapacity;
		if ( lNeed <= mIncreament )
		{
			lNeed = 1;
		}else{
			lNeed = (size_t)( lNeed / mIncreament ) + 1;
		}
		PBYTE tmp = GetAllocator()->NewArray<BYTE>( mCapacity + lNeed * mIncreament );
		memset( tmp, 0, mCapacity + lNeed * mIncreament );
		memcpy( tmp, mpData, mSize );
		memcpy( tmp + mSize, dynBuffer.mpData, dynBuffer.mSize );
		GetAllocator()->DeleteArray<BYTE>( mpData );
		mpData = tmp;
		mSize += dynBuffer.mSize;
		mCapacity += lNeed * mIncreament;
		return dynBuffer.mSize;
	}else{
		memcpy( mpData + mSize, dynBuffer.mpData, dynBuffer.mSize );
		mSize += dynBuffer.mSize;
		return dynBuffer.mSize;
	}
}

size_t CDynBuffer::Read( PBYTE pBuffer, size_t size )
{
	if ( pBuffer == nullptr || size == 0 )
	{
		return 0;
	}
	if ( mSize < size )
	{
		size = mSize;
	}
	memcpy( pBuffer, mpData, size );
	return size;
}

bool CDynBuffer::ReadByte( size_t offset, PBYTE pByte )
{
	if ( offset < mSize  )
	{
		*pByte = *(mpData + offset);
		return TRUE;
	}
	return false;
}

class IDynBufferWrite : public IWrite
{
public:
	IDynBufferWrite( CDynBuffer * pBuffer, CAllocator * pAllocator = nullptr )
		: IWrite( pAllocator ), mpBuffer( pBuffer ) {}

	~IDynBufferWrite() {}

	size_t GetSize()
	{
		if ( mpBuffer )
		{
			return mpBuffer->GetSize();
		}
		return 0;
	}

	size_t GetCurOffset()
	{
		if ( GetSize() == 0 )
		{
			return 0;
		}
		return GetSize()-1;
	}

	size_t Flush() { return 0; }

	bool WriteBlock( const void * pData, size_t offset, size_t size )
	{
		if ( mpBuffer == nullptr || pData == nullptr || size == 0 || offset > GetSize() )
		{
			return false;
		}
		mpBuffer->Write( (PCBYTE)pData, offset, size );
		return TRUE;
	}

	void Release() {}

private:
	CDynBuffer * mpBuffer;
};

IWrite * HE_CreateDynBufferWrite( CDynBuffer * pBuffer, CAllocator * pAllocator )
{
	if ( pBuffer == nullptr )
	{
		return nullptr;
	}
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	return pAllocator->New<IDynBufferWrite>( pBuffer, pAllocator );
}

void CRefCount::AddRef()
{
#ifdef WIN32
	_InterlockedIncrement( &mRefCount );
#else
    OSAtomicIncrement32( &mRefCount );
#endif
}

void CRefCount::DecRef()
{
#ifdef WIN32
	_InterlockedDecrement( &mRefCount );
#else
    OSAtomicDecrement32( &mRefCount );
#endif
}


CLock::CLock()
{
#ifdef WIN32
	mMutex = CreateMutex( nullptr, false, nullptr );
#endif

#ifdef _MAC_OS_X_
    pthread_mutex_init(&mMutex, nullptr);
#endif
}

CLock::~CLock()
{
#ifdef WIN32
	if ( mMutex )
	{
		CloseHandle( mMutex );
		mMutex = nullptr;
	}
#endif
    
#ifdef _MAC_OS_X_
    pthread_mutex_destroy(&mMutex);
#endif
}

void CLock::Lock()
{
#ifdef WIN32
	WaitForSingleObject( mMutex, INFINITE );
#endif
    
#ifdef _MAC_OS_X_
    pthread_mutex_lock(&mMutex);
#endif
}

void CLock::UnLock()
{
#ifdef WIN32
	ReleaseMutex( mMutex );
#endif
    
#ifdef _MAC_OS_X_
    pthread_mutex_unlock(&mMutex);
#endif
}

}//end of namespace chelib