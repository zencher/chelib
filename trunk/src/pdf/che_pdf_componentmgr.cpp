#include "../../include/pdf/che_pdf_componentmgr.h"
#include "../../include/pdf/che_pdf_font.h"


CHE_PDF_ComponentMgr::CHE_PDF_ComponentMgr( CHE_Allocator * pAllocator/*= nullptr*/ )
	: CHE_Object(pAllocator) {}

CHE_PDF_ComponentMgr::~CHE_PDF_ComponentMgr() {}

CHE_PDF_ComponentPtr CHE_PDF_ComponentMgr::LoadFont( const CHE_PDF_ReferencePtr & refPtr )
{
    CHE_PDF_ComponentPtr cmptPtr = GetComponent(refPtr);
    if (cmptPtr)
    {
        return cmptPtr;
    }
    if (refPtr)
    {
        CHE_PDF_ObjectPtr objPtr = refPtr->GetRefObj(OBJ_TYPE_DICTIONARY);
        if (objPtr) {
            cmptPtr = CHE_PDF_Font::Create(objPtr->GetDictPtr(), GetAllocator());
            if (cmptPtr) {
                PushComponent(refPtr, cmptPtr);
            }
        }
    }
    return cmptPtr;
}

CHE_PDF_ComponentPtr CHE_PDF_ComponentMgr::GetComponent( const CHE_PDF_ReferencePtr & refPtr )
{
	CHE_PDF_ComponentPtr ptr;
	if ( refPtr )
	{
		std::unordered_map<size_t,CHE_PDF_ComponentPtr>::iterator it;
		it = mComponentsMap.find( refPtr->GetRefNum() );
		if ( it != mComponentsMap.end() )
		{
			ptr = it->second;
		}
	}
	return ptr;
}

CHE_PDF_ComponentPtr CHE_PDF_ComponentMgr::GetComponent( const CHE_PDF_ReferencePtr & refPtr, PDF_COMPONENT_TYPE type )
{
	CHE_PDF_ComponentPtr ptr = GetComponent( refPtr );
	if ( ptr && ptr->GetType() == type )
	{
		return ptr;
	}
	return CHE_PDF_ComponentPtr();
}

bool	CHE_PDF_ComponentMgr::PushComponent( const CHE_PDF_ReferencePtr & refPtr, const CHE_PDF_ComponentPtr & component )
{
	if ( refPtr && component )
	{
		mComponentsMap.insert( pair<size_t,CHE_PDF_ComponentPtr>( refPtr->GetRefNum(), component ) );
		return TRUE;
	}
	return false;
}