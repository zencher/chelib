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
	
	CHE_ByteString& operator=( HE_CHAR ch );		//�ú������Բ�Ҫ����Ϊ����������ͨ�����캯��������ת����
													//һ����ʱ��const CHE_ByteString����
	CHE_ByteString& operator=( HE_LPCSTR lpStr );	//ͬ��
	CHE_ByteString& operator=( const CHE_ByteString& str );

	bool operator==( HE_CHAR ch );
	bool operator==( HE_LPCSTR lpStr );
	bool operator==( const CHE_ByteString& str );

	friend bool operator==( HE_CHAR ch, const CHE_ByteString& str );
	friend bool operator==( HE_LPSTR lpStr, const CHE_ByteString& str );

	HE_CHAR operator[]( HE_INT32 index );		//�����Դ�ֵ�ķ�ʽ����һ����ʱ��HE_CHAR����HE_CHAR�ĸ�ֵ����Ӱ��string�ڲ�
												//��ֵ��ʵ���ϣ���ʱ������const���������ܱ���ֵ��
	//HE_CHAR& operator[]( HE_INT32 index );		//���������˶��󱣴���ַ�����һ���ַ������ã��κβ������ụ��Ӱ�졣���ã�����

// 	//�����ܹ�ǰ��ߵ��Ĳ���������Ҫ��ȫ�ֵĿռ����涨��ȫ�ֵĺ�����
// 	const CHE_ByteString& operator+( const CHE_ByteString& str );
// 	const CHE_ByteString& operator+( HE_CHAR ch );
// 	const CHE_ByteString& operator+( const HE_LPSTR lpStr );
// 	//������������Ӧ�÷���һ��const�������ã���Ϊ�������������Ĳ��������������Ƿ���һ����ʱ���󣬶���ʱ����ֵ�Ǵ����

// 	friend const CHE_ByteString& operator+( HE_CHAR ch, const CHE_ByteString& str );
// 	friend const CHE_ByteString& operator+( const HE_LPSTR lpStr, const CHE_ByteString& str );
	
	CHE_ByteString& operator+=( const CHE_ByteString& str );
	CHE_ByteString& operator+=( HE_CHAR ch );
	CHE_ByteString& operator+=( const HE_LPSTR str );
	//��������������Ӧ�÷���һ��const��������ã���Ϊ�����������Զ���������˲��������Է��ص��Ǳ�����������֧�������Ĳ�����ʹ��
	
	bool operator!=( const CHE_ByteString& str );
	bool operator!=( HE_CHAR ch );
	bool operator!=( const HE_LPSTR str );

	friend bool operator!=( HE_CHAR ch, const CHE_ByteString& str );
	friend bool operator!=( const HE_LPSTR lpStr, const CHE_ByteString& str );
	
	//bool operator>( const CHE_ByteString& str );
	//bool operator>( HE_CHAR ch );
	//bool operator>( const HE_LPSTR str );
	
	//bool operator<( const CHE_ByteString& str );
	//bool operator<( HE_CHAR ch );
	//bool operator<( const HE_LPSTR str );
	
private:
	HE_StringData* m_lpData;
};

bool operator==( HE_CHAR ch, const CHE_ByteString& str );
bool operator==( HE_LPSTR lpStr, const CHE_ByteString& str );

// const CHE_ByteString& operator+( HE_CHAR ch, const CHE_ByteString& str );
// const CHE_ByteString& operator+( const HE_LPSTR lpStr, const CHE_ByteString& str );

bool operator!=( HE_CHAR ch, const CHE_ByteString& str );
bool operator!=( const HE_LPSTR lpStr, const CHE_ByteString& str );

#endif