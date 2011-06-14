#include "../../include/pdf/che_pdf_renderer.h"
#include "../../include/pdf/che_pdf_parser.h"

HE_VOID	CHE_PDF_Renderer::Render( CHE_PDF_Page * page, IHE_PDF_DrawGraphics * pIHE_draw )
{
	if ( page == NULL || pIHE_draw == NULL )
	{
		return;
	}

	HE_DWORD lCount = page->ParseContent();
	CHE_PDF_ContentObject * pContentObj = NULL;
	for ( HE_DWORD i = 0; i < lCount; i++  )
	{
		pContentObj = page->GetContentObject( i );
		if ( pContentObj->GetType() == CONTENTOBJ_PATH )
		{
			pIHE_draw->DrawPath( (CHE_PDF_PathObject*)pContentObj );
		}else if ( pContentObj->GetType() == CONTENTOBJ_TEXT )
		{
			pIHE_draw->DrawText( (CHE_PDF_TextObject*)pContentObj );
		}
	}
}
