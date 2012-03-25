#include "../../include/pdf/che_pdf_xref.h"
#include <memory.h>

bool operator == ( const CHE_PDF_XREF_Entry & node1, const CHE_PDF_XREF_Entry & node2 )
{
	if ( node1.ObjNum == node2.ObjNum )
	{
		return true;
	}
	return false;
}

bool operator >  ( const CHE_PDF_XREF_Entry & node1, const CHE_PDF_XREF_Entry & node2 )
{
	if ( node1.ObjNum > node2.ObjNum )
	{
		return true;
	}
	return false;
}

bool operator <  ( const CHE_PDF_XREF_Entry & node1, const CHE_PDF_XREF_Entry & node2 )
{
	if ( node1.ObjNum < node2.ObjNum )
	{
		return true;
	}
	return false;
}

CHE_PDF_XREF_Entry::CHE_PDF_XREF_Entry()
{
	Type = XREF_ENTRY_TYPE_FREE;
	ObjNum = 0;
	Field1 = 0;
	Field2 = 0;
}

CHE_PDF_XREF_Entry::CHE_PDF_XREF_Entry( PDF_XREF_ENTRY_TYPE type, HE_DWORD num, HE_DWORD f1, HE_DWORD f2 )
{
	Type = type;
	ObjNum = num;
	Field1 = f1;
	Field2 = f2;
}

CHE_PDF_XREF_Table::CHE_PDF_XREF_Table( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	m_pFirstTrailer = NULL;
	m_pLastTrailer = NULL;
	m_lTrailerCount = 0;
	mMaxObjNum = 0;
}

CHE_PDF_XREF_Table::~CHE_PDF_XREF_Table()
{
	Clear();
}

HE_VOID CHE_PDF_XREF_Table::Clear()
{
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
	mList.Clear();
}

HE_BOOL CHE_PDF_XREF_Table::Add( const CHE_PDF_XREF_Entry & entry )
{
	CHE_PDF_XREF_Entry tmpEntry = entry;
	if ( mList.Append( tmpEntry ) && entry.ObjNum > mMaxObjNum )
	{
		mMaxObjNum = entry.ObjNum;
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_XREF_Table::Get( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet )
{
	entryRet.ObjNum  = objNum;
	return mList.Find( entryRet );
}

HE_BOOL CHE_PDF_XREF_Table::Update( HE_DWORD objNum, const CHE_PDF_XREF_Entry & entry )
{
	CHE_PDF_XREF_Entry tmpEntry = entry;
	tmpEntry.ObjNum = objNum;
	return mList.Update( tmpEntry );
}

HE_VOID CHE_PDF_XREF_Table::AddNewEntry( CHE_PDF_XREF_Entry & entryRet )
{
	entryRet.ObjNum = ++mMaxObjNum;
	entryRet.Type = XREF_ENTRY_TYPE_NEW;
	entryRet.Field1 = 0;
	entryRet.Field2 = 0;
	if ( ! mList.Append( entryRet ) )
	{
		--mMaxObjNum;
	}
}

HE_BOOL CHE_PDF_XREF_Table::AddTrailerDict( CHE_PDF_Dictionary * pDict, HE_BOOL bNeedDestroy /*= FALSE*/ )
{
	if ( pDict == NULL )
	{
		return FALSE;
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

	return TRUE;
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

CHE_PDF_XREF_Data::CHE_PDF_XREF_Data( CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mpFirstSec( NULL ), mpLastSec( NULL ), mlCount( 0 ) {}

CHE_PDF_XREF_Data::~CHE_PDF_XREF_Data()
{
	PDF_XREF_SECTION * pTmpSec = NULL;
	PDF_XREF_ENTRY_NODE * pTmpNode = NULL;
	while ( mpFirstSec )
	{
		pTmpSec = mpFirstSec;
		while ( pTmpSec->pFirstEntry )
		{
			pTmpNode = pTmpSec->pFirstEntry;
			pTmpSec->pFirstEntry = pTmpSec->pFirstEntry->pNext;
			GetAllocator()->Delete( pTmpNode );	
		}
		mpFirstSec = mpFirstSec->pNextSec;
		GetAllocator()->Delete( pTmpSec );
	}
}

HE_VOID	CHE_PDF_XREF_Data::NewSection( HE_DWORD lBegin )
{
	if ( mpFirstSec == NULL )
	{
		mpFirstSec = GetAllocator()->New<PDF_XREF_SECTION>();
		mpFirstSec->lBeginNum = lBegin;
		mpFirstSec->lCount = 0;
		mpFirstSec->pNextSec = NULL;
		mpFirstSec->pFirstEntry = NULL;
		mpFirstSec->pLastEntry = NULL;
		mpLastSec = mpFirstSec;
	}else{
		if ( mpLastSec->lCount == 0 )
		{
			mpLastSec->lBeginNum = lBegin;
		}else{
			PDF_XREF_SECTION * pNewSection = GetAllocator()->New<PDF_XREF_SECTION>();
			pNewSection->lBeginNum = lBegin;
			pNewSection->lCount = 0;
			pNewSection->pNextSec = NULL;
			pNewSection->pFirstEntry = NULL;
			pNewSection->pLastEntry = NULL;
			mpLastSec->pNextSec = pNewSection;
			mpLastSec = pNewSection;
		}
	}
}

HE_VOID CHE_PDF_XREF_Data::NewNode( CHE_PDF_XREF_Entry & entry )
{
	if ( mpLastSec )
	{
		if ( mpLastSec->pFirstEntry == NULL )
		{
			mpLastSec->pFirstEntry = GetAllocator()->New<PDF_XREF_ENTRY_NODE>();
			mpLastSec->pFirstEntry->entry = entry;
			mpLastSec->pFirstEntry->pNext = NULL;
			mpLastSec->pLastEntry = mpLastSec->pFirstEntry;
		}else{
			PDF_XREF_ENTRY_NODE * pNewNode = GetAllocator()->New<PDF_XREF_ENTRY_NODE>();
			pNewNode->entry = entry;
			pNewNode->pNext = NULL;
			mpLastSec->pLastEntry->pNext = pNewNode;
			mpLastSec->pLastEntry = pNewNode;
		}
		++(mpLastSec->lCount);
		++mlCount;
	}
}