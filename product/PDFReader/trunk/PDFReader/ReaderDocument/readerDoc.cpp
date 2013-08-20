#include "readerDoc.h"

CReaderDocument::CReaderDocument( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object(pAllocator), mpRead(NULL), mpFile(NULL), mpDocument(NULL),
	mpThrumbDrawer(NULL), mpPageDrawer(NULL), mpHungDrawer(NULL) {}

CReaderDocument::~CReaderDocument()
{
}

HE_BOOL	CReaderDocument::OpenFile( char * fileName )
{
	if ( fileName == NULL )
	{
		return FALSE;
	}

	mpRead = HE_CreateFileRead( fileName, FILEREAD_MODE_DEFAULT, 0, GetAllocator() );
	if ( !mpRead )
	{
		return FALSE;
	}

	mpFile = GetAllocator()->New<CHE_PDF_File>( GetAllocator() );
	if ( !mpFile )
	{
		return FALSE;
	}

	if ( ! mpFile->Open( mpRead ) )
	{
		HE_DestoryIHERead( mpRead );
		mpRead = NULL;
		GetAllocator()->Delete( mpFile );
		mpFile = NULL;
		return FALSE;
	}

	mpDocument = CHE_PDF_Document::CreateDocument( mpFile );
	if ( ! mpDocument  )
	{
		return FALSE;
	}
	
	mpPageTree = mpDocument->GetPageTree();

	return TRUE;
}