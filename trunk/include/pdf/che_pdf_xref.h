#ifndef _CHE_PDF_XREF_H_
#define _CHE_PDF_XREF_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include <vector>


enum PDF_XREF_ENTRY_TYPE
{
	XREF_ENTRY_TYPE_FREE		= 0x00,
	XREF_ENTRY_TYPE_COMMON		= 0x01,
	XREF_ENTRY_TYPE_COMPRESSED	= 0x02,
	XREF_ENTRY_TYPE_NEW			= 0x03
};


class CHE_PDF_XREF_Entry
{
public:

	CHE_PDF_XREF_Entry();

	CHE_PDF_XREF_Entry( PDF_XREF_ENTRY_TYPE type, HE_DWORD objNum, HE_DWORD f1, HE_DWORD f2 );

	PDF_XREF_ENTRY_TYPE		GetType() { return Type; }

	HE_DWORD				GetObjNum() { return ObjNum; }

	HE_DWORD				GetOffset() { return Field1; }

	HE_DWORD				GetGenNum() { return Field2; }

	HE_DWORD				GetParentObjNum() { return Field1; }

	HE_DWORD				GetIndex() { return Field2; }

	PDF_XREF_ENTRY_TYPE		Type;

	HE_DWORD				ObjNum;

	HE_DWORD				Field1;

	HE_DWORD				Field2;
};

bool operator == ( const CHE_PDF_XREF_Entry & node1, const CHE_PDF_XREF_Entry & node2 );

bool operator >  ( const CHE_PDF_XREF_Entry & node1, const CHE_PDF_XREF_Entry & node2 );

bool operator <  ( const CHE_PDF_XREF_Entry & node1, const CHE_PDF_XREF_Entry & node2 );

class CHE_PDF_XREF_Table : public CHE_Object
{
public:

	CHE_PDF_XREF_Table( CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_XREF_Table();

	HE_BOOL								Add( const CHE_PDF_XREF_Entry & entry );

	HE_VOID								AddNewEntry( CHE_PDF_XREF_Entry & entryRet );

	HE_BOOL								AddTrailerDict( const CHE_PDF_DictionaryPtr & pDict );

	HE_VOID								Clear();

	HE_BOOL								Get( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet );

	HE_DWORD							GetTrailerCount() { return mTrailerDict.size(); }

	CHE_PDF_DictionaryPtr				GetTrailer( HE_DWORD index = 0 ) const;

	HE_DWORD							GetMaxObjNum() const { return mMaxObjNum; }

	HE_BOOL								Update( HE_DWORD objNum, const CHE_PDF_XREF_Entry & entryRet );

	HE_BOOL								IsEOF() { return mList.IsEOF(); }

	HE_VOID								MoveFirst() { mList.MoveFirst(); }

	HE_VOID								MoveNext() { mList.MoveNext(); }


	HE_BOOL								GetCurNode( CHE_PDF_XREF_Entry & entryRet ) { return mList.GetCurNode( entryRet ); }

private:
	
	HE_DWORD							mMaxObjNum;

	std::vector<CHE_PDF_DictionaryPtr>	mTrailerDict;

	CHE_SkipList<CHE_PDF_XREF_Entry>	mList;
};

#endif