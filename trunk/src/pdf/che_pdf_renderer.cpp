#include "../../include/pdf/che_pdf_renderer.h"

HE_VOID CHE_PDF_Renderer::Render( CHE_PDF_ContentObjectList & content, CHE_GraphicsDrawer & drawer )
{
	CHE_PDF_GState * pGState = NULL;
	ContentObjectList::iterator it = content.Begin();

	HE_FLOAT val = 0;
	CHE_Matrix matrix;
	GRAPHICS_STATE_LINECAP linCap = LineCap_Butt;
	GRAPHICS_STATE_LINEJOIN lineJoin = LineJoin_Miter;
	GRAPHICS_STATE_DASHPATTERN lineDash;
	GRAPHICS_STATE_TEXTRENDERMODE tm;
	CHE_PDF_Color fillColor;
	CHE_PDF_Color strokeColor;
	CHE_PDF_ColorSpace fillColorSpace( COLORSPACE_DEVICE_GRAY );
	CHE_PDF_ColorSpace strokeColorSpace( COLORSPACE_DEVICE_GRAY );
	HE_DWORD fillColorVal = 0xFF000000;
	HE_DWORD strokeColorVal = 0xFF000000;

	for ( ; it != content.End(); ++it )
	{
		pGState = (*it)->GetGState();
		if ( pGState )
		{
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

			fillColorVal = fillColorSpace.GetArgb( fillColor );
			strokeColorVal = strokeColorSpace.GetArgb( strokeColor );
			drawer.SetFillColor( fillColorVal );
			drawer.SetStrokeColor( strokeColorVal );
		}

		switch ( (*it)->GetType() )
		{
		case ContentType_Path:
			{
				CHE_PDF_Path * pPath = (CHE_PDF_Path*)(*it);
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
					drawer.FillPath();
					drawer.StrokePath();
				case Paint_None:
				default:
					break;
				}
				break;
			}
		case ContentType_Text:
			{
				drawer.SetMatrix( CHE_Matrix() );
				CHE_PDF_Text * pText = (CHE_PDF_Text*)(*it);
				for ( size_t i = 0; i < pText->mItems.size(); ++i )
				{
					CHE_PDF_Path * pPath = pText->GetGraphPath( i );
					if ( pPath )
					{
						for ( HE_INT32 i = 0; i < pPath->mItems.size(); ++i )
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
						drawer.ClosePath();
						GRAPHICS_STATE_TEXTRENDERMODE rm;
						pGState->GetTextRenderMode( rm );
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
							break;
						case TextRenderMode_StrokeClip:
							drawer.StrokePath();
							break;
						case TextRenderMode_FillStrokeClip:
							drawer.StrokePath();
							break;
						case TextRenderMode_Clip:
							break;
						}
 						pPath->GetAllocator()->Delete( pPath );
					}
				}
				break;
			}
		default:
			break;
		}
	}
}