#ifndef _CHE_PDF_EXTSTATE_H_
#define _CHE_PDF_EXTSTATE_H_


#include "che_pdf_component.h"


class CHE_PDF_ExtState;

class CHE_PDF_ExtStatePtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_ExtState * operator->() const { return (CHE_PDF_ExtState*)mpCom; }
};

class CHE_PDF_ExtState : protected CHE_PDF_Component
{
public:
	static CHE_PDF_ExtStatePtr Create( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator = NULL );

private:
	CHE_PDF_ExtState( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator = NULL );

	friend class CHE_Allocator;
};

#endif