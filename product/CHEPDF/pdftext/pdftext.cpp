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
					break;
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
			for ( ++it; it != pPageCtxSrt->pContentObjList->End(); ++it )
			{
				if ( (*it)->GetType() == ContentType_Text )
				{
					pPageCtxSrt->it = it;
					pPageText = (PDFPageText *)( *it );
					break;
				}
			}
		}
	}
	return pPageText;
}


PDFMatrix CHEPDF_GetTextMatrix( PDFPageText text )
{
	PDFMatrix mtx;
	mtx.a = 1;
	mtx.b = 0;
	mtx.c = 0;
	mtx.d = 1;
	mtx.e = 0;
	mtx.f = 0;
	if ( text )
	{
		CHE_PDF_Text * pTextObj = (CHE_PDF_Text*)( text );
		CHE_PDF_GState * pGState = pTextObj->GetGState();
		if ( pGState )
		{
			CHE_PDF_Matrix matrix = pGState->GetMatrix();
			CHE_PDF_Matrix textMtx;
			pGState->GetTextMatrix( textMtx );
			textMtx.Concat( matrix );
			mtx.a = textMtx.a;
			mtx.b = textMtx.b;
			mtx.c = textMtx.c;
			mtx.d = textMtx.d;
			mtx.e = textMtx.e;
			mtx.f = textMtx.f;
		}
	}
	return mtx;
}


PDFPosition CHEPDF_GetTextPosition( PDFPageText text )
{
	PDFPosition posi;
	posi.x = 0;
	posi.y = 0;
	
	if ( text )
	{
		PDFMatrix matrix = CHEPDF_GetTextMatrix( text );
		posi.x = matrix.e;
		posi.y = matrix.f;
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


struct _PDFPageCharStruct
{
	CHE_PDF_Text * pText;
	unsigned int index;
};


PDFPageChar	CHEPDF_GetPageChar( PDFPageText text, unsigned int index )
{
	PDFPageChar * pCharRet = NULL;
	if ( text == NULL )
	{
		return pCharRet;
	}
	CHE_PDF_Text * pText = (CHE_PDF_Text*)( text );
	if ( index >= pText->mItems.size()  )
	{
		return pCharRet;
	}
	_PDFPageCharStruct * pCharStruct = GetDefaultAllocator()->New<_PDFPageCharStruct>();
	pCharStruct->pText = pText;
	pCharStruct->index = index;
	return (_PDFPageCharStruct*)( pCharStruct );
}


void CHEPDF_ClosePageChar( PDFPageChar textChar )
{
	if ( textChar != NULL )
	{
		_PDFPageCharStruct * pCharStruct = (_PDFPageCharStruct*)( textChar );
		GetDefaultAllocator()->Delete( pCharStruct );
	}
}


PDFMatrix CHEPDF_GetCharMatirx( PDFPageChar textChar )
{
	PDFMatrix mtx;
	mtx.a = 1;
	mtx.b = 0;
	mtx.c = 0;
	mtx.d = 1;
	mtx.e = 0;
	mtx.f = 0;
	if ( textChar == NULL )
	{
		return mtx;
	}
	_PDFPageCharStruct * pCharStruct = (_PDFPageCharStruct*)( textChar );
	CHE_PDF_Text * pTextObj = pCharStruct->pText;
	if ( pTextObj == NULL || pCharStruct->index >= pTextObj->mItems.size() )
	{
		return mtx;
	}
	CHE_PDF_GState * pGState = pTextObj->GetGState();
	if ( pGState )
	{
		CHE_PDF_Matrix tmpMatrix;
		HE_FLOAT fontSize = 1;
		HE_FLOAT fontScaling = 100;
		HE_FLOAT fontRise = 0;
		HE_FLOAT fontCharSpace = 0;
		HE_FLOAT fontWordSpace = 0;
		pGState->GetTextFontSize( fontSize );
		pGState->GetTextScaling( fontScaling );
		pGState->GetTextRise( fontRise );
		pGState->GetTextCharSpace( fontCharSpace );
		pGState->GetTextWordSpace( fontWordSpace );
		tmpMatrix.a = fontSize * fontScaling / 100;
		tmpMatrix.b = 0;
		tmpMatrix.c = 0;
		tmpMatrix.d = fontSize;
		tmpMatrix.e = 0;
		tmpMatrix.f = fontRise;

		CHE_PDF_Matrix textMatrix;
		pGState->GetTextMatrix( textMatrix );
		
		CHE_PDF_Matrix ctm = pGState->GetMatrix();
		
		tmpMatrix.Concat( textMatrix );
		tmpMatrix.Concat( ctm );

		for ( unsigned int i = 0; i < pCharStruct->index; ++i )
		{
			//计算字符相对于Text Object的起始点的偏移，依据字体横排或者竖排的不同，有不同的计算方法。
			//这里面的计算应该使用字体大小，字体大小的运算在外层的矩阵中参与了。
			HE_FLOAT OffsetX = 0;
			HE_FLOAT OffsetY = 0;
			CHE_PDF_Matrix OffsetMatrix;
			OffsetX = ( ( pTextObj->mItems[i].width + pTextObj->mItems[i].kerning / 1000 ) + fontCharSpace );
			//OffsetY = ( ( pTextObj->mItems[i].width + pTextObj->mItems[i].kerning / 1000 ) + fontCharSpace );
			if ( pTextObj->mItems[i].ucs == L' ' )
			{
				OffsetX += fontWordSpace;
			}
			OffsetMatrix.e = OffsetX;
			OffsetMatrix.f = OffsetY;
			OffsetMatrix.Concat( tmpMatrix );
			tmpMatrix = OffsetMatrix;
		}
	
		mtx.a = tmpMatrix.a;
		mtx.b = tmpMatrix.b;
		mtx.c = tmpMatrix.c;
		mtx.d = tmpMatrix.d;
		mtx.e = tmpMatrix.e;
		mtx.f = tmpMatrix.f;
	}

	return mtx;
}


PDFPosition	CHEPDF_GetCharPosition( PDFPageChar textChar )
{
	PDFPosition posi;
	posi.x = 0;
	posi.y = 0;

	if ( textChar )
	{
		PDFMatrix matrix = CHEPDF_GetCharMatirx( textChar );
		posi.x = matrix.e;
		posi.y = matrix.f;
	}

	return posi;
}


PDFStatus CHEPDF_GetCharUnicode( PDFPageChar textChar, wchar_t * UnicodeRet )
{
	PDFStatus status = 0;
	if ( textChar == NULL )
	{
		return -1;
	}
	_PDFPageCharStruct * pCharStruct = (_PDFPageCharStruct*)( textChar );
	CHE_PDF_Text * pTextObj = pCharStruct->pText;
	if ( pTextObj == NULL || pCharStruct->index >= pTextObj->mItems.size() )
	{
		return -2;
	}

	*UnicodeRet = pTextObj->mItems[pCharStruct->index].ucs;

	return 0;
}