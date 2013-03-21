#include "pdftext.h"
#include "../../../trunk/include/pdf/che_pdf_file.h"
#include "../../../trunk/include/pdf/che_pdf_document.h"
#include "../../../trunk/include/pdf/che_pdf_contents.h"
#include "../../../trunk/extlib/freetype/ft2build.h"
#include "../../../trunk/extlib/freetype/freetype/freetype.h"
#include "../../../trunk/extlib/freetype/freetype/ftglyph.h"
#include "../../../trunk/extlib/freetype/freetype/ftrender.h"
#include "../../../trunk/include/che_bitmap.h"

#include <GdiPlus.h>

// 1. 加载文档对象
// 2. 加载页面对象
// 3. 解析页面内容
// 4. 按顺序获取页面内容
// 5. 文本内容信息获取（文本内容，位置，大小等信息）
// 6. 渲染文本


#define PIXELPERINCH 96


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


#define MAX4(a,b,c,d) fz_max(fz_max(a,b), fz_max(c,d))
#define MIN4(a,b,c,d) fz_min(fz_min(a,b), fz_min(c,d))

#define fz_is_empty_rect(r) ((r).width == 0 || (r).height == 0 )

static inline float fz_min(float a, float b)
{
	return (a < b ? a : b);
}

static inline float fz_max(float a, float b)
{
	return (a > b ? a : b);
}

PDFPosition fz_transform_point( PDFMatrix matrix, PDFPosition point )
{
	PDFPosition pointRet;
	pointRet.x = point.x * matrix.a + point.y * matrix.c + matrix.e;
	pointRet.y = point.x * matrix.b + point.y * matrix.d + matrix.f;
	return pointRet;
}

PDFRect fz_transform_rect( PDFMatrix m, PDFRect r )
{
	PDFPosition s, t, u, v;	

	s.x = r.left;
	s.y = r.bottom;
	t.x = r.left;
	t.y = r.bottom + r.height;
	u.x = r.left + r.width;
	u.y = r.bottom + r.height;
	v.x = r.left + r.width;
	v.y = r.bottom;
	s = fz_transform_point(m, s);
	t = fz_transform_point(m, t);
	u = fz_transform_point(m, u);
	v = fz_transform_point(m, v);
	r.left = MIN4(s.x, t.x, u.x, v.x);
	r.bottom = MIN4(s.y, t.y, u.y, v.y);
	r.width = MAX4(s.x, t.x, u.x, v.x) - MIN4(s.x, t.x, u.x, v.x);
	r.height = MAX4(s.y, t.y, u.y, v.y) - MIN4(s.y, t.y, u.y, v.y);
	return r;
}

PDFRect fz_union_rect( PDFRect a, PDFRect b )
{
	PDFRect r;
	if (fz_is_empty_rect(a)) return b;
	if (fz_is_empty_rect(b)) return a;
	r.left = fz_min(a.left, b.left);
	r.bottom = fz_min(a.bottom, b.bottom);
	r.width = fz_max(a.left + a.width, b.left + b.width) - r.left;
	r.height = fz_max(a.bottom + a.height, b.bottom + b.height) - r.bottom;
	return r;
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


PDFRect CHEPDF_GetPageBox( PDFPage page )
{
	PDFRect rectRet;
	rectRet.left = 0;
	rectRet.bottom = 0;
	rectRet.width = 0;
	rectRet.height = 0;
	if ( page )
	{
		_PDFPageStruct * pPageSrt = (_PDFPageStruct*)page;
		CHE_PDF_ArrayPtr arrayPtr = pPageSrt->pPageObj->GetMediaBoxArray();
		if ( arrayPtr )
		{
			CHE_PDF_ObjectPtr objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				rectRet.left = objPtr->GetNumberPtr()->GetFloat();
			}
			objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				rectRet.bottom = objPtr->GetNumberPtr()->GetFloat();
			}
			objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				rectRet.width = objPtr->GetNumberPtr()->GetFloat();
			}
			objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				rectRet.height = objPtr->GetNumberPtr()->GetFloat();
			}
		}
	}
	return rectRet;
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
		PDFPageChar textChar = CHEPDF_GetPageChar( text, 0 );
		if ( textChar )
		{
			mtx = CHEPDF_GetCharMatirx( textChar );
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
		return PDF_STATUS_PARAM_ERR;
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
			return PDF_STATUS_NOTEXT_ERR;
		}
		if ( pTextObj->mItems.size() > bufSize )
		{
			return PDF_STATUS_BUFFER_TOO_SMALL;
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
	return PDF_STATUS_OK;
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
	CHE_PDF_Matrix tmpMatrix = pTextObj->GetCharMatrix( pCharStruct->index );
	mtx.a = tmpMatrix.a;
	mtx.b = tmpMatrix.b;
	mtx.c = tmpMatrix.c;
	mtx.d = tmpMatrix.d;
	mtx.e = tmpMatrix.e;
	mtx.f = tmpMatrix.f;
// 	CHE_PDF_GState * pGState = pTextObj->GetGState();
// 	if ( pGState )
// 	{
// 		CHE_PDF_Matrix tmpMatrix;
// 		HE_FLOAT fontSize = 1;
// 		HE_FLOAT fontScaling = 100;
// 		HE_FLOAT fontRise = 0;
// 		HE_FLOAT fontCharSpace = 0;
// 		HE_FLOAT fontWordSpace = 0;
// 		pGState->GetTextFontSize( fontSize );
// 		pGState->GetTextScaling( fontScaling );
// 		pGState->GetTextRise( fontRise );
// 		pGState->GetTextCharSpace( fontCharSpace );
// 		pGState->GetTextWordSpace( fontWordSpace );
// 		tmpMatrix.a = fontSize * fontScaling / 100;
// 		tmpMatrix.b = 0;
// 		tmpMatrix.c = 0;
// 		tmpMatrix.d = fontSize;
// 		tmpMatrix.e = 0;
// 		tmpMatrix.f = fontRise;
// 
// 		CHE_PDF_Matrix textMatrix;
// 		pGState->GetTextMatrix( textMatrix );
// 		
// 		CHE_PDF_Matrix ctm = pGState->GetMatrix();
// 		
// 		tmpMatrix.Concat( textMatrix );
// 		tmpMatrix.Concat( ctm );
// 
// 		unsigned int i = 0;
// 		for ( ; i < pCharStruct->index; ++i )
// 		{
// 			//计算字符相对于Text Object的起始点的偏移，依据字体横排或者竖排的不同，有不同的计算方法。
// 			//这里面的计算应该使用字体大小，字体大小的运算在外层的矩阵中参与了。
// 			HE_FLOAT OffsetX = 0;
// 			HE_FLOAT OffsetY = 0;
// 			CHE_PDF_Matrix OffsetMatrix;
// 			OffsetX = ( ( pTextObj->mItems[i].width - pTextObj->mItems[i].kerning / 1000 ) + fontCharSpace );
// 			//OffsetY = ( ( pTextObj->mItems[i].width + pTextObj->mItems[i].kerning / 1000 ) + fontCharSpace );
// 			if ( pTextObj->mItems[i].ucs == L' ' )
// 			{
// 				OffsetX += fontWordSpace;
// 			}
// 			OffsetMatrix.e = OffsetX;
// 			OffsetMatrix.f = OffsetY;
// 			OffsetMatrix.Concat( tmpMatrix );
// 			tmpMatrix = OffsetMatrix;
// 		}
// 
// 		if ( i <= pCharStruct->index )
// 		{
// 			HE_FLOAT OffsetX = 0;
// 			HE_FLOAT OffsetY = 0;
// 			CHE_PDF_Matrix OffsetMatrix;
// 			OffsetX = ( 0 - pTextObj->mItems[i].kerning / 1000 );
// 			//OffsetY = ( ( pTextObj->mItems[i].width + pTextObj->mItems[i].kerning / 1000 ) + fontCharSpace );
// 			OffsetMatrix.e = OffsetX;
// 			OffsetMatrix.f = OffsetY;
// 			OffsetMatrix.Concat( tmpMatrix );
// 			tmpMatrix = OffsetMatrix;
// 		}
// 
// 		mtx.a = tmpMatrix.a;
// 		mtx.b = tmpMatrix.b;
// 		mtx.c = tmpMatrix.c;
// 		mtx.d = tmpMatrix.d;
// 		mtx.e = tmpMatrix.e;
// 		mtx.f = tmpMatrix.f;
// 	}

	return mtx;
}


PDFPosition	CHEPDF_GetCharPosition( PDFPageChar textChar )
{
	PDFRect rect = CHEPDF_GetCharBox( textChar );
	PDFPosition posi;
	posi.x = rect.left;
	posi.y = rect.bottom;
	return posi;
	
// 	PDFPosition posi;
// 	posi.x = 0;
// 	posi.y = 0;
// 
// 	if ( textChar )
// 	{
// 		PDFMatrix matrix = CHEPDF_GetCharMatirx( textChar );
// 		posi.x = matrix.e;
// 		posi.y = matrix.f;
// 	}
	return posi;
}


PDFStatus CHEPDF_GetCharUnicode( PDFPageChar textChar, wchar_t * UnicodeRet )
{
	PDFStatus status = 0;
	if ( textChar == NULL )
	{
		return PDF_STATUS_PARAM_ERR;
	}
	_PDFPageCharStruct * pCharStruct = (_PDFPageCharStruct*)( textChar );
	CHE_PDF_Text * pTextObj = pCharStruct->pText;
	if ( pTextObj == NULL || pCharStruct->index >= pTextObj->mItems.size() )
	{
		return PDF_STATUS_NOTEXT_ERR;
	}

	*UnicodeRet = pTextObj->mItems[pCharStruct->index].ucs;

	return PDF_STATUS_OK;
}


PDFRect CHEPDF_GetCharBox( PDFPageChar textChar )
{
	PDFRect rect;
	rect.left = 0;
	rect.bottom = 0;
	rect.width = 0;
	rect.height = 0;

	if ( textChar )
	{
		PDFMatrix matrix = CHEPDF_GetCharMatirx( textChar );
		_PDFPageCharStruct * pCharStruct = (_PDFPageCharStruct*)( textChar );
		CHE_PDF_Text * pTextObj = pCharStruct->pText;
		CHE_PDF_Rect tmpRect = pTextObj->GetCharRect( pCharStruct->index );
		rect.left = tmpRect.left;
		rect.bottom = tmpRect.bottom;
		rect.width = tmpRect.width;
		rect.height = tmpRect.height;
// 		CHE_PDF_GState * pGState = pTextObj->GetGState();
// 		FT_Face face = NULL;
// 		if ( pGState )
// 		{
// 			face = pGState->GetTextFont()->GetFTFace();
// 		}
// 		if ( pTextObj && pCharStruct->index < pTextObj->mItems.size() )
// 		{
// 			rect.width = pTextObj->mItems[pCharStruct->index].width;
// 			rect.height = pTextObj->mItems[pCharStruct->index].height;
// 			if ( face )
// 			{
// 				rect.bottom = face->descender * 1.0 / face->units_per_EM;
// 				rect.height = ( face->ascender - face->descender ) * 1.0 / face->units_per_EM;
// 			}
// 			rect = fz_transform_rect( matrix, rect );
// 		}
	}
	return rect;
}


PDFRect	CHEPDF_GetTextBox( PDFPageText text )
{
	PDFRect rect;
	rect.bottom = 0;
	rect.left = 0;
	rect.width = 0;
	rect.height = 0;

	for ( unsigned int i = 0; i< CHEPDF_GetTextLength( text ); ++i )
	{
		PDFPageChar textChar = CHEPDF_GetPageChar( text, i );
		if ( textChar )
		{
			PDFRect tmpRect = CHEPDF_GetCharBox( textChar );
			rect = fz_union_rect( tmpRect, rect );
		}
	}

	return rect;
}


PDFStatus _CHEPDF_RenderGlyph( PDFPageChar textChar, float sclae /*= 1*/ )
{
	_PDFPageCharStruct * pCharStruct = (_PDFPageCharStruct*)( textChar );
	CHE_PDF_Text * pTextObj = pCharStruct->pText;
	if ( pTextObj && pCharStruct->index < pTextObj->mItems.size() )
	{
		PDFMatrix mtx = CHEPDF_GetCharMatirx( textChar );
		CHE_PDF_GState * pGState = pTextObj->GetGState();
		if ( pGState )
		{
			CHE_PDF_Font * pFont = pGState->GetTextFont();
			if ( pFont )
			{
				if ( pFont->GetFTFace() )
				{
					FT_Error err = FT_Set_Char_Size( pFont->GetFTFace(), 65536 * sclae, 65536 * sclae, PIXELPERINCH, PIXELPERINCH );
					FT_Matrix matrix;
					matrix.xx = mtx.a * 64;
					matrix.yx = mtx.b * 64;
					matrix.xy = mtx.c * 64;
					matrix.yy = mtx.d * 64;
					FT_Set_Transform( pFont->GetFTFace(), &matrix, NULL );
					FT_UInt gid = pTextObj->mItems[pCharStruct->index].gid;
					err = FT_Load_Glyph( pFont->GetFTFace(), gid, /*FT_LOAD_DEFAULT*/FT_LOAD_NO_BITMAP | FT_LOAD_TARGET_MONO );
					//FT_Load_Char( pFont->GetFTFace(), pTextObj->mItems[pCharStruct->index].charCode, FT_LOAD_DEFAULT );
					err = FT_Render_Glyph( pFont->GetFTFace()->glyph, FT_RENDER_MODE_NORMAL );
					return PDF_STATUS_OK;
				}
			}
		}
	}
	return PDF_STATUS_ERROR;
}


PDFBitmap CHEPDF_RenderText( PDFPageText text, float sclae /*= 1*/ )
{
	PDFBitmap * pBitmap = NULL;
	if ( text )
	{
		PDFRect bbox = CHEPDF_GetTextBox( text );
		CHE_Bitmap * pBitmapRet = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
		pBitmapRet->Create( bbox.width * sclae * PIXELPERINCH / 72, bbox.height * sclae * PIXELPERINCH / 72, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_UP );
		pBitmapRet->Fill( 0xFFFFFFFF );

		CHE_PDF_Text * pText = (CHE_PDF_Text*)text;
		CHE_PDF_Font * pFont = pText->GetGState()->GetTextFont();
		FT_Face face = pFont->GetFTFace();
		if ( face )
		{
			int xPosition = 0;
			int yBaseline = 0;
			for ( unsigned int i = 0; i < pText->mItems.size(); ++i )
			{
				PDFPageChar textChar = CHEPDF_GetPageChar( text, i );
				PDFMatrix cmtx = CHEPDF_GetCharMatirx( textChar );
				PDFPosition oriPoint;
				oriPoint.x = cmtx.e;
				oriPoint.y = cmtx.f;
				yBaseline = ( bbox.bottom + bbox.height - oriPoint.y ) * sclae * PIXELPERINCH / 72;
				xPosition = ( oriPoint.x - bbox.left ) * sclae * PIXELPERINCH / 72;

				_CHEPDF_RenderGlyph( textChar, sclae );
				if ( face->glyph && face->glyph->bitmap.buffer )
				{
					FT_Bitmap & bitmap = face->glyph->bitmap;
					HE_ARGB argb = 0;
					for ( int i = 0; i < bitmap.width; ++i )
					{
						for ( int j = 0; j < bitmap.rows; ++j )
						{
							if ( bitmap.buffer[i + bitmap.width*j] != 0x00 )
							{
								argb = 0xFFFFFFFF - ( bitmap.buffer[i + bitmap.width*j] | bitmap.buffer[i + bitmap.width*j] << 8 | bitmap.buffer[i + bitmap.width*j] << 16 );
								pBitmapRet->SetPixelColor( i + xPosition + face->glyph->bitmap_left, j + yBaseline - face->glyph->bitmap_top, argb );
							}
						}
					}
				}
			}
			pBitmap = (PDFBitmap *)( pBitmapRet );
		}
	}
	return pBitmap;
}


PDFBitmap CHEPDF_RenderChar( PDFPageChar textChar, float scale /*= 1*/ )
{
	CHE_Bitmap * pBitmap = NULL;
	if ( textChar )
	{
		_PDFPageCharStruct * pCharStruct = (_PDFPageCharStruct*)( textChar );
		CHE_PDF_Text * pTextObj = pCharStruct->pText;
		if ( pTextObj && pCharStruct->index < pTextObj->mItems.size() )
		{
			_CHEPDF_RenderGlyph( textChar, scale );

			CHE_PDF_Font * pFont = pTextObj->GetGState()->GetTextFont();
			if ( pFont->GetFTFace() && pFont->GetFTFace()->glyph && pFont->GetFTFace()->glyph->bitmap.buffer )
			{
				PDFMatrix mtx = CHEPDF_GetCharMatirx( textChar );
				PDFRect bbox = CHEPDF_GetCharBox( textChar );

				PDFPosition baselinePoint;
				baselinePoint.x = mtx.e;
				baselinePoint.y = mtx.f;

				int yBaseline = ( bbox.bottom + bbox.height - baselinePoint.y ) * scale * PIXELPERINCH / 72;
				int xBaseline = ( baselinePoint.x - bbox.left ) * scale * PIXELPERINCH / 72;

				pBitmap = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
				if ( pBitmap )
				{
					pBitmap->Create( bbox.width * scale * PIXELPERINCH / 72, bbox.height * scale * PIXELPERINCH / 72, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_UP );
					pBitmap->Fill( 0xFFFFFFFF );
				}

				FT_Bitmap & bitmap = pFont->GetFTFace()->glyph->bitmap;
				HE_ARGB argb = 0;
				for ( int i = 0; i < bitmap.width; ++i )
				{
					for ( int j = 0; j < bitmap.rows; ++j )
					{
						if ( bitmap.buffer[i + bitmap.width*j] != 0x00 )
						{
							argb = 0xFFFFFFFF - ( bitmap.buffer[i + bitmap.width*j] | bitmap.buffer[i + bitmap.width*j] << 8 | bitmap.buffer[i + bitmap.width*j] << 16 );
							pBitmap->SetPixelColor( i + xBaseline + pFont->GetFTFace()->glyph->bitmap_left, j + yBaseline - pFont->GetFTFace()->glyph->bitmap_top, argb );
						}
					}
				}
			}
		}
	}
	return pBitmap;
}

void CHEPDF_CloseBitmap( PDFBitmap bitmap )
{
	if ( bitmap )
	{
		CHE_Bitmap * pBitmap = (CHE_Bitmap*)( bitmap );
		GetDefaultAllocator()->Delete<CHE_Bitmap>( pBitmap );
	}
}


unsigned int CHEPDF_GetBitmapWidth( PDFBitmap bitmap )
{
	if ( bitmap )
	{
		CHE_Bitmap * pBitmap = (CHE_Bitmap*)( bitmap );
		return pBitmap->Width();
	}
	return 0;
}


unsigned int CHEPDF_GetBitmapHeight( PDFBitmap bitmap )
{
	if ( bitmap )
	{
		CHE_Bitmap * pBitmap = (CHE_Bitmap*)( bitmap );
		return pBitmap->Height();
	}
	return 0;
}


PDFStatus CHEPDF_SaveBitmapToFile( PDFBitmap bitmap, char * filePath )
{
	if ( bitmap && filePath )
	{
		CHE_Bitmap * pBitmap = (CHE_Bitmap*)( bitmap );
		if ( pBitmap->Save( filePath ) )
		{
			return PDF_STATUS_OK;
		}
	}
	return PDF_STATUS_ERROR;
}

struct _PDFPageWordStruct
{
	std::vector<_PDFPageCharStruct> charVector;
};

struct _PDFPageWordSetStruct
{
	std::vector<_PDFPageWordStruct> wordVector;
	std::vector<_PDFPageWordStruct>::iterator it;
};

int _JoinRectToList( std::list<PDFRect> & rectlist, PDFRect rect, std::list<PDFRect>::iterator & itRet )
{
	int index = -1;
	std::list<PDFRect>::iterator it = rectlist.begin();
	bool bMatch = false;
	for ( ; it != rectlist.end(); ++it )
	{
		PDFRect tmpRect = *it;
		++index;
		//rect在tmpRect之后
		if ( fabs( rect.bottom - tmpRect.bottom ) < 0.5 && fabs( tmpRect.left + tmpRect.width - rect.left ) < 0.5  )
		{
			*it = fz_union_rect( tmpRect, rect );
			bMatch = true;
			break;
		}
		//rect在tmpRect之前。。。其实是不是可以不考虑呢？
	}
	if ( bMatch )
	{
		itRet = it;
	}else{
		index = -1;
		rectlist.push_back( rect );
	}
	return index;
}

PDFPageWordSet CHEPDF_GetPageWordSet( PDFPageContent content )
{
	_PDFPageWordSetStruct * pPageWordSet = NULL;
	std::list<PDFRect> rectList;
	std::list<PDFRect>::iterator rectListIt = rectList.end();
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
					if ( pPageWordSet == NULL )
					{
						pPageWordSet = GetDefaultAllocator()->New<_PDFPageWordSetStruct>();
						pPageWordSet->it = pPageWordSet->wordVector.begin();
					}

					//获取到text object
					CHE_PDF_Text * pText = (CHE_PDF_Text *)(*it);
					if ( pText )
					{
						//开始遍历text object中的char
						for ( size_t i = 0; i < pText->mItems.size(); ++i )
						{
							//获取char的rect
							_PDFPageCharStruct pageChar;
							pageChar.index = i;
							pageChar.pText = pText;
							PDFRect rect = CHEPDF_GetCharBox( &pageChar );

							if (	pText->mItems[i].charCode == ' ' || pText->mItems[i].ucs == L' ' ||
									pText->mItems[i].charCode == '\t' || pText->mItems[i].ucs == L'\t' )
							{
								continue;
							}

							rectListIt = rectList.end();
							int retValue = _JoinRectToList( rectList, rect, rectListIt );
							if ( retValue == -1 )
							{
								//将rect放入rectlist中进行比较
								//结果一，完全独立的rect
								//新添加rect到list中，新添加一个word
								_PDFPageWordStruct pageWord;
								pageWord.charVector.push_back( pageChar );
								pPageWordSet->wordVector.push_back( pageWord );
							}else if ( retValue >= 0 && retValue < pPageWordSet->wordVector.size() )
							{
								//结果二，与某个rect连续
								//扩充rectlist的对应rect的大小，将char加入到这个word中
								pPageWordSet->wordVector[retValue].charVector.push_back( pageChar );
							}
						}
					}
				}
			}
		}
	}
	return pPageWordSet;
}


PDFPageWord	CHEPDF_GetFirstPageWord( PDFPageWordSet wordset )
{
	if ( wordset )
	{
		_PDFPageWordSetStruct * pPageWordSet = (_PDFPageWordSetStruct *)wordset;
		pPageWordSet->it = pPageWordSet->wordVector.begin();
		if ( pPageWordSet->it != pPageWordSet->wordVector.end() )
		{
			return (PDFPageWord)( &*pPageWordSet->it );
		}
	}
	return NULL;
}


PDFPageWord	CHEPDF_GetNextPageWord( PDFPageWordSet wordset )
{
	if ( wordset )
	{
		_PDFPageWordSetStruct * pPageWordSet = (_PDFPageWordSetStruct *)wordset;
		++(pPageWordSet->it);
		if ( pPageWordSet->it != pPageWordSet->wordVector.end() )
		{
			return (PDFPageWord)( &*pPageWordSet->it );
		}
	}
	return NULL;
}


void CHEPDF_ReleasePageWordSet( PDFPageWordSet wordset )
{
	if ( wordset )
	{
		_PDFPageWordSetStruct * pPageWordSet = (_PDFPageWordSetStruct*)( wordset );
		GetDefaultAllocator()->Delete<_PDFPageWordSetStruct>( pPageWordSet );
	}
}


PDFRect	CHEPDF_GetWordBox( PDFPageWord word )
{
	PDFRect rect;
	rect.bottom = 0;
	rect.left = 0;
	rect.width = 0;
	rect.height = 0;

	if ( word )
	{
		_PDFPageWordStruct * pPageWord = (_PDFPageWordStruct*)( word );
		for ( size_t i = 0; i < pPageWord->charVector.size(); ++i )
		{
			PDFRect tmpRect = CHEPDF_GetCharBox( &pPageWord->charVector[i] );
			rect = fz_union_rect( rect, tmpRect );
		}
	}

	return rect;
}


PDFStatus CHEPDF_IsWordSymbolic( PDFPageWord word )
{
	if ( ! word )
	{
		return PDF_STATUS_ERROR;
	}
	_PDFPageWordStruct * pPageWord = (_PDFPageWordStruct*)( word );
	CHE_PDF_Text * pText = NULL;
	CHE_PDF_GState * pGState = NULL;
	CHE_PDF_Font * pFont = NULL;
	CHE_PDF_FontDescriptor * pFontDescriptor = NULL;
	FT_Face tmpFace = NULL;
	for ( size_t i = 0; i < pPageWord->charVector.size(); ++i )
	{
		pText = pPageWord->charVector[i].pText;
		pGState = pText->GetGState();
		if ( pGState )
		{
			pFont = pGState->GetTextFont();
			if ( pFont )
			{
				tmpFace = pFont->GetFTFace();
				if ( tmpFace->num_faces > 1 )
				{
					pFontDescriptor = pFont->GetFontDescriptor();
					if ( pFontDescriptor )
					{
						if ( pFontDescriptor->IsSymbolic() )
						{
							return PDF_WORD_SYMBOLIC;
						}
					}
				}
			}
		}
	}
	return PDF_WORD_NOTSYMBOLIC;
}


unsigned int CHEPDF_GetWordLength( PDFPageWord word )
{
	if ( ! word )
	{
		return -1;
	}
	_PDFPageWordStruct * pWordStruct = (_PDFPageWordStruct*)( word );
	return pWordStruct->charVector.size();
}


PDFStatus CHEPDF_GetWordUnicodes( PDFPageWord word, wchar_t * pBuf, unsigned int bufSize )
{
	if ( pBuf == NULL )
	{
		return PDF_STATUS_PARAM_ERR;
	}
	if ( word )
	{
		_PDFPageWordStruct * pWordStrcut = (_PDFPageWordStruct*)( word );
		if ( pWordStrcut->charVector.size() > bufSize )
		{
			return PDF_STATUS_BUFFER_TOO_SMALL;
		}
		CHE_PDF_Text * pTextObj = NULL;
		unsigned int index = 0;
		PDF_FONT_TYPE fontType = FONT_TYPE1;
		CHE_PDF_GState * pGState = NULL;
		CHE_PDF_Font * pFont = NULL;
		size_t i = 0;
		for ( ; i < pWordStrcut->charVector.size(); ++i )
		{
			pTextObj = (CHE_PDF_Text *)( pWordStrcut->charVector[i].pText );
			index = pWordStrcut->charVector[i].index;
			fontType = FONT_TYPE1;
			pGState = pTextObj->GetGState();
			if ( pGState )
			{
				pFont = pGState->GetTextFont();
				if ( pFont )
				{ 
					fontType = pFont->GetType();
				}
			}
			if ( fontType == FONT_TYPE0 )
			{
				*(pBuf + i) = pTextObj->mItems[index].ucs;
			}else{
				if ( pTextObj->mItems[index].ucs != 0 )
				{
					*(pBuf + i) = pTextObj->mItems[index].ucs;
				}else{
					*(pBuf + i) = pTextObj->mItems[index].charCode;
				}
			}
		}
		*(pBuf + i) = L'\0';
	}
	return PDF_STATUS_OK;
}


PDFBitmap CHEPDF_RenderWord( PDFPageWord word, float sclae /*= 1*/ )
{
	PDFBitmap * pBitmap = NULL;
	if ( word )
	{
		PDFRect bbox = CHEPDF_GetWordBox( word );
		CHE_Bitmap * pBitmapRet = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
		pBitmapRet->Create( bbox.width * sclae * PIXELPERINCH / 72, bbox.height * sclae * PIXELPERINCH / 72, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_UP );
		pBitmapRet->Fill( 0xFFFFFFFF );

		_PDFPageWordStruct * pPageWord = (_PDFPageWordStruct*)( word );
		for ( size_t i = 0; i < pPageWord->charVector.size(); ++i )
		{
			CHE_PDF_Text * pText = pPageWord->charVector[i].pText;
			if ( pText )
			{
				CHE_PDF_Font * pFont = pText->GetGState()->GetTextFont();
				FT_Face face = pFont->GetFTFace();
				if ( face )
				{
					int xPosition = 0;
					int yBaseline = 0;
					for ( unsigned int i = 0; i < pText->mItems.size(); ++i )
					{
						PDFPageChar textChar = CHEPDF_GetPageChar( pText, i );
						PDFMatrix cmtx = CHEPDF_GetCharMatirx( textChar );
						PDFPosition oriPoint;
						oriPoint.x = cmtx.e;
						oriPoint.y = cmtx.f;
						yBaseline = ( bbox.bottom + bbox.height - oriPoint.y ) * sclae * PIXELPERINCH / 72;
						xPosition = ( oriPoint.x - bbox.left ) * sclae * PIXELPERINCH / 72;

						_CHEPDF_RenderGlyph( textChar, sclae );
						if ( face->glyph && face->glyph->bitmap.buffer )
						{
							FT_Bitmap & bitmap = face->glyph->bitmap;
							HE_ARGB argb = 0;
							for ( int i = 0; i < bitmap.width; ++i )
							{
								for ( int j = 0; j < bitmap.rows; ++j )
								{
									if ( bitmap.buffer[i + bitmap.width*j] != 0x00 )
									{
										argb = 0xFFFFFFFF - ( bitmap.buffer[i + bitmap.width*j] | bitmap.buffer[i + bitmap.width*j] << 8 | bitmap.buffer[i + bitmap.width*j] << 16 );
										pBitmapRet->SetPixelColor( i + xPosition + face->glyph->bitmap_left, j + yBaseline - face->glyph->bitmap_top, argb );
									}
								}
							}
						}
					}
				}
			}
		}
		pBitmap = (PDFBitmap *)( pBitmapRet );
	}
	return pBitmap;
}


PDFBitmap CHEPDF_RenderPage( PDFPage page, float sclae /*= 1*/ )
{
	PDFBitmap * pBitmap = NULL;
	if ( page )
	{
		PDFRect bbox = CHEPDF_GetPageBox( page );
		PDFPageContent content = CHEPDF_GetPageContent( page );
		if ( content )
		{
			CHE_Bitmap * pBitmapRet = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
			pBitmapRet->Create( bbox.width * sclae * PIXELPERINCH / 72, bbox.height * sclae * PIXELPERINCH / 72, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_UP );
			pBitmapRet->Fill( 0xFFFFFFFF );

			PDFPageText text = CHEPDF_GetFirstPageText( content );
			while ( text )
			{
				CHE_PDF_Text * pText = (CHE_PDF_Text*)( text );
				CHE_PDF_Font * pFont = pText->GetGState()->GetTextFont();
				FT_Face face = pFont->GetFTFace();
				if ( face )
				{
					int xPosition = 0;
					int yBaseline = 0;
					//int xPageOffset = 0;
					//int yPageOffset = 0;
					for ( unsigned int i = 0; i < pText->mItems.size(); ++i )
					{
						PDFPageChar textChar = CHEPDF_GetPageChar( pText, i );
						PDFRect charBox = CHEPDF_GetCharBox( textChar );
// 						PDFMatrix cmtx = CHEPDF_GetCharMatirx( textChar );
// 						PDFPosition oriPoint;
// 						oriPoint.x = cmtx.e;
// 						oriPoint.y = cmtx.f;
						yBaseline = ( bbox.bottom + bbox.height - charBox.bottom - charBox.height/* - oriPoint.y*/ ) * sclae * PIXELPERINCH / 72;
						xPosition = ( /*oriPoint.x - */charBox.left ) * sclae * PIXELPERINCH / 72;
						_CHEPDF_RenderGlyph( textChar, sclae );
						CHEPDF_ClosePageChar( textChar );
						if ( face->glyph && face->glyph->bitmap.buffer )
						{
							FT_Bitmap & bitmap = face->glyph->bitmap;
							HE_ARGB argb = 0;
							for ( int i = 0; i < bitmap.width; ++i )
							{
								for ( int j = 0; j < bitmap.rows; ++j )
								{
									if ( bitmap.buffer[i + bitmap.width*j] != 0x00 )
									{
										argb = 0xFFFFFFFF - ( bitmap.buffer[i + bitmap.width*j] | bitmap.buffer[i + bitmap.width*j] << 8 | bitmap.buffer[i + bitmap.width*j] << 16 );
										pBitmapRet->SetPixelColor( i + xPosition + face->glyph->bitmap_left, j + yBaseline - face->glyph->bitmap_top, argb );
									}
								}
							}
						}
					}
				}
				text = CHEPDF_GetNextPageText( content );  
			}
			pBitmap = (PDFBitmap *)pBitmapRet;
		}
		CHEPDF_ReleasePageContent( content );
	}
	return pBitmap;
}