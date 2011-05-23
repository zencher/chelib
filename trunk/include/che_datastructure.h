#ifndef _CHE_DATASTRUCTURE_H_
#define _CHE_DATASTRUCTURE_H_

#include "che_base.h"
#include "che_string.h"

class CHE_PtrNode : CHE_Object
{
public:
	CHE_PtrNode( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) { pObj = NULL; pNext = NULL; }
	~CHE_PtrNode() {}	//这里不需要销毁子节点的对象，而是由Stack来负责释放

	HE_VOID * pObj;
	CHE_PtrNode * pNext;
};

class CHE_PtrStack : CHE_Object
{
public:
	CHE_PtrStack( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) { m_pTop = NULL; }
	~CHE_PtrStack();
	
	HE_VOID Clear();
	HE_BOOL IsEmpty();
	HE_BOOL Pop( HE_VOID ** ptr );
	HE_BOOL Push( HE_VOID * ptr );

private:
	CHE_PtrNode * m_pTop;
};

class CHE_PtrQueue : public CHE_Object
{
public:
	CHE_PtrQueue( CHE_Allocator * pAllocator = NULL );
	~CHE_PtrQueue();

	HE_VOID Clear();
	HE_BOOL IsEmpty();
	HE_BOOL	Pop( HE_VOID** pptr );
	HE_BOOL	Push( HE_VOID* ptr );

private:
	CHE_PtrNode * m_pHead;
	CHE_PtrNode * m_pTail;
};

class CHE_PtrArray : public CHE_Object
{
public:
	CHE_PtrArray( CHE_Allocator * pAllocator = NULL );
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
	CHE_ByteStringToPtrMap( CHE_Allocator * pAllocator = NULL );
	~CHE_ByteStringToPtrMap();
	
	HE_BOOL		Append( const CHE_ByteString & str, HE_LPVOID ptr );
	
	HE_LPVOID	GetItem( const CHE_ByteString & str ) const;

	HE_LPVOID	GetItemByIndex( HE_DWORD index );
	
	HE_DWORD	GetCount() { return m_lCount; }
	
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
	CHE_NumToPtrMap( CHE_Allocator * pAllocator = NULL );
	~CHE_NumToPtrMap();
	
	HE_BOOL		Append( HE_DWORD num, HE_LPVOID ptr );
	
	HE_LPVOID	GetItem( HE_DWORD num ) const;
	
	HE_LPVOID	GetItemByIndex( HE_DWORD index );

	HE_DWORD	GetNumByIndex( HE_DWORD index );
	
	HE_DWORD	GetCount() { return m_lCount; }
	
	HE_VOID		Clear();
	
private:
	HE_DWORD *		m_pNum;
	HE_LPVOID *		m_pData;
	HE_DWORD	m_lSize;
	HE_DWORD	m_lCount;
};

#endif
