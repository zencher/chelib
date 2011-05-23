#ifndef _CHE_STRING_
#define _CHE_STRING_

#include "che_base.h"

struct HE_ByteStringData
{
	HE_LPSTR m_lpString;
	HE_DWORD m_dwLength;
	HE_DWORD m_dwRef;
};

class CHE_ByteString : public CHE_Object
{
public:
	CHE_ByteString() {}
	~CHE_ByteString()
	{
		Clear();
	}

	CHE_ByteString( HE_CHAR ch );
	CHE_ByteString( HE_LPCSTR str, HE_LONG nStrSize = -1 );
	CHE_ByteString( const CHE_ByteString& str );
	
	CHE_ByteString& operator=( HE_CHAR ch );		//该函数可以不要，因为编译器可以通过构造函数将参数转化成
													//一个临时的const CHE_ByteString引用
	CHE_ByteString& operator=( HE_LPCSTR lpStr );	//同上
	CHE_ByteString& operator=( const CHE_ByteString& str );

	HE_BOOL	SetData( HE_BYTE * pData, HE_DWORD size );

	HE_LPCSTR	GetData() const { return ( m_lpData ) ? m_lpData->m_lpString : NULL; }

	bool operator==( HE_CHAR ch )const;
	bool operator==( HE_LPCSTR lpStr )const;
	bool operator==( const CHE_ByteString& str )const;

	friend bool operator==( HE_CHAR ch, CHE_ByteString& str );
	friend bool operator==( HE_LPSTR lpStr, CHE_ByteString& str );

	HE_DWORD GetLength() const;

	HE_CHAR operator[]( HE_DWORD index )const;		//函数以传值的方式返回一个临时的HE_CHAR，对HE_CHAR的赋值不会影响string内部
													//的值，实际上，临时变量是const，根本不能被赋值。

	//如下能够前后颠倒的操作，还需要在全局的空间里面定义全局的函数。
	CHE_ByteString operator+( HE_CHAR ch );
	CHE_ByteString operator+( HE_LPCSTR lpStr );
	CHE_ByteString operator+( const CHE_ByteString& str );
	//以上三个函数的操作，在语义上是返回一个临时对象，返回一个不是const的临时对象，可以支持连续操作+的使用。

 	friend CHE_ByteString operator+( HE_CHAR ch, CHE_ByteString& str );
 	friend CHE_ByteString operator+( HE_LPCSTR lpStr, CHE_ByteString& str );
	
	CHE_ByteString& operator+=( HE_CHAR ch );
	CHE_ByteString& operator+=( HE_LPCSTR lpStr );
	CHE_ByteString& operator+=( const CHE_ByteString& str );
	//以上三个函数不应该返回一个const对象的引用(这个引用时对象本身的引用)，因为这三个函数对对象本身进行了操作，所以返回的是本身，这样可以支持连续的操作符使用
	
	bool operator!=( HE_CHAR ch )const;
	bool operator!=( HE_LPCSTR lpStr )const;
	bool operator!=( const CHE_ByteString& str )const;

	friend bool operator!=( HE_CHAR ch, CHE_ByteString& str );
	friend bool operator!=( HE_LPCSTR lpStr, CHE_ByteString& str );
	
private:
	HE_VOID Clear();

	HE_ByteStringData* m_lpData;
};

bool operator==( HE_CHAR ch, CHE_ByteString& str );
bool operator==( HE_LPCSTR lpStr, CHE_ByteString& str );

CHE_ByteString operator+( HE_CHAR ch, CHE_ByteString& str );
CHE_ByteString operator+( HE_LPCSTR lpStr, CHE_ByteString& str );

bool operator!=( HE_CHAR ch, CHE_ByteString& str );
bool operator!=( HE_LPCSTR lpStr, CHE_ByteString& str );


struct HE_WideStringData
{
	HE_LPWSTR m_lpString;
	HE_DWORD m_dwLength;
	HE_DWORD m_dwRef;
};

class CHE_WideString : public CHE_Object
{
public:
	CHE_WideString() {}
	~CHE_WideString()
	{
		Clear();
	}
	
	CHE_WideString( HE_WCHAR wch );
	CHE_WideString( HE_LPCWSTR wstr, HE_LONG nStrSize = -1 );
	CHE_WideString( const CHE_WideString& wstr );
	
	CHE_WideString& operator=( HE_WCHAR wch );		//该函数可以不要，因为编译器可以通过构造函数将参数转化成一个临时的const CHE_ByteString引用
	CHE_WideString& operator=( HE_LPCWSTR lpWstr );	//同上
	CHE_WideString& operator=( const CHE_WideString& wstr );
	
	HE_BOOL	SetData( HE_WCHAR * pData, HE_DWORD size );

	HE_LPCWSTR	GetData() const { return ( m_lpData ) ? m_lpData->m_lpString : NULL; }
	
	bool operator==( HE_WCHAR wch )const;
	bool operator==( HE_LPCWSTR lpWstr )const;
	bool operator==( const CHE_WideString& wstr )const;
	
	friend bool operator==( HE_WCHAR wch, CHE_WideString& wstr );
	friend bool operator==( HE_LPCWSTR lpWstr, CHE_WideString& wstr );
	
	HE_DWORD GetLength() const;
	
	HE_WCHAR operator[]( HE_DWORD index )const;		//函数以传值的方式返回一个临时的HE_CHAR，对HE_CHAR的赋值不会影响string内部
	//的值，实际上，临时变量是const，根本不能被赋值。
	
	//如下能够前后颠倒的操作，还需要在全局的空间里面定义全局的函数。
	CHE_WideString operator+( const CHE_WideString& wstr );
	CHE_WideString operator+( HE_WCHAR wch );
	CHE_WideString operator+( HE_LPCWSTR lpStr );
	//以上三个函数的操作，在语义上是返回一个临时对象，返回一个不是const的临时对象，可以支持连续操作+的使用。
	
	friend CHE_WideString operator+( HE_WCHAR wch, CHE_WideString& wstr );
	friend CHE_WideString operator+( HE_LPCWSTR lpWstr, CHE_WideString& wstr );
	
	CHE_WideString& operator+=( const CHE_WideString& wstr );
	CHE_WideString& operator+=( HE_WCHAR wch );
	CHE_WideString& operator+=( HE_LPCWSTR lpWstr );
	//以上三个函数不应该返回一个const对象的引用(这个引用时对象本身的引用)，因为这三个函数对对象本身进行了操作，所以返回的是本身，这样可以支持连续的操作符使用
	
	bool operator!=( const CHE_WideString& wstr )const;
	bool operator!=( HE_WCHAR wch )const;
	bool operator!=( HE_LPCWSTR lpWstr )const;
	
	friend bool operator!=( HE_WCHAR wch, CHE_WideString& wstr );
	friend bool operator!=( HE_LPCWSTR lpWstr, CHE_WideString& wstr );
	
private:
	HE_VOID Clear();
	
	HE_WideStringData* m_lpData;
};

bool operator==( HE_WCHAR wch, CHE_WideString& wstr );
bool operator==( HE_LPWSTR lpWstr, CHE_WideString& wstr );

CHE_WideString operator+( HE_WCHAR wch, CHE_WideString& wstr );
CHE_WideString operator+( HE_LPCWSTR lpWstr, CHE_WideString& wstr );

bool operator!=( HE_WCHAR wch, CHE_WideString& wstr );
bool operator!=( HE_LPCWSTR lpWstr, CHE_ByteString& wstr );

#endif
