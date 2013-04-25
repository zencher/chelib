#include "../../include/pdf/che_pdf_renderer.h"

HE_VOID CHE_PDF_Renderer::Render( CHE_PDF_ContentObjectList & content, IHE_PDF_GraphicsDraw * pDraw )
{
	if ( pDraw == NULL )
	{
		return;
	}

	CHE_PDF_GState * pGState = NULL;
	ContentObjectList::iterator it = content.Begin();

	HE_FLOAT val = 0;
	CHE_PDF_Matrix matrix;
	PDF_GSTATE_LINECAP linCap = LineCap_Butt;
	PDF_GSTATE_LINEJOIN lineJoin = LineJoin_Miter;
	PDF_GSTATE_DASHPATTERN lineDash;
	PDF_GSTATE_TEXTRENDERMODE tm;
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
			pDraw->SetLineWidth( val );

			pGState->GetLineCap( linCap );
			pDraw->SetLineCap( linCap );

			pGState->GetLineJoin( lineJoin );
			pDraw->SetLineJoin( lineJoin );

			pGState->GetLineDash( lineDash );
			pDraw->SetLineDash( lineDash );

			matrix = pGState->GetMatrix();
			pDraw->SetMatrix( matrix );

			pGState->GetFillColor( fillColor );
			pGState->GetStrokeColor( strokeColor );
			pGState->GetFillColorSpace( fillColorSpace );
			pGState->GetStrokeColorSpace( strokeColorSpace );

			fillColorVal = fillColorSpace.GetArgb( fillColor );
			strokeColorVal = strokeColorSpace.GetArgb( strokeColor );
			pDraw->SetFillColor( fillColorVal );
			pDraw->SetStrokeColor( strokeColorVal );
		}

		switch ( (*it)->GetType() )
		{
		case ContentType_Path:
			{
				CHE_PDF_Path * pPath = (CHE_PDF_Path*)(*it);
				CHE_PDF_Point p1, p2, p3;
				//CHE_PDF_Matrix matrix = pGState->GetMatrix();
				for ( size_t i = 0; i < pPath->mItems.size(); ++i )
				{
					switch ( pPath->mItems[i].type )
					{
					case PathItem_MoveTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							//p1 = matrix.Transform( p1 );
							pDraw->MoveTo( p1.x, p1.y );
							i+=2;
							break;
						}
					case PathItem_LineTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							//p1 = matrix.Transform( p1 );
							pDraw->LineTo( p1.x, p1.y );
							i+=2;
							break;
						}
					case PathItem_CurveTo:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							//p1 = matrix.Transform( p1 );
							p2.x = pPath->mItems[i+3].value;
							p2.y = pPath->mItems[i+4].value;
							//p2 = matrix.Transform( p2 );
							p3.x = pPath->mItems[i+5].value;
							p3.y = pPath->mItems[i+6].value;
							//p3 = matrix.Transform( p3 );
							pDraw->CurveTo( p1.x, p1.y, p2.x, p2.y, p3.x, p3.y );
							i+=6;
							break;
						}
					case PathItem_Rectangle:
						{
							p1.x = pPath->mItems[i+1].value;
							p1.y = pPath->mItems[i+2].value;
							//p1 = matrix.Transform( p1 );
							pDraw->MoveTo( p1.x, p1.y );
							pDraw->LineTo( p1.x + pPath->mItems[i+3].value, p1.y );
							pDraw->LineTo( p1.x + pPath->mItems[i+3].value, p1.y + pPath->mItems[i+4].value );
							pDraw->LineTo( p1.x, p1.y + pPath->mItems[4].value );
							pDraw->ClosePath();
							i+=4;
							break;
						}
					case PathItem_Close:
						{
							pDraw->ClosePath();
							break;
						}
					default:
						break;
					}
				}
				//pPath->GetFillMode();
				switch ( pPath->GetPaintType() )
				{
				case Paint_Fill:
					pDraw->FillPath();
					break;
				case Paint_Stroke:
					pDraw->StrokePath();
					break;
				case Paint_FillStroke:
					pDraw->FillPath();
					pDraw->StrokePath();
				case Paint_None:
				default:
					break;
				}
				break;
			}
		case ContentType_Text:
			{
				pDraw->SetMatrix( CHE_PDF_Matrix() );
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
									pDraw->MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
									i+=2;
									break;
								}
							case PathItem_LineTo:
								{
									pDraw->LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
									i+=2;
									break;
								}
							case PathItem_CurveTo:
								{
									pDraw->CurveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value,
										pPath->mItems[i+3].value, pPath->mItems[i+4].value,
										pPath->mItems[i+5].value, pPath->mItems[i+6].value );
									i+=6;
									break;
								}
							case PathItem_Rectangle:
								{
									pDraw->MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
									pDraw->LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value );
									pDraw->LineTo( pPath->mItems[i+1].value + pPath->mItems[i+3].value, pPath->mItems[i+2].value + pPath->mItems[i+4].value );
									pDraw->LineTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value + pPath->mItems[4].value );
									pDraw->ClosePath();
									i+=4;
									break;
								}
							case PathItem_Close:
								{
									pDraw->ClosePath();
									i+=1;
									break;
								}
							default:
								break;
							}
						}
						pDraw->ClosePath();
						PDF_GSTATE_TEXTRENDERMODE rm;
						pGState->GetTextRenderMode( rm );
						switch( rm )
						{
						case TextRenderMode_Fill:
							pDraw->FillPath();
							break;
						case TextRenderMode_Stroke:
							pDraw->StrokePath();
							break;
						case TextRenderMode_FillStroke:
							pDraw->FillStrokePath();
							break;
						case TextRenderMode_Invisible:
							break;
						case TextRenderMode_FillClip:
							pDraw->FillPath();
							break;
						case TextRenderMode_StrokeClip:
							pDraw->StrokePath();
							break;
						case TextRenderMode_FillStrokeClip:
							pDraw->StrokePath();
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