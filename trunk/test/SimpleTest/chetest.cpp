#include <cstdio>
#include <memory.h>

#include "../../include/che_bitmap.h"
#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/pdf/che_pdf_pages.h"
#include "../../include/pdf/che_pdf_encrypt.h"


int main()
{
	CHE_HeapAllocator gDefAlloc;
	
	//CHE_ByteString * pStr = NewArray<CHE_ByteString>( 2, gDefAlloc );

	CHE_ByteString * pStr = gDefAlloc.New<CHE_ByteString>( "adsfadsf" );

	gDefAlloc.Delete( pStr );

	//*pStr = "sdfasd";

	//delete pStr;
	//DeleteArray( pStr, gDefAlloc );

	//IHE_Read * pIHE_Read = HE_CreateFileRead( "D:\\PDFFiles\\AcroJS_7.0.5.pdf" ); //"C:\\EncryptTest.pdf"
	//CHE_PDF_Parser parser;
	//parser.StartParse( pIHE_Read );
	//parser.ParseXRef();
	//parser.Authenticate( CHE_ByteString("") );

	//CHE_ByteString id1, id2;
	//CHE_PDF_Dictionary * pRootDict = parser.GetTrailerDict();
	//CHE_PDF_Array * pIDArray = (CHE_PDF_Array*)pRootDict->GetElement( CHE_ByteString("ID") );
	//CHE_PDF_String * pID1 = (CHE_PDF_String*)pIDArray->GetElement( 0 );
	//id1 = pID1->GetString();

	//CHE_PDF_Object * pEncryptRef = pRootDict->GetElement( CHE_ByteString("Encrypt") );
	//CHE_PDF_IndirectObject * pEncryptInObj = parser.GetIndirectObject( ((CHE_PDF_Reference*)pEncryptRef)->GetRefNuml() );
//	CHE_PDF_Dictionary * pEncryptDict = pEncryptInObj->GetDict();
	
	//HE_BYTE OValue[32];
	//HE_BYTE UValue[32];

	//CHE_PDF_Object * pElement = pEncryptDict->GetElement( CHE_ByteString("O") );
	//CHE_ByteString stro = ((CHE_PDF_String*)pElement)->GetString();
	//pElement = pEncryptDict->GetElement( CHE_ByteString("U") );
	//CHE_ByteString stru = ((CHE_PDF_String*)pElement)->GetString();
	//for( HE_DWORD i = 0 ; i < stro.GetLength(); i++ )
	//{
	//	OValue[i] = stro[i];
//	}
	//for( HE_DWORD j = 0 ; j < stru.GetLength(); j++ )
	//{
	//	UValue[j] = stru[j];
	//}

	//CHE_PDF_Encrypt encrypt1( id1, OValue, UValue, 2, 128, 3, TRUE, 0 );
	//HE_BOOL bRightPassword = encrypt1.Authenticate( CHE_ByteString("456") );

// 	CHE_PDF_IndirectObject * pInObj = parser.GetIndirectObject( 102293 );
//  	CHE_PDF_Stream * pStream = pInObj->GetStream();
//  	CHE_PDF_StreamAcc stmAcc;
//  	stmAcc.Attach( pStream );
//  	stmAcc.GetData();
// 	IHE_Write * pWrite = HE_CreateFileWrite( "c:\\102293.txt" );
// 	pWrite->WriteBlock( (HE_LPVOID)stmAcc.GetData(), stmAcc.GetSize() );
// 	pWrite->Flush();
// 	HE_DestoryIHEWrite( pWrite );
// 	pWrite = NULL;

// 	CHE_PDF_Dictionary*  pDict = (CHE_PDF_Dictionary*)pInObj->GetObject();
// 	CHE_PDF_String * pObj = (CHE_PDF_String*)pDict->GetElement( CHE_ByteString("Producer") );
// 	CHE_ByteString str = pObj->GetString();

	//encrypt1.Decrypt( str, 13, 0 );

//	pInObj = parser.GetIndirectObject( 10 );
//	CHE_PDF_Stream * pStm = pInObj->GetStream();
//	HE_BYTE * pRawData = new HE_BYTE[pStm->GetRawSize()];
//	pStm->ReadRawData( 0, pRawData, pStm->GetRawSize() );
//
//	HE_DWORD lengthRet = encrypt1.Decrypt( pRawData, pStm->GetRawSize(), 10, 0 );
//
//	CHE_PDF_FlateFilter flateFliter;
//	CHE_DynBuffer buf;
//	flateFliter.Decode( pRawData, lengthRet, buf );

	
		/*IHE_Read * pIHE_Read = HE_CreateFileRead( "D:\\1.pdf" );
	CHE_PDF_Document doc;
	doc.Load( pIHE_Read );

	CHE_ByteString id1, id2;
	doc.GetID( id1, id2 );
	
	CHE_PDF_Object * pObj = doc.GetParser()->GetTrailerDict()->GetElement( CHE_ByteString("Encrypt") );
	if ( pObj == NULL || pObj->GetType() != PDFOBJ_REFERENCE )
	{
		return 0;
	}
	CHE_PDF_IndirectObject * pInObj = doc.GetParser()->GetIndirectObject( ((CHE_PDF_Reference*)pObj)->GetRefNuml() );
	pObj = pInObj->GetDict();
	if ( pObj->GetType() != PDFOBJ_DICTIONARY )
	{
		return 0;
	}
	pObj = ((CHE_PDF_Dictionary*)pObj)->GetElement( CHE_ByteString("O") );
	CHE_ByteString o = ((CHE_PDF_String*)pObj)->GetString();

	
	CHE_PDF_Encrypt encrypt;
	bool ok;

	for ( int i = 0; i < o.GetLength(); i++ )
	{
		encrypt.m_oValue[i] = o[i];
	}


	encrypt.m_documentId = id1;
	//encrypt.m_oValue = "Mær™æU?ëùq÷µÁðqÑûR¤?.";
	encrypt.m_pValue = -1340;
	encrypt.m_keyLength = 128 / 8;
	encrypt.m_rValue = 4;
    // Pad password
    unsigned char userKey[32];
    unsigned char pswd[32];
    encrypt.PadPassword( CHE_ByteString(""), pswd );
	
    // Check password: 1) as user password, 2) as owner password
    encrypt.ComputeEncryptionKey( encrypt.m_documentId, pswd, encrypt.m_oValue, encrypt.m_pValue, encrypt.m_keyLength * 8, encrypt.m_rValue, userKey);
	
    ok = encrypt.CheckKey(userKey, encrypt.m_uValue);
    if (!ok)
    {
        unsigned char userpswd[32];
        encrypt.ComputeOwnerKey( encrypt.m_oValue, pswd, encrypt.m_keyLength, encrypt.m_rValue, true, userpswd );
        encrypt.ComputeEncryptionKey( encrypt.m_documentId, userpswd, encrypt.m_oValue, encrypt.m_pValue, encrypt.m_keyLength * 8, encrypt.m_rValue, userKey );
        ok = encrypt.CheckKey( userKey, encrypt.m_oValue );
		
        if( ok ) 
            encrypt.m_ownerPass = "";
    }
    else
        encrypt.m_userPass = "";
*/
	//CHE_DynBuffer dynBuffer;

	//CHE_PDF_RLEFileter filter;
	//CHE_PDF_FlateFilter filter;
	//CHE_PDF_ASCII85Filter filter;
	//CHE_PDF_LZWFilter lzwfilter;
	//CHE_PDF_HexFilter filter;
	//HE_BYTE	tmpByte[18] = { 120, 156, 50, 52, 50, 54, 49, 77, 76, 74, 78, 73, 5, 0, 0, 0, 255, 255    /*'t', 'O', 'A', '2', '#', '~', '>'*//*'0', 'e', 't', 'O', 'A', '2', '.', '9', 's', '(', 'A', '7', 'Y', '~', '>'*/ };
	//HE_BYTE	tmpByte[10] = { '1', '1', '3', '4', '5' , 'a', 'a', 'c', 'c', 'e' };
	//HE_BYTE buffer[30];
	//HE_BYTE buffer2[20];

	/*IHE_FileRead * pFileRead = HE_CreateFileRead( "c:\\1.txt" );
	HE_LPBYTE pTmp = new HE_BYTE[pFileRead->GetSize()];
	pFileRead->ReadBlock( pTmp, 0, pFileRead->GetSize() );
	filter.Decode( pTmp, pFileRead->GetSize(), dynBuffer );

	CHE_DynBuffer dynBuffer2;

	HE_LPBYTE pTmp2 = new HE_BYTE[dynBuffer.GetByteCount()];
	dynBuffer.Read( pTmp2, dynBuffer.GetByteCount() );
	lzwfilter.Decode( pTmp2, dynBuffer.GetByteCount(), dynBuffer2 );
	HE_LPBYTE pTmp3 = new HE_BYTE[dynBuffer2.GetByteCount()];
	dynBuffer2.Read( pTmp3, dynBuffer2.GetByteCount() );

	FILE * pFile = fopen( "c:\\1d.txt", "wb+" );
	fwrite( pTmp3, 1, dynBuffer2.GetByteCount(), pFile );
	fclose( pFile );*/

	



	//filter.Encode( tmpByte, 10, dynBuffer );

	//HE_BYTE * pTmp = new HE_BYTE[dynBuffer.GetByteCount()];
	//dynBuffer.Read( pTmp, dynBuffer.GetByteCount() );

	//dynBuffer.Clear();
	//filter.Decode( pTmp, 13, dynBuffer );

	//IHE_FileRead * file = HE_CreateFileRead( "c:\\das.txt" );

// 	FILE * pFile = fopen( "c:\\d.pdf", "rb" );
// 	if ( pFile )
// 	{
// 		CHE_DynBuffer bf( 7 * 1024 * 1024, 1024 * 1024 ) ;
// 		fseek( pFile, 0x12e, SEEK_SET );
// 		HE_BYTE * byte= new HE_BYTE[173405];
// 		fread( byte, 1, 173405, pFile );
// 		filter.Decode( byte, 173405, bf );
// 		HE_BYTE * p = new HE_BYTE[bf.GetByteCount()];
// 		bf.Read( p, bf.GetByteCount() );
	//	FILE * pF = fopen( "C:\\adsf.txt", "wb" );
	//	fwrite( p, 1, bf.GetByteCount(), pF );
	//	fclose( pF );
		

// 		CHE_Bitmap bitmap;
// 		bitmap.Create( 1920, 1280, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_DOWN, bf.GetByteCount(), p );
// 		bitmap.ExchangeChannel( BITMAP_CHANNEL_Blue, BITMAP_CHANNEL_Red );
// 		bitmap.Save( "c:\\sd.bmp" );
// 
// 		return 0;
// 
// 	}

	/*if ( file )
	{
		HE_BYTE * pBtye = new HE_BYTE[ file->GetSize() ];
		file->ReadBlock( pBtye, 0, file->GetSize() );
		if ( pBtye )
		{
			filter.Encode( pBtye, file->GetSize(), dynBuffer );
			HE_BYTE * pEnByte = new HE_BYTE[dynBuffer.GetByteCount()];
			dynBuffer.Read( pEnByte, dynBuffer.GetByteCount() );
			FILE * pFile = fopen( "c:\\2.txt", "wb" );
			if ( pFile )
			{
				fwrite( pEnByte, 1, dynBuffer.GetByteCount(), pFile );
				fclose( pFile );
				pFile = NULL;
			}
			return 0;
			HE_DWORD size = dynBuffer.GetByteCount();
			CHE_DynBuffer dynBuffer2;
			filter.Decode( pEnByte, size, dynBuffer2 );
			HE_BYTE * pDeByte = new HE_BYTE[dynBuffer2.GetByteCount()];
			dynBuffer2.Read( pDeByte, dynBuffer2.GetByteCount() );

			pFile = fopen( "c:\\3.txt", "wb" );
			if ( pFile )
			{
				fwrite( pDeByte, 1, dynBuffer2.GetByteCount(), pFile );
				fclose( pFile );
				pFile = NULL;
			}
		}
	}*/

	

	//memset( buffer, 0, 20 );
	//memset( buffer2, 0, 20 );
	//filter.Encode( tmpByte, 10, dynBuffer );
	//filter.Decode( tmpByte, 18, dynBuffer );
	//filter.Decode( buffer, 0, buffer2, 20 );




// 	IHE_FileRead * pFileRead = HE_CreateFileRead( "c:\\pdf.pdf" );
// 	if ( pFileRead )
// 	{
// 		CHE_PDF_Parser parser;
// 		parser.StartParse( pFileRead );
// 		HE_DWORD version = parser.GetPDFVersion();
// 		HE_DWORD xrefoffset = parser.GetStartxrefOffset( 512 );
// 		HE_BOOL bRet = parser.GetXRefTable();
// 		//CHE_PDF_IndirectObject * pObj = parser.GetIndirectObject( 1 );
// 		//CHE_PDF_Dictionary* pDict = (CHE_PDF_Dictionary*)(pObj->GetDict()->GetElement( CHE_ByteString("OCProperties") ));
// 		//CHE_PDF_Array* pArr = (CHE_PDF_Array*)(pDict->GetElement( CHE_ByteString("OCGs") ) );
// 		//CHE_PDF_Dictionary* pDict2 = (CHE_PDF_Dictionary*)(pDict->GetElement( CHE_ByteString("D") ) );
// 		//pDict->Release();
// 		//int x = 0;
// 		HE_DWORD pageCount = parser.GetPageCount();
		
// 		parser.CloseParser();
// 
// 		CHE_PDF_SyntaxParser syntaxParser;
// 		syntaxParser.InitParser( pFileRead );
// 		CHE_ByteString str;
// 		HE_BYTE	byteType;
// 		while ( syntaxParser.GetPos() < syntaxParser.GetFileSize() )
// 		{
// 			str = syntaxParser.GetWord();
// 			byteType = syntaxParser.GetType();
// 		//	syntaxParser.NextLine();
// 		//	syntaxParser.NextLine();
// 			int x = 0;
// 		}
//	}
 	return 0;
}