#ifndef _CHE_STRING_
#define _CHE_STRING_

#include "che_base.h"

struct HE_ByteStringData
{
	HE_ByteStringData()
	{
		m_lpString = nullptr;
		m_dwLength = 0;
	}
	char * m_lpString;
	size_t m_dwLength;
	CHE_RefCount m_dwRef;
};

class CHE_ByteString : public CHE_Object
{
public:
	CHE_ByteString( CHE_Allocator * pAllocator = nullptr ) : CHE_Object( pAllocator )
	{
		m_lpData = nullptr;
	}
	~CHE_ByteString()
	{
		Clear();
	}

	CHE_ByteString( char ch, CHE_Allocator * pAllocator = nullptr );
	CHE_ByteString( char const * str, size_t nStrSize = 0, CHE_Allocator * pAllocator = nullptr );
	CHE_ByteString( const CHE_ByteString& str );
	
	CHE_ByteString & operator=( char ch );
	CHE_ByteString & operator=( char const * lpStr );
	CHE_ByteString & operator=( const CHE_ByteString& str );

	bool operator==( char ch ) const;
	bool operator==( char const * lpStr ) const;
	bool operator==( const CHE_ByteString & str ) const;
	friend bool operator==( char ch, CHE_ByteString & str );
	friend bool operator==( char * lpStr, CHE_ByteString & str );

	CHE_ByteString operator+( char ch );
	CHE_ByteString operator+( char const * lpStr );
	CHE_ByteString operator+( const CHE_ByteString & str );
 	friend CHE_ByteString operator+( char ch, CHE_ByteString & str );
 	friend CHE_ByteString operator+( char const * lpStr, CHE_ByteString & str );
	
	CHE_ByteString & operator+=( char ch );
	CHE_ByteString & operator+=( char const * lpStr );
	CHE_ByteString & operator+=( const CHE_ByteString& str );
	
	bool operator!=( char ch ) const;
	bool operator!=( char const * lpStr ) const;
	bool operator!=( const CHE_ByteString& str ) const;
	friend bool operator!=( char ch, CHE_ByteString & str );
	friend bool operator!=( char const * lpStr, CHE_ByteString & str );
    
    char operator[]( size_t index ) const;
    
    size_t GetLength() const;
    
    bool SetData( BYTE * data, size_t size );
    char const * GetData() const { return m_lpData ? m_lpData->m_lpString : nullptr; }
    
    int32 GetInteger() const;
    FLOAT GetFloat() const;
    
    void Clear();
    
private:
	HE_ByteStringData* m_lpData;
};

bool operator==( char ch, CHE_ByteString & str );
bool operator==( char const * lpStr, CHE_ByteString & str );

CHE_ByteString operator+( char ch, CHE_ByteString & str );
CHE_ByteString operator+( char const * lpStr, CHE_ByteString & str );

bool operator!=( char ch, CHE_ByteString & str );
bool operator!=( char const * lpStr, CHE_ByteString & str );


struct HE_WideStringData
{
    HE_WideStringData()
    {
        m_lpString = nullptr;
        m_dwLength = 0;
    }
    PWSTR m_lpString;
	size_t m_dwLength;
	CHE_RefCount m_dwRef;
};

class CHE_WideString : public CHE_Object
{
public:
	CHE_WideString( CHE_Allocator * pAllocator = nullptr ) : CHE_Object( pAllocator )
	{
		m_lpData = nullptr;
	}

	~CHE_WideString()
	{
		Clear();
	}
	
	CHE_WideString( WCHAR wch, CHE_Allocator * pAllocator = nullptr );
	CHE_WideString( PCWSTR wstr, size_t nStrSize = 0, CHE_Allocator * pAllocator = nullptr );
	CHE_WideString( const CHE_WideString& wstr );

	CHE_WideString & operator=( WCHAR wch );
    CHE_WideString & operator=( PCWSTR lpWstr );
	CHE_WideString & operator=( const CHE_WideString& wstr );
	
	bool operator==( WCHAR wch ) const;
	bool operator==( PCWSTR lpWstr ) const;
	bool operator==( const CHE_WideString & wstr ) const;
	friend bool operator==( WCHAR wch, CHE_WideString & wstr );
	friend bool operator==( PCWSTR lpWstr, CHE_WideString & wstr );

	CHE_WideString operator+( const CHE_WideString & wstr );
	CHE_WideString operator+( WCHAR wch );
	CHE_WideString operator+( PCWSTR lpStr );
	friend CHE_WideString operator+( WCHAR wch, CHE_WideString & wstr );
	friend CHE_WideString operator+( PCWSTR lpWstr, CHE_WideString & wstr );
	
	CHE_WideString & operator+=( const CHE_WideString & wstr );
	CHE_WideString & operator+=( WCHAR wch );
	CHE_WideString & operator+=( PCWSTR lpWstr );

	bool operator!=( const CHE_WideString & wstr ) const;
	bool operator!=( WCHAR wch ) const;
	bool operator!=( PCWSTR lpWstr ) const;
	friend bool operator!=( WCHAR wch, CHE_WideString & wstr );
	friend bool operator!=( PCWSTR lpWstr, CHE_WideString & wstr );
    
    WCHAR operator [] ( size_t index ) const;
    
    size_t GetLength() const;
    
    bool SetData( WCHAR * pData, size_t size );
    PCWSTR GetData() const { return ( m_lpData ) ? m_lpData->m_lpString : nullptr; }
    
    int32 GetInteger() const;

    FLOAT GetFloat() const;
    
    void Clear();
    
private:
	HE_WideStringData * m_lpData;
};

bool operator==( WCHAR wch, CHE_WideString & wstr );
bool operator==( PWSTR lpWstr, CHE_WideString & wstr );

CHE_WideString operator+( WCHAR wch, CHE_WideString & wstr );
CHE_WideString operator+( PCWSTR lpWstr, CHE_WideString & wstr );

bool operator!=( WCHAR wch, CHE_WideString & wstr );
bool operator!=( PCWSTR lpWstr, CHE_ByteString & wstr );

#define A(a)		(a)
#define B(a,b)		(a | b<<8)
#define C(a,b,c)	(a | b<<8 | c<<16)
#define D(a,b,c,d)	(a | b<<8 | c<<16 | d<<24)

uint32 StringToUINT32( const CHE_ByteString & str );

#endif
