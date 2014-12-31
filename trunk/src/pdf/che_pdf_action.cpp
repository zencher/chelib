#include "../../include/pdf/che_pdf_action.h"


CHE_PDF_ActionPtr CHE_PDF_Action::Create(const CHE_PDF_DictionaryPtr & dict, CHE_PDF_NameDict * pNameDict /*= NULL*/, CHE_Allocator * pAllocator /*= NULL*/)
{
	CHE_PDF_ActionPtr ptr;
	if (pAllocator == NULL)
	{
		pAllocator = GetDefaultAllocator();
	}
	if (dict)
	{
		CHE_PDF_ObjectPtr objPtr = dict->GetElement("S", OBJ_TYPE_NAME);
		if (objPtr)
		{
			if (objPtr->GetNamePtr()->GetString() == "GoTo")
			{
				ptr.Reset(pAllocator->New<CHE_PDF_GoToAction>(dict, pNameDict, pAllocator));
			}
		}
	}
	return ptr;
}


CHE_PDF_Action::CHE_PDF_Action(PDF_ACTION_TYPE type, const CHE_PDF_ObjectPtr & obj, CHE_Allocator * pAllocator/* = NULL*/)
: CHE_PDF_Component(COMPONENT_TYPE_Action, obj, pAllocator), mActionType(type) {}

CHE_PDF_GoToAction::CHE_PDF_GoToAction(const CHE_PDF_DictionaryPtr & dict, CHE_PDF_NameDict * pNameDict /*= NULL*/, CHE_Allocator * pAllocator /*= NULL*/)
: CHE_PDF_Action( ACTION_GOTO, dict, pAllocator )
{
	CHE_PDF_ObjectPtr objPtr = dict->GetElement("D");
	if (objPtr)
	{
		if ( pNameDict && objPtr->GetType() == OBJ_TYPE_NAME )
		{
			objPtr = pNameDict->GetDest(objPtr->GetNamePtr()->GetString());	
		}
		if ( objPtr->GetType() == OBJ_TYPE_ARRAY )
		{
			mDestPtr = CHE_PDF_Destination::Create(objPtr->GetArrayPtr(), GetAllocator());
		}
	}
}