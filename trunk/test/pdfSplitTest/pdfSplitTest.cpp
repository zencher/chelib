// pdfSplitTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_define.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_creator.h"
#include "../../include/pdf/che_pdf_objclone.h"

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		printf( "no param!\n" );
		return 0;
	}

	IHE_Read * pFileRead = HE_CreateFileRead( argv[1], FILEREAD_MODE_DEFAULT, 4096 );
	if ( pFileRead == NULL )
	{
		printf( "no file!\n" );
		return 0;
	}else{
		printf( "%s \n",argv[1] );
		printf( "file size : %ld\n", pFileRead->GetSize() );
	}

	CHE_PDF_Parser parser;
	if ( parser.Open( pFileRead ) == FALSE )
	{
		return 0;
	}
	CHE_ByteString password;
	if ( !parser.Authenticate( password ) )
	{
		printf( "encrypt file!\n" );
		return 0;
	}
	
	printf( "page count : %d\n", parser.GetPageCount() );
	if ( parser.GetPageCount() == 0 )
	{
		return 0;
	}

	CHE_PDF_Creator creator;
	if ( creator.NewDocument() == FALSE )
	{
		return 0;
	}

	CHE_ObjectCloneMgr ObjCloneMgr;

	for ( unsigned int i = 0; i < parser.GetPageCount(); i++ )
	{
		CHE_PDF_Dictionary * pFirstPageDict = parser.GetObject( parser.GetPageObjNum( i ) )->ToDict();
		if ( pFirstPageDict == NULL )
		{
			return 0;
		}
		CHE_PDF_Dictionary * pNewPageDict = creator.NewPage( 0, 0 );
		if ( pNewPageDict == NULL )
		{
			return 0;
		}

		creator.SetVersion( parser.GetPDFVersion() );

		CHE_PDF_Object * pTmpObj = NULL;
		CHE_ByteString key;
		for ( HE_DWORD i = 0; i < pFirstPageDict->GetCount(); i++ )
		{
			pTmpObj = pFirstPageDict->GetElementByIndex( i );
			pFirstPageDict->GetKeyByIndex( i, key );
			if ( key == "Type" || key == "Parent"  )
			{
				continue;
			}
			switch( pTmpObj->GetType() )
			{
			case OBJ_TYPE_NULL:
				pNewPageDict->SetAtNull( key );
				break;
			case OBJ_TYPE_BOOLEAN:
				pNewPageDict->SetAtBoolean( key, pTmpObj->ToBoolean()->GetValue() );
				break;
			case OBJ_TYPE_NUMBER:
				if ( pTmpObj->ToNumber()->IsInteger() )
				{
					pNewPageDict->SetAtInteger( key, pTmpObj->ToNumber()->GetInteger() );
				}else{
					pNewPageDict->SetAtFloatNumber( key, pTmpObj->ToNumber()->GetFloat() );
				}
				break;
			case OBJ_TYPE_STRING:
				pNewPageDict->SetAtString( key, pTmpObj->ToString()->GetString() );
				break;
			case OBJ_TYPE_NAME:
				pNewPageDict->SetAtName( key, pTmpObj->ToName()->GetString() );
				break;
			case OBJ_TYPE_ARRAY:
				pNewPageDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), &creator, &ObjCloneMgr ) );
				break;
			case OBJ_TYPE_DICTIONARY:
				pNewPageDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), &creator, &ObjCloneMgr ) );
				break;
			case OBJ_TYPE_REFERENCE:
				{
					HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), &creator, &ObjCloneMgr );
					pNewPageDict->SetAtReference( key, refNum, NULL );
					break;
				}
			default:
				break;
			}
		}
	}

	char tmpStr[1024];

	sprintf( tmpStr, "%s_new.pdf", argv[1] );

	IHE_Write * pWrite = HE_CreateFileWrite( tmpStr );

	creator.Save( pWrite );

}

