#include "../../include/pdf/che_pdf_contentobjs.h"

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