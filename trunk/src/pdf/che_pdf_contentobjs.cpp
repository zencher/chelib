#include "../../include/pdf/che_pdf_contentobjs.h"

#include "../../extlib/freetype/include/ft2build.h"
#include "../../extlib/freetype/include/freetype/freetype.h"
#include "../../extlib/freetype/include/freetype/ftoutln.h"

#include "../../extlib/libjpeg/jpeglib.h"
#include "../../extlib/openjpeg/openjpeg.h"
#include "../../extlib/jbig2dec/jbig2.h"

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

CHE_PDF_ContentObjectList::~CHE_PDF_ContentObjectList()
{
	Clear();
}

CHE_PDF_ContentObjectList * CHE_PDF_ContentObjectList::Clone()
{
	CHE_PDF_ContentObjectList * pObjList = GetAllocator()->New<CHE_PDF_ContentObjectList>();

	ContentObjectList::iterator it = Begin();
	for ( ; it != End(); ++it )
	{
		pObjList->Append( *it );
	}
	return pObjList;
}

HE_VOID CHE_PDF_ContentObjectList::Clear()
{
	ContentObjectList::iterator it = Begin();
	CHE_PDF_ContentObject * pObj = NULL;
	for ( ; it != End(); ++it )
	{
		pObj = *it;
		if ( pObj )
		{
			pObj->GetAllocator()->Delete( pObj );
		}
	}
	mList.clear();
}


bool CHE_PDF_ContentObjectList::Append( CHE_PDF_ContentObject * pObj )
{
	if ( pObj )
	{
		mList.push_back( pObj );
		return true;
	}
	return false;
}

ContentObjectList::iterator CHE_PDF_ContentObjectList::Begin()
{
	return mList.begin();
}

ContentObjectList::iterator CHE_PDF_ContentObjectList::End()
{
	return mList.end();
}

HE_VOID CHE_PDF_ContentObjectList::SetType3BBox( const HE_INT32 type, std::vector<HE_FLOAT> & param )
{
	mType3DType = type;
	mType3Param = param;
}

HE_BOOL CHE_PDF_ContentObjectList::GetType3BBox( HE_INT32 & type, std::vector<HE_FLOAT> & param )
{
	type = mType3DType;
	param = mType3Param;
	return TRUE;
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
		if ( pFont->GetType() != FONT_TYPE0 )
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
						if ( pFont->GetEncodingType() == FONT_ENCODING_NONE )
						{
							//对于简单字体而言，没有编码信息的时候
							pFont->GetGlyphId( item.charCode, item.gid );
							pFont->GetUnicode( item.charCode, item.ucs );
						}else if ( pFont->GetEncodingType() == FONT_ENCODING_BUILDINCMAP )
						{
							//本来说应该是不合适的，但是有可能由于encoding字典有错误，需要做容错处理
							pFont->GetGlyphId( item.charCode, item.gid );
							pFont->GetUnicode( item.charCode, item.ucs );
						}else if ( pFont->GetEncodingType() == FONT_ENCODING_IDENTITY )
						{
							item.gid = item.charCode;
							pFont->GetUnicode( item.charCode, item.ucs );
						}else{
							pFont->GetGlyphId( item.charCode, item.gid );
							pFont->GetUnicode( item.charCode, item.ucs );
							if ( item.gid == 0 )
							{
								item.gid = item.charCode;
							}
						}
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
						if ( pFont->GetEncodingType() == FONT_ENCODING_NONE )
						{
							item.gid = item.charCode;
							pFont->GetUnicode( item.charCode, item.ucs );
						}
						else if ( pFont->GetEncodingType() == FONT_ENCODING_BUILDINCMAP )
						{
							if ( pType0Font->GetCID( item.charCode, item.cid ) )
							{
								if ( ! pFont->GetUnicode( item.cid, item.ucs ) )
								{
									pFont->GetGlyphId( item.charCode, item.gid );
								}else{
									pFont->GetGlyphId( item.ucs, item.gid );
								}
							}else{
								pFont->GetUnicode( item.charCode, item.ucs );
								if ( item.ucs == 0 )
								{
									item.ucs = item.charCode;
								}
								pFont->GetGlyphId( item.charCode, item.gid );
								if ( item.gid == 0 )
								{
									pFont->GetGlyphId( item.ucs, item.gid );
									if ( item.gid == 0 )
									{
										item.gid = item.charCode;
									}
								}
							}
						}else if ( pFont->GetEncodingType() == FONT_ENCODING_IDENTITY )
						{
							if ( pFont->GetUnicode( item.charCode, item.ucs ) )
							{
								item.cid = 0;
								if ( ! pFont->GetGlyphId( item.ucs, item.gid ) )
								{
									item.gid = item.charCode;
								}
							}
						}else{
							pFont->GetGlyphId( item.charCode, item.gid );
							pFont->GetUnicode( item.charCode, item.ucs );
							pType0Font->GetCID( item.charCode, item.cid );
							if ( item.gid == 0 )
							{
								item.gid = item.charCode;
							}
						}
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

		CHE_Matrix ctm = pGState->GetMatrix();
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
		textMatrix.Concat( ctm );
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

HE_FLOAT gCurX = 0;
HE_FLOAT gCurY = 0;

inline int move_to(const FT_Vector *p, void *cc)
{
	CHE_PDF_Path * pPath = (CHE_PDF_Path*)cc;
	if ( pPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_MoveTo;
		pPath->mItems.push_back( pathItem );
		gCurX = pathItem.value = p->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		gCurY = pathItem.value = p->y / 64.0f;
		pPath->mItems.push_back( pathItem );
	}
	return 0;
}

inline int line_to(const FT_Vector *p, void *cc)
{
	CHE_PDF_Path * pPath = (CHE_PDF_Path*)cc;
	if ( pPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_LineTo;
		pPath->mItems.push_back( pathItem );
		gCurX = pathItem.value = p->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		gCurY = pathItem.value = p->y / 64.0f;
		pPath->mItems.push_back( pathItem );
	}
	return 0;
}

inline int conic_to(const FT_Vector *c, const FT_Vector *p, void *cc)
{
	CHE_PDF_Path * pPath = (CHE_PDF_Path*)cc;
	if ( pPath )
	{
		CHE_PDF_PathItem pathItem;
		pathItem.type = PathItem_CurveTo;
		pPath->mItems.push_back( pathItem );
		pathItem.value = (gCurX + c->x / 64.0f * 2) / 3;
		pPath->mItems.push_back( pathItem );
		pathItem.value = (gCurY + c->y / 64.0f * 2) / 3;
		pPath->mItems.push_back( pathItem );
		pathItem.value = (p->x / 64.0f + c->x / 64.0f * 2) / 3;
		pPath->mItems.push_back( pathItem );
		pathItem.value = (p->y / 64.0f + c->y / 64.0f * 2) / 3;
		pPath->mItems.push_back( pathItem );
		gCurX = pathItem.value = p->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		gCurY = pathItem.value = p->y / 64.0f;
		pPath->mItems.push_back( pathItem );
	}
	return 0;
}

inline int cubic_to(const FT_Vector *c1, const FT_Vector *c2, const FT_Vector *p, void *cc)
{
	CHE_PDF_Path * pPath = (CHE_PDF_Path*)cc;
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
		gCurX = pathItem.value = p->x / 64.0f;
		pPath->mItems.push_back( pathItem );
		gCurY = pathItem.value = p->y / 64.0f;
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
		FT_Face face = (FT_Face)( GetGState()->GetTextFont()->GetFTFace() );
		if ( face )
		{
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
			
// 			FT_Outline_Funcs outline_funcs;
// 			outline_funcs.move_to = move_to;
// 			outline_funcs.line_to = line_to;
// 			outline_funcs.conic_to = conic_to;
// 			outline_funcs.cubic_to = cubic_to;
// 			outline_funcs.delta = NULL;
// 			outline_funcs.shift = NULL;
// 			FT_Outline_Decompose( &face->glyph->outline, &outline_funcs, pPathRet );
 

			FT_Outline * outline = &face->glyph->outline;
			void*                    user = (void*)pPathRet;

#undef SCALED
#define SCALED( x )  ( ( (x) << shift ) - delta )

			FT_Vector   v_last;
			FT_Vector   v_control;
			FT_Vector   v_start;

			FT_Vector*  point;
			FT_Vector*  limit;
			char*       tags;

			FT_Error    error;

			FT_Int   n;         /* index of contour in outline     */
			FT_UInt  first;     /* index of first point in contour */
			FT_Int   tag;       /* current point's state           */

			FT_Int   shift;
			FT_Pos   delta;


			
			shift = 0;//outline_funcs->shift;
			delta = 0;//outline_funcs->delta;
			first = 0;

			for ( n = 0; n < outline->n_contours; n++ )
			{
				FT_Int  last;  /* index of last point in contour */

				last = outline->contours[n];
// 				if ( last < 0 )
// 					goto Invalid_Outline;
				limit = outline->points + last;

				v_start   = outline->points[first];
				v_start.x = SCALED( v_start.x );
				v_start.y = SCALED( v_start.y );

				v_last   = outline->points[last];
				v_last.x = SCALED( v_last.x );
				v_last.y = SCALED( v_last.y );

				v_control = v_start;

				point = outline->points + first;
				tags  = outline->tags   + first;
				tag   = FT_CURVE_TAG( tags[0] );

				/* A contour cannot start with a cubic control point! */
				//if ( tag == FT_CURVE_TAG_CUBIC )
				//	

				/* check first point to determine origin */
				if ( tag == FT_CURVE_TAG_CONIC )
				{
					/* first point is conic control.  Yes, this happens. */
					if ( FT_CURVE_TAG( outline->tags[last] ) == FT_CURVE_TAG_ON )
					{
						/* start at last point if it is on the curve */
						v_start = v_last;
						limit--;
					}
					else
					{
						/* if both first and last points are conic,         */
						/* start at their middle and record its position    */
						/* for closure                                      */
						v_start.x = ( v_start.x + v_last.x ) / 2;
						v_start.y = ( v_start.y + v_last.y ) / 2;

						v_last = v_start;
					}
					point--;
					tags--;
				}

				//FT_TRACE5(( "  move to (%.2f, %.2f)\n",
				//	v_start.x / 64.0, v_start.y / 64.0 ));
				move_to( &v_start, user );
				//if ( error )
				//	goto Exit;

				while ( point < limit )
				{
					point++;
					tags++;

					tag = FT_CURVE_TAG( tags[0] );
					switch ( tag )
					{
					case FT_CURVE_TAG_ON:  /* emit a single line_to */
						{
							FT_Vector  vec;


							vec.x = SCALED( point->x );
							vec.y = SCALED( point->y );

							//FT_TRACE5(( "  line to (%.2f, %.2f)\n",
							//	vec.x / 64.0, vec.y / 64.0 ));
							error = line_to( &vec, user );
							//if ( error )
							//	goto Exit;
							continue;
						}

					case FT_CURVE_TAG_CONIC:  /* consume conic arcs */
						v_control.x = SCALED( point->x );
						v_control.y = SCALED( point->y );

Do_Conic:
						if ( point < limit )
						{
							FT_Vector  vec;
							FT_Vector  v_middle;


							point++;
							tags++;
							tag = FT_CURVE_TAG( tags[0] );

							vec.x = SCALED( point->x );
							vec.y = SCALED( point->y );

							if ( tag == FT_CURVE_TAG_ON )
							{
// 								FT_TRACE5(( "  conic to (%.2f, %.2f)"
// 									" with control (%.2f, %.2f)\n",
// 									vec.x / 64.0, vec.y / 64.0,
// 									v_control.x / 64.0, v_control.y / 64.0 ));
								conic_to( &v_control, &vec, user );
								//if ( error )
								//	goto Exit;
								continue;
							}

// 							if ( tag != FT_CURVE_TAG_CONIC )
// 								goto Invalid_Outline;

							v_middle.x = ( v_control.x + vec.x ) / 2;
							v_middle.y = ( v_control.y + vec.y ) / 2;

							
							conic_to( &v_control, &v_middle, user );
							
							v_control = vec;
							goto Do_Conic;
						}

						conic_to( &v_control, &v_start, user );
						goto Close;

					default:  /* FT_CURVE_TAG_CUBIC */
						{
							FT_Vector  vec1, vec2;


// 							if ( point + 1 > limit                             ||
// 								FT_CURVE_TAG( tags[1] ) != FT_CURVE_TAG_CUBIC )
// 								goto Invalid_Outline;

							point += 2;
							tags  += 2;

							vec1.x = SCALED( point[-2].x );
							vec1.y = SCALED( point[-2].y );

							vec2.x = SCALED( point[-1].x );
							vec2.y = SCALED( point[-1].y );

							if ( point <= limit )
							{
								FT_Vector  vec;


								vec.x = SCALED( point->x );
								vec.y = SCALED( point->y );

								cubic_to( &vec1, &vec2, &vec, user );
								continue;
							}

							cubic_to( &vec1, &vec2, &v_start, user );
							goto Close;
						}
					}
				}

				/* close the contour with a line segment */
				line_to( &v_start, user );

Close:
				first = last + 1;
			}
		}
	}
//Exit:
	return pPathRet;
}


CHE_PDF_Path::~CHE_PDF_Path()
{
	mItems.clear();
}

CHE_PDF_Form::~CHE_PDF_Form()
{

}


CHE_PDF_RefImage::CHE_PDF_RefImage( const CHE_ByteString & name, const CHE_PDF_ReferencePtr & pRef, CHE_Allocator * pAllocator /*= NULL*/ )
    : CHE_PDF_NamedContentObject(name, pAllocator), mRefPtr(pRef), mWidth(0), mHeight(0), mBpc(0), mpColorspace(NULL), mbMask(FALSE),
	mMaskDecode(0), mpBitmapCache(NULL)
{
    if ( mRefPtr )
    {
        CHE_PDF_ObjectPtr objPtr = mRefPtr->GetRefObj( OBJ_TYPE_STREAM );
        if ( objPtr )
        {
            mStmPtr = objPtr->GetStreamPtr();
            CHE_PDF_DictionaryPtr dictPtr = mStmPtr->GetDictPtr();
            if ( dictPtr )
            {
                objPtr = dictPtr->GetElement( "Width", OBJ_TYPE_NUMBER );
                if ( objPtr )
                {
                    mWidth = objPtr->GetNumberPtr()->GetInteger();
                }
                objPtr = dictPtr->GetElement( "Height", OBJ_TYPE_NUMBER );
                if ( objPtr )
                {
                    mHeight = objPtr->GetNumberPtr()->GetInteger();
                }
                objPtr = dictPtr->GetElement( "BitsPerComponent", OBJ_TYPE_NUMBER );
                if ( objPtr )
                {
                    mBpc = objPtr->GetNumberPtr()->GetInteger();
                }
				objPtr = dictPtr->GetElement( "ColorSpace", OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
					mpColorspace = CHE_PDF_ColorSpace::Create( objPtr->GetArrayPtr(), GetAllocator() );
				}else{
					objPtr = dictPtr->GetElement( "ColorSpace", OBJ_TYPE_NAME );
					if ( objPtr )
					{
						mpColorspace = CHE_PDF_ColorSpace::Create( objPtr->GetNamePtr(), GetAllocator() );
					}
				}
				objPtr = dictPtr->GetElement( "ImageMask", OBJ_TYPE_BOOLEAN );
				if ( objPtr )
				{
					if ( objPtr->GetBooleanPtr()->GetValue() == TRUE )
					{
						mbMask = TRUE;
					}
				}
				objPtr = dictPtr->GetElement( "Decode", OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
					CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
					if ( arrayPtr->GetCount() == 2 )
					{
						objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
						if ( objPtr )
						{
							if ( objPtr->GetNumberPtr()->GetInteger() == 1 )
							{
								mMaskDecode = 1;
							}else{
								mMaskDecode = 0;
							}
						}
					}
				}
				objPtr = dictPtr->GetElement( "Mask" );
				if ( objPtr )
				{
					mMaskPtr = objPtr;
				}
            }
        }
    }
}

CHE_PDF_RefImage::~CHE_PDF_RefImage()
{
	if ( mpColorspace )
	{
		mpColorspace->GetAllocator()->Delete( mpColorspace );
		mpColorspace = NULL;
	}
	if ( mpBitmapCache )
	{
		mpBitmapCache->GetAllocator()->Delete( mpBitmapCache );
		mpBitmapCache = NULL;
	}
}

CHE_Bitmap * CHE_PDF_RefImage::GetBitmap()
{
	if ( mpBitmapCache )
	{
		return mpBitmapCache;
	}

	CHE_Bitmap * pBitmapRet = NULL;
	if( mStmPtr )
	{
		HE_BOOL bMultiFilter = FALSE;
		CHE_PDF_ObjectPtr objPtr;
		CHE_ByteString filterName( GetAllocator() );
		CHE_PDF_DictionaryPtr dictPtr = mStmPtr->GetDictPtr();
		if ( dictPtr )
		{
			objPtr = dictPtr->GetElement( "Filter", OBJ_TYPE_NAME );
			if ( objPtr )
			{
				filterName = objPtr->GetNamePtr()->GetString();
			}else{
				objPtr = dictPtr->GetElement( "Filter", OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
					CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
					if ( arrayPtr->GetCount() > 1 )
					{
						bMultiFilter = TRUE;
					}
					objPtr = arrayPtr->GetElement( arrayPtr->GetCount()-1, OBJ_TYPE_NAME );
					if ( objPtr )
					{
						filterName = objPtr->GetNamePtr()->GetString();
					}
				}
			}
		}
		if ( filterName.GetLength() > 0 )
		{
			HE_ULONG ulSize = 0;
			HE_LPBYTE pData = NULL;
			if ( filterName == "JPXDecode" )
			{
				if ( bMultiFilter )
				{
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
					if ( stmAcc.Attach( mStmPtr, STREAM_DECODE_NOTLASTFILTER ) )
					{
						pData = stmAcc.GetData();
						ulSize = stmAcc.GetSize();
						pBitmapRet = JPXStreamToBitmap( pData, ulSize );
					}
				}else{
					ulSize = mStmPtr->GetRawSize();
					pData = GetAllocator()->NewArray<HE_BYTE>( ulSize );
					mStmPtr->GetRawData( 0, pData, ulSize );
					pBitmapRet = JPXStreamToBitmap( pData, ulSize );
					GetAllocator()->DeleteArray( pData );
				}
			}else if ( filterName == "DCTDecode" )
			{
				if ( bMultiFilter )
				{
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
					if ( stmAcc.Attach( mStmPtr, STREAM_DECODE_NOTLASTFILTER ) )
					{
						pData = stmAcc.GetData();
						ulSize = stmAcc.GetSize();
						pBitmapRet = JpegStreamToBitmap( pData, ulSize );
					}
				}else{
					ulSize = mStmPtr->GetRawSize();
					pData = GetAllocator()->NewArray<HE_BYTE>( ulSize );
					mStmPtr->GetRawData( 0, pData, ulSize );
					pBitmapRet = JpegStreamToBitmap( pData, ulSize );
					GetAllocator()->DeleteArray( pData );
				}
			}else if ( filterName == "JBIG2Decode" )
			{
				HE_LPBYTE pParam = NULL;
				HE_ULONG paramSize = 0;
				objPtr = dictPtr->GetElement( "JBIG2Globals", OBJ_TYPE_STREAM );
				if ( objPtr )
				{
					CHE_PDF_StreamPtr paramStrPtr = objPtr->GetStreamPtr();
					paramSize = paramStrPtr->GetRawSize();
					pParam = GetAllocator()->NewArray<HE_BYTE>( paramSize );
					paramStrPtr->GetRawData( 0, pParam, paramSize );
					//似乎有些不对，这个流要不要解码呢？
				}
				if ( bMultiFilter )
				{
					CHE_PDF_StreamAcc stmAcc( GetAllocator() );
					if ( stmAcc.Attach( mStmPtr, STREAM_DECODE_NOTLASTFILTER ) )
					{
						pData = stmAcc.GetData();
						ulSize = stmAcc.GetSize();
						pBitmapRet = JBig2StreamToBitmap( pData, ulSize, pParam, paramSize );
					}
				}else{
					ulSize = mStmPtr->GetRawSize();
					pData = GetAllocator()->NewArray<HE_BYTE>( ulSize );
					mStmPtr->GetRawData( 0, pData, ulSize );
					pBitmapRet = pBitmapRet = JBig2StreamToBitmap( pData, ulSize, pParam, paramSize );
					GetAllocator()->DeleteArray( pData );
				}
				if ( pParam )
				{
					GetAllocator()->DeleteArray( pParam );
				}
			}else{	
				pBitmapRet = StreamToBitmap();
			}
		}else{
			pBitmapRet = StreamToBitmap();
		}
	}

	mpBitmapCache = pBitmapRet;

	return pBitmapRet;
}

CHE_Bitmap * CHE_PDF_RefImage::StreamToBitmap()
{
	CHE_Bitmap * pBitmapRet = NULL;
	if ( mStmPtr )
	{
		CHE_PDF_StreamAcc stmAcc( GetAllocator() ) ;
		if ( stmAcc.Attach( mStmPtr ) )
		{
			if ( mbMask )
			{
				return GetStencilMaskingBitmap( stmAcc.GetData(), stmAcc.GetSize() );
			}

			CHE_PDF_Color	color;
			HE_LPBYTE		pData = stmAcc.GetData();
			HE_LPBYTE		pTmpByte = NULL;
			HE_BYTE			tmpByte1 = 0xFF;
			HE_BYTE			tmpByte2 = 0xFF;
			HE_ARGB			colorARGB1 = 0xFF000000;
			HE_ARGB			colorARGB2 = 0xFF000000;
			HE_ARGB *		pColors = GetAllocator()->NewArray<HE_ARGB>( mWidth );
			HE_ULONG		colorIndex = 0;
			HE_ULONG		componentCount = 1;
			HE_ULONG		stride = (mWidth * componentCount * mBpc + 7)/8;
			
			if ( mpColorspace == NULL )
			{
				mpColorspace = CHE_PDF_ColorSpace::Create( "DeviceGray", GetAllocator() );
			}
			componentCount = mpColorspace->GetComponentCount();
			if ( mpColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
			{
				componentCount = 1;
			}

			switch ( mpColorspace->GetType() )
			{
			case COLORSPACE_DEVICE_GRAY:
			case COLORSPACE_CIEBASE_CALGRAY:
				{
					pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
					pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
					if ( mBpc == 8 )
					{
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0; x < mWidth; ++x )
							{
								pTmpByte = pData + ( ( y * mWidth + x ) /** componentCount*/ );
								colorARGB1 = 0xFF000000 + *pTmpByte + (*pTmpByte << 8) + (*pTmpByte << 16); 
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 4 )
					{
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							pTmpByte = pData + ( y * stride );
							colorIndex = 0;
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 2; ++j )
								{
									tmpByte2 = ((tmpByte1>>(1-j)*4)&0x0F)*16.0f;
									colorARGB1 = 0xFF000000 + tmpByte2 + (tmpByte2 << 8) + (tmpByte2 << 16);
									*(pColors+colorIndex++) = colorARGB1;
									if ( colorIndex == mWidth )
									{
										break;
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 2 )
					{
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							pTmpByte = pData + ( y * stride );
							colorIndex = 0;
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 4; ++j )
								{	
									tmpByte2 = ((tmpByte1>>((3-j)*2))&0x03)*64.0f;
									colorARGB1 = 0xFF000000 + tmpByte2 + (tmpByte2 << 8) + (tmpByte2 << 16);
									*(pColors+colorIndex++) = colorARGB1;
									if ( colorIndex == mWidth )
									{
										break;
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 1 )
					{
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							pTmpByte = pData + ( y * stride );
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 8; ++j )
								{
									tmpByte2 = ((tmpByte1>>(7-j))&0x01)*255.0f;
									colorARGB1 = 0xFF000000 + tmpByte2 + (tmpByte2 << 8) + (tmpByte2 << 16);
									*(pColors+colorIndex++) = colorARGB1;
									if ( colorIndex == mWidth )
									{
										break;
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}
					break;
				}
			case COLORSPACE_DEVICE_RGB:
			case COLORSPACE_CIEBASE_CALRGB:
				{
					pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
					pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
					for ( HE_ULONG y = 0; y < mHeight; ++y  )
					{
						colorIndex = 0;
						for ( HE_ULONG x = 0; x < mWidth; ++x )
						{
							pTmpByte = pData + ( ( y * mWidth + x ) * 3/*componentCount*/ );
							colorARGB1 = 0xFF000000 + *(pTmpByte+2) + (*(pTmpByte+1) << 8) + (*pTmpByte << 16);
							*(pColors+colorIndex++) = colorARGB1;
						}
						pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
					}
					break;
				}
			case COLORSPACE_CIEBASE_ICCBASED:
				{
					if ( componentCount == 1 )
					{
						pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
						pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
						for ( HE_ULONG y = 0; y < mHeight; ++y  )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0; x < mWidth; ++x )
							{
								pTmpByte = pData + ( ( y * mWidth + x ) /** componentCount*/ );
								colorARGB1 = *pTmpByte + *pTmpByte << 8 + *pTmpByte << 16; 
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( componentCount == 3 )
					{
						pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
						pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
						for ( HE_ULONG y = 0; y < mHeight; ++y  )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0; x < mWidth; ++x )
							{
								pTmpByte = pData + ( ( y * mWidth + x ) * 3/*componentCount*/ );
								colorARGB1 = 0xFF000000 + *(pTmpByte+2) + (*(pTmpByte+1) << 8) + (*pTmpByte << 16);
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( componentCount == 4 )
					{
						pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
						pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
						for ( HE_ULONG y = 0; y < mHeight; ++y  )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0; x < mWidth; ++x )
							{
								color.Clear();
								pTmpByte = pData + ( ( y * mWidth + x ) * 4/*componentCount*/ );
								for ( HE_ULONG k = 0; k < componentCount; ++k )
								{
									color.Push( ( *pTmpByte ) / 255.0f );
									pTmpByte++;
								}
								colorARGB1 = mpColorspace->GetARGBValue( color );
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}
					break;
				}
			case COLORSPACE_DEVICE_CMYK:
			case COLORSPACE_CIEBASE_CALCMYK:
				{
					pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
					pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
					for ( HE_ULONG y = 0; y < mHeight; ++y  )
					{
						colorIndex = 0;
						for ( HE_ULONG x = 0; x < mWidth; ++x )
						{
							color.Clear();
							pTmpByte = pData + ( ( y * mWidth + x ) * 4/*componentCount*/ );
							for ( HE_ULONG k = 0; k < componentCount; ++k )
							{
								color.Push( ( *pTmpByte ) / 255.0f );
								pTmpByte++;
							}
							colorARGB1 = mpColorspace->GetARGBValue( color );
							*(pColors+colorIndex++) = colorARGB1;
						}
						pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
					}
					break;
				}
			case COLORSPACE_SPECIAL_INDEXED:
			case COLORSPACE_SPECIAL_SEPARATION:
			case COLORSPACE_SPECIAL_DEVICEN:
				{
					pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
					pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
					if ( mBpc == 8 )
					{
						color.Clear();
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							for ( HE_ULONG x = 0 ; x < mWidth; ++x )
							{
								pTmpByte = pData + ( ( y * mWidth + x ) * componentCount );
								for ( HE_ULONG k = 0; k < componentCount; ++k )
								{
									if ( mpColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
									{
										color.Push( *( pTmpByte ) );
									}else{
										color.Push( *( pTmpByte ) / 255.0f );
									}
									pTmpByte++;
								}
								colorARGB1 = mpColorspace->GetARGBValue( color );
								color.Clear();
								*(pColors+colorIndex++) = colorARGB1;
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}else if ( mBpc == 4 )
					{
						color.Clear();
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							pTmpByte = pData + ( y * stride );
							colorIndex = 0;
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 2; ++j )
								{
									if ( mpColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
									{
										color.Push( (tmpByte1>>(1-j)*4)&0x0F );
									}else{
										color.Push( ((tmpByte1>>(1-j)*4)&0x0F)/16.0f );
									}
									if ( color.GetComponentCount() == componentCount )
									{
										colorARGB1 = mpColorspace->GetARGBValue( color );
										color.Clear();
										*(pColors+colorIndex++) = colorARGB1;
										if ( colorIndex == mWidth )
										{
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
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							pTmpByte = pData + ( y * stride );
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 4; ++j )
								{
									if ( mpColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
									{
										color.Push( tmpByte1>>((3-j)*2) & 0x03 );
									}else{
										color.Push( ((tmpByte1>>((3-j)*2)) & 0x03) /4.0f );
									}
									if ( color.GetComponentCount() == componentCount )
									{
										colorARGB1 = mpColorspace->GetARGBValue( color );
										color.Clear();
										*(pColors+colorIndex++) = colorARGB1;
										if ( colorIndex == mWidth )
										{
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
						for ( HE_ULONG y = 0; y < mHeight; ++y )
						{
							colorIndex = 0;
							pTmpByte = pData + ( y * stride );
							for ( HE_ULONG i = 0; i < stride; ++i )
							{
								tmpByte1 = *(pTmpByte + i);
								for ( HE_ULONG j = 0; j < 8; ++j )
								{
									color.Push( ( tmpByte1 >> (7-j) ) & 0x01  );
									if ( color.GetComponentCount() == componentCount )
									{
										colorARGB1 = mpColorspace->GetARGBValue( color );
										color.Clear();
										*(pColors+colorIndex++) = colorARGB1;
										if ( colorIndex == mWidth )
										{
											break;
										}
									}
								}
							}
							pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
						}
					}
				}
			default:
				break;
			}

			if ( mMaskPtr )
			{
				//get mask data
				if ( mMaskPtr->GetType() == OBJ_TYPE_REFERENCE )
				{
					HE_ULONG maskWidth = 0;
					HE_ULONG maskHeight = 0;
					HE_ULONG maskBPC = 0;
					CHE_PDF_ObjectPtr objPtr = mMaskPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
					if ( objPtr )
					{
						CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
						CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
						if ( dictPtr )
						{
							objPtr = dictPtr->GetElement( "Width", OBJ_TYPE_NUMBER );
							if ( objPtr )
							{
								maskWidth = objPtr->GetNumberPtr()->GetInteger();
							}
							objPtr = dictPtr->GetElement( "Height", OBJ_TYPE_NUMBER );
							if ( objPtr )
							{
								maskHeight = objPtr->GetNumberPtr()->GetInteger();
							}
							objPtr = dictPtr->GetElement( "BitsPerComponent", OBJ_TYPE_NUMBER );
							if ( objPtr )
							{
								maskBPC = objPtr->GetNumberPtr()->GetInteger();
							}
							objPtr = dictPtr->GetElement( "ImageMask", OBJ_TYPE_BOOLEAN );
							if ( objPtr )
							{
								if ( objPtr->GetBooleanPtr()->GetValue() == FALSE )
								{
									//todo
								}
							}

							CHE_PDF_StreamAcc maskStmAcc( GetAllocator() ) ;
							if ( maskStmAcc.Attach( stmPtr ) )
							{
								HE_LPBYTE pMaskData = maskStmAcc.GetData();
								HE_LPBYTE pTmpByte = NULL;
								HE_BYTE	tmpByte = 0;
								HE_BYTE tmpValue = 0;
								HE_ARGB colorARGB = 0xFF000000;
								HE_ARGB colorTmpARGB = 0xFF000000;
								CHE_PDF_Color color;
								HE_ULONG colorsIndex = 0;
								HE_ULONG stride = (maskWidth * maskBPC + 7)/8;

								if ( maskBPC == 8 )
								{
									for ( HE_ULONG y = 0; y < mHeight; ++y )
									{
										colorsIndex = 0;
										for ( HE_ULONG x = 0; x < mWidth; ++x )
										{
											pTmpByte = pMaskData+(y*mWidth+x);
											colorTmpARGB = *pTmpByte;
											colorTmpARGB = 255 - tmpValue;
											colorTmpARGB = colorTmpARGB<<24;
											pBitmapRet->GetPixelColor( x, y, colorARGB );
											colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
											*(pColors+colorsIndex++) = colorTmpARGB;
										}
										pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
									}
								}else if ( maskBPC == 4 )
								{
									for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
									{
										pTmpByte = pMaskData + ( y * stride );
										colorsIndex = 0;
										x = 0;
										for ( HE_ULONG i = 0; i < stride; ++i )
										{
											tmpByte = *(pTmpByte + i);
											for ( HE_ULONG j = 0; j < 2; ++j )
											{
												tmpValue = ((tmpByte>>(1-j)*4)&0x0F)*255.0f;
												colorTmpARGB = 255 - tmpValue;
												colorTmpARGB = tmpValue;
												colorTmpARGB = colorTmpARGB<<24;
												colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
												*(pColors+colorsIndex++) = colorTmpARGB;
												if ( ++x == mWidth )
												{
													x = 0;
													break;
												}
											}
										}
										pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
									}
								}else if ( maskBPC == 2 )
								{
									for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
									{
										colorsIndex = 0;
										pTmpByte = pMaskData + ( y * stride );
										x = 0;
										for ( HE_ULONG i = 0; i < stride; ++i )
										{
											tmpByte = *(pTmpByte + i);
											for ( HE_ULONG j = 0; j < 4; ++j )
											{	
												tmpValue = ((tmpByte>>((3-j)*2))&0x03)*255.0f;
												colorTmpARGB = 255 - tmpValue;
												colorTmpARGB = tmpValue;
												colorTmpARGB = colorTmpARGB<<24;
												pBitmapRet->GetPixelColor( x, y, colorARGB );
												colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
												*(pColors+colorsIndex++) = colorTmpARGB;
												if ( ++x == mWidth )
												{
													x = 0;
													break;
												}
											}
										}
										pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
									}
								}else if ( maskBPC == 1 )
								{
									for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
									{
										colorsIndex = 0;
										pTmpByte = pMaskData + ( y * stride );
										x = 0;
										for ( HE_ULONG i = 0; i < stride; ++i )
										{
											tmpByte = *(pTmpByte + i);
											for ( HE_ULONG j = 0; j < 8; ++j )
											{
												tmpValue = ((tmpByte>>(7-j))&0x01)*255.0f;
												colorTmpARGB = 255 - tmpValue;
												colorTmpARGB = colorTmpARGB<<24;
												pBitmapRet->GetPixelColor( x, y, colorARGB );
												colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
												*(pColors+colorsIndex++) = colorTmpARGB;
												if ( ++x == mWidth )
												{
													x = 0;
													break;
												}
											}
										}
										pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
									}
								}
							}
						}
					}
				}
			}

			GetAllocator()->DeleteArray( pColors );
			stmAcc.Detach();
		}
	}
	return pBitmapRet;
}

static void fz_opj_error_callback(const char *msg, void *client_data)
{
}

static void fz_opj_warning_callback(const char *msg, void *client_data)
{
}

static void fz_opj_info_callback(const char *msg, void *client_data)
{
}

CHE_Bitmap * CHE_PDF_RefImage::JPXStreamToBitmap( HE_LPBYTE data, HE_ULONG size )
{
	opj_event_mgr_t     evtmgr;
	opj_dparameters_t   params;
	opj_dinfo_t *       info = NULL;
	opj_cio_t *         cio = NULL;
	opj_image_t *       jpx = NULL;
	CODEC_FORMAT        format;

	if ( data[0] == 0xFF && data[1] == 0x4F )
	{
		format = CODEC_J2K;
	}else{
		format = CODEC_JP2;
	}

	memset( &evtmgr, 0, sizeof(evtmgr) );
	evtmgr.error_handler	= fz_opj_error_callback;
	evtmgr.warning_handler	= fz_opj_warning_callback;
	evtmgr.info_handler		= fz_opj_info_callback;
	opj_set_default_decoder_parameters( &params );
	info = opj_create_decompress( format );
	opj_set_event_mgr( (opj_common_ptr)info, &evtmgr, NULL );
	opj_setup_decoder( info, &params );
	cio = opj_cio_open( (opj_common_ptr)info, data, size );
	jpx = opj_decode( info, cio );
	opj_cio_close( cio );
	opj_destroy_decompress( info );

	if ( !jpx )
	{
		return NULL;
	}

	for ( HE_ULONG i = 1; i < jpx->numcomps; ++i )
	{
		if ( jpx->comps[i].w != jpx->comps[0].w )
		{
			opj_image_destroy(jpx);
			return NULL;
		}
		if ( jpx->comps[i].h != jpx->comps[0].h )
		{
			opj_image_destroy(jpx);
			return NULL;
		}
		if (jpx->comps[i].prec != jpx->comps[0].prec )
		{
			opj_image_destroy(jpx);
			return NULL;
		}
	}

	CHE_Bitmap * pBitmapRet = NULL;
	HE_ARGB * pColors = NULL;
	HE_ULONG colorsIndex = 0;
	HE_ARGB colorARGB = 0xFF000000;
	HE_ULONG nComps, nWidth, nHeight, nAlpha, nDepth, nSgnd, k, x, y;
	int v;
	nComps = jpx->numcomps;
	nWidth = jpx->comps[0].w;
	nHeight = jpx->comps[0].h;
	nDepth = jpx->comps[0].prec;
	nSgnd = jpx->comps[0].sgnd;

	if ( jpx->color_space == CLRSPC_SRGB && nComps == 4 )
	{
		nComps = 3; nAlpha = 1;
	}
	else if ( jpx->color_space == CLRSPC_SYCC && nComps == 4 )
	{
		nComps = 3; nAlpha = 1;
	}
	else if ( nComps == 2 )
	{ 
		nComps = 1; nAlpha = 1;
	}
	else if ( nComps > 4 )
	{
		nComps = 4; nAlpha = 1;
	}else{
		nAlpha = 0;
	}

	pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
	pBitmapRet->Create( nWidth, nHeight, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
	pColors = GetAllocator()->NewArray<HE_ARGB>( nWidth );

	for ( y = 0; y < nHeight; ++y )
	{
		colorsIndex = 0;
		for ( x = 0; x < nWidth; ++x )
		{
			colorARGB = 0x000000FF;
			for ( k = 0; k < nComps + nAlpha; ++k )
			{
				v = jpx->comps[k].data[y * nWidth + x];
				if (nSgnd)
					v = v + (1 << (nDepth - 1));
				if (nDepth > 8)
					v = v >> (nDepth - 8);

				colorARGB = (colorARGB << 8) + v;
			}
			*(pColors+colorsIndex++) = colorARGB;
		}
		pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
	}

	opj_image_destroy( jpx );

	GetAllocator()->DeleteArray( pColors );

	return pBitmapRet;
}

// #include <jpeglib.h>
#include <setjmp.h>

struct jpeg_error_mgr_jmp
{
	struct jpeg_error_mgr super;
	jmp_buf env;
	char msg[JMSG_LENGTH_MAX];
};

static void error_exit(j_common_ptr cinfo)
{
	struct jpeg_error_mgr_jmp *err = (struct jpeg_error_mgr_jmp *)cinfo->err;
	cinfo->err->format_message(cinfo, err->msg);
	longjmp(err->env, 1);
}

static void init_source(j_decompress_ptr cinfo)
{
	/* nothing to do */
}

static void term_source(j_decompress_ptr cinfo)
{
	/* nothing to do */
}

static boolean fill_input_buffer(j_decompress_ptr cinfo)
{
	static unsigned char eoi[2] = { 0xFF, JPEG_EOI };
	struct jpeg_source_mgr *src = cinfo->src;
	src->next_input_byte = eoi;
	src->bytes_in_buffer = 2;
	return 1;
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	struct jpeg_source_mgr *src = cinfo->src;
	if (num_bytes > 0)
	{
		src->next_input_byte += num_bytes;
		src->bytes_in_buffer -= num_bytes;
	}
}

CHE_Bitmap * CHE_PDF_RefImage::JpegStreamToBitmap( HE_LPBYTE data, HE_ULONG size )
{
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_source_mgr			src;
	struct jpeg_error_mgr			err;

	HE_LPBYTE row[1], sp, dp;

	cinfo.err = &err;
	jpeg_std_error(cinfo.err);
	cinfo.err->error_exit = error_exit;
	cinfo.client_data = NULL;

	jpeg_create_decompress(&cinfo);

	cinfo.src = &src;
	src.init_source = init_source;
	src.fill_input_buffer = fill_input_buffer;
	src.skip_input_data = skip_input_data;
	src.resync_to_restart = jpeg_resync_to_restart;
	src.term_source = term_source;
	src.next_input_byte = data;
	src.bytes_in_buffer = size;

	jpeg_read_header( &cinfo, 1 );

	cinfo.dct_method = JDCT_FASTEST;
	cinfo.do_fancy_upsampling = FALSE;

	jpeg_start_decompress(&cinfo);

	CHE_Bitmap * pBitmapRet = NULL;
	CHE_PDF_Color color;
	HE_ARGB * pColors = NULL;
	HE_ULONG colorsIndex = 0;
	HE_ARGB colorARGB = 0xFF000000;
	HE_ULONG componentCount = 1;
	HE_ULONG stride = (mWidth * componentCount * mBpc + 7)/8;
	HE_BYTE tmpByte = 0;

	if ( mbMask )
	{
		CHE_PDF_GState * pGStae = GetGState();
		CHE_PDF_Color color;
		CHE_PDF_ColorSpace * pColorspace = CHE_PDF_ColorSpace::Create( "DeviceGray", GetAllocator() );
		HE_ARGB colorARGB = 0xFF000000;
		HE_ARGB colorTmpARGB = 0xFF00000;

		pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
		pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
		if ( pGStae )
		{
			pGStae->GetFillColor( color );
			pGStae->GetFillColorSpace( *pColorspace );
			if ( pColorspace )
			{
				colorARGB = pColorspace->GetARGBValue( color );
			}
		}

		row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );

		if ( mBpc == 8 )
		{
			for ( HE_ULONG y = 0; y < mHeight; ++y )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				for ( HE_ULONG x = 0; x < mWidth; ++x )
				{
					colorTmpARGB = *sp++;
					colorTmpARGB = colorTmpARGB<<24;
					colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
					*(pColors+colorsIndex++) = colorTmpARGB;
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
			}
		}else if ( mBpc == 4 )
		{
			for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				x = 0;
				for ( HE_ULONG i = 0; i < stride; ++i )
				{
					tmpByte = *(sp + i);
					for ( HE_ULONG j = 0; j < 2; ++j )
					{
						colorTmpARGB = ((tmpByte>>(1-j)*4)&0x0F)*255.0f;
						colorTmpARGB = colorTmpARGB<<24;
						colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
						*(pColors+colorsIndex++) = colorTmpARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
			}
		}else if ( mBpc == 2 )
		{
			for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				x = 0;
				for ( HE_ULONG i = 0; i < stride; ++i )
				{
					tmpByte = *(sp + i);
					for ( HE_ULONG j = 0; j < 4; ++j )
					{	
						colorTmpARGB = ((tmpByte>>((3-j)*2))&0x03)*255.0f;
						colorTmpARGB = colorTmpARGB<<24;
						colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
						*(pColors+colorsIndex++) = colorTmpARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
			}
		}else if ( mBpc == 1 )
		{
			for ( HE_ULONG y = 0, x = 0; y < mHeight; ++y )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				x = 0;
				for ( HE_ULONG i = 0; i < stride; ++i )
				{
					tmpByte = *(sp + i);
					for ( HE_ULONG j = 0; j < 8; ++j )
					{
						colorTmpARGB = ((tmpByte>>(7-j))&0x01)*255.0f;
						colorTmpARGB = colorTmpARGB<<24;
						colorTmpARGB = colorARGB & 0x00FFFFFF + colorTmpARGB;
						*(pColors+colorsIndex++) = colorTmpARGB;
						if ( ++x == mWidth )
						{
							x = 0;
							break;
						}
					}
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
			}
		}
		GetAllocator()->DeleteArray( pColors );
		GetAllocator()->DeleteArray( row[0] );
		return pBitmapRet;
	}

	if ( mpColorspace == NULL )
	{
	 	if ( cinfo.output_components == 4 )
		{
		 	mpColorspace = CHE_PDF_ColorSpace::Create( "DeviceCMYK", GetAllocator() );
		}else if ( cinfo.output_components == 3 )
		{
			mpColorspace = CHE_PDF_ColorSpace::Create( "DeviceRGB", GetAllocator() );
		}else if ( cinfo.output_components == 1 )
		{
		 	mpColorspace = CHE_PDF_ColorSpace::Create( "DeviceGray", GetAllocator() );
		}
	}
	
	switch ( mpColorspace->GetType() )
	{
	case COLORSPACE_DEVICE_GRAY:
	case COLORSPACE_CIEBASE_CALGRAY:
		{
			pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
			pBitmapRet->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
			row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
			pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
			colorsIndex = 0;
			colorARGB = 0xFF000000;
			for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
			{
				jpeg_read_scanlines( &cinfo, row,  1 );
				sp = row[0];
				colorsIndex = 0;
				for ( HE_ULONG x = 0; x < cinfo.output_width; x++)
				{
					colorARGB = *sp + (*sp << 8) + (*sp << 16);
					sp++;
					*(pColors+colorsIndex++) = colorARGB;
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
			}
			GetAllocator()->DeleteArray( row[0] );
			GetAllocator()->DeleteArray( pColors );
			break;
		}
	case COLORSPACE_DEVICE_RGB:
	case COLORSPACE_CIEBASE_CALRGB:
		{
			pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
			pBitmapRet->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
			row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
			pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
			for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
			{
				jpeg_read_scanlines( &cinfo, row, 1 );
				sp = row[0];
				colorsIndex = 0;
				for ( HE_ULONG x = 0; x < cinfo.output_width; x++)
				{
					colorARGB = *(sp+2) + (*(sp+1) << 8) + (*sp << 16);
					sp += 3;
 					*(pColors+colorsIndex++) = colorARGB;
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
			}
			GetAllocator()->DeleteArray( row[0] );
			GetAllocator()->DeleteArray( pColors );
			break;
		}
	case COLORSPACE_CIEBASE_ICCBASED:
		{
			componentCount = mpColorspace->GetComponentCount();
			if ( componentCount == 1 )
			{
				pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pBitmapRet->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
				row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
				pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
				colorsIndex = 0;
				colorARGB = 0xFF000000;
				for ( HE_ULONG y = 0; y < cinfo.output_height; ++y )
				{
					jpeg_read_scanlines( &cinfo, row,  1 );
					sp = row[0];
					colorsIndex = 0;
					for ( HE_ULONG x = 0; x < cinfo.output_width; ++x )
					{
						colorARGB = *sp + (*sp << 8) + (*sp << 16);
						sp++;
						*(pColors+colorsIndex++) = colorARGB;
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
				}
				GetAllocator()->DeleteArray( row[0] );
				GetAllocator()->DeleteArray( pColors );
			}else if ( componentCount == 3 )
			{
				pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pBitmapRet->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
				row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
				pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
				for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
				{
					jpeg_read_scanlines( &cinfo, row, 1 );
					sp = row[0];
					colorsIndex = 0;
					for ( HE_ULONG x = 0; x < cinfo.output_width; x++)
					{
						colorARGB = *(sp+2) + (*(sp+1) << 8) + (*sp << 16);
						sp += 3;
						*(pColors+colorsIndex++) = colorARGB;
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
				}
				GetAllocator()->DeleteArray( row[0] );
				GetAllocator()->DeleteArray( pColors );
			}else if ( componentCount == 4 )
			{
				pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pBitmapRet->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
				row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
				pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
				colorsIndex = 0;
				colorARGB = 0xFF000000;
				componentCount = mpColorspace->GetComponentCount();
				for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
				{
					jpeg_read_scanlines( &cinfo, row, 1 );
					sp = row[0];
					colorsIndex = 0;
					for ( HE_ULONG x = 0; x < cinfo.output_width; ++x )
					{
						color.Clear();
						for ( HE_ULONG k = 0; k < componentCount; ++k )
						{
							color.Push( ( *sp ) / 255.0f );
							sp++;
						}
						colorARGB = mpColorspace->GetARGBValue( color );
						*(pColors+colorsIndex++) = colorARGB;
					}
					pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
				}
				GetAllocator()->DeleteArray( row[0] );
				GetAllocator()->DeleteArray( pColors );
			}
			break;
		}
	case COLORSPACE_DEVICE_CMYK:
	case COLORSPACE_CIEBASE_CALCMYK:
	case COLORSPACE_SPECIAL_INDEXED:
	case COLORSPACE_SPECIAL_SEPARATION:
	case COLORSPACE_SPECIAL_DEVICEN:
		{
			pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
			pBitmapRet->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
			row[0] = GetAllocator()->NewArray<HE_BYTE>( cinfo.output_components * cinfo.output_width );
			pColors = GetAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
			colorsIndex = 0;
			colorARGB = 0xFF000000;
			componentCount = mpColorspace->GetComponentCount();
			for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
			{
				jpeg_read_scanlines( &cinfo, row, 1 );
				sp = row[0];
				colorsIndex = 0;
				for ( HE_ULONG x = 0; x < cinfo.output_width; ++x )
				{
					color.Clear();
					for ( HE_ULONG k = 0; k < componentCount; ++k )
					{
						color.Push( ( *sp ) / 255.0f );
						sp++;
					}
					colorARGB = mpColorspace->GetARGBValue( color );
					*(pColors+colorsIndex++) = colorARGB;
				}
				pBitmapRet->SetPixelColor( 0, y, pColors, cinfo.output_width );
			}
			GetAllocator()->DeleteArray( row[0] );
			GetAllocator()->DeleteArray( pColors );
			break;
		}
	default:
		break;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return pBitmapRet;
}

CHE_Bitmap * CHE_PDF_RefImage::JBig2StreamToBitmap( HE_LPBYTE data, HE_ULONG size, HE_LPBYTE globals, HE_ULONG globalsSize )
{
	Jbig2Ctx *ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, NULL, NULL, NULL );
	Jbig2GlobalCtx *gctx = NULL;
	Jbig2Image *page = NULL;

	if ( globals )
 	{
		jbig2_data_in( ctx, globals, globalsSize );
		gctx = jbig2_make_global_ctx( ctx );
		ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, gctx, NULL, NULL );
	}

	if ( !page )
	{
		jbig2_data_in( ctx, data, size );
		jbig2_complete_page( ctx );
		page = jbig2_page_out( ctx );
		//if page = NULL error happened!
	}

	//HE_LPBYTE p = desRet.GetData();
	//HE_LPBYTE ep = desRet.GetData() + desRet.GetSize();
	HE_LPBYTE s = page->data;
	HE_INT32 w = page->height * page->stride;
	HE_INT32 index = 0;

	if ( mbMask )
	{
		return GetStencilMaskingBitmap( s, w );
	}

	CHE_Bitmap * pBitmap = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
	pBitmap->Create( page->width, page->height, BITMAP_DEPTH_1BPP, BITMAP_DIRECTION_DOWN, w, s );

	//for ( HE_INT32 y = 0; y < page->height; ++y )
	//{
	//	for ( HE_INT32 x = 0; x < page->width; ++x )
	//	{
	//		pBitmap->SetPixelColor( x, y, s[y * page->stride + x] ^ 0xff );
	//	}
	//}
	// 	while (p < ep && x < w)
	// 	{
	// 		*p++ = s[x++] ^ 0xff;
	// 	}

	if ( page )
	{
		jbig2_release_page( ctx, page );
	}
	if ( gctx )
	{
		jbig2_global_ctx_free( gctx );
	}
	if ( ctx )
	{
		jbig2_ctx_free( ctx );
	}
	return pBitmap;
}

CHE_Bitmap * CHE_PDF_InlineImage::GetBitmap()
{
	CHE_Bitmap * pBitmapRet = NULL;
	if ( mpColorspace == NULL )
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
	
	HE_ULONG component = mpColorspace->GetComponentCount();
	if ( mpColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
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
					if ( mpColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
					{
						color.Push( *( pTmpByte ) );
					}else{
						color.Push( *( pTmpByte ) / 255.0f );
					}
					pTmpByte++;
				}
				colorARGB = mpColorspace->GetARGBValue( color );
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
					if ( mpColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
					{
						color.Push( (tmpByte>>(1-j)*4)&0x0F );
					}else{
						color.Push( ((tmpByte>>(1-j)*4)&0x0F)/16.0f );
					}
					if ( color.GetComponentCount() == component )
					{
						colorARGB = mpColorspace->GetARGBValue( color );
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
					if ( mpColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
					{
						color.Push( tmpByte>>((3-j)*2) & 0x03 );
					}else{
						color.Push( ((tmpByte>>((3-j)*2)) & 0x03) /4.0f );
					}
					if ( color.GetComponentCount() == component )
					{
						colorARGB = mpColorspace->GetARGBValue( color );
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
						colorARGB = mpColorspace->GetARGBValue( color );
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


CHE_Bitmap * CHE_PDF_RefImage::GetStencilMaskingBitmap( HE_LPBYTE pData, HE_ULONG size )
{
	if ( pData == NULL )
	{
		return NULL;
	}

	HE_ULONG stride = (mWidth * mBpc + 7)/8;

	if ( size < mHeight * stride  )
	{
		return NULL;
	}

	CHE_PDF_GState * pGStae = GetGState();
	CHE_PDF_Color color;
	CHE_PDF_ColorSpace colorspace( COLORSPACE_DEVICE_GRAY, GetAllocator() );
	HE_ARGB	colorARGB1 = 0xFF000000;
	HE_ARGB	colorARGB2 = 0xFF000000;
	CHE_Bitmap * pBitmapRet = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );

	if ( pGStae )
	{
		pGStae->GetFillColor( color );
		pGStae->GetFillColorSpace( colorspace );
		colorARGB1 = colorspace.GetARGBValue( color );
	}

	pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
	
	HE_ULONG colorIndex = 0;
	HE_ULONG byteIndex = 0;
	HE_ULONG bitIndex = 0;
	HE_LPBYTE pTmpByte = NULL;
	HE_BYTE tmpByte = 0xFF;
	HE_ARGB * pColors = GetAllocator()->NewArray<HE_ARGB>( mWidth );

	for ( HE_ULONG y = 0; y < mHeight; ++y )
	{
		pTmpByte = pData + ( y * stride );
		colorIndex = 0;
		for ( byteIndex = 0; byteIndex < stride; ++byteIndex )
		{
			tmpByte = *(pTmpByte + byteIndex);
			for ( bitIndex = 0; bitIndex < 8; ++bitIndex )
			{
				if ( mMaskDecode == 0 )
				{
					colorARGB2 = ((tmpByte>>(7-bitIndex))&0x01)*255.0f;
				}else{
					colorARGB2 = 255 - ((tmpByte>>(7-bitIndex))&0x01)*255.0f;
				}
				colorARGB2 = colorARGB2 << 24;
				colorARGB2 = colorARGB1 & 0x00FFFFFF + colorARGB2;
				*(pColors+colorIndex++) = colorARGB2;
				if ( colorIndex == mWidth )
				{
					break;
				}
			}
		}
		pBitmapRet->SetPixelColor( 0, y, pColors, mWidth );
	}
	GetAllocator()->DeleteArray( pColors );
	return pBitmapRet;
}













