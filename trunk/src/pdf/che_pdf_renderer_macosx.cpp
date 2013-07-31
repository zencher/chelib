#include "../../include/pdf/che_pdf_renderer_macosx.h"
#include "../../include/che_bitmap.h"


inline HE_VOID OutputCommonGSatae( CHE_GraphicsDrawer & drawer, CHE_PDF_GState * pGState )
{
	static HE_FLOAT val = 0;
	static CHE_Matrix matrix;
	static GRAPHICS_STATE_LINECAP linCap = LineCap_Butt;
	static GRAPHICS_STATE_LINEJOIN lineJoin = LineJoin_Miter;
	static GRAPHICS_STATE_DASHPATTERN lineDash;
	static CHE_PDF_Color fillColor;
	static CHE_PDF_Color strokeColor;
	static CHE_PDF_ColorSpace fillColorSpace( COLORSPACE_DEVICE_GRAY );
	static CHE_PDF_ColorSpace strokeColorSpace( COLORSPACE_DEVICE_GRAY );
	static HE_UINT32 fillColorVal = 0xFF000000;
	static HE_UINT32 strokeColorVal = 0xFF000000;

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
	
	drawer.ResetClip();
}

inline HE_VOID OutputClipState( CHE_GraphicsDrawer & drawer, CHE_PDF_ClipState * pClipState )
{
	CHE_PDF_ContentObject * pObj = NULL;
	std::list<CHE_PDF_ClipStateItem*>::iterator it = pClipState->mClipElementList.begin();
	for ( ; it != pClipState->mClipElementList.end(); it++ )
	{
		pObj = (*it)->GetElement();
		if ( !pObj )
		{
			continue;
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

HE_BOOL OutputPath( CHE_PDF_Path * pPath, CHE_GraphicsDrawer & drawer )
{
    if ( pPath == NULL )
    {
        return FALSE;
    }
    
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
    
    return TRUE;
}

HE_BOOL OutputText( CHE_PDF_Text * pText, CHE_GraphicsDrawer & drawer )
{
    if ( pText == NULL )
    {
        return FALSE;
    }
    
    drawer.SetMatrix( CHE_Matrix() );
    
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
   
            GRAPHICS_STATE_TEXTRENDERMODE rm = TextRenderMode_Fill;
            CHE_PDF_GState * pGState = pText->GetGState();
            if ( pGState )
            {
                pGState->GetTextRenderMode( rm );
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
                    drawer.FillClipPath();
                    break;
                case TextRenderMode_StrokeClip:
                    drawer.StrokeClipPath();
                    break;
                case TextRenderMode_FillStrokeClip:
                    drawer.FillStrokeClipPath();
                    break;
                case TextRenderMode_Clip:
                    drawer.ClipPath();
                    break;
            }
            pPath->GetAllocator()->Delete( pPath );
        }
    }
    
    return TRUE;
}


HE_BOOL OutputRefImage( CHE_PDF_RefImage * pImage, CHE_GraphicsDrawer & drawer )
{
    if ( pImage == NULL )
    {
        return FALSE;
    }
    
    CHE_Bitmap * pBitmap = pImage->GetBitmap();
    if ( pBitmap )
    {
        drawer.DrawBitmap( pBitmap );
    }
    
    return TRUE;
}

HE_BOOL OutputInlineImage( CHE_PDF_InlineImage * pImage, CHE_GraphicsDrawer & drawer )
{
    if ( pImage == NULL )
    {
        return FALSE;
    }
    
    CHE_Bitmap * pBitmap = pImage->GetBitmap();
    if ( pBitmap )
    {
        drawer.DrawBitmap( pBitmap );
    }
    
    return TRUE;
}

HE_BOOL OutputShading( CHE_PDF_Shading * pShading, CHE_GraphicsDrawer & drawer )
{
    if ( pShading == NULL )
    {
        return FALSE;
    }
    
    return TRUE;
}

HE_BOOL OutputForm( CHE_PDF_Form * pForm, CHE_GraphicsDrawer & drawer )
{
    if ( pForm == NULL )
    {
        return FALSE;
    }
    
    CHE_PDF_GState * pGState = NULL;
	CHE_PDF_ClipState * pClipState = NULL;
    CHE_PDF_ContentObjectList & content = pForm->GetList();
	ContentObjectList::iterator it = content.Begin();
	for ( ; it != content.End(); ++it )
	{
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
            case ContentType_Path:
			{
				OutputPath( (CHE_PDF_Path*)(*it), drawer );
				break;
			}
            case ContentType_Text:
			{
				OutputText( (CHE_PDF_Text*)(*it), drawer );
				break;
			}
            case ContentType_RefImage:
			{
				OutputRefImage( (CHE_PDF_RefImage*)(*it), drawer );
				break;
			}
            case ContentType_InlineImage:
			{
				OutputInlineImage( (CHE_PDF_InlineImage*)(*it), drawer );
				break;
			}
            case ContentType_Shading:
            {
                OutputShading( (CHE_PDF_Shading*)(*it), drawer );
                break;
            }
            case ContentType_Form:
            {
                OutputForm( (CHE_PDF_Form*)(*it) , drawer );
                break;
            }
            default:
                break;
		}
	}
    return TRUE;
}

HE_VOID CHE_PDF_Renderer::Render(	CHE_PDF_ContentObjectList & content, CHE_GraphicsDrawer & drawer, CHE_Rect pageRect,
									HE_FLOAT scale, HE_FLOAT dipx, HE_FLOAT dipy, CHE_Rect * pClipRect )
{
	if ( pClipRect != NULL )
	{
		drawer.Resize( pClipRect->width * scale * dipx / 72, pClipRect->height * scale * dipy / 72 );
	}else{
		drawer.Resize( pageRect.width * scale * dipx / 72, pageRect.height * scale * dipy / 72 );
	}
    
    drawer.Clear();
    
	CHE_Matrix tmpMatrix;
	tmpMatrix.a = dipx * scale / 72;
	tmpMatrix.b = 0;
	tmpMatrix.c = 0;
	tmpMatrix.d = dipy * scale / 72;
	tmpMatrix.e = 0;
	tmpMatrix.f = 0;
	CHE_Matrix extMatrix;
	tmpMatrix.Concat( extMatrix );
	drawer.SetExtMatrix( tmpMatrix );

	CHE_PDF_GState * pGState = NULL;
	CHE_PDF_ClipState * pClipState = NULL;
	ContentObjectList::iterator it = content.Begin();
	for ( ; it != content.End(); ++it )
	{
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
		case ContentType_Path:
			{
				OutputPath( (CHE_PDF_Path*)(*it), drawer );
				break;
			}
		case ContentType_Text:
			{
				OutputText( (CHE_PDF_Text*)(*it), drawer );
				break;
			}
        case ContentType_RefImage:
			{
				OutputRefImage( (CHE_PDF_RefImage*)(*it), drawer );
				break;
			}
        case ContentType_InlineImage:
			{
				OutputInlineImage( (CHE_PDF_InlineImage*)(*it), drawer );
				break;
			}
        case ContentType_Shading:
            {
                OutputShading( (CHE_PDF_Shading*)(*it), drawer );
                break;
            }
        case ContentType_Form:
            {
                //OutputForm( (CHE_PDF_Form*)(*it) , drawer );
                break;
            }
		default:
			break;
		}
	}
}