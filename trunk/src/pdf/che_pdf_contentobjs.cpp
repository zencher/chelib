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
    : CHE_PDF_NamedContentObject(name, pAllocator), mRefPtr(pRef), mWidth(0), mHeight(0), mBpc(0), mpColorspace(NULL)
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
}

CHE_Bitmap * CHE_PDF_RefImage::GetBitmap()
{
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
			}
			else if ( filterName == "CCITTFaxDecode" )
			{
				CHE_PDF_StreamAcc stmAcc( GetAllocator() );
				if ( stmAcc.Attach( mStmPtr ) )
				{
					HE_LPBYTE pData = stmAcc.GetData();
					HE_LPBYTE pTargetByte = NULL;

					HE_ULONG bitmapStride = ( ( ( mWidth + 31 ) & ~31 ) >> 3 );
					HE_ULONG imageStride = ( ( mWidth - 1 ) >> 3 ) + 1;

					pBitmapRet = GetDefaultAllocator()->New<CHE_Bitmap>();
					pBitmapRet->Create( mWidth, mHeight, BITMAP_DEPTH_1BPP, BITMAP_DIRECTION_DOWN );

					pTargetByte = (HE_LPBYTE)( pBitmapRet->GetBuffer() );

					for ( HE_ULONG i = 0; i < mHeight; ++i )
					{
						memcpy( pTargetByte, pData, imageStride );

						pTargetByte = pTargetByte + bitmapStride;
						pData = pData + imageStride;
					}
					stmAcc.Detach();
				}
			}else{	
				pBitmapRet = CommonStreamToBitmap();
			}
		}else{
			pBitmapRet = CommonStreamToBitmap();
		}
	}
	return pBitmapRet;
}

CHE_Bitmap * CHE_PDF_RefImage::CommonStreamToBitmap()
{
	CHE_Bitmap * pBitmapRet = NULL;
	if ( mStmPtr )
	{
		CHE_PDF_StreamAcc stmAcc( GetAllocator() ) ;
		if ( stmAcc.Attach( mStmPtr ) )
		{
			if ( mpColorspace == NULL )
			{
				pBitmapRet = GetDefaultAllocator()->New<CHE_Bitmap>( GetAllocator() );
				pBitmapRet->Create( mWidth, mHeight, (HE_BITMAP_DEPTH)mBpc, BITMAP_DIRECTION_DOWN, stmAcc.GetSize(), stmAcc.GetData() );
			}else{
				HE_LPBYTE pData = stmAcc.GetData();
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

				pBitmapRet = GetDefaultAllocator()->New<CHE_Bitmap>( GetAllocator() );
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
                GetAllocator()->DeleteArray( pColors );
			}
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

	int a, n, w, h, depth, sgnd;
	int x, y, k, v;

	/* Check for SOC marker -- if found we have a bare J2K stream */
	if ( data[0] == 0xFF && data[1] == 0x4F )
	{
		format = CODEC_J2K;
	}else{
		format = CODEC_JP2;
	}

	memset( &evtmgr, 0, sizeof(evtmgr) );
	evtmgr.error_handler = fz_opj_error_callback;
	evtmgr.warning_handler = fz_opj_warning_callback;
	evtmgr.info_handler = fz_opj_info_callback;

	opj_set_default_decoder_parameters( &params );

// 	//if (indexed) ???
// 	{
// 		params.flags |= OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG;
// 	}

	info = opj_create_decompress( format );

	opj_set_event_mgr( (opj_common_ptr)info, &evtmgr, NULL );
	opj_setup_decoder( info, &params );

	cio = opj_cio_open( (opj_common_ptr)info, data, size );

	jpx = opj_decode( info, cio );

	opj_cio_close( cio );
	opj_destroy_decompress( info );

	if ( !jpx )
	{
		//decode error!!!
		return FALSE;
	}

	for (k = 1; k < jpx->numcomps; k++)
	{
		if (jpx->comps[k].w != jpx->comps[0].w)
		{
			opj_image_destroy(jpx);
			return FALSE;
			//fz_throw(ctx, "image components have different width");
		}
		if (jpx->comps[k].h != jpx->comps[0].h)
		{
			opj_image_destroy(jpx);
			return FALSE;
			//fz_throw(ctx, "image components have different height");
		}
		if (jpx->comps[k].prec != jpx->comps[0].prec)
		{
			opj_image_destroy(jpx);
			return FALSE;
			//fz_throw(ctx, "image components have different precision");
		}
	}

	n = jpx->numcomps;
	w = jpx->comps[0].w;
	h = jpx->comps[0].h;
	depth = jpx->comps[0].prec;
	sgnd = jpx->comps[0].sgnd;

	CHE_Bitmap * pBitmap = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );
	pBitmap->Create( w, h, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );

	if (jpx->color_space == CLRSPC_SRGB && n == 4) { n = 3; a = 1; }
	else if (jpx->color_space == CLRSPC_SYCC && n == 4) { n = 3; a = 1; }
	else if (n == 2) { n = 1; a = 1; }
	else if (n > 4) { n = 4; a = 1; }
	else { a = 0; }

	HE_ARGB color = 0;
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			color = 0;
			for (k = 0; k < n + a ; ++k )
			{
				v = jpx->comps[k].data[y * w + x];
				if (sgnd)
					v = v + (1 << (depth - 1));
				if (depth > 8)
					v = v >> (depth - 8);

				color = (color << 8) + v;
			}
			//if (!a)
			//	*p++ = 255;
			pBitmap->SetPixelColor( x, y, color );
			//pBitmap->ExchangeChannel( BITMAP_CHANNEL_Blue, BITMAP_CHANNEL_Red );
		}
	}

	opj_image_destroy( jpx );

	return pBitmap;
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
	struct jpeg_decompress_struct cinfo;
	struct jpeg_source_mgr src;
	struct jpeg_error_mgr err;
	/*	jmp_buf jb;*/

	unsigned char *row[1], *sp, *dp;
	/*fz_colorspace *colorspace;*/
	unsigned int x;
	int k;

	CHE_PDF_ColorSpace * colorspace = NULL;

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

	jpeg_read_header(&cinfo, 1);

	/* speed up jpeg decoding a bit */
	cinfo.dct_method = JDCT_FASTEST;
	cinfo.do_fancy_upsampling = FALSE;

	jpeg_start_decompress(&cinfo);

	if (cinfo.output_components == 4)
		colorspace = CHE_PDF_ColorSpace::Create( "DeviceCMYK", GetDefaultAllocator() );
	else if (cinfo.output_components == 3)
		colorspace = CHE_PDF_ColorSpace::Create( "DeviceRGB", GetDefaultAllocator() );
	else if (cinfo.output_components == 1)
		colorspace = CHE_PDF_ColorSpace::Create( "DeviceGray", GetDefaultAllocator() );
	// 	else
	// 		fz_throw(ctx, "bad number of components in jpeg: %d", cinfo.output_components);

	// 	fz_try(ctx)
	// 	{
	// 		image = fz_new_pixmap(ctx, colorspace, cinfo.output_width, cinfo.output_height);
	// 	}
	// 	fz_catch(ctx)
	// 	{
	//jpeg_finish_decompress(&cinfo);
	//jpeg_destroy_decompress(&cinfo);
	// 		fz_throw(ctx, "out of memory");
	// 	}

	// 	if (cinfo.density_unit == 1)
	// 	{
	// 		image->xres = cinfo.X_density;
	// 		image->yres = cinfo.Y_density;
	// 	}
	// 	else if (cinfo.density_unit == 2)
	// 	{
	// 		image->xres = cinfo.X_density * 254 / 100;
	// 		image->yres = cinfo.Y_density * 254 / 100;
	// 	}
	// 
	// 	if (image->xres <= 0) image->xres = 72;
	// 	if (image->yres <= 0) image->yres = 72;

	/*	fz_clear_pixmap(ctx, image);*/

	row[0] = GetDefaultAllocator()->NewArray<unsigned char>( cinfo.output_components * cinfo.output_width );
	//row[0] = fz_malloc(ctx, cinfo.output_components * cinfo.output_width);

	CHE_Bitmap * pBitmap = GetDefaultAllocator()->New<CHE_Bitmap>( GetDefaultAllocator() );

	if ( cinfo.output_components == 1 )
	{
		pBitmap->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_8BPP, BITMAP_DIRECTION_DOWN );
	}else{
		pBitmap->Create( cinfo.output_width, cinfo.output_height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
	}

	if ( colorspace )
	{
		CHE_PDF_Color color;
		HE_ARGB * pColors = GetDefaultAllocator()->NewArray<HE_ARGB>( cinfo.output_width );
		HE_ULONG colorsIndex = 0;
		HE_ARGB colorARGB = 0xFF000000;
		for ( HE_ULONG y = 0; y < cinfo.output_height; ++y  )
		{
			jpeg_read_scanlines(&cinfo, row, 1);
			sp = row[0];
			colorsIndex = 0;
			for ( x = 0; x < cinfo.output_width; x++)
			{
				color.Clear();
				for (k = 0; k < cinfo.output_components; k++)
				{
					color.Push( ( *sp ) / 255.0f );
					sp++;
				}
				colorARGB = colorspace->GetARGBValue( color );
				*(pColors+colorsIndex++) = colorARGB;
			}
			pBitmap->SetPixelColor( 0, y, pColors, cinfo.output_width );
		}
	}else{
		dp = (HE_LPBYTE)( pBitmap->GetBuffer() );
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, row, 1);
			sp = row[0];
			for (x = 0; x < cinfo.output_width; x++)
			{
				for (k = 0; k < cinfo.output_components; k++)
					*dp++ = *sp++;
				*dp++ = 255;
			}
		}
	}

	GetDefaultAllocator()->DeleteArray( row[0] );

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return pBitmap;
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
















