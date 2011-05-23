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
	
	CHE_ByteString& operator=( HE_CHAR ch );		//�ú������Բ�Ҫ����Ϊ����������ͨ�����캯��������ת����
													//һ����ʱ��const CHE_ByteString����
	CHE_ByteString& operator=( HE_LPCSTR lpStr );	//ͬ��
	CHE_ByteString& operator=( const CHE_ByteString& str );

	HE_BOOL	SetData( HE_BYTE * pData, HE_DWORD size );

	HE_LPCSTR	GetData() const { return ( m_lpData ) ? m_lpData->m_lpString : NULL; }

	bool operator==( HE_CHAR ch )const;
	bool operator==( HE_LPCSTR lpStr )const;
	bool operator==( const CHE_ByteString& str )const;

	friend bool operator==( HE_CHAR ch, CHE_ByteString& str );
	friend bool operator==( HE_LPSTR lpStr, CHE_ByteString& str );

	HE_DWORD GetLength() const;

	HE_CHAR operator[]( HE_DWORD index )const;		//�����Դ�ֵ�ķ�ʽ����һ����ʱ��HE_CHAR����HE_CHAR�ĸ�ֵ����Ӱ��string�ڲ�
													//��ֵ��ʵ���ϣ���ʱ������const���������ܱ���ֵ��

	//�����ܹ�ǰ��ߵ��Ĳ���������Ҫ��ȫ�ֵĿռ����涨��ȫ�ֵĺ�����
	CHE_ByteString operator+( HE_CHAR ch );
	CHE_ByteString operator+( HE_LPCSTR lpStr );
	CHE_ByteString operator+( const CHE_ByteString& str );
	//�������������Ĳ��������������Ƿ���һ����ʱ���󣬷���һ������const����ʱ���󣬿���֧����������+��ʹ�á�

 	friend CHE_ByteString operator+( HE_CHAR ch, CHE_ByteString& str );
 	friend CHE_ByteString operator+( HE_LPCSTR lpStr, CHE_ByteString& str );
	
	CHE_ByteString& operator+=( HE_CHAR ch );
	CHE_ByteString& operator+=( HE_LPCSTR lpStr );
	CHE_ByteString& operator+=( const CHE_ByteString& str );
	//��������������Ӧ�÷���һ��const���������(�������ʱ�����������)����Ϊ�����������Զ���������˲��������Է��ص��Ǳ�����������֧�������Ĳ�����ʹ��
	
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
	
	CHE_WideString& operator=( HE_WCHAR wch );		//�ú������Բ�Ҫ����Ϊ����������ͨ�����캯��������ת����һ����ʱ��const CHE_ByteString����
	CHE_WideString& operator=( HE_LPCWSTR lpWstr );	//ͬ��
	CHE_WideString& operator=( const CHE_WideString& wstr );
	
	HE_BOOL	SetData( HE_WCHAR * pData, HE_DWORD size );

	HE_LPCWSTR	GetData() const { return ( m_lpData ) ? m_lpData->m_lpString : NULL; }
	
	bool operator==( HE_WCHAR wch )const;
	bool operator==( HE_LPCWSTR lpWstr )const;
	bool operator==( const CHE_WideString& wstr )const;
	
	friend bool operator==( HE_WCHAR wch, CHE_WideString& wstr );
	friend bool operator==( HE_LPCWSTR lpWstr, CHE_WideString& wstr );
	
	HE_DWORD GetLength() const;
	
	HE_WCHAR operator[]( HE_DWORD index )const;		//�����Դ�ֵ�ķ�ʽ����һ����ʱ��HE_CHAR����HE_CHAR�ĸ�ֵ����Ӱ��string�ڲ�
	//��ֵ��ʵ���ϣ���ʱ������const���������ܱ���ֵ��
	
	//�����ܹ�ǰ��ߵ��Ĳ���������Ҫ��ȫ�ֵĿռ����涨��ȫ�ֵĺ�����
	CHE_WideString operator+( const CHE_WideString& wstr );
	CHE_WideString operator+( HE_WCHAR wch );
	CHE_WideString operator+( HE_LPCWSTR lpStr );
	//�������������Ĳ��������������Ƿ���һ����ʱ���󣬷���һ������const����ʱ���󣬿���֧����������+��ʹ�á�
	
	friend CHE_WideString operator+( HE_WCHAR wch, CHE_WideString& wstr );
	friend CHE_WideString operator+( HE_LPCWSTR lpWstr, CHE_WideString& wstr );
	
	CHE_WideString& operator+=( const CHE_WideString& wstr );
	CHE_WideString& operator+=( HE_WCHAR wch );
	CHE_WideString& operator+=( HE_LPCWSTR lpWstr );
	//��������������Ӧ�÷���һ��const���������(�������ʱ�����������)����Ϊ�����������Զ���������˲��������Է��ص��Ǳ�����������֧�������Ĳ�����ʹ��
	
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
