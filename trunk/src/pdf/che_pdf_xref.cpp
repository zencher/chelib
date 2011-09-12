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
	m_pFirstFastAccess = NULL;
	m_pLastFastAccess = NULL;
	m_pFirstTrailer = NULL;
	m_pLastTrailer = NULL;
	m_lTrailerCount = 0;
	m_lNextSecNum = 0;
	m_lNextObjNum = 0;
	m_lCount = 0;
	m_lMaxObjNum = 0;
	m_lSkipCount = 0;
	m_bSkiped = FALSE;
	m_bIndexed = FALSE;
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
	PDF_XREF_SECTION_PART * pTmpSecPart = NULL;
	PDF_XREF_ENTRY_NODE * pTmpNode = NULL;
	while ( m_pFirstSecPart )
	{
		pTmpSecPart = m_pFirstSecPart;
		pTmpNode = m_pFirstSecPart->pFirstEntry;
		while ( pTmpNode )
		{
			m_pFirstSecPart->pFirstEntry = m_pFirstSecPart->pFirstEntry->pNext;
			GetAllocator()->Delete<PDF_XREF_ENTRY_NODE>( pTmpNode );
			pTmpNode = m_pFirstSecPart->pFirstEntry;
		}
		m_pFirstSecPart = m_pFirstSecPart->pNextSecPart;
		GetAllocator()->Delete<PDF_XREF_SECTION_PART>( pTmpSecPart );
	}

	PDF_XREF_FASTACCESS_NODE* pTmpAccessNode = NULL;
	while ( m_pFirstFastAccess )
	{
		pTmpAccessNode = m_pFirstFastAccess;
		if ( pTmpAccessNode->pAccessArr )
		{
			GetAllocator()->DeleteArray( pTmpAccessNode->pAccessArr );
		}
		m_pFirstFastAccess = m_pFirstFastAccess->pNext;
		GetAllocator()->Delete( pTmpAccessNode );
	}

	PDF_XREF_TRAILER_NODE * pTmpTrailer = NULL;
	while( m_pFirstTrailer )
	{
		pTmpTrailer = m_pFirstTrailer;
		if ( pTmpTrailer && pTmpTrailer->bNeedDestroy == TRUE )
		{
			GetAllocator()->DeleteArray( pTmpTrailer->pDict );
		}
		m_pFirstTrailer = m_pFirstTrailer->pNext;
		GetAllocator()->DeleteArray( pTmpTrailer );
	}
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
	m_bIndexed = FALSE;

	if ( m_pFirstFastAccess == NULL )
	{
		m_pFirstFastAccess = GetAllocator()->New<PDF_XREF_FASTACCESS_NODE>();
		m_pLastFastAccess = m_pFirstFastAccess;
		m_pLastFastAccess->lBeginNum = m_pLastSecPart->lBeginNum;
		m_pLastFastAccess->lSectionIndex = m_pLastSecPart->lSectionIndex;
		m_pLastFastAccess->lSize = 0;
		m_pLastFastAccess->pAccessArr = NULL;
		m_pLastFastAccess->pFirstSecPart = m_pLastSecPart;
		m_pLastFastAccess->pNext = NULL;
	}else{
		m_pLastFastAccess->pNext = GetAllocator()->New<PDF_XREF_FASTACCESS_NODE>();
		m_pLastFastAccess = m_pLastFastAccess->pNext;
		m_pLastFastAccess->lBeginNum = m_pLastSecPart->lBeginNum;
		m_pLastFastAccess->lSectionIndex = m_pLastSecPart->lSectionIndex;
		m_pLastFastAccess->lSize = 0;
		m_pLastFastAccess->pAccessArr = NULL;
		m_pLastFastAccess->pFirstSecPart = m_pLastSecPart;
		m_pLastFastAccess->pNext = NULL;
	}
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
		m_pLastSecPart->lSectionIndex = m_lNextSecNum-1;
		m_pLastFastAccess->lSize += m_lSkipCount;
		m_bSkiped = FALSE;
		m_lSkipCount = 0;
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
	m_pLastFastAccess->lSize++;
	if ( m_pLastSecPart->lCount + m_pLastSecPart->lBeginNum - 1 > m_lMaxObjNum )
	{
		m_lMaxObjNum = m_pLastSecPart->lCount + m_pLastSecPart->lBeginNum - 1;
	}
}

HE_VOID CHE_PDF_XREF_Table::SkipNode()
{
	m_bSkiped = TRUE;
	m_lNextObjNum++;
	m_lSkipCount++;
}

HE_VOID CHE_PDF_XREF_Table::UpdateNode( HE_DWORD objNum, CHE_PDF_XREF_Entry & entry )
{
	if ( m_bIndexed == FALSE )
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
	}else{
		PDF_XREF_FASTACCESS_NODE * pTmp = m_pFirstFastAccess;
		while( pTmp )
		{
			if ( pTmp->lBeginNum <= objNum && pTmp->lSize != 0 && objNum < pTmp->lBeginNum + pTmp->lSize )
			{
				if ( pTmp->pAccessArr[objNum-pTmp->lBeginNum] != NULL )
				{
					pTmp->pAccessArr[objNum-pTmp->lBeginNum]->entry = entry;
				}
				return;
			}
			pTmp = pTmp->pNext;
		}
	}
}

HE_VOID CHE_PDF_XREF_Table::BuildIndex()
{
	if ( m_lMaxObjNum == 0 )
	{
		return;
	}

	PDF_XREF_FASTACCESS_NODE * pTmpAccess = m_pFirstFastAccess;
	while( pTmpAccess )
	{
		if ( pTmpAccess->pAccessArr )
		{
			GetAllocator()->DeleteArray( pTmpAccess->pAccessArr );
		}
		pTmpAccess->pAccessArr = GetAllocator()->NewArray<PDF_XREF_ENTRY_NODE*>( pTmpAccess->lSize );
		memset( pTmpAccess->pAccessArr, 0, sizeof(PDF_XREF_FASTACCESS_NODE*)*pTmpAccess->lSize );
		pTmpAccess = pTmpAccess->pNext;
	}

	PDF_XREF_SECTION_PART * pTmpSecPart = m_pFirstSecPart;
	PDF_XREF_ENTRY_NODE * pTmpEntry = NULL;
	pTmpAccess = m_pFirstFastAccess;
	HE_DWORD lIndex = 0;
	while ( pTmpSecPart )
	{
		while ( pTmpSecPart->lSectionIndex != pTmpAccess->lSectionIndex  )
		{
			pTmpAccess = pTmpAccess->pNext;
		}
		pTmpEntry = pTmpSecPart->pFirstEntry;
		lIndex = pTmpSecPart->lBeginNum;
		while( pTmpEntry )
		{
			pTmpAccess->pAccessArr[lIndex-pTmpAccess->lBeginNum] = pTmpEntry;
			lIndex++;
			pTmpEntry = pTmpEntry->pNext;
		}
		pTmpSecPart = pTmpSecPart->pNextSecPart;
	}
	m_bIndexed = TRUE;
}

HE_BOOL CHE_PDF_XREF_Table::GetEntry( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet )
{
	if ( objNum > m_lMaxObjNum  )
	{
		return FALSE;
	}
	if ( m_bIndexed == FALSE )
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
	}else{
		PDF_XREF_FASTACCESS_NODE * pTmp = m_pFirstFastAccess;
		while( pTmp )
		{
			if ( pTmp->lBeginNum <= objNum && objNum < pTmp->lBeginNum + pTmp->lSize )
			{
				if ( pTmp->pAccessArr[objNum-pTmp->lBeginNum] != NULL )
				{
					entryRet = pTmp->pAccessArr[objNum-pTmp->lBeginNum]->entry;
					return TRUE;
				}else{
					return FALSE;
				}
			}
			pTmp = pTmp->pNext;
		}
	}
	return FALSE;
}

HE_BOOL CHE_PDF_XREF_Table::IsExist( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet )
{
	if ( objNum > m_lMaxObjNum )
	{
		return FALSE;
	}
	if ( m_bIndexed == FALSE )
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
	}else{
		PDF_XREF_FASTACCESS_NODE * pTmp = m_pFirstFastAccess;
		while( pTmp )
		{
			if ( pTmp->lBeginNum <= objNum && objNum < pTmp->lBeginNum + pTmp->lSize )
			{
				if ( pTmp->pAccessArr[objNum-pTmp->lBeginNum] != NULL )
				{
					entryRet = pTmp->pAccessArr[objNum-pTmp->lBeginNum]->entry;
					return TRUE;
				}else{
					return FALSE;
				}
			}
			pTmp = pTmp->pNext;
		}
	}
	return FALSE;
}

HE_BOOL CHE_PDF_XREF_Table::IsExist( HE_DWORD objNum )
{
	if ( objNum > m_lMaxObjNum )
	{
		return FALSE;
	}
	if ( m_bIndexed == FALSE )
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
					return TRUE;
				}
			}
			pTmpSecPart = pTmpSecPart->pNextSecPart;
		}
	}else{
		PDF_XREF_FASTACCESS_NODE * pTmp = m_pFirstFastAccess;
		while( pTmp )
		{
			if ( pTmp->lBeginNum <= objNum && objNum < pTmp->lBeginNum + pTmp->lSize )
			{
				if ( pTmp->pAccessArr[objNum-pTmp->lBeginNum] != NULL )
				{
					return TRUE;
				}else{
					return FALSE;
				}
			}
		}
	}
	return FALSE;
}


HE_VOID CHE_PDF_XREF_Table::NewTrailer( CHE_PDF_Dictionary * pDict, HE_BOOL bNeedDestroy /*= FALSE*/ )
{
	if ( pDict == NULL )
	{
		return;
	}
	if ( m_pFirstTrailer == NULL )
	{
		m_pFirstTrailer = GetAllocator()->New<PDF_XREF_TRAILER_NODE>();
		m_pLastTrailer = m_pFirstTrailer;
	}else{
		m_pLastTrailer->pNext = GetAllocator()->New<PDF_XREF_TRAILER_NODE>();
		m_pLastTrailer = m_pLastTrailer->pNext;
	}
	m_pLastTrailer->bNeedDestroy = bNeedDestroy;
	m_pLastTrailer->pDict = pDict;
	m_pLastTrailer->pNext = NULL;
	m_lTrailerCount++;
}

CHE_PDF_Dictionary * CHE_PDF_XREF_Table::GetTrailer( HE_DWORD index /*= 0*/ ) const
{
	if ( index >= m_lTrailerCount )
	{
		return NULL;
	}
	PDF_XREF_TRAILER_NODE * pTmp = m_pFirstTrailer;
	for ( HE_DWORD i = index; i > 0 &&  pTmp ; i-- )
	{
		pTmp = pTmp->pNext;
	}
	return pTmp->pDict;
}