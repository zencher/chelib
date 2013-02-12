#include "../../include/pdf/che_pdf_renderer.h"

HE_VOID CHE_PDF_Renderer::Render( CHE_PDF_ContentObjectList & content, IHE_PDF_GraphicsDraw * pDraw )
{
	if ( pDraw == NULL )
	{
		return;
	}

	ContentObjectList::iterator it = content.Begin();
	for ( ; it != content.End(); ++it )
	{
		if ( (*it)->GetType() == ContentType_Path )
		{
			CHE_PDF_Path * pPath = (CHE_PDF_Path*)(*it);
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
						//pDraw->MoveTo( pPath->mItems[i+1].value, pPath->mItems[i+2].value );
						//pDraw->LineTo( pPath->mItems[] )
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
			pDraw->FillPath();
		}
	}
}