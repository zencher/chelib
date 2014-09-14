#include "../../include/pdf/che_pdf_xref.h"
#include <memory.h>

CHE_PDF_XRefEntry::CHE_PDF_XRefEntry()
{
	Type = XREF_ENTRY_TYPE_FREE;
	ObjNum = 0;
	Field1 = 0;
	Field2 = 0;
}

CHE_PDF_XRefEntry::CHE_PDF_XRefEntry( PDF_XREF_ENTRY_TYPE type, HE_ULONG num, HE_ULONG f1, HE_ULONG f2 )
{
	Type = type;
	ObjNum = num;
	Field1 = f1;
	Field2 = f2;
}

CHE_PDF_XRefTable::CHE_PDF_XRefTable( CHE_Allocator * pAllocator )
	: CHE_Object( pAllocator ), mMaxObjNum( 0 ) {}

CHE_PDF_XRefTable::~CHE_PDF_XRefTable()
{
	Clear();
}

HE_VOID CHE_PDF_XRefTable::Clear()
{
	mTrailerDict.clear();
	mMap.clear();
}

HE_BOOL CHE_PDF_XRefTable::Add( const CHE_PDF_XRefEntry & entry )
{
	mMap[entry.ObjNum] = entry;
	if ( entry.ObjNum > mMaxObjNum )
	{
		mMaxObjNum = entry.ObjNum;
		return TRUE;
	}
	return FALSE;
}

HE_VOID CHE_PDF_XRefTable::AddNewEntry( CHE_PDF_XRefEntry & entryRet )
{
	entryRet.ObjNum = ++mMaxObjNum;
	entryRet.Type = XREF_ENTRY_TYPE_NEW;
	entryRet.Field1 = 0;
	entryRet.Field2 = 0;
	mMap[entryRet.ObjNum] = entryRet;
}

HE_BOOL CHE_PDF_XRefTable::AddTrailerDict( const CHE_PDF_DictionaryPtr & pDict )
{
	if ( pDict )
	{
		mTrailerDict.push_back( pDict );
		return TRUE;
	}
	return FALSE;
}

CHE_PDF_DictionaryPtr CHE_PDF_XRefTable::GetTrailer( HE_ULONG index /*= 0*/ ) const
{
	if ( index >= mTrailerDict.size() )
	{
		return CHE_PDF_DictionaryPtr();
	}
	return mTrailerDict[index];
}

HE_BOOL CHE_PDF_XRefTable::Get( HE_ULONG objNum, CHE_PDF_XRefEntry & entryRet )
{
	unordered_map<HE_ULONG,CHE_PDF_XRefEntry>::iterator it;
	it = mMap.find( objNum );
	if ( it != mMap.end() )
	{
		entryRet = it->second;
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_XRefTable::GetCurNode( CHE_PDF_XRefEntry & entryRet )
{
	if ( mIt != mMap.end() )
	{
		entryRet = mIt->second;
		return TRUE;
	}
	return FALSE;
}

HE_BOOL CHE_PDF_XRefTable::Update( HE_ULONG objNum, const CHE_PDF_XRefEntry & entry )
{
	unordered_map<HE_ULONG,CHE_PDF_XRefEntry>::iterator it;
	it = mMap.find( objNum );
	if ( it != mMap.end() )
	{
		it->second = entry;
		return TRUE;
	}
	return FALSE;
}