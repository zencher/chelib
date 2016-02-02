#include "../../include/pdf/che_pdf_contentobjs.h"
#include "../../include/pdf/che_pdf_gstate.h"

#include "../../extlib/freetype/include/ft2build.h"
#include "../../extlib/freetype/include/freetype/freetype.h"
#include "../../extlib/freetype/include/freetype/ftoutln.h"

CHE_PDF_ContentObject::~CHE_PDF_ContentObject()
{
	if ( mpGState )
	{
		mpGState->GetAllocator()->Delete( mpGState );
	}
}

HE_BOOL CHE_PDF_ContentObject::SetGState( CHE_PDF_GState * pGSatae )
{
	if ( mpGState )
	{
		mpGState->GetAllocator()->Delete( mpGState );
		mpGState = NULL;
	}
	if ( pGSatae )
	{
		mpGState = pGSatae;
		return TRUE;
	}
	return FALSE;
}



CHE_PDF_ContentObject * CHE_PDF_Text::Clone() const
{
	CHE_PDF_Text * pTextRet = GetAllocator()->New<CHE_PDF_Text>( GetAllocator() );
	pTextRet->mItems = mItems;
	//pTextRet->mExtMatrixl = mExtMatrixl;
	if ( mpGState )
	{
		pTextRet->mpGState = mpGState->Clone();
	}
	if ( mpObj )
	{
		pTextRet->mpObj = mpObj->Clone();
	}
	return pTextRet;
}

HE_BOOL CHE_PDF_Text::SetTextObject( const CHE_PDF_ObjectPtr & pObj )
{
	if ( pObj )
	{
		mpObj = pObj;

		//获取字体
		CHE_PDF_Font * pFont = GetGState()->GetTextFont();
		if ( pFont == NULL )
		{
			return FALSE;
		}

		std::vector<CHE_PDF_ObjectPtr> tmpArray; 
		if ( pObj->GetType() == OBJ_TYPE_STRING )
		{
			tmpArray.push_back( pObj );
		}
		else if ( pObj->GetType() == OBJ_TYPE_ARRAY )
		{
			CHE_PDF_ArrayPtr pArray = pObj->GetArrayPtr();
			CHE_PDF_ObjectPtr pObj;
			for ( HE_ULONG i = 0; i < pArray->GetCount(); ++i )
			{
				pObj = pArray->GetElement( i );
				if ( pObj && ( pObj->GetType() == OBJ_TYPE_STRING || pObj->GetType() == OBJ_TYPE_NUMBER ) )
				{
					tmpArray.push_back( pObj );
				}
			}
		}
		CHE_ByteString tmpStr;
		HE_INT32 kerning = 0;
		CHE_PDF_TextItem item;
		if ( pFont->GetFontType() != FONT_TYPE0 )
		{
			for ( HE_ULONG i = 0; i < tmpArray.size(); ++i )
			{
				if ( tmpArray[i]->GetType() == OBJ_TYPE_STRING )
				{
					tmpStr = tmpArray[i]->GetStringPtr()->GetString();
					for ( HE_ULONG index = 0; index < tmpStr.GetLength(); ++index )
					{
						item.charCode = HE_BYTE( tmpStr[index] );
						item.gid = 0;
						item.cid = 0;
						item.ucs = 0;
                        pFont->Decode( item.charCode, item.ucs, item.gid, item.cid );
						item.kerning = kerning;
						item.width = pFont->GetWidth( item );
						item.height = 1; //font height??
						mItems.push_back( item );
						kerning = 0;
					}
				}
				else if ( tmpArray[i]->GetType() == OBJ_TYPE_NUMBER )
				{
					kerning = tmpArray[i]->GetNumberPtr()->GetInteger();
				}
			}
			if ( kerning != 0 )
			{
				mpLastKerning = kerning;
			}
		}
		else
		{
			CHE_PDF_Type0_Font * pType0Font = (CHE_PDF_Type0_Font *)( pFont );
			for ( HE_ULONG i = 0; i < tmpArray.size(); ++i )
			{
				if ( tmpArray[i]->GetType() == OBJ_TYPE_STRING )
				{
					tmpStr = tmpArray[i]->GetStringPtr()->GetString();
					item.charCode = 0;
					HE_BYTE byteCount = 0;
					for ( HE_ULONG index = 0; index < tmpStr.GetLength(); ++index )
					{
						++byteCount;
						item.charCode = ( item.charCode << 8 ) + HE_BYTE( tmpStr[index] );
						if ( pFont->GetEncodingType() == FONT_ENCODING_BUILDINCMAP )
						{
							if ( pType0Font->IsCode( item.charCode, byteCount ) == FALSE )
							{
								continue;
							}
						}else if ( index % 2 == 0 )
						{
							continue;
						}
						item.gid = 0;
						item.cid = 0;
						item.ucs = 0;
                        pFont->Decode( item.charCode, item.ucs, item.gid, item.cid );
						item.kerning = kerning;
						item.width = pFont->GetWidth( item );
						item.height = 1; //font height??
						mItems.push_back( item );
						kerning = 0;

						//very important!
						item.charCode = 0;
						byteCount = 0;
					}
				}
				else if ( tmpArray[i]->GetType() == OBJ_TYPE_NUMBER )
				{
					kerning = tmpArray[i]->GetNumberPtr()->GetInteger();
				}
			}
			if ( kerning != 0 )
			{
				mpLastKerning = kerning;
			}
		}
		return TRUE;
	}
	return FALSE;
}

CHE_Matrix CHE_PDF_Text::GetTextMatrix() const
{
	CHE_PDF_GState * pGState = GetGState();
	if ( pGState )
	{
		CHE_Matrix textMatrix;
		CHE_Matrix ctm;
		CHE_Matrix tmpMatrix;
		HE_FLOAT fontSize = 1;
		HE_FLOAT fontScaling = 100;
		HE_FLOAT fontRise = 0;
		pGState->GetTextMatrix( textMatrix );
		ctm = pGState->GetMatrix();
		pGState->GetTextFontSize( fontSize );
		pGState->GetTextScaling( fontScaling );
		pGState->GetTextRise( fontRise );
		tmpMatrix.a = fontSize * fontScaling / 100;
		tmpMatrix.b = 0;
		tmpMatrix.c = 0;
		tmpMatrix.d = fontSize;
		tmpMatrix.e = 0;
		tmpMatrix.f = fontRise;
		tmpMatrix.Concat( textMatrix );
		tmpMatrix.Concat( ctm );
		return tmpMatrix;
	}
	return CHE_Matrix();
}


CHE_Matrix CHE_PDF_Text::GetCharMatrix( HE_ULONG index ) const
{
	if ( index >= mItems.size() )
	{
		return CHE_Matrix();
	}
	CHE_PDF_GState * pGState = GetGState();
	if ( pGState )
	{
		CHE_PDF_Font * pFont = pGState->GetTextFont();
		HE_ULONG wMode = 0;
		if ( pFont )
		{
			wMode = pFont->GetWMode();
		}

		HE_FLOAT fontCharSpace = 0;
		HE_FLOAT fontWordSpace = 0;
		HE_FLOAT fontScaling = 100;
		HE_FLOAT fontSize = 1;
		HE_FLOAT fontRise = 0;
		
		pGState->GetTextCharSpace( fontCharSpace );
		pGState->GetTextWordSpace( fontWordSpace );
		pGState->GetTextScaling( fontScaling );
		fontScaling /= 100;
		pGState->GetTextFontSize( fontSize );
		pGState->GetTextRise( fontRise );

		HE_FLOAT offset = 0;
		HE_FLOAT kerning = 0;
		HE_ULONG i = 0;
		for (; i < index; ++i )
		{
			offset += mItems[i].width * fontSize + fontCharSpace;	//wMode==1的时候，是不是还加width？
			if ( mItems[i].ucs == L' ' || mItems[i].charCode == ' ' )	//如果这里的ucs和charcode处理有误的话就会算错了！
			{
				offset += fontWordSpace;
			}
			kerning -= mItems[i].kerning * fontSize / 1000;
		}
		if ( i <= index )
		{
			kerning -= mItems[i].kerning * fontSize / 1000;
		}

		//CHE_Matrix ctm = pGState->GetMatrix();
		CHE_Matrix tm;
		pGState->GetTextMatrix( tm );

		CHE_Matrix textMatrix;
		textMatrix.a = fontSize * fontScaling;
		textMatrix.b = 0;
		textMatrix.c = 0;
		textMatrix.d = fontSize;
		textMatrix.e = 0;
		textMatrix.f = fontRise;
		
		CHE_Matrix OffsetMatrix;
		if ( wMode == 0 )
		{
			OffsetMatrix.e = offset * fontScaling + kerning;
		}else{
			OffsetMatrix.f = offset + kerning;
		}
		textMatrix.Concat( OffsetMatrix );
		textMatrix.Concat( tm );
		//textMatrix.Concat( ctm );
		return textMatrix;
	}
	return CHE_Matrix();
}


CHE_Rect CHE_PDF_Text::GetTextRect() const
{
	CHE_Rect rect;
	CHE_Rect tmpRect;
	for ( HE_ULONG i = 0; i < mItems.size(); ++i )
	{
		tmpRect = GetCharRect( i );
		rect.Union( tmpRect );
	}
	return rect;
}


CHE_Rect CHE_PDF_Text::GetCharRect( HE_ULONG index ) const
{
	CHE_Rect rect;
	if ( index < mItems.size() )
	{
		FT_Face face = NULL;
		CHE_PDF_GState * pGState = GetGState();
		if ( pGState )
		{
			face = (FT_Face)( pGState->GetTextFont()->GetFTFace() );
		}
		CHE_Matrix matrix = GetCharMatrix( index );
		rect.width = mItems[index].width;
		rect.height = mItems[index].height;
		if ( face )
		{
			rect.bottom = face->descender * 1.0f / face->units_per_EM;
			rect.height = ( face->ascender - face->descender ) * 1.0f / face->units_per_EM;
		}
		rect = matrix.Transform( rect );
	}
	return rect;
}


//获得这个字符串对象中包括kerning和字符宽度，间隔等所有数据说产生的offset
//这个offset还没有被外部的tm、ctm矩阵进行运行。而在使用该offset的地方要进行这个运算。可以直接加在tm里面
HE_FLOAT CHE_PDF_Text::GetOffSet() const
{
	CHE_PDF_GState * pGState = GetGState();
	if ( pGState )
	{
		HE_FLOAT fontCharSpace = 0;
		HE_FLOAT fontWordSpace = 0;
		HE_FLOAT fontScaling = 100;
		HE_FLOAT fontSize = 1;
		HE_FLOAT fontRise = 0;
		pGState->GetTextCharSpace( fontCharSpace );
		pGState->GetTextWordSpace( fontWordSpace );
		pGState->GetTextFontSize( fontSize );
		pGState->GetTextRise( fontRise );
		pGState->GetTextScaling( fontScaling );
		fontScaling /= 100;

		//这里之所以可以忽略fontSize，fontScaling，fontRise等信息的矩阵的原因是
		//这些信息所组成的矩阵在乘以offset的矩阵的时候不会影响offset的值（不受放大系数影响）

		HE_FLOAT offset = 0;
		HE_FLOAT kerning = 0;
		for ( HE_ULONG i = 0; i < mItems.size(); ++i )
		{
			offset += mItems[i].width * fontSize + fontCharSpace;
			if ( mItems[i].ucs == L' ' || mItems[i].charCode == ' ' )
			{
				offset += fontWordSpace;
			}
			kerning -= mItems[i].kerning * fontSize / 1000;
		}
		return offset * fontScaling + kerning - mpLastKerning * fontSize / 1000;
	}
	return 0;
}



struct _TEXTPATHOUTPUT_PARAM_
{
	CHE_PDF_Path *	pPath;
	HE_FLOAT		curX;
	HE_FLOAT		curY;
};


inline int move_to(const FT_Vector *p, void *cc)
{
	_TEXTPATHOUTPUT_PARAM_ * param = (_TEXTPATHOUTPUT_PARAM_*)(cc);
	if ( param == NULL )
	{
		return 0;
	}
	CHE_PDF_Path * pPath = param->pPath;
	if ( pPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_MoveTo;
		pPath->mItems.push_back( pathItem );
		param->curX = pathItem.value = p->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		param->curY = pathItem.value = p->y / 64.0f;
		pPath->mItems.push_back( pathItem );
	}
	return 0;
}

inline int line_to(const FT_Vector *p, void *cc)
{
	_TEXTPATHOUTPUT_PARAM_ * param = (_TEXTPATHOUTPUT_PARAM_*)(cc);
	if ( param == NULL )
	{
		return 0;
	}
	CHE_PDF_Path * pPath = param->pPath;
	if ( pPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_LineTo;
		pPath->mItems.push_back( pathItem );
		param->curX = pathItem.value = p->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		param->curY = pathItem.value = p->y / 64.0f;
		pPath->mItems.push_back( pathItem );
	}
	return 0;
}

inline int conic_to(const FT_Vector *c, const FT_Vector *p, void *cc)
{
	_TEXTPATHOUTPUT_PARAM_ * param = (_TEXTPATHOUTPUT_PARAM_*)(cc);
	if ( param == NULL )
	{
		return 0;
	}
	CHE_PDF_Path * pPath = param->pPath;
	if ( pPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_CurveTo;
		pPath->mItems.push_back( pathItem );
		pathItem.value = (param->curX + c->x / 64.0f * 2) / 3;
		pPath->mItems.push_back( pathItem );
		pathItem.value = (param->curY + c->y / 64.0f * 2) / 3;
		pPath->mItems.push_back( pathItem );
		pathItem.value = (p->x / 64.0f + c->x / 64.0f * 2) / 3;
		pPath->mItems.push_back( pathItem );
		pathItem.value = (p->y / 64.0f + c->y / 64.0f * 2) / 3;
		pPath->mItems.push_back( pathItem );
		param->curX = pathItem.value = p->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		param->curY = pathItem.value = p->y / 64.0f;
		pPath->mItems.push_back( pathItem );
	}
	return 0;
}

inline int cubic_to(const FT_Vector *c1, const FT_Vector *c2, const FT_Vector *p, void *cc)
{
	_TEXTPATHOUTPUT_PARAM_ * param = (_TEXTPATHOUTPUT_PARAM_*)(cc);
	if ( param == NULL )
	{
		return 0;
	}
	CHE_PDF_Path * pPath = param->pPath;
	if ( pPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_CurveTo;
		pPath->mItems.push_back( pathItem );
		pathItem.value = c1->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		pathItem.value = c1->y / 64.0f;
		pPath->mItems.push_back( pathItem );
		pathItem.value = c2->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		pathItem.value = c2->y / 64.0f;
		pPath->mItems.push_back( pathItem );
		param->curX = pathItem.value = p->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		param->curY = pathItem.value = p->y / 64.0f;
		pPath->mItems.push_back( pathItem );
	}
	return 0;
}


CHE_PDF_Path * CHE_PDF_Text::GetGraphPath( HE_ULONG index )
{
	if ( index >= mItems.size() )
	{
		return NULL;
	}

	CHE_PDF_Path * pPathRet = GetAllocator()->New<CHE_PDF_Path>( GetAllocator() );
	if ( pPathRet )
	{
		CHE_PDF_Font * pFont = GetGState()->GetTextFont();
		FT_Face face = (FT_Face)( pFont->GetFTFace() );
		if ( face )
		{
			pFont->Lock();
			FT_Error err = FT_Set_Char_Size( face, 65536, 65536, /*PIXELPERINCH*/72, /*PIXELPERINCH*/72 );
			CHE_Matrix mtx = GetCharMatrix( index );
			FT_Matrix matrix;
			FT_Vector vector;
			matrix.xx = mtx.a * 64;
			matrix.yx = mtx.b * 64;
			matrix.xy = mtx.c * 64;
			matrix.yy = mtx.d * 64;
			vector.x = mtx.e * 64;
			vector.y = mtx.f * 64;
			FT_Set_Transform( face, &matrix, &vector );
			
			FT_UInt gid = mItems[index].gid;
			err = FT_Load_Glyph( face, gid, FT_LOAD_NO_BITMAP | FT_LOAD_TARGET_MONO | FT_LOAD_NO_HINTING /*| FT_LOAD_NO_HINTING*//*FT_LOAD_TARGET_MONO*/ );
			
			FT_Outline_Funcs outline_funcs;
			outline_funcs.move_to = move_to;
			outline_funcs.line_to = line_to;
			outline_funcs.conic_to = conic_to;
			outline_funcs.cubic_to = cubic_to;
			outline_funcs.delta = NULL;
			outline_funcs.shift = NULL;

			_TEXTPATHOUTPUT_PARAM_ * pParam = GetAllocator()->New<_TEXTPATHOUTPUT_PARAM_>();
			pParam->pPath = pPathRet;
			pParam->curX = 0.0f;
			pParam->curY = 0.0f;

			FT_Outline_Decompose( &face->glyph->outline, &outline_funcs, pParam );

			GetAllocator()->Delete( pParam );

			pFont->UnLock();

// 			FT_Outline * outline = &face->glyph->outline;
// 			void*                    user = (void*)pPathRet;
// 
// #undef SCALED
// #define SCALED( x )  ( ( (x) << shift ) - delta )
// 
// 			FT_Vector   v_last;
// 			FT_Vector   v_control;
// 			FT_Vector   v_start;
// 
// 			FT_Vector*  point;
// 			FT_Vector*  limit;
// 			char*       tags;
// 
// 			FT_Error    error;
// 
// 			FT_Int   n;         /* index of contour in outline     */
// 			FT_UInt  first;     /* index of first point in contour */
// 			FT_Int   tag;       /* current point's state           */
// 
// 			FT_Int   shift;
// 			FT_Pos   delta;
// 
// 
// 			
// 			shift = 0;//outline_funcs->shift;
// 			delta = 0;//outline_funcs->delta;
// 			first = 0;
// 
// 			for ( n = 0; n < outline->n_contours; n++ )
// 			{
// 				FT_Int  last;  /* index of last point in contour */
// 
// 				last = outline->contours[n];
// // 				if ( last < 0 )
// // 					goto Invalid_Outline;
// 				limit = outline->points + last;
// 
// 				v_start   = outline->points[first];
// 				v_start.x = SCALED( v_start.x );
// 				v_start.y = SCALED( v_start.y );
// 
// 				v_last   = outline->points[last];
// 				v_last.x = SCALED( v_last.x );
// 				v_last.y = SCALED( v_last.y );
// 
// 				v_control = v_start;
// 
// 				point = outline->points + first;
// 				tags  = outline->tags   + first;
// 				tag   = FT_CURVE_TAG( tags[0] );
// 
// 				/* A contour cannot start with a cubic control point! */
// 				//if ( tag == FT_CURVE_TAG_CUBIC )
// 				//	
// 
// 				/* check first point to determine origin */
// 				if ( tag == FT_CURVE_TAG_CONIC )
// 				{
// 					/* first point is conic control.  Yes, this happens. */
// 					if ( FT_CURVE_TAG( outline->tags[last] ) == FT_CURVE_TAG_ON )
// 					{
// 						/* start at last point if it is on the curve */
// 						v_start = v_last;
// 						limit--;
// 					}
// 					else
// 					{
// 						/* if both first and last points are conic,         */
// 						/* start at their middle and record its position    */
// 						/* for closure                                      */
// 						v_start.x = ( v_start.x + v_last.x ) / 2;
// 						v_start.y = ( v_start.y + v_last.y ) / 2;
// 
// 						v_last = v_start;
// 					}
// 					point--;
// 					tags--;
// 				}
// 
// 				//FT_TRACE5(( "  move to (%.2f, %.2f)\n",
// 				//	v_start.x / 64.0, v_start.y / 64.0 ));
// 				move_to( &v_start, user );
// 				//if ( error )
// 				//	goto Exit;
// 
// 				while ( point < limit )
// 				{
// 					point++;
// 					tags++;
// 
// 					tag = FT_CURVE_TAG( tags[0] );
// 					switch ( tag )
// 					{
// 					case FT_CURVE_TAG_ON:  /* emit a single line_to */
// 						{
// 							FT_Vector  vec;
// 
// 
// 							vec.x = SCALED( point->x );
// 							vec.y = SCALED( point->y );
// 
// 							//FT_TRACE5(( "  line to (%.2f, %.2f)\n",
// 							//	vec.x / 64.0, vec.y / 64.0 ));
// 							error = line_to( &vec, user );
// 							//if ( error )
// 							//	goto Exit;
// 							continue;
// 						}
// 
// 					case FT_CURVE_TAG_CONIC:  /* consume conic arcs */
// 						v_control.x = SCALED( point->x );
// 						v_control.y = SCALED( point->y );
// 
// Do_Conic:
// 						if ( point < limit )
// 						{
// 							FT_Vector  vec;
// 							FT_Vector  v_middle;
// 
// 
// 							point++;
// 							tags++;
// 							tag = FT_CURVE_TAG( tags[0] );
// 
// 							vec.x = SCALED( point->x );
// 							vec.y = SCALED( point->y );
// 
// 							if ( tag == FT_CURVE_TAG_ON )
// 							{
// // 								FT_TRACE5(( "  conic to (%.2f, %.2f)"
// // 									" with control (%.2f, %.2f)\n",
// // 									vec.x / 64.0, vec.y / 64.0,
// // 									v_control.x / 64.0, v_control.y / 64.0 ));
// 								conic_to( &v_control, &vec, user );
// 								//if ( error )
// 								//	goto Exit;
// 								continue;
// 							}
// 
// // 							if ( tag != FT_CURVE_TAG_CONIC )
// // 								goto Invalid_Outline;
// 
// 							v_middle.x = ( v_control.x + vec.x ) / 2;
// 							v_middle.y = ( v_control.y + vec.y ) / 2;
// 
// 							
// 							conic_to( &v_control, &v_middle, user );
// 							
// 							v_control = vec;
// 							goto Do_Conic;
// 						}
// 
// 						conic_to( &v_control, &v_start, user );
// 						goto Close;
// 
// 					default:  /* FT_CURVE_TAG_CUBIC */
// 						{
// 							FT_Vector  vec1, vec2;
// 
// 
// // 							if ( point + 1 > limit                             ||
// // 								FT_CURVE_TAG( tags[1] ) != FT_CURVE_TAG_CUBIC )
// // 								goto Invalid_Outline;
// 
// 							point += 2;
// 							tags  += 2;
// 
// 							vec1.x = SCALED( point[-2].x );
// 							vec1.y = SCALED( point[-2].y );
// 
// 							vec2.x = SCALED( point[-1].x );
// 							vec2.y = SCALED( point[-1].y );
// 
// 							if ( point <= limit )
// 							{
// 								FT_Vector  vec;
// 
// 
// 								vec.x = SCALED( point->x );
// 								vec.y = SCALED( point->y );
// 
// 								cubic_to( &vec1, &vec2, &vec, user );
// 								continue;
// 							}
// 
// 							cubic_to( &vec1, &vec2, &v_start, user );
// 							goto Close;
// 						}
// 					}
// 				}
// 
// 				/* close the contour with a line segment */
// 				line_to( &v_start, user );
// 
// Close:
// 				first = last + 1;
// 
// // 				CHE_PDF_PathItem pathItem;
// // 				pathItem.type = PathItem_Close;
// // 				pPathRet->mItems.push_back( pathItem );
// 			}
		}
	}
//Exit:
	return pPathRet;
}

CHE_PDF_ContentObject * CHE_PDF_Path::Clone() const
{
	CHE_PDF_Path * pTmpPath = GetAllocator()->New<CHE_PDF_Path>( GetAllocator() );
	pTmpPath->mType = mType;
	pTmpPath->mFillMode = mFillMode;
	pTmpPath->mItems = mItems;
	pTmpPath->mpGState = NULL;
	if ( mpGState )
	{
		pTmpPath->mpGState = mpGState->Clone();
	}
	return pTmpPath;
}

CHE_PDF_Path::~CHE_PDF_Path()
{
	mItems.clear();
}

CHE_PDF_InlineImage::CHE_PDF_InlineImage( HE_BOOL bMask, HE_ULONG width, HE_ULONG hight, HE_ULONG bpc,
    HE_LPBYTE pBytes, HE_ULONG size, CHE_PDF_ObjectPtr objPtr, CHE_PDF_ColorSpacePtr colorspace,
    GRAPHICS_STATE_RENDERINTENTS ri, CHE_Allocator * pAllocator/*= NULL*/ )
	: CHE_PDF_ContentObject( ContentType_InlineImage, pAllocator ), mbMask( bMask ), mWidth( width ), mHeight( hight ), 
	mBpc( bpc ), mpData( NULL), mDataSize( 0 ), mDecodeObjPtr( objPtr ), mRI(ri)
{
	if ( pBytes )
	{
		mDataSize = size;
		mpData = GetAllocator()->NewArray<HE_BYTE>( mDataSize );
		memcpy( mpData, pBytes, mDataSize );
	}
}

CHE_PDF_InlineImage::~CHE_PDF_InlineImage()
{
	if ( mpData )
	{
		GetAllocator()->DeleteArray( mpData );
	}
}

CHE_PDF_ContentObject * CHE_PDF_InlineImage::Clone() const
{
	// 		CHE_PDF_ColorSpace * pTmpColorSpace = NULL;
	// 		if ( mpColorspace )
	// 		{
	// 			pTmpColorSpace = mpColorspace->Clone();
	// 		}
	CHE_PDF_ObjectPtr objPtr;
	if ( mDecodeObjPtr )
	{
		objPtr = mDecodeObjPtr->Clone();
	}
	return GetAllocator()->New<CHE_PDF_InlineImage>( mbMask, mWidth, mHeight, mBpc, mpData, mDataSize, objPtr, mColorspace, mRI, GetAllocator() );
}

#ifdef WIN32

CHE_Bitmap * CHE_PDF_InlineImage::GetBitmap()
{
	CHE_Bitmap * pBitmapRet = NULL;
	if ( !mColorspace )
	{
		return pBitmapRet;
	}
	HE_LPBYTE pData = mpData;
	HE_LPBYTE pTmpByte = NULL;
	HE_BYTE	tmpByte = 0;
	HE_ARGB colorARGB = 0xFF000000;
	CHE_PDF_Color color;
	HE_ARGB * pColors = GetAllocator()->NewArray<HE_ARGB>( mWidth );
	HE_ULONG colorsIndex = 0;
	
	HE_ULONG component = mColorspace->GetComponentCount();
	if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
	{
		component = 1;
	}
	HE_ULONG stride = (mWidth * component * mBpc + 7)/8;

	pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
	pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );

	if ( mBpc == 8 )
	{
		color.Clear();
		for ( HE_ULONG y = 0; y < mHeight; ++y )
		{
			colorsIndex = 0;
			for ( HE_ULONG x = 0 ; x < mWidth; ++x )
			{
				pTmpByte = pData + ( ( y * mWidth + x ) * component );
				for ( HE_ULONG i = 0; i < component; ++i )
				{
					if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
					{
						color.Push( *( pTmpByte ) );
					}else{
						color.Push( *( pTmpByte ) / 255.0f );
					}
					pTmpByte++;
				}
				colorARGB = mColorspace->GetARGBValue( color );
				color.Clear();
				*(pColors+colorsIndex++) = colorARGB;
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}else if ( mBpc == 4 )
	{
		color.Clear();
		for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
		{
			pTmpByte = pData + ( y * stride );
			colorsIndex = 0;
			x = 0;
			for ( HE_ULONG i = 0; i < stride; ++i )
			{
				tmpByte = *(pTmpByte + i);
				for ( HE_ULONG j = 0; j < 2; ++j )
				{
					if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
					{
						color.Push( (tmpByte>>(1-j)*4)&0x0F );
					}else{
						color.Push( ((tmpByte>>(1-j)*4)&0x0F)/16.0f );
					}
					if ( color.GetComponentCount() == component )
					{
						colorARGB = mColorspace->GetARGBValue( color );
						color.Clear();
						*(pColors+colorsIndex++) = colorARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}else if ( mBpc == 2 )
	{
		color.Clear();
		for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
		{
			colorsIndex = 0;
			pTmpByte = pData + ( y * stride );
			x = 0;
			for ( HE_ULONG i = 0; i < stride; ++i )
			{
				tmpByte = *(pTmpByte + i);
				for ( HE_ULONG j = 0; j < 4; ++j )
				{
					if ( mColorspace->GetColorSpaceType() == COLORSPACE_SPECIAL_INDEXED )
					{
						color.Push( tmpByte>>((3-j)*2) & 0x03 );
					}else{
						color.Push( ((tmpByte>>((3-j)*2)) & 0x03) /4.0f );
					}
					if ( color.GetComponentCount() == component )
					{
						colorARGB = mColorspace->GetARGBValue( color );
						color.Clear();
						*(pColors+colorsIndex++) = colorARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}else if ( mBpc == 1 )
	{
		color.Clear();
		for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
		{
			colorsIndex = 0;
			pTmpByte = pData + ( y * stride );
			x = 0;
			for ( HE_ULONG i = 0; i < stride; ++i )
			{
				tmpByte = *(pTmpByte + i);
				for ( HE_ULONG j = 0; j < 8; ++j )
				{
					color.Push( ( tmpByte >> (7-j) ) & 0x01  );
					if ( color.GetComponentCount() == component )
					{
						colorARGB = mColorspace->GetARGBValue( color );
						color.Clear();
						*(pColors+colorsIndex++) = colorARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
			}
			pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
		}
	}
	return pBitmapRet;
}

#endif


CHE_PDF_ContentObject * CHE_PDF_ComponentRef::Clone() const
{
	CHE_PDF_ComponentRef * pCmpRef = GetAllocator()->New<CHE_PDF_ComponentRef>( mName, mComponentPtr, GetAllocator() );
	if ( mpGState )
	{
		pCmpRef->mpGState = mpGState->Clone();
	}
	return pCmpRef;
}

