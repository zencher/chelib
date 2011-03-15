#ifndef _CHE_DATASTRUCTURE_H_
#define _CHE_DATASTRUCTURE_H_

#include "che_base.h"
#include "che_string.h"

class CHE_PtrStackNode
{
public:
	CHE_PtrStackNode() { pObj = NULL; pNext = NULL; }
	~CHE_PtrStackNode() {}	//这里不需要销毁子节点的对象，而是由Stack来负责释放

	HE_VOID * pObj;
	CHE_PtrStackNode * pNext;
};

class CHE_PtrStack
{
public:
	CHE_PtrStack() { m_pTop = NULL; }
	~CHE_PtrStack();
	
	void Clear();
	bool IsEmpty();
	bool Pop( HE_VOID ** ptr );
	bool Push( HE_VOID * ptr );

private:
	CHE_PtrStackNode * m_pTop;
};

class CHE_PtrArray : public CHE_Object
{
public:
	CHE_PtrArray();
	~CHE_PtrArray();
	
	HE_BOOL		Append( HE_LPVOID ptr );
	
	HE_LPVOID	GetItem( HE_DWORD index ) const;
	
	HE_VOID		Clear();
	
	HE_DWORD	GetCount() const { return m_lCount; }
	
private:
	HE_LPVOID *	m_pData;
	HE_DWORD	m_lSize;
	HE_DWORD	m_lCount;
};

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

class CHE_NumToPtrMap : public CHE_Object
{
public:
	CHE_NumToPtrMap();
	~CHE_NumToPtrMap();
	
	HE_BOOL		Append( HE_DWORD num, HE_LPVOID ptr );
	
	HE_LPVOID	GetItem( HE_DWORD num ) const;
	
	HE_LPVOID	GetItemByIndex( HE_DWORD index );
	
	HE_DWORD	GetCount() { return m_lCount; }
	
	HE_VOID		Clear();
	
private:
	HE_DWORD *		m_pNum;
	HE_LPVOID *		m_pData;
	HE_DWORD	m_lSize;
	HE_DWORD	m_lCount;
};

#endif
