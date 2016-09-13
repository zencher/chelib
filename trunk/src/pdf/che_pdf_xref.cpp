#include "../../include/pdf/che_pdf_xref.h"
#include <memory.h>

namespace chelib {
    
using namespace std;

CPDF_XRefEntry::CPDF_XRefEntry()
{
	Type = XREF_ENTRY_TYPE_FREE;
	ObjNum = 0;
	Field1 = 0;
	Field2 = 0;
}

CPDF_XRefEntry::CPDF_XRefEntry( PDF_XREF_ENTRY_TYPE type, size_t num, size_t f1, size_t f2 )
{
	Type = type;
	ObjNum = num;
	Field1 = f1;
	Field2 = f2;
}

CPDF_XRefTable::CPDF_XRefTable( CAllocator * pAllocator )
	: CObject( pAllocator ), mMaxObjNum( 0 ) {}

CPDF_XRefTable::~CPDF_XRefTable()
{
	Clear();
}

void CPDF_XRefTable::Clear()
{
	mTrailerDict.clear();
	mMap.clear();
}

bool CPDF_XRefTable::Add( const CPDF_XRefEntry & entry )
{
	mMap[entry.ObjNum] = entry;
	if ( entry.ObjNum > mMaxObjNum )
	{
		mMaxObjNum = entry.ObjNum;
		return TRUE;
	}
	return false;
}

void CPDF_XRefTable::AddNewEntry( CPDF_XRefEntry & entryRet )
{
	entryRet.ObjNum = ++mMaxObjNum;
	entryRet.Type = XREF_ENTRY_TYPE_NEW;
	entryRet.Field1 = 0;
	entryRet.Field2 = 0;
	mMap[entryRet.ObjNum] = entryRet;
}

bool CPDF_XRefTable::AddTrailerDict( const CPDF_DictionaryPtr & pDict )
{
	if ( pDict )
	{
		mTrailerDict.push_back( pDict );
		return TRUE;
	}
	return false;
}

CPDF_DictionaryPtr CPDF_XRefTable::GetTrailer( size_t index /*= 0*/ ) const
{
	if ( index >= mTrailerDict.size() )
	{
		return CPDF_DictionaryPtr();
	}
	return mTrailerDict[index];
}

bool CPDF_XRefTable::Get( size_t objNum, CPDF_XRefEntry & entryRet )
{
	map<size_t,CPDF_XRefEntry>::iterator it;
	it = mMap.find( objNum );
	if ( it != mMap.end() )
	{
		entryRet = it->second;
		return TRUE;
	}
	return false;
}

bool CPDF_XRefTable::GetCurNode( CPDF_XRefEntry & entryRet )
{
	if ( mIt != mMap.end() )
	{
		entryRet = mIt->second;
		return TRUE;
	}
	return false;
}

bool CPDF_XRefTable::Update( size_t objNum, const CPDF_XRefEntry & entry )
{
	map<size_t,CPDF_XRefEntry>::iterator it;
	it = mMap.find( objNum );
	if ( it != mMap.end() )
	{
		it->second = entry;
		return TRUE;
	}
	return false;
}

}//namespace