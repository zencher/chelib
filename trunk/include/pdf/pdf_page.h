#ifndef _CHELIB_PDF_PAGE_H_
#define _CHELIB_PDF_PAGE_H_

#include "pdf_object.h"
#include "pdf_file.h"
#include "pdf_contentlist.h"
#include "pdf_componentmgr.h"

#include <stack>
#include <unordered_map>

namespace chelib {

class CPDF_Page;
class CPDF_Document;

class CPDF_PageTree : public CObject
{
public:
    CPDF_PageTree( const CPDF_DictionaryPtr & pPagesDict, CPDF_File * pFile,
                   CAllocator * pAllocator = nullptr );
	~CPDF_PageTree();

	size_t GetPageCount();
	CPDF_Page * GetPage( size_t index );
	void ReleasePage( CPDF_Page * pPage );
	bool GetPageRefInfo( size_t index, PDF_RefInfo & refRet );
	void AppendPage( size_t width, size_t height );

private:
	void ParseAllPageRefInfo();

    CPDF_File *                     mpFile;
	size_t                          mPageCount;
	std::vector<PDF_RefInfo>		mPageObjList;
	std::stack<CPDF_ReferencePtr>   mPageNodeStack;
    std::unordered_map<size_t, CPDF_Page*> mPageMap;
    
	friend class CAllocator;
};

class CPDF_Page : public CObject
{
public:
    static bool ReleasePage( CPDF_Page * pPage );
    
	CPDF_DictionaryPtr GetPageDict() const { return mPageDict; }

	CRect GetMediaBox() const;
	CRect GetPageRect() const;

	CPDF_DictionaryPtr GetResourcesDict() const;
	CPDF_ArrayPtr GetMediaBoxArray() const;
	CPDF_ArrayPtr GetCropBoxArray() const;
	CPDF_ArrayPtr GetBleedBoxArray() const;
	CPDF_ArrayPtr GetTrimBoxArray() const;
	CPDF_ArrayPtr GetArtBoxArray() const;

	int32 GetRotate() const;

	bool ParsePageContent( CPDF_ComponentMgr * pComMgr );

	CPDF_ContentObjectList & GetPageContentList() { return mContentList; }

private:
	CPDF_Page( const CPDF_DictionaryPtr pageDict, CAllocator * pAllocator = nullptr )
		: CObject(pAllocator), mbParsed(false), mPageDict(pageDict) {}
	~CPDF_Page() {}

	bool                    mbParsed;
	CPDF_DictionaryPtr		mPageDict;
	CPDF_ContentObjectList	mContentList;

	friend class CAllocator;
	friend class CPDF_PageTree;
};

}//namespace

#endif
