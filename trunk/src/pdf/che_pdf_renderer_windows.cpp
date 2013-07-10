#include "../../include/pdf/che_pdf_renderer_windows.h"
#include "../../include/che_image.h"

CHE_Bitmap * FlateImageToBitmap( CHE_PDF_RefImage * pImage )
{
	CHE_Bitmap * pRet = NULL;
	if ( pImage == NULL )
	{
		return pRet;
	}
	
	CHE_PDF_StreamPtr stmPtr = pImage->GetStreamPtr();
	if ( stmPtr )
	{
		CHE_PDF_ObjectPtr objPtr;
		CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();

		objPtr = dictPtr->GetElement( "Filter", OBJ_TYPE_NAME );
		if ( objPtr )
		{
			if ( objPtr->GetNamePtr()->GetString() == "FlateDecode" )
			{
				HE_ULONG width = pImage->GetWidth();
				HE_ULONG height = pImage->GetHeight();
				HE_ULONG bpc = pImage->GetBPC();
				CHE_PDF_ColorSpace * pColorspace = pImage->GetColorspace();

				if ( pColorspace )
				{
					CHE_PDF_StreamAcc stmAcc;
					if ( stmAcc.Attach( stmPtr ) )
					{
						HE_LPBYTE pData = stmAcc.GetData();
						HE_LPBYTE pTmpByte = NULL;
						HE_ARGB colorARGB = 0xFF000000;
						CHE_PDF_Color color;
						HE_ULONG component = pColorspace->GetComponentCount();

						pRet = GetDefaultAllocator()->New<CHE_Bitmap>();

						if ( pColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
						{
							component = 1;
						}
						pRet->Create( width, height, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN );
						if ( bpc == 8 )
						{
							for ( HE_ULONG y = 0; y < height; ++y )
							{
								for ( HE_ULONG x = 0 ; x < width; ++x )
								{
									color.mConponents.clear();
									pTmpByte = pData + ( ( y * width + x ) * component );
									for ( HE_ULONG i = 0; i < component; ++i )
									{
										if ( pColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
										{
											color.mConponents.push_back( *( pTmpByte ) );
										}else{
											color.mConponents.push_back( *( pTmpByte ) / 255.0f );
										}
										pTmpByte++;
									}
									colorARGB = pColorspace->GetARGBValue( color );
									pRet->SetPixelColor( x, y, colorARGB );
								}
							}
						}else if ( bpc == 4 )
						{
							HE_ULONG stride = (width * component * bpc + 7) / 8;							
							HE_BYTE tmpByte = 0;
							color.mConponents.clear();
							for ( HE_ULONG y = 0, x = 0; y < height; ++y )
							{
								x = 0;
								pTmpByte = pData + ( y * stride );
								for ( HE_ULONG i = 0; i < stride; ++i )
								{
									tmpByte = *(pTmpByte + i);

									if ( pColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
									{
										color.mConponents.push_back( tmpByte >> 4 );
									}else{
										color.mConponents.push_back( ( tmpByte >> 4 ) / 16.0f );
									}
									if ( color.mConponents.size() == component )
									{
										colorARGB = pColorspace->GetARGBValue( color );
										pRet->SetPixelColor( x++, y, colorARGB );
										color.mConponents.clear();
									}
									if ( pColorspace->GetType() == COLORSPACE_SPECIAL_INDEXED )
									{
										color.mConponents.push_back( tmpByte & 0x0F );
									}else{
										color.mConponents.push_back( ( tmpByte & 0x0F ) / 16.0f );
									}
									if ( color.mConponents.size() == component )
									{
										colorARGB = pColorspace->GetARGBValue( color );
										pRet->SetPixelColor( x++, y, colorARGB );
										color.mConponents.clear();
									}
								}
							}
						}else if ( bpc == 2 )
						{
						}else if ( bpc == 1 )
						{
						}
						stmAcc.Detach();
					}
				}
			}
		}
	}

	return pRet;
}

inline HE_VOID OutputCommonGSatae( CHE_GraphicsDrawer & drawer, CHE_PDF_GState * pGState )
{
	static HE_FLOAT val = 0;
	static CHE_Matrix matrix;
	static GRAPHICS_STATE_LINECAP linCap = LineCap_Butt;
	static GRAPHICS_STATE_LINEJOIN lineJoin = LineJoin_Miter;
	static GRAPHICS_STATE_DASHPATTERN lineDash;
	static GRAPHICS_STATE_TEXTRENDERMODE tm;
	static CHE_PDF_Color fillColor;
	static CHE_PDF_Color strokeColor;
	static CHE_PDF_ColorSpace fillColorSpace( COLORSPACE_DEVICE_GRAY );
	static CHE_PDF_ColorSpace strokeColorSpace( COLORSPACE_DEVICE_GRAY );
	static HE_ULONG fillColorVal = 0xFF000000;
	static HE_ULONG strokeColorVal = 0xFF000000;

	pGState->GetLineWidth( val );
	drawer.SetLineWidth( val );

	pGState->GetLineCap( linCap );
	drawer.SetLineCap( linCap );

	pGState->GetLineJoin( lineJoin );
	drawer.SetLineJoin( lineJoin );

	pGState->GetLineDash( lineDash );
	drawer.SetLineDash( lineDash );

	matrix = pGState->GetMatrix();
	drawer.SetMatrix( matrix );

	pGState->GetFillColor( fillColor );
	pGState->GetStrokeColor( strokeColor );
	pGState->GetFillColorSpace( fillColorSpace );
	pGState->GetStrokeColorSpace( strokeColorSpace );

	fillColorVal = fillColorSpace.GetARGBValue( fillColor );
	strokeColorVal = strokeColorSpace.GetARGBValue( strokeColor );
	drawer.SetFillColor( fillColorVal );
	drawer.SetStrokeColor( strokeColorVal );
	drawer.SetFillMode( FillMode_Nonzero );
}

inline HE_VOID OutputClipState( CHE_GraphicsDrawer & drawer, CHE_PDF_ClipState * pClipState )
{
	CHE_PDF_ContentObject * pObj = NULL;
	CHE_PDF_GState * pGState = NULL;
	std::list<CHE_PDF_ClipStateItem*>::iterator it = pClipState->mClipElementList.begin();
	for ( ; it != pClipState->mClipElementList.end(); it++ )
	{
		pObj = (*it)->GetElement();
		if ( !pObj )
		{
			continue;
		}

		pGState = pObj->GetGState();
		if ( pGState )
		{
			OutputCommonGSatae( drawer, pGState );
		}

		switch ( pObj->GetType() )
		{
		case ContentType_Path:
			{
				CHE_PDF_Path * pPath = (CHE_PDF_Path*)(pObj);
				CHE_Point p1, p2, p3;
				for ( size_t i = 0; i < pPath->mItems.size(); ++i )
				{
					switch ( pPath->mItems[i].type )
					{
					case PathItem_MoveTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							drawer.MoveTo( p1.x, p1.y );
							i+=2;
							break;
						}
					case PathItem_LineTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							drawer.LineTo( p1.x, p1.y );
							i+=2;
							break;
						}
					case PathItem_CurveTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							p2.x = pPath->mItems[i+3].value;
							p2.y = pPath->mItems[i+4].value;
							p3.x = pPath->mItems[i+5].value;
							p3.y = pPath->mItems[i+6].value;
							drawer.CurveTo( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y );
							i+=6;
							break;
						}
					case PathItem_Rectangle:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							drawer.MoveTo( p1.x, p1.y );
							drawer.LineTo( p1.x + pPath->mItems[i+3].value, p1.y );
							drawer.LineTo( p1.x + pPath->mItems[i+3].value, p1.y + pPath->mItems[i+4].value );
							drawer.LineTo( p1.x, p1.y + pPath->mItems[i+4].value );
							drawer.ClosePath();
							i+=4;
							break;
						}
					case PathItem_Close:
						{
							drawer.ClosePath();
							break;
						}
					default:
						break;
					}
				}
				break;
			}
		case ContentType_Text:
			{
				drawer.SetMatrix( CHE_Matrix() );
				CHE_PDF_Text * pText = (CHE_PDF_Text*)(pObj);
				for ( HE_ULONG j = 0; j < pText->mItems.size(); ++j )
				{
					CHE_PDF_Path * pPath = pText->GetGraphPath( j );
					if ( pPath )
					{
						for ( HE_ULONG i = 0; i < pPath->mItems.size(); ++i )
						{
							switch ( pPath->mItems[i].type )
							{
							case PathItem_MoveTo:
								{
									drawer.MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
									i+=2;
									break;
								}
							case PathItem_LineTo:
								{
									drawer.LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
									i+=2;
									break;
								}
							case PathItem_CurveTo:
								{
									drawer.CurveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value,
										pPath->mItems[i+3].value, pPath->mItems[i+4].value,
										pPath->mItems[i+5].value, pPath->mItems[i+6].value );
									i+=6;
									break;
								}
							case PathItem_Rectangle:
								{
									drawer.MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
									drawer.LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value );
									drawer.LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
									drawer.LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
									drawer.ClosePath();
									i+=4;
									break;
								}
							case PathItem_Close:
								{
									//drawer.ClosePath();
									i+=1;
									break;
								}
							default:
								break;
							}
						}
						//drawer.ClosePath();
						pPath->GetAllocator()->Delete( pPath );
					}
				}
				break;
			}
		default:
			break;
		}
	}

	drawer.ClipPath();
}

HE_VOID outputForm( CHE_PDF_ContentObject & list, CHE_GraphicsDrawer & drawer )
{

}

HE_VOID CHE_PDF_Renderer::Render(	CHE_PDF_ContentObjectList & content, CHE_GraphicsDrawer & drawer, CHE_Rect pageRect,
									HE_FLOAT scale, HE_FLOAT dipx, HE_FLOAT dipy, CHE_Rect * pClipRect )
{
	//设置好bitmap的大小
	if ( pClipRect != NULL )
	{
		drawer.Resize( pClipRect->width * scale * dipx / 72, pClipRect->height * scale * dipy / 72 );
	}else{
		drawer.Resize( pageRect.width * scale * dipx / 72, pageRect.height * scale * dipy / 72 );
	}

	//计算外部matrix，以为gdi+的坐标和pdf的不同
	CHE_Matrix extMatrix;
	extMatrix.a = dipx * scale / 72;
	extMatrix.b = 0;
	extMatrix.c = 0;
	extMatrix.d = - dipy * scale / 72;
	extMatrix.e = 0;
	extMatrix.f = 0;
	CHE_Matrix tmpMatrix;
	if ( pClipRect != NULL )
	{
		tmpMatrix.e = - pClipRect->left * dipx * scale / 72;
		tmpMatrix.f = ( pClipRect->height + pClipRect->bottom ) * dipy * scale / 72;
	}else{
		tmpMatrix.e = 0;
		tmpMatrix.f = pageRect.height * dipy * scale / 72;
	}
	extMatrix.Concat( tmpMatrix );
	drawer.SetExtMatrix( extMatrix );

	CHE_PDF_GState * pGState = NULL;
	CHE_PDF_ClipState * pClipState = NULL;
	ContentObjectList::iterator it = content.Begin();
	for ( ; it != content.End(); ++it )
	{
		//设置图形状态
		//先将clip信息清零
		drawer.ResetClip();
		pGState = (*it)->GetGState();
		if ( pGState )
		{
			pClipState = pGState->GetClipState();
			if ( pClipState )
			{
				OutputClipState( drawer, pClipState );
			}
			OutputCommonGSatae( drawer, pGState );
		}

		switch ( (*it)->GetType() )
		{
		case ContentType_Form:
			{
				CHE_PDF_Form * pForm = (CHE_PDF_Form *)( *it );
				break;
			}
		case ContentType_Path:
			{
				CHE_PDF_Path * pPath = (CHE_PDF_Path*)(*it);
				if ( pPath->GetFillMode() == Mode_Nonzero )
				{
					drawer.SetFillMode( FillMode_Nonzero );
				}
				else{
					drawer.SetFillMode( FillMode_EvenOdd );
				}
				CHE_Point p1, p2, p3;
				for ( size_t i = 0; i < pPath->mItems.size(); ++i )
				{
					switch ( pPath->mItems[i].type )
					{
					case PathItem_MoveTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							drawer.MoveTo( p1.x, p1.y );
							i+=2;
							break;
						}
					case PathItem_LineTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							drawer.LineTo( p1.x, p1.y );
							i+=2;
							break;
						}
					case PathItem_CurveTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							p2.x = pPath->mItems[i+3].value;
							p2.y = pPath->mItems[i+4].value;
							p3.x = pPath->mItems[i+5].value;
							p3.y = pPath->mItems[i+6].value;
							drawer.CurveTo( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y );
							i+=6;
							break;
						}
					case PathItem_Rectangle:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							drawer.MoveTo( p1.x, p1.y );
							drawer.LineTo( p1.x + pPath->mItems[i+3].value, p1.y );
							drawer.LineTo( p1.x + pPath->mItems[i+3].value, p1.y + pPath->mItems[i+4].value );
							drawer.LineTo( p1.x, p1.y + pPath->mItems[i+4].value );
							drawer.ClosePath();
							i+=4;
							break;
						}
					case PathItem_Close:
						{
							drawer.ClosePath();
							break;
						}
					default:
						break;
					}
				}
				switch ( pPath->GetPaintType() )
				{
				case Paint_Fill:
					drawer.FillPath();
					break;
				case Paint_Stroke:
					drawer.StrokePath();
					break;
				case Paint_FillStroke:
					drawer.FillStrokePath();
					break;
				case Paint_None:
				default:
					break;
				}
				break;
			}
		case ContentType_Text:
			{
				GRAPHICS_STATE_TEXTRENDERMODE rm;
				pGState->GetTextRenderMode( rm );
				switch( rm )
				{
				case TextRenderMode_Fill:
				case TextRenderMode_FillClip:
				case TextRenderMode_FillStroke:
				case TextRenderMode_Stroke:
				case TextRenderMode_StrokeClip:
				case TextRenderMode_FillStrokeClip:
					{
						drawer.SetMatrix( CHE_Matrix() );
						CHE_PDF_Text * pText = (CHE_PDF_Text*)(*it);
						for ( HE_ULONG j = 0; j < pText->mItems.size(); ++j )
						{
							CHE_PDF_Path * pPath = pText->GetGraphPath( j );
							if ( pPath )
							{
								for ( HE_ULONG i = 0; i < pPath->mItems.size(); ++i )
								{
									switch ( pPath->mItems[i].type )
									{
									case PathItem_MoveTo:
										{
											drawer.MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
											i+=2;
											break;
										}
									case PathItem_LineTo:
										{
											drawer.LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
											i+=2;
											break;
										}
									case PathItem_CurveTo:
										{
											drawer.CurveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value,
												pPath->mItems[i+3].value, pPath->mItems[i+4].value,
												pPath->mItems[i+5].value, pPath->mItems[i+6].value );
											i+=6;
											break;
										}
									case PathItem_Rectangle:
										{
											drawer.MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
											drawer.LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value );
											drawer.LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
											drawer.LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
											drawer.ClosePath();
											i+=4;
											break;
										}
									case PathItem_Close:
										{
											drawer.ClosePath();
											i+=1;
											break;
										}
									default:
										break;
									}
								}
								pPath->GetAllocator()->Delete( pPath );
							}
						}
						break;
					}
				default:
					break;
				}
				switch( rm )
				{
				case TextRenderMode_Fill:
					drawer.FillPath();
					break;
				case TextRenderMode_Stroke:
					drawer.StrokePath();
					break;
				case TextRenderMode_FillStroke:
					drawer.FillStrokePath();
					break;
				case TextRenderMode_Invisible:
					break;
				case TextRenderMode_FillClip:
					drawer.FillPath();
					//drawer.FillClipPath();
					break;
				case TextRenderMode_StrokeClip:
					drawer.StrokePath();
					//drawer.StrokeClipPath();
					break;
				case TextRenderMode_FillStrokeClip:
					drawer.StrokePath();
					//drawer.FillStrokeClipPath();
					break;
				case TextRenderMode_Clip:
					//drawer.ClipPath();
					break;
				}
				break;
			}
		case ContentType_RefImage:
			{
                CHE_PDF_RefImage * pImage = (CHE_PDF_RefImage*)(*it);
				CHE_PDF_StreamPtr stmPtr = pImage->GetStreamPtr();
				if ( stmPtr )
				{
					CHE_PDF_ObjectPtr objPtr;
					CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
					objPtr = dictPtr->GetElement( "Filter", OBJ_TYPE_NAME );
					if ( objPtr )
					{
						if ( objPtr->GetNamePtr()->GetString() == "JBIG2Decode" )
						{
							CHE_ImageDecoder imgDecoder;
							HE_LPBYTE pParam = NULL;
							HE_ULONG paramSize = 0;

							objPtr = dictPtr->GetElement( "JBIG2Globals", OBJ_TYPE_STREAM );
							if ( objPtr )
							{
								CHE_PDF_StreamPtr paramStrPtr = objPtr->GetStreamPtr();
								paramSize = paramStrPtr->GetRawSize();
								pParam = GetDefaultAllocator()->NewArray<HE_BYTE>( paramSize );
								paramStrPtr->GetRawData( 0, pParam, paramSize );
								imgDecoder.SetDecodeParam( pParam, paramSize );
								//todo
								//似乎有些不对，这个流要不要解码呢？
							}

							HE_LPBYTE pBuf = new HE_BYTE[stmPtr->GetRawSize()];
							stmPtr->GetRawData( 0, pBuf, stmPtr->GetRawSize() );

							CHE_Bitmap * pBitmap = imgDecoder.Decode( IMAGE_TYPE_JBIG2, pBuf, stmPtr->GetRawSize() );
							if ( pBitmap )
							{
								HE_LPBYTE bitmap = new HE_BYTE[pBitmap->GetMemBitmapDataSize()+14];
								pBitmap->SaveToMem( bitmap, pBitmap->GetMemBitmapDataSize()+14 );
								drawer.DrawImage( IMAGE_BMP, bitmap, pBitmap->GetMemBitmapDataSize()+14 );
								delete [] pBuf;
								delete [] bitmap;
								pBitmap->GetAllocator()->Delete( pBitmap );
								pBitmap = NULL;
							}

							if ( pParam )
							{
								GetDefaultAllocator()->DeleteArray( pParam );
							}

						}else if( objPtr->GetNamePtr()->GetString() == "JPXDecode" )
						{
							CHE_ImageDecoder imgDecoder;

							HE_LPBYTE pBuf = new HE_BYTE[stmPtr->GetRawSize()];
							stmPtr->GetRawData( 0, pBuf, stmPtr->GetRawSize() );
							CHE_Bitmap * pBitmap = imgDecoder.Decode( IMAGE_TYPE_JPX, pBuf, stmPtr->GetRawSize() );
							HE_LPBYTE bitmap = new HE_BYTE[pBitmap->GetMemBitmapDataSize()+14];
							pBitmap->SaveToMem( bitmap, pBitmap->GetMemBitmapDataSize()+14 );
							drawer.DrawImage( IMAGE_BMP, bitmap, pBitmap->GetMemBitmapDataSize()+14 );
							delete [] pBuf;
							delete [] bitmap;
							pBitmap->GetAllocator()->Delete( pBitmap );
							pBitmap = NULL;
						}else if ( objPtr->GetNamePtr()->GetString() == "FlateDecode" )
						{
							CHE_Bitmap * pBitmap = FlateImageToBitmap( pImage );
							if ( pBitmap )
							{
								HE_LPBYTE pBuf = GetDefaultAllocator()->NewArray<HE_BYTE>( pBitmap->GetMemBitmapDataSize() + 14 );
								pBitmap->SaveToMem( pBuf,pBitmap->GetMemBitmapDataSize()+14 );
								drawer.DrawImage( IMAGE_BMP, pBuf, pBitmap->GetMemBitmapDataSize()+14 );
								GetDefaultAllocator()->DeleteArray( pBuf );
								pBitmap->GetAllocator()->Delete( pBitmap );
								pBitmap = NULL;
							}
						}else{
							HE_LPBYTE pBuf = new HE_BYTE[stmPtr->GetRawSize()];
							stmPtr->GetRawData( 0, pBuf, stmPtr->GetRawSize() );
							drawer.DrawImage( IMAGE_BMP, pBuf, stmPtr->GetRawSize() );
							delete [] pBuf;
							pBuf = NULL;
						}
					}
				}
				break;
			}
		case ContentType_InlineImage:
			{
				CHE_PDF_InlineImage * pImage = (CHE_PDF_InlineImage*)(*it);
				CHE_Bitmap * pBitmap = pImage->GetBitmap();
				if ( pBitmap )
				{
					HE_LPBYTE pBuf = GetDefaultAllocator()->NewArray<HE_BYTE>( pBitmap->GetMemBitmapDataSize()+14 );
					pBitmap->SaveToMem( pBuf, pBitmap->GetMemBitmapDataSize()+14 );
					drawer.DrawImage( IMAGE_BMP, pBuf, pBitmap->GetMemBitmapDataSize()+14 );
					GetDefaultAllocator()->DeleteArray( pBuf );
					pBitmap->GetAllocator()->Delete( pBitmap );
				}
				break;
			}
		default:
			break;
		}
	}
}