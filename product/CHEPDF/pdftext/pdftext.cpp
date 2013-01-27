#include "pdftext.h"
#include "../../../trunk/include/pdf/che_pdf_file.h"
#include "../../../trunk/include/pdf/che_pdf_document.h"
#include "../../../trunk/include/pdf/che_pdf_contents.h"
#include "../../../trunk/extlib/freetype/ft2build.h"
#include "../../../trunk/extlib/freetype/freetype/freetype.h"
#include "../../../trunk/extlib/freetype/freetype/ftglyph.h"
#include "../../../trunk/extlib/freetype/freetype/ftrender.h"
#include "../../../trunk/include/che_bitmap.h"

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
		CHE_PDF_GState * pGState = pTextObj->GetGState();
		FT_Face face = NULL;
		if ( pGState )
		{
			face = pGState->GetTextFont()->GetFTFace();
		}
		if ( pTextObj && pCharStruct->index < pTextObj->mItems.size() )
		{
			rect.width = pTextObj->mItems[pCharStruct->index].width;
			rect.height = pTextObj->mItems[pCharStruct->index].height;
			if ( face )
			{
				rect.bottom = face->descender * 1.0 / face->units_per_EM;
				rect.height = ( face->ascender - face->descender ) * 1.0 / face->units_per_EM;
			}
			rect = fz_transform_rect( matrix, rect );
		}
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


PDFStatus _CHEPDF_RenderGlyph( PDFPageChar textChar )
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
				FT_Set_Char_Size( pFont->GetFTFace(), 65536, 65536, PIXELPERINCH, PIXELPERINCH );
				FT_Matrix matrix;
				matrix.xx = mtx.a * 64;
				matrix.yx = mtx.b * 64;
				matrix.xy = mtx.c * 64;
				matrix.yy = mtx.d * 64;
				FT_Set_Transform( pFont->GetFTFace(), &matrix, NULL );
				FT_UInt gid = FT_Get_Char_Index( pFont->GetFTFace(), pTextObj->mItems[pCharStruct->index].charCode );
				FT_Load_Glyph( pFont->GetFTFace(), gid, FT_LOAD_DEFAULT );
				FT_Render_Glyph( pFont->GetFTFace()->glyph, FT_RENDER_MODE_NORMAL );
				return 0;
			}
		}
	}
	return 1;
}


PDFBitmap CHEPDF_RenderText( PDFPageText text/*, float scale/ * = 1.0* /*/ )
{
	PDFBitmap * pBitmap = NULL;
	if ( text )
	{
		PDFRect bbox = CHEPDF_GetTextBox( text );
		CHE_Bitmap * pBitmapRet = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
		pBitmapRet->Create( bbox.width /** scale*/ * PIXELPERINCH / 72, bbox.height /** scale*/ * PIXELPERINCH / 72, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_UP );
		pBitmapRet->Fill( 0xFFFFFFFF );

		CHE_PDF_Text * pText = (CHE_PDF_Text*)text;
		CHE_PDF_Font * pFont = pText->GetGState()->GetTextFont();
		FT_Face face = pFont->GetFTFace();
		int xPosition = 0;
		int yBaseline = 0;
		for ( unsigned int i = 0; i < pText->mItems.size(); ++i )
		{
			PDFPageChar textChar = CHEPDF_GetPageChar( text, i );
			PDFMatrix cmtx = CHEPDF_GetCharMatirx( textChar );
			PDFRect cbox = CHEPDF_GetCharBox( textChar );
			PDFPosition oriPoint;
			oriPoint.x = cmtx.e;
			oriPoint.y = cmtx.f;
			//fz_transform_point( cmtx, baselinePoint );
			yBaseline = ( bbox.bottom + bbox.height - oriPoint.y ) * PIXELPERINCH / 72;
			xPosition = ( oriPoint.x - bbox.left ) * PIXELPERINCH / 72;

			_CHEPDF_RenderGlyph( textChar );
			if ( face->glyph && face->glyph->bitmap.buffer )
			{
				FT_Bitmap & bitmap = face->glyph->bitmap;
				for ( int i = 0; i < bitmap.width; ++i )
				{
					for ( int j = 0; j < bitmap.rows; ++j )
					{
						pBitmapRet->SetPixelColor( i + xPosition + face->glyph->bitmap_left, j + yBaseline - face->glyph->bitmap_top, 0xFFFFFFFF - ( bitmap.buffer[i + bitmap.width*j] | bitmap.buffer[i + bitmap.width*j] << 8 | bitmap.buffer[i + bitmap.width*j] << 16 ) );
					}
				}
			}
		}
		pBitmap = (PDFBitmap *)( pBitmapRet );
	}
	return pBitmap;
}


PDFBitmap CHEPDF_RenderChar( PDFPageChar textChar )
{
	CHE_Bitmap * pBitmap = NULL;
	if ( textChar )
	{
		_PDFPageCharStruct * pCharStruct = (_PDFPageCharStruct*)( textChar );
		CHE_PDF_Text * pTextObj = pCharStruct->pText;
		if ( pTextObj && pCharStruct->index < pTextObj->mItems.size() )
		{
			_CHEPDF_RenderGlyph( textChar );

			CHE_PDF_Font * pFont = pTextObj->GetGState()->GetTextFont();
			if ( pFont->GetFTFace() && pFont->GetFTFace()->glyph && pFont->GetFTFace()->glyph->bitmap.buffer )
			{
				PDFMatrix mtx = CHEPDF_GetCharMatirx( textChar );
				PDFRect bbox = CHEPDF_GetCharBox( textChar );

				PDFPosition baselinePoint;
				baselinePoint.x = mtx.e;
				baselinePoint.y = mtx.f;

				fz_transform_point( mtx, baselinePoint );

				int yBaseline = ( bbox.bottom + bbox.height - baselinePoint.y ) * PIXELPERINCH / 72;
				int xBaseline = ( baselinePoint.x - bbox.left ) * PIXELPERINCH / 72;

				pBitmap = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
				if ( pBitmap )
				{
					pBitmap->Create( bbox.width * PIXELPERINCH / 72, bbox.height * PIXELPERINCH / 72, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_UP );
					pBitmap->Fill( 0xFFFFFFFF );
				}

				FT_Bitmap & bitmap = pFont->GetFTFace()->glyph->bitmap;
				for ( int i = 0; i < bitmap.width; ++i )
				{
					for ( int j = 0; j < bitmap.rows; ++j )
					{
						pBitmap->SetPixelColor( i + xBaseline + pFont->GetFTFace()->glyph->bitmap_left, j + yBaseline - pFont->GetFTFace()->glyph->bitmap_top, 0xFFFFFFFF - ( bitmap.buffer[i + bitmap.width*j] | bitmap.buffer[i + bitmap.width*j] << 8 | bitmap.buffer[i + bitmap.width*j] << 16 ) );
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
			return 0;
		}
	}
	return -1;
}