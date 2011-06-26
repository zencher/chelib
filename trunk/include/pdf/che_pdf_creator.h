#ifndef _CHE_PDF_CREATOR_H_
#define _CHE_PDF_CREATOR_H_

#include "../che_base.h"
#include "che_pdf_collector.h"
#include "che_pdf_objects.h"

class CHE_PDF_Creator : public CHE_Object
{
public:
	CHE_PDF_Creator( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Creator();

 	HE_VOID	NewDocument();

	HE_BOOL SetDocumentParam();

	HE_VOID NewPage();

	HE_BOOL SetPageParam();

	HE_BOOL SetPageContent();

	HE_VOID	Save( IHE_Write * pWrite );

private:

	HE_VOID OutPutObject( CHE_PDF_Object * pObj, IHE_Write * pWrite );

private:
	HE_BOOL	m_bNewDocument;	//是否开始了一个PDF文件创建的标志

	CHE_PDF_Dictionary *	m_pCatalogDict;
	CHE_PDF_Dictionary *	m_pPagesDict;

	HE_DWORD m_dwObjNumIndex;

	CHE_PDF_Collector m_collector;
};

#endif