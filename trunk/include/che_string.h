#ifndef _CHE_STRING_
#define _CHE_STRING_

#include "che_base.h"

struct HE_StringData
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
	CHE_ByteString& operator=( HE_LPCSTR str );		//同上
	CHE_ByteString& operator=( const CHE_ByteString& str );

	bool operator==( HE_CHAR ch );
	bool operator==( HE_LPCSTR str );
	bool operator==( const CHE_ByteString& str );

	HE_CHAR operator[]( HE_INT32 index );		//函数以传值的方式返回一个临时的HE_CHAR，对HE_CHAR的赋值不会影响string内部
												//的值，实际上，临时变量是const，根本不能被赋值。
	//HE_CHAR& operator[]( HE_INT32 index );		//函数返回了对象保存的字符串的一个字符的引用，任何操作都会互相影响。不好！！！


	

	//如下能够前后颠倒的操作，还需要在全局的空间里面定义全局的函数。
	CHE_ByteString& operator+( const CHE_ByteString& str );
	CHE_ByteString& operator+( HE_CHAR ch );
	CHE_ByteString& operator+( const HE_LPSTR str );
	
	CHE_ByteString& operator+=( const CHE_ByteString& str );
	CHE_ByteString& operator+=( HE_CHAR ch );
	CHE_ByteString& operator+=( const HE_LPSTR str );
	
	bool operator!=( const CHE_ByteString& str );
	bool operator!=( HE_CHAR ch );
	bool operator!=( const HE_LPSTR str );
	
	bool operator>( const CHE_ByteString& str );
	bool operator>( HE_CHAR ch );
	bool operator>( const HE_LPSTR str );
	
	bool operator<( const CHE_ByteString& str );
	bool operator<( HE_CHAR ch );
	bool operator<( const HE_LPSTR str );
	
private:
	HE_StringData* m_lpData;
};

#endif