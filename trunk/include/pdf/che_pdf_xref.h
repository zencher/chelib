#ifndef _CHE_PDF_XREF_H_
#define _CHE_PDF_XREF_H_

#include "../che_base.h"
#include "che_pdf_objects.h"

#include <vector>
#include <map>

using namespace std;

enum PDF_XREF_ENTRY_TYPE
{
	XREF_ENTRY_TYPE_FREE		= 0x00,
	XREF_ENTRY_TYPE_COMMON		= 0x01,
	XREF_ENTRY_TYPE_COMPRESSED	= 0x02,
	XREF_ENTRY_TYPE_NEW			= 0x03
};


class CHE_PDF_XRefEntry
{
public:
	CHE_PDF_XRefEntry();
	CHE_PDF_XRefEntry( PDF_XREF_ENTRY_TYPE type, HE_ULONG objNum, HE_ULONG f1, HE_ULONG f2 );

	PDF_XREF_ENTRY_TYPE		GetType() const { return Type; }
	HE_ULONG				GetObjNum() const { return ObjNum; }
	HE_ULONG				GetOffset() const { return Field1; }
	HE_ULONG				GetGenNum() const { return Field2; }
	HE_ULONG				GetParentObjNum() const { return Field1; }
	HE_ULONG				GetIndex() const { return Field2; }

	PDF_XREF_ENTRY_TYPE		Type;
	HE_ULONG				ObjNum;
	HE_ULONG				Field1;
	HE_ULONG				Field2;
};


class CHE_PDF_XRefTable : public CHE_Object
{
public:
	CHE_PDF_XRefTable( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_XRefTable();

	HE_BOOL					Add( const CHE_PDF_XRefEntry & entry );
	HE_VOID					AddNewEntry( CHE_PDF_XRefEntry & entryRet );
	HE_BOOL					AddTrailerDict( const CHE_PDF_DictionaryPtr & pDict );

	HE_VOID					Clear();

	HE_BOOL					Get( HE_ULONG objNum, CHE_PDF_XRefEntry & entryRet );
	HE_ULONG				GetTrailerCount() const { return mTrailerDict.size(); }
	CHE_PDF_DictionaryPtr	GetTrailer( HE_ULONG index = 0 ) const;
	HE_ULONG				GetMaxObjNum() const { return mMaxObjNum; }

	HE_BOOL					IsEOF() { return mIt == mMap.end(); }
	HE_VOID					MoveFirst() { mIt = mMap.begin(); }
	HE_VOID					MoveNext() { ++mIt; }

	HE_BOOL					GetCurNode( CHE_PDF_XRefEntry & entryRet );
	HE_BOOL					Update( HE_ULONG objNum, const CHE_PDF_XRefEntry & entryRet );

private:
	HE_ULONG											mMaxObjNum;
	std::vector<CHE_PDF_DictionaryPtr>					mTrailerDict;
	map<HE_ULONG,CHE_PDF_XRefEntry>                     mMap;
	map<HE_ULONG,CHE_PDF_XRefEntry>::iterator           mIt;
};

#endif