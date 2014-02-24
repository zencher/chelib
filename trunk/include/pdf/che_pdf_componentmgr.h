#ifndef _CHE_PDF_COMPONENTMGR_H_
#define _CHE_PDF_COMPONENTMGR_H_

#include "che_pdf_component.h"
#include "che_pdf_fontmgr.h"


class CHE_PDF_ComponentMgr : public CHE_Object
{
public:
	CHE_PDF_ComponentMgr( CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_ComponentMgr();

	CHE_PDF_Font * LoadFont( const CHE_PDF_ReferencePtr & refPtr );

	CHE_PDF_ComponentPtr GetComponent( const CHE_PDF_ReferencePtr & refPtr );

	CHE_PDF_ComponentPtr GetComponent( const CHE_PDF_ReferencePtr & refPtr, PDF_COMPONENT_TYPE type );

	HE_BOOL	PushComponent( const CHE_PDF_ReferencePtr & refPtr, const CHE_PDF_ComponentPtr & component );

private:
	CHE_PDF_FontMgr	mFontMgr;
	CHE_NumToPtrMap mComponentsMap;
};

#endif