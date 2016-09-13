#include "../../include/che_pdf_componentmgr.h"
#include "../../include/che_pdf_font.h"

namespace chelib {

CPDF_ComponentMgr::CPDF_ComponentMgr( CAllocator * pAllocator/*= nullptr*/ )
	: CObject(pAllocator) {}

CPDF_ComponentMgr::~CPDF_ComponentMgr() {}

CPDF_ComponentPtr CPDF_ComponentMgr::LoadFont( const CPDF_ReferencePtr & refPtr )
{
    CPDF_ComponentPtr cmptPtr = GetComponent(refPtr);
    if (cmptPtr)
    {
        return cmptPtr;
    }
    if (refPtr)
    {
        CPDF_ObjectPtr objPtr = refPtr->GetRefObj(OBJ_TYPE_DICTIONARY);
        if (objPtr) {
            cmptPtr = CPDF_Font::Create(objPtr->GetDictPtr(), GetAllocator());
            if (cmptPtr) {
                PushComponent(refPtr, cmptPtr);
            }
        }
    }
    return cmptPtr;
}

CPDF_ComponentPtr CPDF_ComponentMgr::GetComponent( const CPDF_ReferencePtr & refPtr )
{
	CPDF_ComponentPtr ptr;
	if ( refPtr )
	{
		std::unordered_map<size_t,CPDF_ComponentPtr>::iterator it;
		it = mComponentsMap.find( refPtr->GetRefNum() );
		if ( it != mComponentsMap.end() )
		{
			ptr = it->second;
		}
	}
	return ptr;
}

CPDF_ComponentPtr CPDF_ComponentMgr::GetComponent( const CPDF_ReferencePtr & refPtr, PDF_COMPONENT_TYPE type )
{
	CPDF_ComponentPtr ptr = GetComponent( refPtr );
	if ( ptr && ptr->GetType() == type )
	{
		return ptr;
	}
	return CPDF_ComponentPtr();
}

bool	CPDF_ComponentMgr::PushComponent( const CPDF_ReferencePtr & refPtr, const CPDF_ComponentPtr & component )
{
	if ( refPtr && component )
	{
        mComponentsMap.insert( std::pair<size_t,CPDF_ComponentPtr>( refPtr->GetRefNum(), component ) );
		return TRUE;
	}
	return false;
}

}//namespace