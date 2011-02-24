#include "../../include/pdf/che_pdf_xref.h"

#define NULL 0

CHE_PDF_XREF_Entry::CHE_PDF_XREF_Entry()
{
	m_iByteOffset = 0;
	m_iObjNum = 0;
	m_iGenNum = 0;
	m_byteFlag = 0;
}

CHE_PDF_XREF_Entry::CHE_PDF_XREF_Entry( unsigned int offset, unsigned int objNum, unsigned int genNum, unsigned char flag )
{
	m_iByteOffset = offset;
	m_iObjNum = objNum;
	m_iGenNum = genNum;
	m_byteFlag = flag;
}

CHE_PDF_XREF_Table::CHE_PDF_XREF_Table()
{
	m_lCount = 0;
	m_pFirstEntry = NULL;
	m_pLastEntry = NULL;
}

// CHE_PDF_XREF_Table::CHE_PDF_XREF_Table( const CHE_PDF_XREF_Table & table )
// {
// 	
// }

CHE_PDF_XREF_Table::~CHE_PDF_XREF_Table()
{
	m_lCount = 0;
	m_pFirstEntry = NULL;
	m_pLastEntry = NULL;
}

// const CHE_PDF_XREF_Table & CHE_PDF_XREF_Table::operator = ( const CHE_PDF_XREF_Table & table )
// {
// 
// }

void CHE_PDF_XREF_Table::Clear()
{
	PDF_XREF_ENTRY_NODE * pTmp = m_pFirstEntry;
	while ( pTmp )
	{
		m_pFirstEntry = m_pFirstEntry->pNext;
		delete pTmp;
		pTmp = m_pFirstEntry;
	}
	m_lCount = 0;
}

// void CHE_PDF_XREF_Table::Clone( const CHE_PDF_XREF_Table & table )
// {
// 	if ( this == &table )
// 	{
// 		return;
// 	}
// 	Clear();
// 
// }

bool CHE_PDF_XREF_Table::Append( unsigned int offset, unsigned int objNum, unsigned int genNum, unsigned char flag )
{
	if ( m_pFirstEntry == NULL )
	{
		m_pFirstEntry = new PDF_XREF_ENTRY_NODE;
		m_pFirstEntry->pNext = NULL;
		m_pLastEntry = m_pFirstEntry;
		m_pFirstEntry->entry.SetOffset( offset );
		m_pFirstEntry->entry.SetObjNum( objNum );
		m_pFirstEntry->entry.SetGenNum( genNum );
		m_pFirstEntry->entry.SetFlag( flag );
	}else{
		m_pLastEntry->pNext = new PDF_XREF_ENTRY_NODE;
		m_pLastEntry->pNext->pNext = NULL;
		m_pLastEntry = m_pLastEntry->pNext;
		m_pLastEntry->entry.SetOffset( offset );
		m_pLastEntry->entry.SetObjNum( objNum );
		m_pLastEntry->entry.SetGenNum( genNum );
		m_pLastEntry->entry.SetFlag( flag );
	}
	m_lCount++;
	return true;
}

bool CHE_PDF_XREF_Table::Append( CHE_PDF_XREF_Entry & entry )
{
	if ( m_pFirstEntry == NULL )
	{
		m_pFirstEntry = new PDF_XREF_ENTRY_NODE;
		m_pFirstEntry->pNext = NULL;
		m_pLastEntry = m_pFirstEntry;
		m_pFirstEntry->entry = entry;
	}else{
		m_pLastEntry->pNext = new PDF_XREF_ENTRY_NODE;
		m_pLastEntry->pNext->pNext = NULL;
		m_pLastEntry = m_pLastEntry->pNext;
		m_pLastEntry->entry = entry;
	}
	m_lCount++;
	return true;
}

bool CHE_PDF_XREF_Table::Update( unsigned int offset, unsigned int objNum, unsigned int genNum, unsigned char flag )
{
	return false;
}

bool CHE_PDF_XREF_Table::Update( CHE_PDF_XREF_Entry & entry )
{
	return false;
}

bool CHE_PDF_XREF_Table::Remove( unsigned int objNum )
{
	return false;
}

bool CHE_PDF_XREF_Table::GetEntry( unsigned int objNum, CHE_PDF_XREF_Entry & entryRet )
{
	PDF_XREF_ENTRY_NODE * pTmp = m_pFirstEntry;
	while ( pTmp )
	{
		if ( objNum == pTmp->entry.GetObjNum() )
		{
			entryRet = pTmp->entry;
			return true;
		}
		pTmp = m_pFirstEntry->pNext;
	}
	return false;
}

bool CHE_PDF_XREF_Table::IsExist( unsigned int objNum )
{
	PDF_XREF_ENTRY_NODE * pTmp = m_pFirstEntry;
	while ( pTmp )
	{
		if ( objNum == pTmp->entry.GetObjNum() )
		{
			return true;
		}
		pTmp = m_pFirstEntry->pNext;
	}
	return false;
}
