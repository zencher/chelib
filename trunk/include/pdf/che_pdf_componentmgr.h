#ifndef _CHE_PDF_COMPONENTMGR_H_
#define _CHE_PDF_COMPONENTMGR_H_

#include "che_pdf_component.h"
#include <unordered_map>


class CHE_PDF_ComponentMgr : public CHE_Object
{
public:
	CHE_PDF_ComponentMgr( CHE_Allocator * pAllocator = nullptr );

	~CHE_PDF_ComponentMgr();

	CHE_PDF_ComponentPtr LoadFont( const CHE_PDF_ReferencePtr & refPtr );

	CHE_PDF_ComponentPtr GetComponent( const CHE_PDF_ReferencePtr & refPtr );

	CHE_PDF_ComponentPtr GetComponent( const CHE_PDF_ReferencePtr & refPtr, PDF_COMPONENT_TYPE type );

	bool	PushComponent( const CHE_PDF_ReferencePtr & refPtr, const CHE_PDF_ComponentPtr & component );

private:
	//CHE_PDF_FontMgr	mFontMgr;
	std::unordered_map<size_t,CHE_PDF_ComponentPtr> mComponentsMap;
};

#endif