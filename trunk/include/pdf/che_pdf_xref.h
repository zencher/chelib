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
	CHE_PDF_XRefEntry( PDF_XREF_ENTRY_TYPE type, size_t objNum, size_t f1, size_t f2 );

	PDF_XREF_ENTRY_TYPE		GetType() const { return Type; }
	size_t                  GetObjNum() const { return ObjNum; }
	size_t                  GetOffset() const { return Field1; }
	size_t                  GetGenNum() const { return Field2; }
	size_t                  GetParentObjNum() const { return Field1; }
	size_t                  GetIndex() const { return Field2; }

	PDF_XREF_ENTRY_TYPE		Type;
	size_t                  ObjNum;
	size_t                  Field1;
	size_t                  Field2;
};


class CHE_PDF_XRefTable : public CHE_Object
{
public:
	CHE_PDF_XRefTable( CHE_Allocator * pAllocator = nullptr );
	~CHE_PDF_XRefTable();

	bool					Add( const CHE_PDF_XRefEntry & entry );
	void					AddNewEntry( CHE_PDF_XRefEntry & entryRet );
	bool					AddTrailerDict( const CHE_PDF_DictionaryPtr & pDict );

	void					Clear();

	bool					Get( size_t objNum, CHE_PDF_XRefEntry & entryRet );
	size_t                  GetTrailerCount() const { return mTrailerDict.size(); }
	CHE_PDF_DictionaryPtr	GetTrailer( size_t index = 0 ) const;
	size_t                  GetMaxObjNum() const { return mMaxObjNum; }

	bool					IsEOF() { return mIt == mMap.end(); }
	void					MoveFirst() { mIt = mMap.begin(); }
	void					MoveNext() { ++mIt; }

	bool					GetCurNode( CHE_PDF_XRefEntry & entryRet );
	bool					Update( size_t objNum, const CHE_PDF_XRefEntry & entryRet );

private:
	size_t                                          mMaxObjNum;
	std::vector<CHE_PDF_DictionaryPtr>				mTrailerDict;
	map<size_t,CHE_PDF_XRefEntry>                   mMap;
	map<size_t,CHE_PDF_XRefEntry>::iterator         mIt;
};

#endif