#include "../../include/pdf/che_pdf_extgstate.h"


CHE_PDF_ExtGStatePtr CHE_PDF_ExtGState::Create( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_ExtGStatePtr ptr;
	if ( objPtr )
	{
		if ( pAllocator == NULL )
		{
			pAllocator = GetDefaultAllocator();
		}
		ptr.Reset( pAllocator->New<CHE_PDF_ExtGState>( objPtr, pAllocator ) );
	}
	return ptr;
}


CHE_PDF_ExtGState::CHE_PDF_ExtGState( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator /*= NULL*/ )
	:CHE_PDF_Component( COMPONENT_TYPE_ExtGState, objPtr, pAllocator )/*, mRootObjPtr( objPtr )*/
{
	//a lot of work todo!

	//LW
	//LJ
	//LC
	//ML
	//D
	//RI
	//OP
	//op
	//OPM
	//Font
	//BG
	//BG2
	//UCR
	//UCR2
	//TR
	//TR2
	//HT
	//FL
	//SM
	//SA
	//BM
	//SMask
	//CA
	//ca
	//AIS
	//TK
}