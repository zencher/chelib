#ifndef _CHE_PDF_XREF_H_
#define _CHE_PDF_XREF_H_

#include "../che_base.h"

class CHE_PDF_Parser;

#define PDF_XREF_ENTRY_INUSE	'n'
#define PDF_XREF_ENTRY_FREE		'f'

#define OBJTYPE_COMMON			0
#define OBJTYPE_COMPRESSED		1

class CHE_PDF_XREF_Entry
{
public:
	CHE_PDF_XREF_Entry();
	CHE_PDF_XREF_Entry( HE_DWORD f1, HE_DWORD f2, HE_DWORD f3, HE_DWORD objNum );

	HE_DWORD field1;
	HE_DWORD field2;
	HE_DWORD field3;
	HE_DWORD objNum;

	HE_DWORD GetType() { return ( field1 == 0 ) ? -1 : ( ( field1 == 2 ) ? OBJTYPE_COMPRESSED : OBJTYPE_COMMON ); }
	HE_DWORD GetOffset() { return field2; }
	HE_DWORD GetParentObjNum() { return field2; }
	HE_DWORD GetObjNum() { return objNum; }
	HE_DWORD GetIndex() { return field3; }
};

struct PDF_XREF_ENTRY_NODE
{
	CHE_PDF_XREF_Entry entry;
	PDF_XREF_ENTRY_NODE * pPrv;
	PDF_XREF_ENTRY_NODE * pNext;
};

struct PDF_XREF_SECTION
{
	PDF_XREF_ENTRY_NODE * pFirstEntry;
	PDF_XREF_ENTRY_NODE * pLastEntry;
	HE_DWORD lBeginNum;
	HE_DWORD lCount;
	PDF_XREF_SECTION * pPreSection;
	PDF_XREF_SECTION * pNextSection;
};

class CHE_PDF_XREF_Table : public CHE_Object
{
public:
	CHE_PDF_XREF_Table( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_XREF_Table();
	
	HE_VOID Clear();

	HE_VOID	NewSection( HE_DWORD lBegin );

	HE_VOID NewNode( CHE_PDF_XREF_Entry & entry );

	HE_VOID BuildIndex();

	//HE_BOOL Remove( unsigned int objNum );

	HE_BOOL GetEntry( HE_DWORD objNum, CHE_PDF_XREF_Entry & entryRet );

	HE_BOOL IsExist( HE_DWORD objNum );

	HE_DWORD GetCount() { return m_lCount; }
	HE_DWORD GetMaxObjNum() { return m_lMaxObjNum; }

private:
	PDF_XREF_SECTION *		m_pFirstSection;
	PDF_XREF_SECTION *		m_pCurSection;
	HE_DWORD				m_lCount;
	HE_DWORD				m_lMaxObjNum;
	PDF_XREF_ENTRY_NODE **	m_pFastAccessArr;

	friend class CHE_PDF_Parser;
};

#endif