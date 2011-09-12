#ifndef _CHE_PDF_XREF_H_
#define _CHE_PDF_XREF_H_

#include "../che_base.h"
#include "che_pdf_define.h"
#include "che_pdf_objects.h"

class CHE_PDF_Parser;

class CHE_PDF_XREF_Entry
{
public:
	CHE_PDF_XREF_Entry();
	CHE_PDF_XREF_Entry( PDF_XREF_ENTRY_TYPE type, HE_DWORD f1, HE_DWORD f2 );

	PDF_XREF_ENTRY_TYPE		Type;
	HE_DWORD				Field1;
	HE_DWORD				Field2;

	PDF_XREF_ENTRY_TYPE GetType() { return Type; }
	HE_DWORD GetOffset() { return Field1; }
	HE_DWORD GetParentObjNum() { return Field1; }
	HE_DWORD GetIndex() { return Field2; }
};

struct PDF_XREF_ENTRY_NODE
{
	CHE_PDF_XREF_Entry entry;
	PDF_XREF_ENTRY_NODE * pNext;
};

struct PDF_XREF_SECTION_PART
{
	PDF_XREF_SECTION_PART * pNextSecPart;
	PDF_XREF_ENTRY_NODE * pFirstEntry;
	PDF_XREF_ENTRY_NODE * pLastEntry;
	HE_DWORD lBeginNum;
	HE_DWORD lCount;
	HE_DWORD lSectionIndex;
};

struct PDF_XREF_TRAILER_NODE
{
	CHE_PDF_Dictionary * pDict;
	HE_BOOL bNeedDestroy;
	PDF_XREF_TRAILER_NODE * pNext;
};

struct PDF_XREF_FASTACCESS_NODE
{
	HE_DWORD lSectionIndex;
	HE_DWORD lBeginNum;
	HE_DWORD lSize;
	PDF_XREF_SECTION_PART * pFirstSecPart;
	PDF_XREF_ENTRY_NODE ** pAccessArr;
	PDF_XREF_FASTACCESS_NODE * pNext;
};

class CHE_PDF_XREF_Table : public CHE_Object
{
public:
	CHE_PDF_XREF_Table( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_XREF_Table();
	
	//Register the Entry, Finally call BuildIndex().
	HE_VOID	NewSection( HE_DWORD lBegin );
	HE_VOID NewNode( CHE_PDF_XREF_Entry & entry );
	HE_VOID SkipNode();
	HE_VOID UpdateNode( HE_DWORD objNum, CHE_PDF_XREF_Entry & entry );
	HE_VOID NewTrailer( CHE_PDF_Dictionary * pDict, HE_BOOL bNeedDestroy = FALSE );
	HE_VOID BuildIndex();
	HE_VOID Clear();

	HE_BOOL GetEntry( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet );
	HE_BOOL IsExist( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet );
	HE_BOOL IsExist( HE_DWORD objNum );

	HE_DWORD GetTrailerCount() { m_lTrailerCount; }
	CHE_PDF_Dictionary * GetTrailer( HE_DWORD index = 0 ) const;

	HE_DWORD GetCount() { return m_lCount; }
	HE_DWORD GetMaxObjNum() { return m_lMaxObjNum; }
	HE_DWORD RequestObjNum() { return ++m_lMaxObjNum; }

private:
	PDF_XREF_SECTION_PART *		m_pFirstSecPart;
	PDF_XREF_SECTION_PART *		m_pLastSecPart;

	PDF_XREF_FASTACCESS_NODE *	m_pFirstFastAccess;
	PDF_XREF_FASTACCESS_NODE *	m_pLastFastAccess;

	PDF_XREF_TRAILER_NODE *		m_pFirstTrailer;
	PDF_XREF_TRAILER_NODE *		m_pLastTrailer;
	HE_DWORD					m_lTrailerCount;

	HE_DWORD					m_lNextSecNum;
	HE_DWORD					m_lNextObjNum;
	HE_DWORD					m_lMaxObjNum;
	HE_DWORD					m_lCount;
	HE_DWORD					m_lSkipCount;
	HE_BOOL						m_bSkiped;
	HE_BOOL						m_bIndexed;

	friend class CHE_PDF_Parser;
};

#endif