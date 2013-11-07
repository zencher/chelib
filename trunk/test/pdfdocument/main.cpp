#include <cstdio>
#include "../../include/pdf/che_pdf_document.h"

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		printf( "no param!\n" );
		return -1;
	}

	IHE_Read * pFileRead = HE_CreateFileRead( argv[1] );
	//IHE_Read * pFileRead = HE_CreateFileRead( argv[1], FILEREAD_MODE_MEMCOPY );

	if ( pFileRead == NULL )
	{
		printf( "no file!\n" );
		return -2;
	}else{
		printf( "%s \n",argv[1] );
		printf( "file size : %d\n", pFileRead->GetSize() );
	}

	CHE_PDF_File file;

	file.Open( pFileRead );

// 	CHE_ByteString password;
// 	if ( ! file.Authenticate( password ) )
// 	{
// 		password = "<Password=\"Swift\" Date=\"2009.01.01\" Copyright=\"Foxit Software\"/>";
// 
// 		if ( ! file.Authenticate( password ) )
// 		{
// 			printf( "password error!\n" );
// 			file.Close();
// 			return -4;
// 		}
// 	}

	CHE_PDF_DictionaryPtr infoDict = file.GetInfoDict();

	if ( ! infoDict || infoDict->GetCount() == 0 )
	{
		printf( "no info dict!\n" );
		return -3;
	}

	HE_BOOL bRet = FALSE;
	CHE_ByteString keyStr;
	CHE_PDF_ObjectPtr objPtr;	

	for ( unsigned int i = 0; i < infoDict->GetCount(); ++i )
	{
		bRet = infoDict->GetKeyByIndex( i, keyStr );
		objPtr = infoDict->GetElementByIndex( i );

		if ( ! bRet || ! objPtr )
		{
			continue;
		}

		switch ( objPtr->GetType() )
		{
		case OBJ_TYPE_NAME:
			{
				printf( "%s : ", keyStr.GetData() );

				CHE_PDF_NamePtr namePtr = objPtr->GetNamePtr();
				if ( namePtr->GetString().GetLength() > 0 )
				{
					printf( "%s", namePtr->GetString().GetData() );
				}
				printf( "\n" );
				break;
			}
		case OBJ_TYPE_NUMBER:
			{
				printf( "%s : ", keyStr.GetData() );
				CHE_PDF_NumberPtr number = objPtr->GetNumberPtr();
				if ( number->IsInteger() )
				{
					printf( "%d\n", number->GetInteger() );
				}else{
					printf( "%f\n", number->GetFloat() );
				}
				break;
			}
		case OBJ_TYPE_STRING:
			{
				printf( "%s : ", keyStr.GetData() );

				CHE_PDF_StringPtr strPtr = objPtr->GetStringPtr();
				if ( strPtr->GetString().GetLength() > 0 )
				{
					printf( "%s", strPtr->GetString().GetData() );
				}
				printf( "\n" );
				break;
			}
		case OBJ_TYPE_BOOLEAN:
			{
				printf( "%s : ", keyStr.GetData() );
				CHE_PDF_BooleanPtr bBool = objPtr->GetBooleanPtr();
				if ( bBool->GetValue() )
				{
					printf( "true\n" );
				}else{
					printf( "false\n" );
				}
				break;
			}
		default:
			break;
		}
	}

	file.Close();
	HE_DestoryIHERead( pFileRead );

	return 0;
}