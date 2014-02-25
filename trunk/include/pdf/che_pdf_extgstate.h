#ifndef _CHE_PDF_EXTGSTATE_H_
#define _CHE_PDF_EXTGSTATE_H_


#include "che_pdf_component.h"


class CHE_PDF_ExtGState;

class CHE_PDF_ExtGStatePtr : public CHE_PDF_ComponentPtr
{
public:
	CHE_PDF_ExtGState * operator->() const { return (CHE_PDF_ExtGState*)mpCom; }
};

class CHE_PDF_ExtGState : protected CHE_PDF_Component
{
public:
	static CHE_PDF_ExtGStatePtr Create( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator = NULL );

private:
	CHE_PDF_ExtGState( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator = NULL );

	friend class CHE_Allocator;
};

#endif