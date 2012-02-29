// #ifndef _CHE_PDF_PAGES_H_
// #define _CHE_PDF_PAGES_H_
// 
// #include "../che_base.h"
// #include "../che_datastructure.h"
// #include "che_pdf_parser.h"
// #include "che_pdf_font.h"
// #include "che_pdf_pageobjs.h"
// 
// class CHE_PDF_Page;
// 
// class IHE_PDF_GetFont : public CHE_Object
// {
// public:
// 	IHE_PDF_GetFont( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {};
// 
// 	virtual ~IHE_PDF_GetFont() {};
// 	
// 	virtual CHE_PDF_Font * GetFont( HE_DWORD objNum ) = 0;
// };
// 
// class CHE_PDF_Document : public CHE_Object
// {
// public:
// 	CHE_PDF_Document( CHE_Allocator * pAllocator = NULL );
// 	CHE_PDF_Document( IHE_Read * pFileRead, CHE_Allocator * pAllocator = NULL );
// 	~CHE_PDF_Document();
// 
// 	HE_BOOL						Load( IHE_Read * pFileRead );
// 
// 	HE_VOID						Unload();
// 
// 	CHE_PDF_Parser *			GetParser() const { return m_pParser; }
// 
// 	CHE_PDF_Dictionary*			GetRoot() const { return m_pRootDict; }
// 
// 	CHE_PDF_Dictionary*			GetInfo() const { return m_pInfoDict; }
// 
// 	HE_VOID						GetID( CHE_ByteString& id1, CHE_ByteString& id2 ) const { id1 = m_ID1; id2 = m_ID2; }
// 
// 	HE_DWORD					GetPageCount() const;
// 
// 	CHE_PDF_Page*				GetPage( HE_DWORD iPageIndex );
// 
// 	HE_VOID						ClosePage( CHE_PDF_Page * pPage );
// 
// 	HE_DWORD					GetPageIndex( HE_DWORD objnum );
// 
// 	HE_BOOL						IsEncrypted();
// 
// 	CHE_PDF_Array *				GetPageMediaBox( CHE_PDF_Dictionary * pPageDict );
// 
// 	CHE_PDF_Dictionary *		GetPageResources( CHE_PDF_Dictionary * pPageDict );
// 
// 	CHE_PDF_Font *				GetFont( HE_DWORD objNum );
// 
// 	IHE_PDF_GetFont *			GetIHE_GetPDFFont() { return m_pIHE_GetPDFFont; }
// 
// protected:
// 	CHE_PDF_Parser *			m_pParser;
// 	CHE_PDF_Dictionary*			m_pRootDict;
// 	CHE_PDF_Dictionary*			m_pInfoDict;
// 	CHE_ByteString				m_ID1;
// 	CHE_ByteString				m_ID2;
// 	CHE_NumToPtrMap				m_FontMap;
// 	IHE_PDF_GetFont*			m_pIHE_GetPDFFont;
// };
// 
// class CHE_PDF_Page : public CHE_Object
// {
// public:
// 	~CHE_PDF_Page();
// 
// 	HE_FLOAT			GetPageWidth() { return m_fPageWidth; }
// 	HE_FLOAT			GetPageHeight() { return m_fPageHeight; }
// 
// 	CHE_PDF_Document *	GetDocument() { return m_pDoc; }
// 	
// 	HE_BOOL				GetPageContent( CHE_DynBuffer & buffer );
// 
// 	CHE_PDF_Stream *	GetPageContent();
// 
// 	CHE_PDF_Dictionary*	GetPageDictionary() { return m_pPageObj; }
// 
// 	CHE_PDF_Dictionary* GetPageResources() { return m_pDoc?m_pDoc->GetPageResources( m_pPageObj ) : NULL ; }
// 
// 	HE_DWORD			GetPageIndex() { return m_lPageIndex; }
// 
// 	//HE_DWORD			ParseContent();
// 
// 	//HE_DWORD			GetContentObjectCount() { return m_arrContentObj.GetCount(); }
// 
// 	//CHE_PDF_ContentObject *	GetContentObject( HE_DWORD index ) { return (CHE_PDF_ContentObject*)m_arrContentObj.GetItem( index );  }
// 
// private:
// 	CHE_PDF_Page( HE_DWORD pageIndex, CHE_PDF_Dictionary * pDict, CHE_PDF_Document * pDoc, CHE_Allocator * pAllocator = NULL );
// 
// 	HE_DWORD			m_lPageIndex;
// 
// 	HE_FLOAT			m_fPageWidth;
// 	HE_FLOAT			m_fPageHeight;
// 
// 	CHE_PDF_Dictionary* m_pPageObj;
// 	CHE_PDF_Document*	m_pDoc;
// 
// 	//CHE_PtrArray		m_arrContentObj;
// 	
// 	friend class		CHE_PDF_Document;
// 	friend class		CHE_Allocator;
// };
// 
// #endif
