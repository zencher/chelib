#ifndef _CHE_PDF_EXTGSTATE_H_
#define _CHE_PDF_EXTGSTATE_H_

#include <vector>

#include "che_pdf_component.h"


enum PDF_EXTGSTATE_TYPE
{
	PDF_EXTGSTATE_LW,
	PDF_EXTGSTATE_LJ,
	PDF_EXTGSTATE_LC,
	PDF_EXTGSTATE_ML,
	PDF_EXTGSTATE_D,
	PDF_EXTGSTATE_RI,
	PDF_EXTGSTATE_OP,
	PDF_EXTGSTATE_op,
	PDF_EXTGSTATE_OPM,
	PDF_EXTGSTATE_Font,
	PDF_EXTGSTATE_BG,
	PDF_EXTGSTATE_BG2,
	PDF_EXTGSTATE_UCR,
	PDF_EXTGSTATE_UCR2,
	PDF_EXTGSTATE_TR,
	PDF_EXTGSTATE_TR2,
	PDF_EXTGSTATE_HT,
	PDF_EXTGSTATE_FL,
	PDF_EXTGSTATE_SM,
	PDF_EXTGSTATE_SA,
	PDF_EXTGSTATE_BM,
	PDF_EXTGSTATE_SMask,
	PDF_EXTGSTATE_CA,
	PDF_EXTGSTATE_ca,
	PDF_EXTGSTATE_AIS,
	PDF_EXTGSTATE_TK
};

class CHE_PDF_ExtGStateItem
{
public:
	PDF_EXTGSTATE_TYPE	type;
	CHE_PDF_ObjectPtr	objPtr;
};

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

	static CHE_PDF_ExtGStatePtr Convert( const CHE_PDF_ComponentPtr & componetPtr );

	std::vector<CHE_PDF_ExtGStateItem> mStateItems;

private:
	CHE_PDF_ExtGState( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator = NULL );

	friend class CHE_Allocator;
};

#endif