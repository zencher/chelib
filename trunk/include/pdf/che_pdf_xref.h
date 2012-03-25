#ifndef _CHE_PDF_XREF_H_
#define _CHE_PDF_XREF_H_

#include "../che_base.h"
#include "che_pdf_define.h"
#include "che_pdf_objects.h"

class CHE_PDF_XREF_Entry
{
public:
	CHE_PDF_XREF_Entry();
	CHE_PDF_XREF_Entry( PDF_XREF_ENTRY_TYPE type, HE_DWORD objNum, HE_DWORD f1, HE_DWORD f2 );

	PDF_XREF_ENTRY_TYPE		Type;
	HE_DWORD				ObjNum;
	HE_DWORD				Field1;
	HE_DWORD				Field2;

	PDF_XREF_ENTRY_TYPE GetType() { return Type; }
	HE_DWORD GetObjNum() { return ObjNum; }
	HE_DWORD GetOffset() { return Field1; }
	HE_DWORD GetGenNum() { return Field2; }
	HE_DWORD GetParentObjNum() { return Field1; }
	HE_DWORD GetIndex() { return Field2; }
};

bool operator == ( const CHE_PDF_XREF_Entry & node1, const CHE_PDF_XREF_Entry & node2 );
bool operator >  ( const CHE_PDF_XREF_Entry & node1, const CHE_PDF_XREF_Entry & node2 );
bool operator <  ( const CHE_PDF_XREF_Entry & node1, const CHE_PDF_XREF_Entry & node2 );

struct PDF_XREF_TRAILER_NODE
{
	CHE_PDF_Dictionary * pDict;
	HE_BOOL bNeedDestroy;
	PDF_XREF_TRAILER_NODE * pNext;
};

class CHE_PDF_XREF_Table : public CHE_Object
{
public:
	CHE_PDF_XREF_Table( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_XREF_Table();

	HE_BOOL Add( const CHE_PDF_XREF_Entry & entry );

	HE_BOOL Get( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet );

	HE_BOOL Update( HE_DWORD objNum, const CHE_PDF_XREF_Entry & entryRet );

	HE_BOOL AddTrailerDict( CHE_PDF_Dictionary * pDict, HE_BOOL bNeedDestroy = FALSE );

	HE_VOID Clear();

	HE_DWORD GetTrailerCount() { m_lTrailerCount; }

	CHE_PDF_Dictionary * GetTrailer( HE_DWORD index = 0 ) const;

	HE_VOID AddNewEntry( CHE_PDF_XREF_Entry & entryRet );

	HE_VOID MoveFirst(){ mList.MoveFirst(); }

	HE_VOID MoveNext(){ mList.MoveNext(); }

	HE_BOOL IsEOF() { return mList.IsEOF(); }

	HE_BOOL GetCurNode( CHE_PDF_XREF_Entry & entryRet ) { return mList.GetCurNode( entryRet ); }

private:
	PDF_XREF_TRAILER_NODE *		m_pFirstTrailer;
	PDF_XREF_TRAILER_NODE *		m_pLastTrailer;
	HE_DWORD					m_lTrailerCount;

	HE_DWORD					mMaxObjNum;

	CHE_SkipList<CHE_PDF_XREF_Entry> mList;
};


struct PDF_XREF_ENTRY_NODE
{
	CHE_PDF_XREF_Entry entry;
	PDF_XREF_ENTRY_NODE * pNext;
};

struct PDF_XREF_SECTION
{
	PDF_XREF_SECTION * pNextSec;
	PDF_XREF_ENTRY_NODE * pFirstEntry;
	PDF_XREF_ENTRY_NODE * pLastEntry;
	HE_DWORD lBeginNum;
	HE_DWORD lCount;
};

class CHE_PDF_XREF_Data : public CHE_Object
{
public:
	CHE_PDF_XREF_Data( CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_XREF_Data();

	HE_VOID	NewSection( HE_DWORD lBegin );

	HE_VOID NewNode( CHE_PDF_XREF_Entry & entry );

	PDF_XREF_SECTION * mpFirstSec;
	PDF_XREF_SECTION * mpLastSec;
	HE_DWORD mlCount;
};

#endif