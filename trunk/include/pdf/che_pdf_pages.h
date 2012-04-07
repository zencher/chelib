#ifndef _CHE_PDF_PAGES_H_
#define _CHE_PDF_PAGES_H_

#include "../che_base.h"
#include "../che_datastructure.h"
#include "che_pdf_objects.h"
#include "che_pdf_file.h"

class CHE_PDF_Page;

class CHE_PDF_PageTree : public CHE_Object
{
public:
	CHE_PDF_PageTree( const CHE_PDF_DictionaryPtr & pPagesDict, CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL );
	
	~CHE_PDF_PageTree();

	HE_DWORD						GetPageCount();

	CHE_PDF_Page *					GetPage( HE_DWORD index );

	PDF_RefInfo					GetPageRefInfo( HE_DWORD index );

private:
	HE_DWORD						mPageCount;
	HE_DWORD						mCurPageIndex;
	std::vector<PDF_RefInfo>		mPageObjList;
	CHE_Stack<CHE_PDF_ReferencePtr>	mPageNodeStack;
	CHE_PDF_File *					mpFile;
};

class CHE_PDF_Page : public CHE_Object
{
public:
	~CHE_PDF_Page();

private:
	CHE_PDF_Page( const CHE_PDF_DictionaryPtr pPageDict, CHE_PDF_PageTree * pPages, CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), mpPageDict( pPageDict ) {}

	CHE_PDF_DictionaryPtr	mpPageDict;

	friend class CHE_Allocator;
	friend class CHE_PDF_PageTree;
};

#endif
