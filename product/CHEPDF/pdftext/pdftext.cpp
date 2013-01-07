#include "pdftext.h"
#include "../../../trunk/include/pdf/che_pdf_file.h"
#include "../../../trunk/include/pdf/che_pdf_document.h"
#include "../../../trunk/include/pdf/che_pdf_contents.h"

// 1. 加载文档对象
// 2. 加载页面对象
// 3. 解析页面内容
// 4. 按顺序获取页面内容
// 5. 文本内容信息获取（文本内容，位置，大小等信息）
// 6. 渲染文本

struct _PDFDocumentStruct
{
	IHE_Read *			pReadItf;
	CHE_PDF_File *		pFileObj;
	CHE_PDF_Document *	pDocObj;
};


HE_VOID _InitPDFDocumentStruct( _PDFDocumentStruct * pSrt )
{
	if ( pSrt )
	{
		pSrt->pReadItf = NULL;
		pSrt->pFileObj = NULL;
		pSrt->pDocObj = NULL;
	}
};

HE_VOID _DestroyPDFDocumentStruct( _PDFDocumentStruct * pSrt )
{
	if ( pSrt )
	{
		if ( pSrt->pDocObj )
		{
			pSrt->pDocObj->GetAllocator()->Delete<CHE_PDF_Document>( pSrt->pDocObj );
			pSrt->pDocObj = NULL;
		}
		if ( pSrt->pFileObj )
		{
			pSrt->pFileObj->Close();
			pSrt->pFileObj->GetAllocator()->Delete<CHE_PDF_File>( pSrt->pFileObj );
			pSrt->pFileObj = NULL;
		}
		if ( pSrt->pReadItf )
		{
			HE_DestoryIHERead( pSrt->pReadItf );
			pSrt->pReadItf = NULL;
		}
	}
}


PDFDocument CHEPDF_OpenDocument( const char * pFilePath )
{
	PDFDocument docRet = NULL;

	_PDFDocumentStruct * pPDFDocSrt = GetDefaultAllocator()->New<_PDFDocumentStruct>();
	if ( pPDFDocSrt == NULL )
	{
		return docRet;
	}

	_InitPDFDocumentStruct( pPDFDocSrt );

	pPDFDocSrt->pReadItf = HE_CreateFileRead( pFilePath );
	if ( pPDFDocSrt->pReadItf == NULL )
	{
		return docRet;
	}

	pPDFDocSrt->pFileObj = GetDefaultAllocator()->New<CHE_PDF_File>( GetDefaultAllocator() );
	if ( pPDFDocSrt->pFileObj == NULL )
	{
		_DestroyPDFDocumentStruct( pPDFDocSrt );
		return docRet;
	}

	pPDFDocSrt->pFileObj->Open( pPDFDocSrt->pReadItf );

	pPDFDocSrt->pDocObj = CHE_PDF_Document::CreateDocument( pPDFDocSrt->pFileObj );
	if ( pPDFDocSrt->pDocObj == NULL )
	{
		_DestroyPDFDocumentStruct( pPDFDocSrt );
		return docRet;
	}

	docRet = (PDFDocument)pPDFDocSrt;

	return docRet;
}


void CHEPDF_CloseDocument( PDFDocument doc )
{
	if ( doc )
	{
		_PDFDocumentStruct * pDocSrt = (_PDFDocumentStruct*)doc;
		_DestroyPDFDocumentStruct( pDocSrt );
		GetDefaultAllocator()->Delete<_PDFDocumentStruct>( pDocSrt );
	}
}


int CHEPDF_GetPageCount( PDFDocument doc )
{
	if ( doc )
	{
		_PDFDocumentStruct * pDocSrt = (_PDFDocumentStruct*)doc;
		if ( pDocSrt->pDocObj )
		{
			return pDocSrt->pDocObj->GetPageCount();
		}
	}
	return -1;
}

struct _PDFPageStruct
{
	CHE_PDF_Document * pDocObj;
	CHE_PDF_Page * pPageObj;
};


PDFPage CHEPDF_GetPage( PDFDocument doc, unsigned int index )
{
	_PDFPageStruct * pPageSrt = NULL;
	if ( doc )
	{
		_PDFDocumentStruct * pDocSrt = (_PDFDocumentStruct*)doc;
		if ( pDocSrt->pDocObj )
		{
			if ( index < pDocSrt->pDocObj->GetPageCount() )
			{
				pPageSrt = GetDefaultAllocator()->New<_PDFPageStruct>();
				if ( pPageSrt )
				{
					pPageSrt->pDocObj = pDocSrt->pDocObj;
					pPageSrt->pPageObj = pDocSrt->pDocObj->GetPage( index );
					return pPageSrt;
				}
			}
		}
	}

	return NULL;
}


void CHEPDF_ClosePage( PDFPage page )
{
	if ( page )
	{
		_PDFPageStruct * pPageSrt = (_PDFPageStruct*)page;
		if ( pPageSrt->pPageObj )
		{
			pPageSrt->pPageObj->GetAllocator()->Delete<CHE_PDF_Page>( pPageSrt->pPageObj );
		}
		GetDefaultAllocator()->Delete<_PDFPageStruct>( pPageSrt );
	}
}


struct _PDFPageContentStruct
{
	//HE_DWORD index;
	ContentObjectList::iterator it;
	CHE_PDF_ContentObjectList * pContentObjList;
	
};

PDFPageContent CHEPDF_GetPageContent( PDFPage page )
{
	PDFPageContent pageCtxRet = NULL;
	if ( page )
	{
		_PDFPageStruct * pPageSrt = (_PDFPageStruct*)page;
		if ( pPageSrt->pPageObj && pPageSrt->pDocObj )
		{
			_PDFPageContentStruct * pPageContentSrt = GetDefaultAllocator()->New<_PDFPageContentStruct>();
			if ( pPageContentSrt )
			{
				pPageContentSrt->pContentObjList = GetDefaultAllocator()->New<CHE_PDF_ContentObjectList>();
				if ( pPageContentSrt->pContentObjList )
				{
					//pPageContentSrt->index = 0;
					if ( GetPageContent( pPageSrt->pPageObj->GetPageDict(), pPageContentSrt->pContentObjList, pPageSrt->pDocObj->GetFontMgr() ) )
					{
						pageCtxRet = (PDFPageContent)pPageContentSrt;
					}else{
						GetDefaultAllocator()->Delete<CHE_PDF_ContentObjectList>( pPageContentSrt->pContentObjList );
						GetDefaultAllocator()->Delete<_PDFPageContentStruct>( pPageContentSrt );
					}
				}else{
					GetDefaultAllocator()->Delete<_PDFPageContentStruct>( pPageContentSrt );
				}
			}
		}
	}

	return pageCtxRet;
}


void CHEPDF_ReleasePageContent( PDFPageContent content )
{
	if ( content )
	{
		_PDFPageContentStruct * pPageContentSrt = (_PDFPageContentStruct *)content;
		if ( pPageContentSrt->pContentObjList )
		{
			GetDefaultAllocator()->Delete<CHE_PDF_ContentObjectList>( pPageContentSrt->pContentObjList );
		}
		GetDefaultAllocator()->Delete<_PDFPageContentStruct>( pPageContentSrt );
	}
}


// struct _PDFPageTextStruct
// {
// 	CHE_PDF_Text * pTextObj;
// };


PDFPageText	CHEPDF_GetFirstPageText( PDFPageContent content )
{
	PDFPageText * pPageText = NULL;
	if ( content )
	{
		_PDFPageContentStruct * pPageCtxSrt = (_PDFPageContentStruct*)( content );
		if ( pPageCtxSrt->pContentObjList )
		{
			ContentObjectList::iterator it = pPageCtxSrt->pContentObjList->Begin();
			pPageCtxSrt->it = pPageCtxSrt->pContentObjList->End();
			for ( ; it != pPageCtxSrt->pContentObjList->End(); ++it )
			{
				if ( (*it)->GetType() == ContentType_Text )
				{
					pPageCtxSrt->it = it;
					pPageText = (PDFPageText *)( *it );
				}
			}
		}
	}
	return pPageText;
}

PDFPageText	CHEPDF_GetNextPageText( PDFPageContent content )
{
	PDFPageText * pPageText = NULL;
	if ( content )
	{
		_PDFPageContentStruct * pPageCtxSrt = (_PDFPageContentStruct*)( content );
		if ( pPageCtxSrt->pContentObjList )
		{
			ContentObjectList::iterator it = pPageCtxSrt->it;
			for ( ; it != pPageCtxSrt->pContentObjList->End(); ++it )
			{
				if ( (*it)->GetType() == ContentType_Text )
				{
					pPageCtxSrt->it = it;
					pPageText = (PDFPageText *)( &(*it) );
				}
			}
		}
	}
	return pPageText;
}


// Matrix CHEPDF_GetTextMatrix( PDFPageText text )
// {
// 	Matrix mtx;
// 	mtx.a = 1;
// 	mtx.b = 0;
// 	mtx.c = 0;
// 	mtx.d = 1;
// 	mtx.e = 0;
// 	mtx.f = 0;
// 	if ( text )
// 	{
// 		CHE_PDF_Text * pTextObj = (CHE_PDF_Text*)( text );
// 		CHE_PDF_GState * pGState = pTextObj->GetGState();
// 		if ( pGState )
// 		{
// 			CHE_PDF_Matrix matrix = pGState->GetMatrix();
// 			CHE_PDF_Matrix textMtx;
// 			pGState->GetTextMatrix( textMtx );
// 			textMtx.Concat( matrix );
// 			mtx.a = textMtx.a;
// 			mtx.b = textMtx.b;
// 			mtx.c = textMtx.c;
// 			mtx.d = textMtx.d;
// 			mtx.e = textMtx.e;
// 			mtx.f = textMtx.f;
// 		}
// 	}
// 	return mtx;
// }


Position CHEPDF_GetTextPosition( PDFPageText text )
{
	Position posi;
	posi.x = 0;
	posi.y = 0;
	
	if ( text )
	{
		CHE_PDF_ContentObject * pCtxObj = (CHE_PDF_ContentObject*)( text );
		if ( pCtxObj->GetType() != ContentType_Text )
		{
			return posi;
		}
		CHE_PDF_GState * pGState = pCtxObj->GetGState();
		CHE_PDF_Text * pTextObj = (CHE_PDF_Text *)( pCtxObj );
		if ( pGState )
		{
			CHE_PDF_Matrix matrix = pGState->GetMatrix();
			CHE_PDF_Matrix textMtx;
			pGState->GetTextMatrix( textMtx );
			textMtx.Concat( matrix );
			posi.x = textMtx.e;
			posi.y = textMtx.f;
		}
	}

	return posi;
}


unsigned int CHEPDF_GetTextLength( PDFPageText text )
{
	if ( text )
	{
		CHE_PDF_Text * pTextObj = (CHE_PDF_Text*)( text );
		return pTextObj->mItems.size();
	}
	return 0;
}


//还应该有办法判断字体是否存在unicode码?? 如何判断？
PDFStatus CHEPDF_GetTextUnicodes( PDFPageText text, wchar_t * pBuf, unsigned int bufSize )
{
	if ( pBuf == NULL )
	{
		return -2;
	}
	if ( text )
	{
		PDF_FONT_TYPE fontType = FONT_TYPE1;
		CHE_PDF_Text * pTextObj = (CHE_PDF_Text*)( text );
		CHE_PDF_GState * pGState = pTextObj->GetGState();
		if ( pGState )
		{
			CHE_PDF_Font * pFont = pGState->GetTextFont();
			if ( pFont )
			{
				fontType = pFont->GetType();
			}
		}
		if ( pTextObj->mItems.size() == 0 )
		{
			return -3;
		}
		if ( pTextObj->mItems.size() > bufSize )
		{
			return -4;
		}
		unsigned int i = 0;
		for ( ; i < pTextObj->mItems.size(); ++i )
		{
			if ( fontType == FONT_TYPE0 )
			{
				*(pBuf + i) = pTextObj->mItems[i].ucs;
			}else{
				if ( pTextObj->mItems[i].ucs != 0 )
				{
					*(pBuf + i) = pTextObj->mItems[i].ucs;
				}else{
					*(pBuf + i) = pTextObj->mItems[i].charCode;
				}
			}
		}
		*(pBuf + i) = L'\0';
	}
	return 0;
}