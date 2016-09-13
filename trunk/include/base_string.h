#ifndef _CHELIB_BASE_STRING_
#define _CHELIB_BASE_STRING_

#include "base_object.h"

namespace chelib {

struct ByteStringData;

class CByteString : public CObject
{
public:
    CByteString( CAllocator * pAllocator = nullptr ) : CObject(pAllocator), mpData(nullptr) {}

	~CByteString()
	{
		Clear();
	}

	CByteString( char ch, CAllocator * pAllocator = nullptr );
	CByteString( char const * pstr, size_t length = 0, CAllocator * pAllocator = nullptr );
	CByteString( const CByteString& str );
	
	CByteString & operator=( char ch );
	CByteString & operator=( char const * pstr );
	CByteString & operator=( const CByteString& str );

	bool operator==( char ch ) const;
	bool operator==( char const * pstr ) const;
	bool operator==( const CByteString & str ) const;
	friend bool operator==( char ch, CByteString & str );
	friend bool operator==( char * pstr, CByteString & str );

	CByteString operator+( char ch );
	CByteString operator+( char const * pstr );
	CByteString operator+( const CByteString & str );
 	friend CByteString operator+( char ch, CByteString & str );
 	friend CByteString operator+( char const * pstr, CByteString & str );
	
	CByteString & operator+=( char ch );
	CByteString & operator+=( char const * pstr );
	CByteString & operator+=( const CByteString & str );
	
	bool operator!=( char ch ) const;
	bool operator!=( char const * pstr ) const;
	bool operator!=( const CByteString& str ) const;
	friend bool operator!=( char ch, CByteString & str );
	friend bool operator!=( char const * pstr, CByteString & str );
    
    char operator[]( size_t index ) const;
    
    size_t GetLength() const;
    
    bool SetData( BYTE * data, size_t length );
    char const * GetData() const;
    
    int32 GetInteger() const;
    FLOAT GetFloat() const;
    
    void Clear();
    
private:
	ByteStringData * mpData;
};

bool operator==( char ch, CByteString & str );
bool operator==( char const * pstr, CByteString & str );

CByteString operator+( char ch, CByteString & str );
CByteString operator+( char const * pstr, CByteString & str );

bool operator!=( char ch, CByteString & str );
bool operator!=( char const * pstr, CByteString & str );


struct WideStringData;

class CWideString : public CObject
{
public:
    CWideString( CAllocator * pAllocator = nullptr ) : CObject(pAllocator), mpData(nullptr) {}

	~CWideString()
	{
		Clear();
	}
	
	CWideString( wchar_t wch, CAllocator * pAllocator = nullptr );
	CWideString( wchar_t const * pstr, size_t nStrSize = 0, CAllocator * pAllocator = nullptr );
	CWideString( const CWideString & str );

	CWideString & operator=( wchar_t wch );
    CWideString & operator=( wchar_t const * pstr );
	CWideString & operator=( const CWideString & str );
	
	bool operator==( wchar_t wch ) const;
	bool operator==( wchar_t const * pstr ) const;
	bool operator==( const CWideString & str ) const;
	friend bool operator==( wchar_t wch, CWideString & str );
	friend bool operator==( wchar_t const * pstr, CWideString & str );

	CWideString operator+( const CWideString & str );
	CWideString operator+( wchar_t wch );
	CWideString operator+( wchar_t const * pstr );
	friend CWideString operator+( wchar_t wch, CWideString & str );
	friend CWideString operator+( wchar_t const * pstr, CWideString & str );
	
	CWideString & operator+=( const CWideString & str );
	CWideString & operator+=( wchar_t wch );
	CWideString & operator+=( wchar_t const * pstr );

	bool operator!=( const CWideString & wstr ) const;
	bool operator!=( wchar_t wch ) const;
	bool operator!=( wchar_t const * pstr ) const;
	friend bool operator!=( wchar_t wch, CWideString & str );
	friend bool operator!=( wchar_t const * pstr, CWideString & str );
    
    wchar_t operator [] ( size_t index ) const;
    
    size_t GetLength() const;
    
    bool SetData( wchar_t * pData, size_t length );
    wchar_t const *  GetData() const;
    
    int32 GetInteger() const;
    FLOAT GetFloat() const;
    
    void Clear();
    
private:
	WideStringData * mpData;
};

bool operator==( wchar_t wch, CWideString & str );
bool operator==( wchar_t * pstr, CWideString & str );

CWideString operator+( wchar_t wch, CWideString & str );
CWideString operator+( wchar_t const * pstr, CWideString & str );

bool operator!=( wchar_t wch, CWideString & str );
bool operator!=( wchar_t const *  lstr, CByteString & str );

#define A(a)		(a)
#define B(a,b)		(a | b<<8)
#define C(a,b,c)	(a | b<<8 | c<<16)
#define D(a,b,c,d)	(a | b<<8 | c<<16 | d<<24)

uint32 StringToUINT32( const CByteString & str );

}//namespace

#endif
