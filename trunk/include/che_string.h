#ifndef _CHE_STRING_
#define _CHE_STRING_

#include "che_base.h"

struct HE_ByteStringData
{
	HE_ByteStringData()
	{
		mpStr = nullptr;
		mLength = 0;
	}
	char *          mpStr;
	size_t          mLength;
	CHE_RefCount    mRefCount;
};

class CHE_ByteString : public CHE_Object
{
public:
    CHE_ByteString( CHE_Allocator * pAllocator = nullptr ) : CHE_Object(pAllocator), mpData(nullptr) {}
	
	~CHE_ByteString()
	{
		Clear();
	}

	CHE_ByteString( char ch, CHE_Allocator * pAllocator = nullptr );
	CHE_ByteString( char const * pstr, size_t length = 0, CHE_Allocator * pAllocator = nullptr );
	CHE_ByteString( const CHE_ByteString& str );
	
	CHE_ByteString & operator=( char ch );
	CHE_ByteString & operator=( char const * pstr );
	CHE_ByteString & operator=( const CHE_ByteString& str );

	bool operator==( char ch ) const;
	bool operator==( char const * pstr ) const;
	bool operator==( const CHE_ByteString & str ) const;
	friend bool operator==( char ch, CHE_ByteString & str );
	friend bool operator==( char * pstr, CHE_ByteString & str );

	CHE_ByteString operator+( char ch );
	CHE_ByteString operator+( char const * pstr );
	CHE_ByteString operator+( const CHE_ByteString & str );
 	friend CHE_ByteString operator+( char ch, CHE_ByteString & str );
 	friend CHE_ByteString operator+( char const * pstr, CHE_ByteString & str );
	
	CHE_ByteString & operator+=( char ch );
	CHE_ByteString & operator+=( char const * pstr );
	CHE_ByteString & operator+=( const CHE_ByteString & str );
	
	bool operator!=( char ch ) const;
	bool operator!=( char const * pstr ) const;
	bool operator!=( const CHE_ByteString& str ) const;
	friend bool operator!=( char ch, CHE_ByteString & str );
	friend bool operator!=( char const * pstr, CHE_ByteString & str );
    
    char operator[]( size_t index ) const;
    
    size_t GetLength() const;
    
    bool SetData( BYTE * data, size_t length );
    char const * GetData() const { return mpData ? mpData->mpStr : nullptr; }
    
    int32 GetInteger() const;
    FLOAT GetFloat() const;
    
    void Clear();
    
private:
	HE_ByteStringData* mpData;
};

bool operator==( char ch, CHE_ByteString & str );
bool operator==( char const * lpStr, CHE_ByteString & str );

CHE_ByteString operator+( char ch, CHE_ByteString & str );
CHE_ByteString operator+( char const * pstr, CHE_ByteString & str );

bool operator!=( char ch, CHE_ByteString & str );
bool operator!=( char const * pstr, CHE_ByteString & str );


struct HE_WideStringData
{
    HE_WideStringData()
    {
        mpStr = nullptr;
        mLength = 0;
    }
    wchar_t *       mpStr;
	size_t          mLength;
	CHE_RefCount    mRefCount;
};

class CHE_WideString : public CHE_Object
{
public:
    CHE_WideString( CHE_Allocator * pAllocator = nullptr ) : CHE_Object(pAllocator), mpData(nullptr) {}

	~CHE_WideString()
	{
		Clear();
	}
	
	CHE_WideString( wchar_t wch, CHE_Allocator * pAllocator = nullptr );
	CHE_WideString( wchar_t const * pstr, size_t nStrSize = 0, CHE_Allocator * pAllocator = nullptr );
	CHE_WideString( const CHE_WideString & str );

	CHE_WideString & operator=( wchar_t wch );
    CHE_WideString & operator=( wchar_t const * pstr );
	CHE_WideString & operator=( const CHE_WideString & str );
	
	bool operator==( wchar_t wch ) const;
	bool operator==( wchar_t const * pstr ) const;
	bool operator==( const CHE_WideString & str ) const;
	friend bool operator==( wchar_t wch, CHE_WideString & str );
	friend bool operator==( wchar_t const * pstr, CHE_WideString & str );

	CHE_WideString operator+( const CHE_WideString & str );
	CHE_WideString operator+( wchar_t wch );
	CHE_WideString operator+( wchar_t const * pstr );
	friend CHE_WideString operator+( wchar_t wch, CHE_WideString & str );
	friend CHE_WideString operator+( wchar_t const * pstr, CHE_WideString & str );
	
	CHE_WideString & operator+=( const CHE_WideString & str );
	CHE_WideString & operator+=( wchar_t wch );
	CHE_WideString & operator+=( wchar_t const * pstr );

	bool operator!=( const CHE_WideString & wstr ) const;
	bool operator!=( wchar_t wch ) const;
	bool operator!=( wchar_t const * pstr ) const;
	friend bool operator!=( wchar_t wch, CHE_WideString & str );
	friend bool operator!=( wchar_t const * pstr, CHE_WideString & str );
    
    wchar_t operator [] ( size_t index ) const;
    
    size_t GetLength() const;
    
    bool SetData( wchar_t * pData, size_t length );
    wchar_t const *  GetData() const { return ( mpData ) ? mpData->mpStr : nullptr; }
    
    int32 GetInteger() const;
    FLOAT GetFloat() const;
    
    void Clear();
    
private:
	HE_WideStringData * mpData;
};

bool operator==( wchar_t wch, CHE_WideString & str );
bool operator==( wchar_t * pstr, CHE_WideString & str );

CHE_WideString operator+( wchar_t wch, CHE_WideString & str );
CHE_WideString operator+( wchar_t const * pstr, CHE_WideString & str );

bool operator!=( wchar_t wch, CHE_WideString & str );
bool operator!=( wchar_t const *  lstr, CHE_ByteString & str );

#define A(a)		(a)
#define B(a,b)		(a | b<<8)
#define C(a,b,c)	(a | b<<8 | c<<16)
#define D(a,b,c,d)	(a | b<<8 | c<<16 | d<<24)

uint32 StringToUINT32( const CHE_ByteString & str );

#endif
