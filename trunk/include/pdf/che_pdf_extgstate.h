#ifndef _CHELIB_PDF_EXTGSTATE_H_
#define _CHELIB_PDF_EXTGSTATE_H_

#include "che_pdf_component.h"

#include <vector>

namespace chelib {

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

class CPDF_ExtGStateItem
{
public:
	PDF_EXTGSTATE_TYPE	type;
	CPDF_ObjectPtr	objPtr;
};

class CPDF_ExtGState;

class CPDF_ExtGStatePtr : public CPDF_ComponentPtr
{
public:
	CPDF_ExtGState * operator->() const { return (CPDF_ExtGState*)mpCom; }
};

class CPDF_ExtGState : protected CPDF_Component
{
public:
	static CPDF_ExtGStatePtr Create( const CPDF_ObjectPtr & objPtr, CAllocator * pAllocator = NULL );

	static CPDF_ExtGStatePtr Convert( const CPDF_ComponentPtr & componetPtr );

	std::vector<CPDF_ExtGStateItem> mStateItems;

private:
	CPDF_ExtGState( const CPDF_ObjectPtr & objPtr, CAllocator * pAllocator = NULL );

	friend class CAllocator;
};

}//namespace

#endif