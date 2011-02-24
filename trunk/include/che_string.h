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
	
	CHE_ByteString& operator=( HE_CHAR ch );		//�ú������Բ�Ҫ����Ϊ����������ͨ�����캯��������ת����
													//һ����ʱ��const CHE_ByteString����
	CHE_ByteString& operator=( HE_LPCSTR lpStr );	//ͬ��
	CHE_ByteString& operator=( const CHE_ByteString& str );

	bool operator==( HE_CHAR ch )const;
	bool operator==( HE_LPCSTR lpStr )const;
	bool operator==( const CHE_ByteString& str )const;

	friend bool operator==( HE_CHAR ch, CHE_ByteString& str );
	friend bool operator==( HE_LPSTR lpStr, CHE_ByteString& str );

	HE_DWORD GetLength();

	HE_CHAR operator[]( HE_INT32 index )const;		//�����Դ�ֵ�ķ�ʽ����һ����ʱ��HE_CHAR����HE_CHAR�ĸ�ֵ����Ӱ��string�ڲ�
												//��ֵ��ʵ���ϣ���ʱ������const���������ܱ���ֵ��
	//HE_CHAR& operator[]( HE_INT32 index );		//���������˶��󱣴���ַ�����һ���ַ������ã��κβ������ụ��Ӱ�졣���ã�����

	//�����ܹ�ǰ��ߵ��Ĳ���������Ҫ��ȫ�ֵĿռ����涨��ȫ�ֵĺ�����
	CHE_ByteString operator+( const CHE_ByteString& str );
	CHE_ByteString operator+( HE_CHAR ch );
	CHE_ByteString operator+( const HE_LPSTR lpStr );
	//�������������Ĳ��������������Ƿ���һ����ʱ���󣬷���һ������const����ʱ���󣬿���֧����������+��ʹ�á�

 	friend CHE_ByteString operator+( HE_CHAR ch, CHE_ByteString& str );
 	friend CHE_ByteString operator+( const HE_LPSTR lpStr, CHE_ByteString& str );
	
	CHE_ByteString& operator+=( const CHE_ByteString& str );
	CHE_ByteString& operator+=( HE_CHAR ch );
	CHE_ByteString& operator+=( const HE_LPSTR lpStr );
	//��������������Ӧ�÷���һ��const���������(�������ʱ�����������)����Ϊ�����������Զ���������˲��������Է��ص��Ǳ�����������֧�������Ĳ�����ʹ��
	
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