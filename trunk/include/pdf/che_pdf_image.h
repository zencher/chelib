#ifndef _CHE_PDF_IMAGE_H_
#define _CHE_PDF_IMAGE_H_
#include "../che_define.h"
#include "che_pdf_file.h"

enum PDF_IMAGE_TYPE
{
	IMAGE_JPEG,
	IMAGE_BMP
};

class CHE_PDF_Image
{
public:
	static CHE_PDF_ReferencePtr InsertImageToFile(	CHE_PDF_File * pFile, PDF_IMAGE_TYPE type, size_t bpc,
													size_t width, size_t hight, PBYTE pData, size_t size,
													CHE_PDF_ReferencePtr mask = CHE_PDF_ReferencePtr() );
};

#endif