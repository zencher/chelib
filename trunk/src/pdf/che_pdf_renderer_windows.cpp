#include "../../include/pdf/che_pdf_renderer_windows.h"
#include "../../include/pdf/che_pdf_gstate.h"
#include "../../include/pdf/che_pdf_xobject.h"

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
	static CHE_PDF_ColorSpacePtr fillColorSpace;
	static CHE_PDF_ColorSpacePtr strokeColorSpace;
	static HE_ULONG fillColorVal = 0xFF000000;
	static HE_ULONG strokeColorVal = 0xFF000000;
	static HE_FLOAT fillAlpha = 1.0f;
	static HE_FLOAT strokeAlpha = 1.0f;

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

	fillColorVal = fillColorSpace->GetARGBValue( fillColor );
	strokeColorVal = strokeColorSpace->GetARGBValue( strokeColor );
	drawer.SetFillColor( fillColorVal );
	drawer.SetStrokeColor( strokeColorVal );
	drawer.SetFillMode( FillMode_Nonzero );

	CHE_PDF_ExtGState * extGState =  pGState->GetExtGState();
	if ( extGState )
	{
		fillAlpha = extGState->GetFillAlpha();
		strokeAlpha = extGState->GetStrokeAlpha();
	}else{
		fillAlpha = 1.0f;
		strokeAlpha = 1.0f;
	}
	drawer.SetFillAlpha( fillAlpha );
	drawer.SetStrokeAlpha( strokeAlpha );
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

inline HE_VOID OutputPath( CHE_PDF_Path * pPath, CHE_GraphicsDrawer & drawer )
{
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
}

inline HE_VOID OutputText( CHE_PDF_Text * pText, CHE_GraphicsDrawer & drawer )
{
    GRAPHICS_STATE_TEXTRENDERMODE rm = TextRenderMode_Fill;
    CHE_PDF_GState * pGState = pText->GetGState();
    if ( pGState )
    {
        pGState->GetTextRenderMode( rm );
    }
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
        break;
    case TextRenderMode_StrokeClip:
        drawer.StrokePath();
        break;
    case TextRenderMode_FillStrokeClip:
        drawer.StrokePath();
        break;
    case TextRenderMode_Clip:
        break;
    default:break;
    }
}

inline HE_VOID OutputInlineImage( CHE_PDF_InlineImage * pImage, CHE_GraphicsDrawer & drawer )
{
	CHE_Bitmap * pBitmap = pImage->GetBitmap();
	if ( pBitmap )
	{
		drawer.DrawBitmap( pBitmap );
	}
}

inline HE_VOID OutputRefImage( CHE_Matrix & matrix, const CHE_PDF_ImageXObjectPtr & image, CHE_GraphicsDrawer & drawer )
{
	if ( image->IsInterpolate() == TRUE )
	{
		CHE_Bitmap * pBitmap = image->GetBitmap();
		if ( pBitmap )
		{
			drawer.DrawBitmap( pBitmap );
		}
	}else{
		CHE_Bitmap * pBitmap = image->GetBitmap();
		if ( pBitmap )
		{
			CHE_Rect rect;
			rect.left = 0;
			rect.bottom = 0;
			rect.width = 1;
			rect.height = 1;
			rect = matrix.Transform( rect );

			if ( rect.width > pBitmap->Width() || rect.height > pBitmap->Height() )
			{
				CHE_Bitmap * pNew = pBitmap->StretchTo( rect.width, rect.height, 0, NULL );
				if ( pNew )
				{
					drawer.DrawBitmap( pNew );
					pNew->GetAllocator()->Delete( pNew ); 
				}
			}else{
				drawer.DrawBitmap( pBitmap );
			}
		}
	}
}

inline HE_VOID OutputComponent( CHE_PDF_ComponentRef * pComponentRef, const CHE_Matrix & matrix, CHE_GraphicsDrawer & drawer );

inline HE_VOID OutputForm( const CHE_PDF_FormXObjectPtr & form, const CHE_Matrix & extMatrix, CHE_GraphicsDrawer & drawer )
{
 	CHE_PDF_GState * pGState = NULL;
 	CHE_PDF_ClipState * pClipState = NULL;
 	CHE_PDF_ContentObjectList & content = form->GetList();
 	ContentObjectList::iterator it = content.Begin();

	for ( ; it != content.End(); ++it )
	{
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
		case ContentType_InlineImage:
			{
				OutputInlineImage( (CHE_PDF_InlineImage*)(*it), drawer );
				break;
			}
		case ContentType_Component:
			{
				OutputComponent( (CHE_PDF_ComponentRef*)(*it), extMatrix, drawer );
				break;
			}
		default:
			break;
		}
	}
}

inline HE_VOID OutputShading( const CHE_PDF_ShadingPtr shading, CHE_GraphicsDrawer & drawer )
{

}

inline HE_VOID OutputComponent( CHE_PDF_ComponentRef * pComponentRef, const CHE_Matrix & extMatrix, CHE_GraphicsDrawer & drawer )
{
	if ( pComponentRef == NULL )
	{
		return;
	}

	CHE_PDF_ComponentPtr componentPtr = pComponentRef->GetComponentPtr();

	switch( componentPtr->GetType() )
	{
	case COMPONENT_TYPE_ImageXObject:
		{
			CHE_Matrix matrix;
			CHE_PDF_GState * pGState = pComponentRef->GetGState();
			if ( pGState )
			{
				matrix = pGState->GetMatrix();
			}
			OutputRefImage( matrix, CHE_PDF_ImageXObject::Convert( componentPtr ), drawer );
			break;
		}
	case COMPONENT_TYPE_FormXObject:
		{
			CHE_Matrix tmpExtMatrix = extMatrix;
			CHE_Matrix newExtMatrix;
			CHE_PDF_GState * pGState = pComponentRef->GetGState();
			if ( pGState )
			{
				newExtMatrix = pGState->GetMatrix();

			}
			/*newExtMatrix.Concat( pForm->GetExtMatrix() );*/
			newExtMatrix.Concat( tmpExtMatrix );

			drawer.SetExtMatrix( newExtMatrix );

			OutputForm( CHE_PDF_FormXObject::Convert( componentPtr ), extMatrix, drawer );

			drawer.SetExtMatrix( extMatrix );
			break;
		}
	case COMPONENT_TYPE_Shading:
		{
			//OutputShading( pComponentRef->GetComponentPtr()->GetShadingPtr(), drawer );
			break;
		}
	default:
		break;
	}
}

HE_VOID CHE_PDF_Renderer::Render(	CHE_PDF_ContentObjectList & content, CHE_GraphicsDrawer & drawer, CHE_Rect pageRect,
									HE_UINT32 rotate, HE_FLOAT scale, HE_FLOAT dipx, HE_FLOAT dipy, CHE_Rect * pClipRect )
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
		tmpMatrix.f = ( pClipRect->height - + pClipRect->bottom ) * dipy * scale / 72;
	}else{
		tmpMatrix.e = 0;
		tmpMatrix.f = pageRect.height * dipy * scale / 72;
	}
	extMatrix.Concat( tmpMatrix );
	
 	CHE_Matrix rectMatrix;
 	rectMatrix.e = -pageRect.left;
 	rectMatrix.f = -pageRect.bottom;

	CHE_Matrix rotateMatrix;
	rotateMatrix = CHE_Matrix::RotateMatrix( rotate );
	rectMatrix.Concat( rotateMatrix ); 

	rectMatrix.Concat( extMatrix );
	extMatrix = rectMatrix;

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
		case ContentType_InlineImage:
			{
				OutputInlineImage( (CHE_PDF_InlineImage*)(*it), drawer );
				break;
			}
		case ContentType_Component:
			{
				OutputComponent( (CHE_PDF_ComponentRef*)(*it), extMatrix, drawer );
				break;
			}
		default:
			break;
		}
	}
}