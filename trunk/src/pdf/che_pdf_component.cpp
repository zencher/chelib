#include "../../include/pdf/che_pdf_component.h"
#include "../../include/pdf/che_pdf_extstate.h"
#include "../../include/pdf/che_pdf_colorspace.h"
#include "../../include/pdf/che_pdf_pattern.h"
#include "../../include/pdf/che_pdf_xobject.h"


HE_VOID	CHE_PDF_Component::Release()
{
	switch ( mType )
	{
	case COMPONENT_TYPE_ExtGState:
		GetAllocator()->Delete<CHE_PDF_ExtState>( (CHE_PDF_ExtState*)this );
		break;
	case COMPONENT_TYPE_ColorSpace:
		GetAllocator()->Delete<CHE_PDF_ColorSpace>( (CHE_PDF_ColorSpace*)this );
		break;
	default:
		GetAllocator()->Delete<CHE_PDF_Component>( this );
		break;
	}
}


CHE_PDF_ComponentPtr::~CHE_PDF_ComponentPtr()
{
	if ( mpCom )
	{
		mpCom->mRefs.DecRef();
		if ( mpCom->mRefs == 0 )
		{
			mpCom->Release();
		}
	}
}


CHE_PDF_ComponentPtr::CHE_PDF_ComponentPtr( const CHE_PDF_ComponentPtr & ptr )
	: mpCom( NULL )
{
	if ( mpCom != ptr.mpCom && ptr.mpCom != NULL )
	{
		if ( mpCom )
		{
			mpCom->mRefs.DecRef();
			if ( mpCom->mRefs == 0 )
			{
				mpCom->Release();
			}
		}
		mpCom = ptr.mpCom;
		if ( mpCom )
		{
			mpCom->mRefs.AddRef();
		}
	}
}

CHE_PDF_ComponentPtr CHE_PDF_ComponentPtr::operator = ( const CHE_PDF_ComponentPtr & ptr )
{
	if ( mpCom )
	{
		if ( mpCom == ptr.mpCom )
		{
			return *this;
		}
		mpCom->mRefs.DecRef();
		if ( mpCom->mRefs == 0 )
		{
			mpCom->Release();
			mpCom = NULL;
		}
	}
	mpCom = ptr.mpCom;
	if ( mpCom )
	{
		mpCom->mRefs.AddRef();
	}
	return *this;
}


HE_VOID	CHE_PDF_ComponentPtr::Reset( CHE_PDF_Component * pCom /*= NULL*/ )
{
	if ( mpCom != pCom )
	{
		if ( mpCom )
		{
			mpCom->mRefs.DecRef();
			if ( mpCom->mRefs == 0 )
			{
				mpCom->Release();
			}
		}
		mpCom = pCom;
		if ( mpCom )
		{
			mpCom->mRefs.AddRef();
		}
	}
}

// CHE_PDF_ColorSpacePtr CHE_PDF_Component::GetColorSpacePtr() const
// {
// 	CHE_PDF_ColorSpacePtr ptr;
// 	if ( mType == COMPONENT_TYPE_ColorSpace )
// 	{
// 		ptr.Reset( (CHE_PDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_ExtStatePtr CHE_PDF_Component::GetExtGStatePtr() const
// {
// 	CHE_PDF_ExtStatePtr ptr;
// 	if ( mType == COMPONENT_TYPE_ExtGState )
// 	{
// 		ptr.Reset( (CHE_PDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_ShadingPtr CHE_PDF_Component::GetShadingPtr() const
// {
// 	CHE_PDF_ShadingPtr ptr;
// 	if ( mType == COMPONENT_TYPE_Shading )
// 	{
// 		ptr.Reset( (CHE_PDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_TilingPtr CHE_PDF_Component::GetTilingPtr() const
// {
// 	CHE_PDF_TilingPtr ptr;
// 	if ( mType == COMPONENT_TYPE_Tiling )
// 	{
// 		ptr.Reset( (CHE_PDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_FormXObjectPtr	CHE_PDF_Component::GetFormPtr() const
// {
// 	CHE_PDF_FormXObjectPtr ptr;
// 	if ( mType == COMPONENT_TYPE_FormXObject )
// 	{
// 		ptr.Reset( (CHE_PDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_ImageXObjectPtr	CHE_PDF_Component::GetRefImagePtr() const
// {
// 	CHE_PDF_ImageXObjectPtr ptr;
// 	if ( mType == COMPONENT_TYPE_ImageXObject )
// 	{
// 		ptr.Reset( (CHE_PDF_Component*)this );
// 	}
// 	return ptr;
// }