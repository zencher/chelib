#include "../../include/wdm/che_wdm_textbox.h"

CHE_WDM_TextBox * CHE_WDM_TextBox::Create( size_t width, size_t height, IHE_WDM_InterActive * pInterActive, CHE_Allocator * pAllocator /*= NULL*/ )
{
	if ( pInterActive == NULL )
	{
		return NULL;
	}

	CHE_WDM_TextBox * pTextBox = NULL;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	pTextBox = pAllocator->New<CHE_WDM_TextBox>( width, height, pInterActive, pAllocator );
	return pTextBox;
}