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

CHE_PDF_XREF_Table::CHE_PDF_XREF_Table( CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mMaxObjNum( 0 ), mList( pAllocator ) {}

CHE_PDF_XREF_Table::~CHE_PDF_XREF_Table()
{
	Clear();
}

HE_VOID CHE_PDF_XREF_Table::Clear()
{
	mTrailerDict.clear();
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

HE_BOOL CHE_PDF_XREF_Table::AddTrailerDict( const CHE_PDF_DictionaryPtr & pDict )
{
	if ( pDict )
	{
		mTrailerDict.push_back( pDict );
		return TRUE;
	}
	return FALSE;
}

CHE_PDF_DictionaryPtr CHE_PDF_XREF_Table::GetTrailer( HE_DWORD index /*= 0*/ ) const
{
	if ( index >= mTrailerDict.size() )
	{
		return CHE_PDF_DictionaryPtr();
	}
	return mTrailerDict[index];
}