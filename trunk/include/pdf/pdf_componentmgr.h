#ifndef _CPDF_COMPONENTMGR_H_
#define _CPDF_COMPONENTMGR_H_

#include "pdf_component.h"
#include <unordered_map>

namespace chelib {

class CPDF_ComponentMgr : public CObject
{
public:
	CPDF_ComponentMgr( CAllocator * pAllocator = nullptr );

	~CPDF_ComponentMgr();

	CPDF_ComponentPtr LoadFont( const CPDF_ReferencePtr & refPtr );

	CPDF_ComponentPtr GetComponent( const CPDF_ReferencePtr & refPtr );

	CPDF_ComponentPtr GetComponent( const CPDF_ReferencePtr & refPtr, PDF_COMPONENT_TYPE type );

	bool PushComponent( const CPDF_ReferencePtr & refPtr, const CPDF_ComponentPtr & component );

private:
	std::unordered_map<size_t,CPDF_ComponentPtr> mComponentsMap;
};

}//namespace

#endif