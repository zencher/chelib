#include <cstdio>
#include "../../include/pdf/che_pdf_file.h"
//#include "../../include/pdf/che_pdf_document.h"
#include "../../include/pdf/che_pdf_outline.h"

LARGE_INTEGER gFeq;
LARGE_INTEGER gBegin, gEnd;
HE_ULONG parseTime = 0;

int main( int argc, char **argv )
{
	QueryPerformanceFrequency( &gFeq );

	IHE_Read * pFileRead = HE_CreateFileRead( argv[1], FILEREAD_MODE_MEMCOPY );
	if ( pFileRead == NULL )
	{
		return FALSE;
	}

	QueryPerformanceCounter( &gBegin );

	CHE_PDF_File file;
	file.Open( pFileRead );

// 	PDF_RefInfo refInfo;
// 	CHE_PDF_ObjectPtr objPtr;
// 	unsigned long count = file.GetXRefTable()->GetMaxObjNum();
// 	for ( unsigned long i = 0; i < count; ++i )
// 	{
// 		refInfo.objNum = i;
// 		refInfo.genNum = 0;
// 		objPtr = file.GetObject( refInfo );
// 	}

// 	HE_ULONG pageCount = 0;
// 	CHE_PDF_Document * pDoc = CHE_PDF_Document::CreateDocument( &file );
// 	CHE_PDF_PageTree * tree = pDoc->GetPageTree();
// 
// 	CHE_PDF_Page * page = NULL;
// 	pageCount = tree->GetPageCount();
// 	for ( HE_ULONG i = 0; i < pageCount; ++i )
// 	{
// 		page = tree->GetPage( i );
// 		//printf("%d\n",i);
// 	}

	CHE_PDF_Outline outline( &file );

	outline.Parse( file.GetRootDict()->GetElement( "Outlines", OBJ_TYPE_REFERENCE )->GetRefPtr() );

	file.Close();
	HE_DestoryIHERead( pFileRead );
	pFileRead = NULL;

	QueryPerformanceCounter( &gEnd );

	parseTime = ( (double)( gEnd.QuadPart - gBegin.QuadPart ) ) * 1000 / ( (double)gFeq.QuadPart );

	printf("parse objects for file : %s.\n", argv[1]);
	printf("done in %d ms.\n", parseTime );

	return 0;
}