#include <cstdio>

#include "../../include/pdf/che_pdf_file.h"
#include "../../include/pdf/che_pdf_document.h"
#include "../../include/pdf/che_pdf_contents.h"
#include "../../include/pdf/che_pdf_contentbuilder.h"
#include "../../include/pdf/che_pdf_image.h"



BOOL AddFileMark( char * pFilePath )
{
	if ( pFilePath == NULL )
	{
		return FALSE;
	}

	IHE_Read * pFileRead = HE_CreateFileRead( pFilePath );
	if ( pFileRead == NULL )
	{
		printf( "no file!\n" );
		return FALSE;
	}else{
		printf( "%s \n", pFilePath );
		printf( "file size : %d\n", pFileRead->GetSize() );
	}

	CHE_PDF_File file;
	file.Open( pFileRead );

	printf( "pdf version : %d\n", file.GetPDFVersion() );

	CHE_PDF_Document * pDocument = CHE_PDF_Document::CreateDocument( &file );
	if ( pDocument == NULL )
	{
		HE_DestoryIHERead( pFileRead );
		return FALSE;
	}

	printf( "page count : %d\n\n", pDocument->GetPageCount() );

	CHE_PDF_PageTree * pPageTree = pDocument->GetPageTree();
	for ( HE_DWORD i = 0; i < pDocument->GetPageCount(); ++i )
	{
		printf( "parse page : %d\n", i );

		CHE_PDF_Page * pPage = pPageTree->GetPage( i );
		if ( pPage )
		{
			CHE_PDF_ObjectPtr pTmpObj = pPage->GetPageDict()->GetElement( "Contents", OBJ_TYPE_STREAM );
			if ( pTmpObj )
			{
				CHE_PDF_StreamPtr stmPtr = pTmpObj->GetStreamPtr();
				CHE_PDF_StreamAcc stmAcc;
				stmAcc.Attach( stmPtr );

				char * pChar = "/OursPdfFiles MP\n";

				CHE_DynBuffer buf;
				buf.Write( (HE_LPCBYTE)pChar, 17 );
				buf.Write( (HE_LPCBYTE)stmAcc.GetData(), stmAcc.GetSize() );

				stmPtr->SetRawData( buf.GetData(), buf.GetSize(), STREAM_FILTER_FLATE );
			}
			pPageTree->ReleasePage( pPage );
			pPage = NULL;
		}
	}

	IHE_Write * pWrite = HE_CreateFileWrite( "d:\\asdfasdf.pdf" );
	file.Save( pWrite );
	pDocument->GetAllocator()->Delete( pDocument );
	HE_DestoryIHERead( pFileRead );
	pFileRead = NULL;

	return TRUE;
}


BOOL CheckFileMark( char * pFilePath )
{
	if ( pFilePath == NULL )
	{
		return FALSE;
	}

	IHE_Read * pFileRead = HE_CreateFileRead( pFilePath );
	if ( pFileRead == NULL )
	{
		return FALSE;
	}

	CHE_PDF_File pdfFile;
	if ( pdfFile.Open( pFileRead ) == FALSE )
	{
		HE_DestoryIHERead( pFileRead );
		return FALSE;
	}

	CHE_PDF_Document * pDocument = CHE_PDF_Document::CreateDocument( &pdfFile );
	if ( pDocument == NULL )
	{
		HE_DestoryIHERead( pFileRead );
		return FALSE;
	}

	BOOL bFound = FALSE;
	CHE_PDF_PageTree * pPageTree = pDocument->GetPageTree();
	for ( HE_DWORD i = 0; i < pDocument->GetPageCount(); ++i )
	{
		CHE_PDF_Page * pPage = pPageTree->GetPage( i );
		CHE_PDF_ContentObjectList contentObjList;
		GetPageContent( pPage->GetPageDict(), &contentObjList, pDocument->GetFontMgr() );
		ContentObjectList::iterator it = contentObjList.Begin();
		for ( ; it != contentObjList.End(); ++it )
		{
			if ( (*it)->GetType() == ContentType_Mark )
			{
				CHE_PDF_Mark * pMark = (CHE_PDF_Mark*)( *it );
				if ( pMark->GetTag() == "OursPdfFiles" )
				{
					bFound = TRUE;
					break;
				}
				
			}
		}
		pPageTree->ReleasePage( pPage );
		pPage = NULL;
		if ( bFound == TRUE )
		{
			break;
		}
	}

	pDocument->GetAllocator()->Delete( pDocument );

	pdfFile.Close();

	HE_DestoryIHERead( pFileRead );

	return bFound;
}

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		printf( "no param!\n" );
		return 0;
	}

	//if ( /*AddFileMark( argv[1] )*/CheckFileMark( "e:\\test002.pdf" ) )
	//{
	//	printf( "file add mark successed!\n" );
	//}



	return 0;
}