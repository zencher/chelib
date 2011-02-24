#ifndef _CHE_STRING_
#define _CHE_STRING_

#include "che_base.h"

struct HE_ByteStringData
{
	HE_LPSTR m_lpString;
	HE_LONG m_dwLength;
	HE_LONG m_dwRef;
};

class CHE_ByteString : public CHE_Object
{
public:
	CHE_ByteString();
	~CHE_ByteString();

	CHE_ByteString( HE_CHAR ch );
	CHE_ByteString( HE_LPCSTR str, HE_LONG nStrSize = -1 );
	CHE_ByteString( const CHE_ByteString& string );
	
	CHE_ByteString& operator=( HE_CHAR ch );		//该函数可以不要，因为编译器可以通过构造函数将参数转化成
													//一个临时的const CHE_ByteString引用
	CHE_ByteString& operator=( HE_LPCSTR lpStr );	//同上
	CHE_ByteString& operator=( const CHE_ByteString& str );

	bool operator==( HE_CHAR ch )const;
	bool operator==( HE_LPCSTR lpStr )const;
	bool operator==( const CHE_ByteString& str )const;

	friend bool operator==( HE_CHAR ch, CHE_ByteString& str );
	friend bool operator==( HE_LPSTR lpStr, CHE_ByteString& str );

	HE_DWORD GetLength();

	HE_CHAR operator[]( HE_INT32 index )const;		//函数以传值的方式返回一个临时的HE_CHAR，对HE_CHAR的赋值不会影响string内部
												//的值，实际上，临时变量是const，根本不能被赋值。
	//HE_CHAR& operator[]( HE_INT32 index );		//函数返回了对象保存的字符串的一个字符的引用，任何操作都会互相影响。不好！！！

	//如下能够前后颠倒的操作，还需要在全局的空间里面定义全局的函数。
	CHE_ByteString operator+( const CHE_ByteString& str );
	CHE_ByteString operator+( HE_CHAR ch );
	CHE_ByteString operator+( const HE_LPSTR lpStr );
	//以上三个函数的操作，在语义上是返回一个临时对象，返回一个不是const的临时对象，可以支持连续操作+的使用。

 	friend CHE_ByteString operator+( HE_CHAR ch, CHE_ByteString& str );
 	friend CHE_ByteString operator+( const HE_LPSTR lpStr, CHE_ByteString& str );
	
	CHE_ByteString& operator+=( const CHE_ByteString& str );
	CHE_ByteString& operator+=( HE_CHAR ch );
	CHE_ByteString& operator+=( const HE_LPSTR lpStr );
	//以上三个函数不应该返回一个const对象的引用(这个引用时对象本身的引用)，因为这三个函数对对象本身进行了操作，所以返回的是本身，这样可以支持连续的操作符使用
	
	bool operator!=( const CHE_ByteString& str )const;
	bool operator!=( HE_CHAR ch )const;
	bool operator!=( const HE_LPSTR lpStr )const;

	friend bool operator!=( HE_CHAR ch, CHE_ByteString& str );
	friend bool operator!=( const HE_LPSTR lpStr, CHE_ByteString& str );
	
private:
	HE_ByteStringData* m_lpData;
};

bool operator==( HE_CHAR ch, CHE_ByteString& str );
bool operator==( HE_LPSTR lpStr, CHE_ByteString& str );

CHE_ByteString operator+( HE_CHAR ch, CHE_ByteString& str );
CHE_ByteString operator+( const HE_LPSTR lpStr, CHE_ByteString& str );

bool operator!=( HE_CHAR ch, CHE_ByteString& str );
bool operator!=( const HE_LPSTR lpStr, CHE_ByteString& str );

class CHE_ByteStringToPtrMap : public CHE_Object
{
public:
	CHE_ByteStringToPtrMap();
	~CHE_ByteStringToPtrMap();
	
	HE_BOOL		Append( CHE_ByteString & str, HE_LPVOID ptr );
	
	HE_LPVOID	GetItem( CHE_ByteString & str ) const;
	
	HE_VOID		Clear();
	
private:
	HE_LPVOID *			m_pData;
	CHE_ByteString **	m_pString;	
	HE_DWORD	m_lSize;
	HE_DWORD	m_lCount;
};

#endif