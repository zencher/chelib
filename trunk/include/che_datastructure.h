#ifndef _CHE_DATASTRUCTURE_H_
#define _CHE_DATASTRUCTURE_H_

#include "che_base.h"
#include "che_string.h"
#include <vector>


class CHE_DynBuffer : public CHE_Object
{
public:
	CHE_DynBuffer( HE_DWORD capacity = 1024, HE_DWORD increament = 1024, CHE_Allocator * pAllocator = NULL );
	CHE_DynBuffer( const CHE_DynBuffer & buf );
	~CHE_DynBuffer();

	const CHE_DynBuffer & operator = ( const CHE_DynBuffer & buf );

	HE_LPBYTE GetData() { return m_lpData; }

	HE_DWORD GetCapacity() { return m_lCapacity; }

	HE_DWORD GetSize() { return m_lSize; }

	HE_DWORD Write( HE_LPCBYTE pBuffer, HE_DWORD size );

	HE_DWORD Read( HE_LPBYTE pBuffer, HE_DWORD size );

	HE_BOOL ReadByte( HE_DWORD offset, HE_LPBYTE pByte );

	HE_DWORD Write( const CHE_DynBuffer & dynBuffer );

	HE_VOID	Clear() { m_lSize = 0; }

private:

	HE_DWORD	m_lCapacity;
	HE_DWORD	m_lIncreament;

	HE_LPBYTE	m_lpData;
	HE_DWORD	m_lSize;
};

class CHE_DynWideByteBuffer : public CHE_Object
{
public:
	CHE_DynWideByteBuffer( HE_DWORD capacity = 1024, HE_DWORD increament = 1024, CHE_Allocator * pAllocator = NULL );
	CHE_DynWideByteBuffer( const CHE_DynWideByteBuffer & buf );
	~CHE_DynWideByteBuffer();

	const CHE_DynWideByteBuffer & operator = ( const CHE_DynWideByteBuffer & buf );

	HE_LPWSTR GetData() { return m_lpData; }

	HE_DWORD GetCapacity() { return m_lCapacity; }

	HE_DWORD GetSize() { return m_lSize; }

	HE_DWORD Write( HE_LPCWSTR pBuffer, HE_DWORD size );

	HE_DWORD Read( HE_LPWSTR pBuffer, HE_DWORD size );

	HE_BOOL ReadByte( HE_DWORD offset, HE_LPWSTR pWcharRet );

	HE_DWORD Write( const CHE_DynWideByteBuffer & dynBuffer );

	HE_VOID	Clear() { m_lSize = 0; }

private:

	HE_DWORD	m_lCapacity;
	HE_DWORD	m_lIncreament;

	HE_LPWSTR	m_lpData;
	HE_DWORD	m_lSize;
};


template <class Type>
class CHE_Stack : CHE_Object
{
public:
	template <class Type>
	struct Node
	{
		Type data;
		Node * pNext;
	};

	CHE_Stack( CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), m_FirstNode( NULL ) {}

	~CHE_Stack() { Clear(); }
	
	HE_VOID Clear()
	{
		if ( m_FirstNode )
		{
			Node<Type> * pTmp = m_FirstNode;
			while( pTmp )
			{
				m_FirstNode = m_FirstNode->pNext;
				GetAllocator()->Delete( pTmp );
				pTmp = m_FirstNode;
			}
		}
	}

	HE_BOOL IsEmpty() const
	{
		if ( m_FirstNode == NULL )
		{
			return TRUE;
		}
		return FALSE;
	}

	HE_BOOL Pop( Type & valRet )
	{
		if ( m_FirstNode == NULL )
		{
			return FALSE;
		}
		Node<Type> * pTmp = m_FirstNode;
		valRet = pTmp->data;
		m_FirstNode = m_FirstNode->pNext;
		GetAllocator()->Delete<>( pTmp );
		pTmp = NULL;
		return TRUE;
	}

	HE_BOOL Push( const Type & val )
	{
		if ( m_FirstNode == NULL )
		{
			m_FirstNode = GetAllocator()->New< Node<Type> >();
			m_FirstNode->pNext = NULL;
			m_FirstNode->data = val;
		}else{
			Node<Type> * pTmp = GetAllocator()->New< Node<Type> >();
			pTmp->pNext = m_FirstNode;
			pTmp->data = val;
			m_FirstNode = pTmp;
		}
		return TRUE;
	}

private:
	Node<Type> * m_FirstNode;
};


template <class Type>
class CHE_Queue : public CHE_Object
{
public:
	template <class Type>
	struct Node
	{
		Type data;
		Node * pNext;
	};

	CHE_Queue( CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator )
	{
		m_pHead = NULL;
		m_pTail = NULL;
	}

	~CHE_Queue()
	{
		Clear();
	}

	HE_VOID Clear()
	{
		Node<Type> * pNode = m_pHead;
		while ( pNode )
		{
			m_pHead = m_pHead->pNext;
			GetAllocator()->Delete< Node<Type> >( pNode );
			pNode = m_pHead;
		}
		m_pTail = NULL;
	}

	HE_BOOL IsEmpty() const
	{
		if ( m_pHead == NULL && m_pTail == NULL )
		{
			return TRUE;
		}else{
			return FALSE;
		}
	}

	HE_BOOL	Pop( Type & valRet )
	{
		if ( m_pTail == NULL )
		{
			return FALSE;
		}
		valRet = m_pHead->data;

		if ( m_pHead == m_pTail )
		{
			GetAllocator()->Delete< Node<Type> >( m_pHead );
			m_pHead = NULL;
			m_pTail =  NULL;
		}else{
			Node<Type> * pTmp = m_pHead;
			m_pHead = m_pHead->pNext;
			GetAllocator()->Delete< Node<Type> >( pTmp );
		}
		return TRUE;
	}

	HE_BOOL	Push( const Type & val )
	{
		if ( m_pHead == NULL )
		{
			m_pHead = GetAllocator()->New< Node<Type> >();
			m_pHead->data = val;
			m_pHead->pNext = NULL;
			m_pTail = m_pHead;
		}else{
			m_pTail->pNext = GetAllocator()->New< Node<Type> >();
			m_pTail = m_pTail->pNext;
			m_pTail->pNext = NULL;
			m_pTail->data = val;
		}
		return TRUE;
	}

private:
	Node<Type> * m_pHead;
	Node<Type> * m_pTail;
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

	HE_BOOL		UpdateItem( const CHE_ByteString & str, HE_LPVOID * ptr );
	
	HE_LPVOID	GetItem( const CHE_ByteString & str ) const;

	HE_LPVOID	GetItemByIndex( HE_DWORD index ) const;

	HE_BOOL		GetKeyByIndex( HE_DWORD index, CHE_ByteString & strRet ) const;
	
	HE_DWORD	GetCount() const { return m_lCount; }
	
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



template <class Type>
class CHE_SkipList : public CHE_Object
{
public:
	template <class Type>
	struct SkipListNode
	{
		Type lValue;
		std::vector<SkipListNode*> Forward;
	};

	CHE_SkipList( CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), m_lLevel( 0 ), m_lCount( 0 ) {}

	~CHE_SkipList()
	{
		Clear();
	}

	HE_BOOL Append( Type & val )
	{
		if ( m_Forward.size() == 0 )
		{
			SkipListNode<Type> * pNewNode = GetAllocator()->New< SkipListNode<Type> >();
			pNewNode->lValue = val;
			m_Forward.push_back( pNewNode );
			m_lCount++;
			return TRUE;
		}else{
			std::vector<SkipListNode<Type>*> nodeVector;
			if ( FindImp( val, nodeVector ) == FALSE )
			{
				if ( nodeVector.size() == 1 )
				{
					SkipListNode<Type> * pNewNode = GetAllocator()->New< SkipListNode<Type> >();
					pNewNode->lValue = val;
					m_Forward.push_back( pNewNode );
					m_lCount++;
					return TRUE;
				}else{
					HE_DWORD targetLevel = RandomLevel();
					if ( targetLevel > m_lLevel )
					{
						m_lLevel = targetLevel;
					}
					SkipListNode<Type> * pNewNode = GetAllocator()->New< SkipListNode<Type> >();
					pNewNode->lValue = val;
					for ( size_t i = 0, index = nodeVector.size() - 1; i <= targetLevel; i++, index-- )
					{
						if ( nodeVector[index] == NULL )
						{
							if ( m_Forward.size() > i )
							{
								pNewNode->Forward.push_back( m_Forward[i] );
								m_Forward[i] = pNewNode;
							}else{
								m_Forward.push_back( pNewNode );
							}
						}else if ( nodeVector[index]->Forward.size() > i )
						{
							pNewNode->Forward.push_back( nodeVector[index]->Forward[i] );
							nodeVector[index]->Forward[i] = pNewNode;
						}else{
							nodeVector[index]->Forward.push_back( pNewNode );
						}
					}
					m_lCount++;
					return TRUE;
				}
			}else{
				return FALSE;
			}
		}
		return FALSE;
	}

	HE_DWORD GetCount() const
	{
		return m_lCount;
	}

	HE_BOOL IsExist( const Type & val ) const
	{
		if ( m_Forward.size() == 0 )
		{
			return FALSE;
		}
		HE_DWORD curLevel = m_lLevel;
		SkipListNode<Type>* pTmpNode = m_Forward[m_Forward.size()-1];
		SkipListNode<Type>* pPreNode = NULL;

		while( pTmpNode )
		{
			if ( val == pTmpNode->lValue )
			{
				return TRUE;
			}else if ( val < pTmpNode->lValue )
			{
				//to lower level
				if ( curLevel == 0 )
				{
					return FALSE;
				}else{
					if ( pPreNode == NULL )
					{
						pTmpNode = m_Forward[--curLevel];
					}else{
						pTmpNode = pPreNode->Forward[--curLevel];
					}
				}
			}else if ( val > pTmpNode->lValue )
			{
				if ( pTmpNode->Forward.size() > curLevel )
				{
					//to next node
					pPreNode = pTmpNode;
					pTmpNode = pTmpNode->Forward[curLevel];
				}else if ( pTmpNode->Forward.size() == 0 )
				{
					return FALSE;
				}else{
					--curLevel;
				}
			}
		}
		return FALSE;
	}

	HE_VOID	Clear()
	{
		if ( m_Forward.size() == 0 )
		{
			return;
		}
		m_lLevel = 0;
		m_lCount = 0;
		SkipListNode<Type>* pTmpNode = m_Forward[0];
		SkipListNode<Type>* pTmpNode2 = NULL;
		m_Forward.clear();
		while( true )
		{
			if ( pTmpNode->Forward.size() > 0 )
			{
				pTmpNode2 = pTmpNode->Forward[0];
				pTmpNode->Forward.clear();
				pTmpNode = pTmpNode2;
			}else{
				break;
			}
		}
	}

	HE_BOOL Find( Type & valRet )
	{
		std::vector<SkipListNode<Type>*> nodeVector;
		return FindImp( valRet, nodeVector );
	}

private:
	HE_BOOL FindImp( Type & val, std::vector< SkipListNode<Type>* > & nodeVector ) const
	{
		if ( m_Forward.size() == 0 )
		{
			return FALSE;
		}
		HE_DWORD curLevel = m_lLevel;
		SkipListNode<Type>* pTmpNode = m_Forward[m_Forward.size()-1];
		SkipListNode<Type>* pPreNode = NULL;

		nodeVector.push_back( NULL );

		while( pTmpNode )
		{
			if ( val == pTmpNode->lValue )
			{
				val = pTmpNode->lValue;
				return TRUE;
			}else if ( val < pTmpNode->lValue )
			{
				//to lower level
				if ( curLevel == 0 )
				{
					nodeVector.push_back( pPreNode );
					return FALSE;
				}else{
					if ( pPreNode == NULL )
					{
						nodeVector.push_back( NULL );
						pTmpNode = m_Forward[--curLevel];
					}else{
						nodeVector.push_back( pPreNode );
						pTmpNode = pPreNode->Forward[--curLevel];
					}
				}
			}else if ( val > pTmpNode->lValue )
			{
				if ( pTmpNode->Forward.size() > curLevel )
				{
					//to next node
					pPreNode = pTmpNode;
					pTmpNode = pTmpNode->Forward[curLevel];
				}else if ( pTmpNode->Forward.size() == 0 )
				{
					for ( HE_DWORD i = curLevel+1; i > 0; i-- )
					{
						nodeVector.push_back( pTmpNode );
					}
					return FALSE;
				}else{
					for ( HE_DWORD i = curLevel+1; i > pTmpNode->Forward.size(); i-- )
					{
						nodeVector.push_back( pTmpNode );
						--curLevel;
					}
				}
			}
		}
		return FALSE;
	}

	HE_DWORD RandomLevel() const
	{
		static HE_DWORD flag = 0;
		HE_DWORD levelRet = 0;
		levelRet = rand()%(m_lLevel+1);
		if ( flag == 500 )
		{
			flag = 0;
			return m_lLevel+1;
		}else{
			flag++;
		}
		return levelRet;
	}

	HE_DWORD m_lLevel;
	HE_DWORD m_lCount;
	std::vector<SkipListNode<Type>*> m_Forward;
};



// template <class Type>
// class CHE_DictionaryTree : public CHE_Object
// {
// public:
// 	template <class Type>
// 	struct TreeNode
// 	{
// 		bool bVal;
// 		char chr;
// 		Type val;
// 		TreeNode * pNext;
// 		TreeNode * pChild;
// 	};
// 
// 	CHE_DictionaryTree( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ), m_pChild( NULL  ) {}
// 
// 
// private:
// 
// 	TreeNode<Type> * m_pChild;
// };



#endif