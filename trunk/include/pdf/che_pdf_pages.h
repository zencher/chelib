#ifndef _CHE_PDF_PAGES_H_
#define _CHE_PDF_PAGES_H_

#include "../che_base.h"
#include "../che_graphics.h"
#include "che_pdf_objects.h"
#include "che_pdf_file.h"
#include "che_pdf_contentlist.h"
#include "che_pdf_componentmgr.h"

#include <stack>

class CHE_PDF_Page;
class CHE_PDF_Document;

class CHE_PDF_PageTree : public CHE_Object
{
public:
    
    CHE_PDF_PageTree( const CHE_PDF_DictionaryPtr & pPagesDict, CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_PageTree();

	HE_ULONG						GetPageCount();

	CHE_PDF_Page *					GetPage( HE_ULONG index );

	HE_VOID							ReleasePage( CHE_PDF_Page * pPage );

	HE_BOOL							GetPageRefInfo( HE_ULONG index, PDF_RefInfo & refRet );

	HE_VOID							AppendPage( HE_ULONG width, HE_ULONG height );

private:

	HE_VOID							ParseAllPageRefInfo();

	HE_ULONG						mPageCount;

	std::vector<PDF_RefInfo>		mPageObjList;

	std::stack<CHE_PDF_ReferencePtr>mPageNodeStack;

	CHE_PDF_File *					mpFile;

	friend class CHE_Allocator;
	//friend class CHE_PDF_Document;
};

class CHE_PDF_Page : public CHE_Object
{
public:
	
	CHE_PDF_DictionaryPtr	GetPageDict() const { return mPageDict; }

	CHE_Rect				GetMediaBox() const;

	CHE_Rect				GetPageRect() const;

	CHE_PDF_DictionaryPtr	GetResourcesDict() const;

	CHE_PDF_ArrayPtr		GetMediaBoxArray() const;

	CHE_PDF_ArrayPtr		GetCropBoxArray() const;

	CHE_PDF_ArrayPtr		GetBleedBoxArray() const;

	CHE_PDF_ArrayPtr		GetTrimBoxArray() const;

	CHE_PDF_ArrayPtr		GetArtBoxArray() const;

	HE_INT32				GetRotate() const;

	static HE_BOOL			ReleasePage( CHE_PDF_Page * pPage );

	HE_BOOL					ParsePageContent( CHE_PDF_ComponentMgr * pComMgr );

	CHE_PDF_ContentObjectList & GetPageContentList() { return mContentList; }

private:
	CHE_PDF_Page( const CHE_PDF_DictionaryPtr pageDict, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mbParsed(false), mPageDict(pageDict) {}

	~CHE_PDF_Page() {}

	HE_BOOL						mbParsed;
	CHE_PDF_DictionaryPtr		mPageDict;
	CHE_PDF_ContentObjectList	mContentList;

	friend class CHE_Allocator;
	friend class CHE_PDF_PageTree;
};

#endif
