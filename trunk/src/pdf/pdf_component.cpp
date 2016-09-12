#include "../../include/pdf/pdf_component.h"
#include "../../include/pdf/pdf_extgstate.h"
#include "../../include/pdf/pdf_colorspace.h"
#include "../../include/pdf/pdf_pattern.h"
#include "../../include/pdf/pdf_xobject.h"
#include "../../include/pdf/pdf_font.h"

namespace chelib {

void	CPDF_Component::Release()
{
	switch ( mType )
	{
	case COMPONENT_TYPE_ExtGState:
		GetAllocator()->Delete<CPDF_ExtGState>( (CPDF_ExtGState*)this );
		break;
	case COMPONENT_TYPE_ColorSpace:
		GetAllocator()->Delete<CPDF_ColorSpace>( (CPDF_ColorSpace*)this );
		break;
	case COMPONENT_TYPE_Function:
		GetAllocator()->Delete<CPDF_Function>( (CPDF_Function*)this );
		break;
    case COMPONENT_TYPE_Font:
        GetAllocator()->Delete<CPDF_Font>( (CPDF_Font*)this );
        break;
	default:
		GetAllocator()->Delete<CPDF_Component>( this );
		break;
	}
}


CPDF_ComponentPtr::~CPDF_ComponentPtr()
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


CPDF_ComponentPtr::CPDF_ComponentPtr( const CPDF_ComponentPtr & ptr )
	: mpCom( nullptr )
{
	if ( mpCom != ptr.mpCom && ptr.mpCom != nullptr )
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

CPDF_ComponentPtr CPDF_ComponentPtr::operator = ( const CPDF_ComponentPtr & ptr )
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
			mpCom = nullptr;
		}
	}
	mpCom = ptr.mpCom;
	if ( mpCom )
	{
		mpCom->mRefs.AddRef();
	}
	return *this;
}


void	CPDF_ComponentPtr::Reset( CPDF_Component * pCom /*= nullptr*/ )
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

// CPDF_ColorSpacePtr CPDF_Component::GetColorSpacePtr() const
// {
// 	CPDF_ColorSpacePtr ptr;
// 	if ( mType == COMPONENT_TYPE_ColorSpace )
// 	{
// 		ptr.Reset( (CPDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CPDF_ExtStatePtr CPDF_Component::GetExtGStatePtr() const
// {
// 	CPDF_ExtStatePtr ptr;
// 	if ( mType == COMPONENT_TYPE_ExtGState )
// 	{
// 		ptr.Reset( (CPDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CPDF_ShadingPtr CPDF_Component::GetShadingPtr() const
// {
// 	CPDF_ShadingPtr ptr;
// 	if ( mType == COMPONENT_TYPE_Shading )
// 	{
// 		ptr.Reset( (CPDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CPDF_TilingPtr CPDF_Component::GetTilingPtr() const
// {
// 	CPDF_TilingPtr ptr;
// 	if ( mType == COMPONENT_TYPE_Tiling )
// 	{
// 		ptr.Reset( (CPDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CPDF_FormXObjectPtr	CPDF_Component::GetFormPtr() const
// {
// 	CPDF_FormXObjectPtr ptr;
// 	if ( mType == COMPONENT_TYPE_FormXObject )
// 	{
// 		ptr.Reset( (CPDF_Component*)this );
// 	}
// 	return ptr;
// }
// 
// CPDF_ImageXObjectPtr	CPDF_Component::GetRefImagePtr() const
// {
// 	CPDF_ImageXObjectPtr ptr;
// 	if ( mType == COMPONENT_TYPE_ImageXObject )
// 	{
// 		ptr.Reset( (CPDF_Component*)this );
// 	}
// 	return ptr;
// }

}//namespace