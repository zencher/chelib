#ifndef _CHE_PDF_PAGES_H_
#define _CHE_PDF_PAGES_H_

#include "../che_base.h"
#include "che_pdf_parser.h"

class CHE_PDF_Page;

class CHE_PDF_Document : public CHE_Object
{
public:
	CHE_PDF_Document();
	CHE_PDF_Document( IHE_Read * pFileRead );
	~CHE_PDF_Document();

	HE_BOOL					Load( IHE_Read * pFileRead );

	HE_BOOL					Save( IHE_Write * pFileRead );

	CHE_PDF_Parser *		GetParser() const { return m_pParser; }

	CHE_PDF_Dictionary*		GetRoot() const { return m_pRootDict; }

	CHE_PDF_Dictionary*		GetInfo() const { return m_pInfoDict; }

	HE_VOID					GetID( CHE_ByteString& id1, CHE_ByteString& id2 ) const { id1 = m_ID1; id2 = m_ID2; }

	HE_DWORD				GetPageCount() const;

	CHE_PDF_Page*			GetPage( HE_DWORD iPageIndex );

	HE_DWORD				GetPageIndex( HE_DWORD objnum );
	/**
	 * Enumerate pages with user-supplied page enumeration handler.
	 *
	 * @param[in] pHandler	The user-supplied page enumeration handler.
	 */
	//void					EnumPages(IPDF_EnumPageHandler* pHandler);

	/** Get the user permission of the document. */
	//HE_DWORD				GetUserPermissions() const;
	/** Whether the user has the owner permission of the document. */
	//HE_DWORD					IsOwner() const;

	/**
	 * @name Get module specific data.
	 */
	/*@{*/

	/** Get document-specific data for PAGE module. */
	//CPDF_DocPageData*		GetPageData() const { return m_pDocPage; }
	/** Get document-specific data for RENDER module. */
	//CPDF_DocRenderData*		GetRenderData() const { return m_pDocRender; }

	/*@}*/ 

	/**
	 * Check if an indirect object is a form stream or not, without actually loading the object
	 * Return TRUE if determined, FALSE if unknown (the default implementation).
	 *
	 * @param[in] objnum	The input indirect object number.
	 * @param[out] bForm	It receive whether it's a form stream or not.
	 * @return Non-zero means determined, otherwise unknown.
	 */
	//FX_BOOL					IsFormStream(FX_DWORD objnum, FX_BOOL& bForm) const;

	/**
	 * @name Functions implemented in PAGE module. 
	 * Load cached resources.
	 */
	/*@{*/
	
	/**
	 * Load a font from font dictionary.
	 *
	 * @param[in] pFontDict		The input font dictionary.
	 * @return A PDF font.
	 */
	//CPDF_Font*				LoadFont(CPDF_Dictionary* pFontDict);
	/**
	 * Find if a font is already loaded.
	 *
	 * @param[in] pFontDict		The input font dictionary.
	 * @return A PDF font object is alreay loaded, or NULL is not loaded.
	 */
	//CPDF_Font*				FindFont(CPDF_Dictionary* pFontDict);
	/**
	 * Load a color space from a PDF object.
	 *
	 * @param[in] pCSObj		The input PDF object.
	 * @param[in] pResources	The current resource dictionary, used for default device CS
	 * @return A PDF color space.
	 */
	//CPDF_ColorSpace*		LoadColorSpace(CPDF_Object* pCSObj, CPDF_Dictionary* pResources = NULL);
	/**
	 * Load a pattern from a PDF object.
	 *
	 * @param[in] pObj			The input PDF object.
	 * @param[in] bShading		Whether the pattern is a shading pattern or not.
	 * @return A PDF pattern.
	 */
	//CPDF_Pattern*			LoadPattern(CPDF_Object* pObj, FX_BOOL bShading);
	/**
	 * Load an image from a PDF object.
	 *
	 * @param[in] pObj			The input PDF object.
	 * @param[in] bCache		Whether we will cache the image or not.
	 * @return A PDF image.
	 */
	//CPDF_Image*				LoadImageF(CPDF_Object* pObj, FX_BOOL bCache = TRUE);
	/**
	 * Load a PDF stream accessor from a PDF stream.
	 *
	 * @param[in] pStream		The input PDF stream.
	 * @return A PDF stream accessor. 
	 */
	//CPDF_StreamAcc*			LoadFontFile(CPDF_Stream* pStream);
	/**
	 * Load an ICC profile from a PDF stream.
	 *
	 * @param[in] pStream		The input PDF stream.
	 * @param[in] nComponents	The components of the color space specified in ICC profile.
	 * @return An ICC profile.
	 */
	//CPDF_IccProfile*		LoadIccProfile(CPDF_Stream* pStream, int nComponents);

	/*@}*/ 

	/**
	 * @name Functions implemented in EDIT module. 
	 */
	/*@{*/
	
	/**
	 * Set the root object number. must be called for an empty document.
	 *
	 * @param[in] RootObjNum	The input root object number.
	 */
	//void					SetRootObjNum(int RootObjNum);
	/**
	 * Set the info object number. must be called for an empty document.
	 *
	 * @param[in] InfoObjNum	The input info object number.
	 */
	//void					SetInfoObjNum(int InfoObjNum);
	/**
	 * Set the PDF file ID.
	 *
	 * @param[in] id1			The first file ID.
	 * @param[in] id2			The second file ID.
	 */
// 	void					SetID(const CFX_ByteString& id1, const CFX_ByteString& id2) { m_ID1 = id1; m_ID2 = id2; }
// 
// #if (_FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN32_MOBILE_ || _FX_OS_ == _FX_WIN64_) && !defined(_FX_NO_WINDOWS_)
	/**
	 * Import a Windows font.
	 *
	 * @param[in] pLogFont			Points to a LOGFONT(WIN32) structure that defines the characteristics of the logical font. 
	 * @param[in] bVert				Whether the font is a vertical font or not.
	 * @param[in] bTranslateName	Whether we will do font name translating or not.
	 * @return A PDF font.
	 */
// 	CPDF_Font*				AddWindowsFont(LOGFONTA* pLogFont, FX_BOOL bVert, FX_BOOL bTranslateName = FALSE);
// 	CPDF_Font*				AddWindowsFont(LOGFONTW* pLogFont, FX_BOOL bVert, FX_BOOL bTranslateName = FALSE);
// #endif

	/**
	 * Add a Type1 base-14 font.
	 *
	 * @param[in] font				The font name.
	 * @param[in] pEncoding			The font encoding.
	 * @return A PDF font.
	 */
	//CPDF_Font*				AddStandardFont(const FX_CHAR* font, CPDF_FontEncoding* pEncoding);

	/**
	 * Build a list of all resources of specified type in this document. On return,
	 * the ResObjList array contains pointers to CPDF_Object objects.
	 *
	 * @param[in] type				The name of specified type.
	 * @param[out] ResObjList		It will receive all resource objects of specified type in the document.
	 */
	//void					BuildResourceList(const FX_CHAR* type, CFX_PtrArray& ResObjList);

	/** Create a new document. */
	//void					CreateNewDoc();
	/**
	 * Create a new page. Returns the created page.
	 *
	 * @param[in] iPage				Specifies the zero-based index of page to be created.
	 * @return The created page dictionary.
	 */
	//CPDF_Dictionary*		CreateNewPage(int iPage);
	/**
	 * Delete specified page.
	 *
	 * @param[in] iPage				Specifies the zero-based index of page to be deleted.
	 */
	//void					DeletePage(int iPage);

	/**
	 * Acrobat often requires stream of dictionary members of an object to be indirect objects.
	 *
	 * @param[in] pObject				The input PDF object.
	 * @param[in] bConvertStream		Whether we will convert stream to indirect object or not.
	 * @param[in] bConvertDictionary	Whether we will convert dictionary to indirect object or not.
	 */
	//void					ConvertIndirectObjects(CPDF_Object* pObject, 
	//							FX_BOOL bConvertStream, FX_BOOL bConvertDictionary);

	/**
	 * Get a modifiable content stream for a page.
	 *
	 * @param[in] pPageDict			The input page dictionary.
	 * @return An modifiable content stream for the page.
	 */
	//CPDF_Stream*			GetPageContentModify(CPDF_Dictionary* pPageDict);

	/*@}*/ 

	/**
	 * Quick search an pattern for specified page.
	 *
	 * @param[in] page_index		The zero-based page index to be searched.
	 * @param[in] pattern			The pattern to search.
	 * @param[in] bCaseSensitive	Whether the pattern matching is case sensitive.
	 * @return Non-zero means searched one successfully.
	 */
	//FX_BOOL					QuickSearch(int page_index, FX_LPCWSTR pattern, FX_BOOL bCaseSensitive);

	/** Reload page list. This can be used when document is progressively downloaded. */
// 	void					ReloadPages();
// 
// 	/** Internally used */
// 	void					LoadDoc();
// 
// 	void					LoadAsynDoc(CPDF_Dictionary *pLinearized);
// 	void					LoadPages();

protected:
	CHE_PDF_Parser *		m_pParser;
	/** The root dictionary. */
	CHE_PDF_Dictionary*		m_pRootDict;
	/** The info dictionary. */
	CHE_PDF_Dictionary*		m_pInfoDict;
	/** The first file ID. */
	CHE_ByteString			m_ID1;
	/** The second file ID. */
	CHE_ByteString			m_ID2;
	
	/** Whether is the linearized PDF. */
	//HE_BOOL					m_bLinearized;
	/** The linearized first page no. */
	//HE_DWORD				m_dwFirstPageNo;
	/** The linearized first page object number. */
	//HE_DWORD				m_dwFirstPageObjNum;

	/** Record object numbers for each page. 0 for unknown. Size of this array is always page count. */
	//CFX_DWordArray			m_PageList;
	HE_DWORD*				m_pPageObjNumList;

	/** Get the page count. */
// 	int						_GetPageCount() const;
// 
// 	CPDF_Dictionary*		_FindPDFPage(CPDF_Dictionary* pPages, int iPage, int nPagesToGo, int level);
// 	int						_FindPageIndex(CPDF_Dictionary* pNode, FX_DWORD& skip_count, FX_DWORD objnum, int& index);
// 	FX_BOOL					IsContentUsedElsewhere(FX_DWORD objnum, CPDF_Dictionary* pPageDict);
// 	FX_BOOL					CheckOCGVisible(CPDF_Dictionary* pOCG, FX_BOOL bPrinting);
// 	void					GetCharStream(CFX_WideTextBuf& buf, CPDF_Dictionary* pFormDict, CPDF_Object* pContent);

	friend class			CHE_PDF_Parser;

	/**
	 * @name Data pointers for extended modules. 
	 */
	/*@{*/

	/** Document-specific data for PAGE module. */
	//CPDF_DocPageData*		m_pDocPage;
	/** Document-specific data for RENDER module. */
	//CPDF_DocRenderData*		m_pDocRender;

	/*@}*/ 
};

class CHE_PDF_Page
{
public:
	CHE_PDF_Page( CHE_PDF_Dictionary * pDict, CHE_PDF_Document * pDoc );
	~CHE_PDF_Page();

	HE_FLOAT	GetPageWidth() { return m_fPageWidth; }
	HE_FLOAT	GetPageHeight() { return m_fPageHeight; }

	CHE_PDF_Document *	GetDocument() { return m_pDoc; }

	CHE_PDF_Dictionary* GetPageDictionary() { return m_pPageObj; }

private:

	HE_FLOAT	m_fPageWidth;
	HE_FLOAT	m_fPageHeight;

	CHE_PDF_Dictionary* m_pPageObj;
	CHE_PDF_Document*	m_pDoc;
	
	friend class CHE_PDF_Document;
};

#endif