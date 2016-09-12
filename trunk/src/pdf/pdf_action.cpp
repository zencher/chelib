#include "../../include/pdf/pdf_action.h"

namespace chelib {

CPDF_ActionPtr CPDF_Action::Create(const CPDF_DictionaryPtr & dict, CPDF_NameDict * pNameDict /*= nullptr*/, CAllocator * pAllocator /*= nullptr*/)
{
	CPDF_ActionPtr ptr;
	if (pAllocator == nullptr)
	{
		pAllocator = GetDefaultAllocator();
	}
	if (dict)
	{
		CPDF_ObjectPtr objPtr = dict->GetElement("S", OBJ_TYPE_NAME);
		if (objPtr)
		{
			if (objPtr->GetNamePtr()->GetString() == "GoTo")
			{
				ptr.Reset(pAllocator->New<CPDF_GoToAction>(dict, pNameDict, pAllocator));
			}
		}
	}
	return ptr;
}


CPDF_Action::CPDF_Action(PDF_ACTION_TYPE type, const CPDF_ObjectPtr & obj, CAllocator * pAllocator/* = nullptr*/)
: CPDF_Component(COMPONENT_TYPE_Action, obj, pAllocator), mActionType(type) {}

CPDF_GoToAction::CPDF_GoToAction(const CPDF_DictionaryPtr & dict, CPDF_NameDict * pNameDict /*= nullptr*/, CAllocator * pAllocator /*= nullptr*/)
: CPDF_Action( ACTION_GOTO, dict, pAllocator )
{
	CPDF_ObjectPtr objPtr = dict->GetElement("D");
	if (objPtr)
	{
		if ( pNameDict && objPtr->GetType() == OBJ_TYPE_NAME )
		{
			objPtr = pNameDict->GetDest(objPtr->GetNamePtr()->GetString());	
		}
		if ( objPtr->GetType() == OBJ_TYPE_ARRAY )
		{
			mDestPtr = CPDF_Destination::Create(objPtr->GetArrayPtr(), GetAllocator());
		}
	}
}

}//namespace