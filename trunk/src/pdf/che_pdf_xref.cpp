#include "../../include/pdf/che_pdf_xref.h"
#include <memory.h>

CHE_PDF_XREF_Entry::CHE_PDF_XREF_Entry()
{
	Type = XREF_ENTRY_TYPE_FREE;
	Field1 = 0;
	Field2 = 0;
}

CHE_PDF_XREF_Entry::CHE_PDF_XREF_Entry( PDF_XREF_ENTRY_TYPE type, HE_DWORD f1, HE_DWORD f2 )
{
	Type = type;
	Field1 = f1;
	Field2 = f2;
}

CHE_PDF_XREF_Table::CHE_PDF_XREF_Table( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	m_pFirstSecPart = NULL;
	m_pLastSecPart = NULL;
	m_pFastAccessArr = NULL;
	//m_pFastAccess = NULL;
	m_lNextSecNum = 0;
	m_lNextObjNum = 0;
	m_lCount = 0;
	m_lMaxObjNum = 0;
	m_bSkiped = FALSE;
}

CHE_PDF_XREF_Table::~CHE_PDF_XREF_Table()
{
	if ( m_pFirstSecPart )
	{
		Clear();
	}
}

HE_VOID CHE_PDF_XREF_Table::Clear()
{
	PDF_XREF_SECTION_PART * pTmpSecPart = m_pFirstSecPart;
	PDF_XREF_ENTRY_NODE * pTmpNode = NULL;
	while ( pTmpSecPart )
	{
		pTmpNode = pTmpSecPart->pFirstEntry;
		while ( pTmpNode )
		{
			pTmpSecPart->pFirstEntry = pTmpSecPart->pFirstEntry->pNext;
			GetAllocator()->Delete<PDF_XREF_ENTRY_NODE>( pTmpNode );
			pTmpNode = pTmpSecPart->pFirstEntry;
		}
		m_pFirstSecPart = m_pFirstSecPart->pNextSecPart;
		GetAllocator()->Delete<PDF_XREF_SECTION_PART>( pTmpSecPart );
		pTmpSecPart = m_pFirstSecPart;
	}

	if ( m_pFastAccessArr )
	{
		GetAllocator()->DeleteArray<PDF_XREF_ENTRY_NODE *>( m_pFastAccessArr );
		m_pFastAccessArr = NULL;
	}
// 	while ( m_pFastAccess )
// 	{
// 		PDF_XREF_FASTACCESS_NODE* pTmp = m_pFastAccess;
// 		if ( pTmp->pAccessArr )
// 		{
// 			delete [] pTmp->pAccessArr;
// 		}
// 		delete pTmp;
// 		m_pFastAccess = m_pFastAccess->pNext;
// 	}
}

HE_VOID	CHE_PDF_XREF_Table::NewSection( HE_DWORD lBegin )
{
	if ( m_pFirstSecPart == NULL )
	{
		m_pFirstSecPart = GetAllocator()->New<PDF_XREF_SECTION_PART>();
		m_pFirstSecPart->pFirstEntry = NULL;
		m_pFirstSecPart->pLastEntry = NULL;
		m_pFirstSecPart->pNextSecPart = NULL;
		m_pLastSecPart = m_pFirstSecPart; 
	}else{
		m_pLastSecPart->pNextSecPart = GetAllocator()->New<PDF_XREF_SECTION_PART>();
		m_pLastSecPart = m_pLastSecPart->pNextSecPart;
		m_pLastSecPart->pFirstEntry = NULL;
		m_pLastSecPart->pLastEntry = NULL;
		m_pLastSecPart->pNextSecPart = NULL;
	}
	m_pLastSecPart->lBeginNum = lBegin;
	m_pLastSecPart->lCount = 0;
	m_pLastSecPart->lSectionIndex = m_lNextSecNum;
	m_lNextSecNum++;
	m_lNextObjNum = lBegin;
	m_bSkiped = FALSE;
}

HE_VOID CHE_PDF_XREF_Table::NewNode( CHE_PDF_XREF_Entry & entry )
{
	if ( m_pLastSecPart == NULL ) return;
	if ( m_bSkiped == TRUE )
	{
		m_pLastSecPart->pNextSecPart = GetAllocator()->New<PDF_XREF_SECTION_PART>();
		m_pLastSecPart = m_pLastSecPart->pNextSecPart;
		m_pLastSecPart->pFirstEntry = NULL;
		m_pLastSecPart->pLastEntry = NULL;
		m_pLastSecPart->pNextSecPart = NULL;
		m_pLastSecPart->lBeginNum = m_lNextObjNum;
		m_pLastSecPart->lCount = 0;
		m_pLastSecPart->lSectionIndex = m_lNextSecNum;
		m_bSkiped = FALSE;
	}
	if ( m_pLastSecPart->pFirstEntry == NULL )
	{
		m_pLastSecPart->pFirstEntry = GetAllocator()->New<PDF_XREF_ENTRY_NODE>();
		m_pLastSecPart->pFirstEntry->entry = entry;
		m_pLastSecPart->pFirstEntry->pNext = NULL;
		m_pLastSecPart->pLastEntry = m_pLastSecPart->pFirstEntry;
	}else{
		m_pLastSecPart->pLastEntry->pNext = GetAllocator()->New<PDF_XREF_ENTRY_NODE>();
		m_pLastSecPart->pLastEntry = m_pLastSecPart->pLastEntry->pNext;
		m_pLastSecPart->pLastEntry->pNext = NULL;
		m_pLastSecPart->pLastEntry->entry = entry;
	}
	m_pLastSecPart->lCount++;
	m_lCount++;
	m_lNextObjNum++;
	if ( m_pLastSecPart->lCount + m_pLastSecPart->lBeginNum - 1 > m_lMaxObjNum )
	{
		m_lMaxObjNum = m_pLastSecPart->lCount + m_pLastSecPart->lBeginNum - 1;
	}
}

HE_VOID CHE_PDF_XREF_Table::SkipNode()
{
	m_bSkiped = TRUE;
	m_lNextObjNum++;
}

HE_VOID CHE_PDF_XREF_Table::Update( HE_DWORD objNum, CHE_PDF_XREF_Entry & entry )
{
	if ( m_pFastAccessArr == NULL )
	{
		PDF_XREF_SECTION_PART * pTmpSecPart = m_pFirstSecPart;
		while ( pTmpSecPart && pTmpSecPart->lCount != 0 )
		{
			HE_DWORD lBegin = pTmpSecPart->lBeginNum;
			HE_DWORD lCount = pTmpSecPart->lCount;
			if ( objNum >= lBegin && objNum < lBegin + lCount )
			{
				PDF_XREF_ENTRY_NODE * pTmpEntry = pTmpSecPart->pFirstEntry;
				for ( HE_DWORD index = lBegin; index < objNum - lBegin; index++ )
				{
					pTmpEntry = pTmpEntry->pNext;
				}
				if ( pTmpEntry )
				{
					pTmpEntry->entry = entry;
					return;
				}
			}
			pTmpSecPart = pTmpSecPart->pNextSecPart;
		}
		return;
	}
	if ( objNum <= m_lMaxObjNum  )
	{
		if ( m_pFastAccessArr[objNum] != NULL )
		{
			m_pFastAccessArr[objNum]->entry = entry;
		}
	}
}

HE_VOID CHE_PDF_XREF_Table::BuildIndex()
{
	if ( m_pFastAccessArr )
	{
		GetAllocator()->DeleteArray<PDF_XREF_ENTRY_NODE *>( m_pFastAccessArr );
		m_pFastAccessArr = NULL;
	}
	if ( m_lMaxObjNum == 0 )
	{
		return;
	}
	m_pFastAccessArr = GetAllocator()->NewArray<PDF_XREF_ENTRY_NODE*>( m_lMaxObjNum+1 );
	memset( m_pFastAccessArr, 0, sizeof(CHE_PDF_XREF_Entry*)*(m_lMaxObjNum+1) );

	PDF_XREF_SECTION_PART * pTmpSecPart = m_pFirstSecPart;
	PDF_XREF_ENTRY_NODE * pTmpEntry = NULL;
	HE_DWORD lIndex = 0;
	while ( pTmpSecPart )
	{
		pTmpEntry = pTmpSecPart->pFirstEntry;
		lIndex = pTmpSecPart->lBeginNum;
		while( pTmpEntry )
		{
			if ( m_pFastAccessArr[lIndex] == NULL )
			{
				m_pFastAccessArr[lIndex] = pTmpEntry;
			}
			lIndex++;
			pTmpEntry = pTmpEntry->pNext;
		}
		pTmpSecPart = pTmpSecPart->pNextSecPart;
	}
}

HE_BOOL CHE_PDF_XREF_Table::GetEntry( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet )
{
	if ( m_pFastAccessArr == NULL )
	{
		PDF_XREF_SECTION_PART * pTmpSecPart = m_pFirstSecPart;
		while ( pTmpSecPart && pTmpSecPart->lCount != 0 )
		{
			HE_DWORD lBegin = pTmpSecPart->lBeginNum;
			HE_DWORD lCount = pTmpSecPart->lCount;
			if ( objNum >= lBegin && objNum < lBegin + lCount )
			{
				PDF_XREF_ENTRY_NODE * pTmpEntry = pTmpSecPart->pFirstEntry;
				for ( HE_DWORD index = lBegin; index < objNum; index++ )
				{
					pTmpEntry = pTmpEntry->pNext;
				}
				if ( pTmpEntry )
				{
					entryRet = pTmpEntry->entry;
					return TRUE;
				}
			}
			pTmpSecPart = pTmpSecPart->pNextSecPart;
		}
		return FALSE;
	}
	if ( objNum > m_lMaxObjNum  )
	{
		return FALSE;
	}
	if ( m_pFastAccessArr[objNum] != NULL )
	{
		entryRet = m_pFastAccessArr[objNum]->entry;
		return TRUE;
	}else{
		return FALSE;
	}
	return TRUE;
}

HE_BOOL CHE_PDF_XREF_Table::IsExist( HE_DWORD objNum )
{
	if ( m_pFastAccessArr == NULL )
	{
		return FALSE;
	}
	if ( objNum > m_lMaxObjNum )
	{
		return FALSE;
	}
	if ( m_pFastAccessArr[m_lMaxObjNum] == NULL )
	{
		return FALSE;
	}
	return TRUE;
}