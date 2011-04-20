#include "../../include/pdf/che_pdf_xref.h"
#include <memory.h>

CHE_PDF_XREF_Entry::CHE_PDF_XREF_Entry()
{
	field1 = 0;
	field2 = 0;
	field3 = 0;
	objNum = 0;
}

CHE_PDF_XREF_Entry::CHE_PDF_XREF_Entry( HE_DWORD f1, HE_DWORD f2, HE_DWORD f3, HE_DWORD obj )
{
	field1 = f1;
	field2 = f2;
	field3 = f3;
	objNum = obj;
}

CHE_PDF_XREF_Table::CHE_PDF_XREF_Table()
{
	m_pFirstSection = NULL;
	m_pCurSection = NULL;
	m_lCount = 0;
	m_lMaxObjNum = 0;
	m_pFastAccessArr = NULL;
}

CHE_PDF_XREF_Table::~CHE_PDF_XREF_Table()
{
	if ( m_pFirstSection )
	{
		Clear();
	}
}

HE_VOID CHE_PDF_XREF_Table::Clear()
{
	PDF_XREF_SECTION * pTmpSection = m_pFirstSection;
	PDF_XREF_ENTRY_NODE * pTmpNode = NULL;
	while ( pTmpSection )
	{
		pTmpNode = pTmpSection->pFirstEntry;
		while ( pTmpNode )
		{
			pTmpSection->pFirstEntry = pTmpSection->pFirstEntry->pNext;
			delete pTmpNode;
			pTmpNode = pTmpSection->pFirstEntry;
		}
		m_pFirstSection = m_pFirstSection->pNextSection;
		delete pTmpSection;
		pTmpSection = m_pFirstSection;
	}

	if ( m_pFastAccessArr )
	{
		delete [] m_pFastAccessArr;
		m_pFastAccessArr = NULL;
	}
}

HE_VOID	CHE_PDF_XREF_Table::NewSection( HE_DWORD lBegin )
{
	if ( m_pFirstSection == NULL )
	{
		m_pFirstSection = new PDF_XREF_SECTION;
		m_pFirstSection->lBeginNum = lBegin;
		m_pFirstSection->lCount = 0;
		m_pFirstSection->pFirstEntry = NULL;
		m_pFirstSection->pLastEntry = NULL;
		m_pFirstSection->pNextSection = NULL;
		m_pFirstSection->pPreSection = NULL;
		m_pCurSection = m_pFirstSection;
	}else{
		m_pCurSection->pNextSection = new PDF_XREF_SECTION;
		m_pCurSection->pNextSection->pPreSection = m_pCurSection;
		m_pCurSection = m_pCurSection->pNextSection;
		m_pCurSection->lBeginNum = lBegin;
		m_pCurSection->lCount = 0;
		m_pCurSection->pFirstEntry = NULL;
		m_pCurSection->pLastEntry = NULL;
		m_pCurSection->pNextSection = NULL;
	}
}

HE_VOID CHE_PDF_XREF_Table::NewNode( CHE_PDF_XREF_Entry & entry )
{
	if ( m_pCurSection == NULL ) return;
	if ( m_pCurSection->pFirstEntry == NULL )
	{
		m_pCurSection->pFirstEntry = new PDF_XREF_ENTRY_NODE;
		m_pCurSection->pFirstEntry->entry = entry;
		m_pCurSection->pFirstEntry->pNext = NULL;
		m_pCurSection->pFirstEntry->pPrv = NULL;
		m_pCurSection->pLastEntry = m_pCurSection->pFirstEntry;
	}else{
		m_pCurSection->pLastEntry->pNext = new PDF_XREF_ENTRY_NODE;
		m_pCurSection->pLastEntry->pNext->pPrv = m_pCurSection->pLastEntry;
		m_pCurSection->pLastEntry = m_pCurSection->pLastEntry->pNext;
		m_pCurSection->pLastEntry->pNext = NULL;
		m_pCurSection->pLastEntry->entry = entry;
	}
	m_pCurSection->lCount++;
	m_lCount++;
	if ( m_pCurSection->lCount + m_pCurSection->lBeginNum > m_lMaxObjNum )
	{
		m_lMaxObjNum = m_pCurSection->lCount + m_pCurSection->lBeginNum;
	}
}

HE_VOID CHE_PDF_XREF_Table::BuildIndex()
{
	if ( m_pFastAccessArr )
	{
		delete [] m_pFastAccessArr;
		m_pFastAccessArr = NULL;
	}
	if ( m_lMaxObjNum == 0 )
	{
		return;
	}
	m_pFastAccessArr = new PDF_XREF_ENTRY_NODE*[m_lMaxObjNum+1];
	memset( m_pFastAccessArr, 0, sizeof(CHE_PDF_XREF_Entry*)*(m_lMaxObjNum+1) );

	PDF_XREF_SECTION * pTmpSection = m_pCurSection;
	while ( pTmpSection )
	{
		PDF_XREF_ENTRY_NODE * pTmpEntry = pTmpSection->pFirstEntry;
		HE_DWORD lIndex = pTmpSection->lBeginNum;
		while( pTmpEntry )
		{
			pTmpEntry->entry.objNum = lIndex;
			m_pFastAccessArr[lIndex++] = pTmpEntry;
			pTmpEntry = pTmpEntry->pNext;
		}
		pTmpSection = pTmpSection->pPreSection;
	}
}

HE_BOOL CHE_PDF_XREF_Table::GetEntry( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet )
{
	if ( m_pFastAccessArr == NULL )
	{
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
