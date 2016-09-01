#ifndef _CHE_PDF_PAGES_H_
#define _CHE_PDF_PAGES_H_

#include "../che_base.h"
#include "../che_graphics.h"
#include "che_pdf_objects.h"
#include "che_pdf_file.h"
#include "che_pdf_contentlist.h"
#include "che_pdf_componentmgr.h"

#include <stack>
#include <unordered_map>

class CHE_PDF_Page;
class CHE_PDF_Document;

class CHE_PDF_PageTree : public CHE_Object
{
public:
    
    CHE_PDF_PageTree( const CHE_PDF_DictionaryPtr & pPagesDict, CHE_PDF_File * pFile, CHE_Allocator * pAllocator = nullptr );

	~CHE_PDF_PageTree();

	size_t                          GetPageCount();

	CHE_PDF_Page *					GetPage( size_t index );

	void							ReleasePage( CHE_PDF_Page * pPage );

	bool							GetPageRefInfo( size_t index, PDF_RefInfo & refRet );

	void							AppendPage( size_t width, size_t height );

private:

	void							ParseAllPageRefInfo();

	size_t                          mPageCount;

	std::vector<PDF_RefInfo>		mPageObjList;

	std::stack<CHE_PDF_ReferencePtr>mPageNodeStack;

    std::unordered_map<size_t, CHE_PDF_Page*> mPageMap;
    
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

	int32                   GetRotate() const;

	static bool             ReleasePage( CHE_PDF_Page * pPage );

	bool					ParsePageContent( CHE_PDF_ComponentMgr * pComMgr );

	CHE_PDF_ContentObjectList & GetPageContentList() { return mContentList; }

private:
	CHE_PDF_Page( const CHE_PDF_DictionaryPtr pageDict, CHE_Allocator * pAllocator = nullptr )
		: CHE_Object(pAllocator), mbParsed(false), mPageDict(pageDict) {}

	~CHE_PDF_Page() {}

	bool						mbParsed;
	CHE_PDF_DictionaryPtr		mPageDict;
	CHE_PDF_ContentObjectList	mContentList;

	friend class CHE_Allocator;
	friend class CHE_PDF_PageTree;
};

#endif
