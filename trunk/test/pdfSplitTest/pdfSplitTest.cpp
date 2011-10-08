// pdfSplitTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_define.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_creator.h"

class IHE_ObjectCloneMgr
{
public:
	virtual HE_BOOL		IsCloned( HE_DWORD OriObjNum ) = 0;
	virtual HE_DWORD	GetMapObjNum( HE_DWORD OriObjNum ) = 0;
	virtual HE_VOID		SetMap( HE_DWORD OriObjNum, HE_DWORD NewObjNum ) = 0;
};

class ObjectCloneMgrData
{
public:
	ObjectCloneMgrData() { mObjNum = 0; mNewObjNum = 0;}
	HE_DWORD mObjNum;
	HE_DWORD mNewObjNum;

	ObjectCloneMgrData & operator = ( const ObjectCloneMgrData data )
	{
		mObjNum = data.mObjNum;
		mNewObjNum = data.mNewObjNum;
		return *this;
	}
};

bool operator == ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
{
	if ( data1.mObjNum == data2.mObjNum ) return true;
	return false;
}

bool operator > ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
{
	if ( data1.mObjNum > data2.mObjNum ) return true;
	return false;
}

bool operator < ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
{
	if ( data1.mObjNum < data2.mObjNum ) return true;
	return false;
}

class CHE_ObjectCloneMgr : public IHE_ObjectCloneMgr
{
public:
	HE_BOOL IsCloned( HE_DWORD OriObjNum )
	{
		ObjectCloneMgrData data;
		data.mObjNum = OriObjNum;
		return mDataList.Find( data );
	}

	HE_DWORD GetMapObjNum( HE_DWORD OriObjNum )
	{
		ObjectCloneMgrData data;
		data.mObjNum = OriObjNum;
		mDataList.Find( data );
		return data.mNewObjNum;
	}

	HE_VOID	SetMap( HE_DWORD OriObjNum, HE_DWORD NewObjNum )
	{
		ObjectCloneMgrData data;
		data.mObjNum = OriObjNum;
		data.mNewObjNum = NewObjNum;
		mDataList.Append( data );
	}

	CHE_SkipList< ObjectCloneMgrData > mDataList;
};

CHE_PDF_Dictionary*	CloneDirectDictObj( CHE_PDF_Dictionary * pDict, CHE_PDF_Creator * creator, HE_DWORD objNum, HE_DWORD genNum, IHE_ObjectCloneMgr * pMgr = NULL );
CHE_PDF_Array*		CloneDirectArrayObj( CHE_PDF_Array * pArray, CHE_PDF_Creator * creator, HE_DWORD objNum, HE_DWORD genNum, IHE_ObjectCloneMgr * pMgr = NULL );
HE_DWORD			CloneIndirectObject( CHE_PDF_Reference * pRefObj, CHE_PDF_Creator * creator, IHE_ObjectCloneMgr * pMgr = NULL );

CHE_PDF_Array* CloneDirectArrayObj( CHE_PDF_Array * pArray, CHE_PDF_Creator * creator, HE_DWORD objNum, HE_DWORD genNum, IHE_ObjectCloneMgr * pMgr /*= NULL*/ )
{
	if ( pArray == NULL || creator == NULL )
	{
		return NULL;
	}
	CHE_PDF_Array * pNewArray = CHE_PDF_Array::Create( objNum, genNum );
	CHE_PDF_Object * pTmpObj = NULL;
	for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
	{
		pTmpObj = pArray->GetElement( i );
		switch ( pTmpObj->GetType() )
		{
		case OBJ_TYPE_NULL:
			pNewArray->Append( CHE_PDF_Null::Create( pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
			break;
		case OBJ_TYPE_BOOLEAN:
			pNewArray->Append( CHE_PDF_Boolean::Create( pTmpObj->ToBoolean()->GetValue(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
			break;
		case OBJ_TYPE_NUMBER:
			if ( pTmpObj->ToNumber()->IsInteger() )
			{
				pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetInteger(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
			}else{
				pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetFloat(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
			}
			break;
		case OBJ_TYPE_STRING:
			pNewArray->Append( CHE_PDF_String::Create( pTmpObj->ToString()->GetString(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
			break;
		case OBJ_TYPE_NAME:
			pNewArray->Append( CHE_PDF_Name::Create( pTmpObj->ToName()->GetString(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
			break;
		case OBJ_TYPE_ARRAY:
			pNewArray->Append( CloneDirectArrayObj( pTmpObj->ToArray(), creator, objNum, genNum, pMgr ) );
			break;
		case OBJ_TYPE_DICTIONARY:
			pNewArray->Append( CloneDirectDictObj( pTmpObj->ToDict(), creator, objNum, genNum, pMgr ) );
			break;
		case OBJ_TYPE_REFERENCE:
			{
				HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), creator, pMgr );
				pNewArray->Append( CHE_PDF_Reference::Create( refNum, objNum, genNum ) );
				break;
			}
		default:
			break;
		}
	}
	return pNewArray;
}

CHE_PDF_Dictionary* CloneDirectDictObj( CHE_PDF_Dictionary * pDict, CHE_PDF_Creator * creator, HE_DWORD objNum, HE_DWORD genNum, IHE_ObjectCloneMgr * pMgr /*= NULL*/ )
{
	if ( pDict == NULL || creator == NULL )
	{
		return NULL;
	}

	CHE_PDF_Dictionary * pNewDict = CHE_PDF_Dictionary::Create( objNum, genNum );
	CHE_PDF_Object * pTmpObj = NULL;
	CHE_ByteString key;
	for ( HE_DWORD i = 0; i < pDict->GetCount(); i++ )
	{
		pTmpObj = pDict->GetElementByIndex( i );
		pDict->GetKeyByIndex( i, key );
		switch ( pTmpObj->GetType() )
		{
		case OBJ_TYPE_NULL:
			pNewDict->SetAtNull( key );
			break;
		case OBJ_TYPE_BOOLEAN:
			pNewDict->SetAtBoolean( key, pTmpObj->ToBoolean()->GetValue() );
			break;
		case OBJ_TYPE_NUMBER:
			if ( pTmpObj->ToNumber()->IsInteger() )
			{
				pNewDict->SetAtInteger( key, pTmpObj->ToNumber()->GetInteger() );
			}else{
				pNewDict->SetAtFloatNumber( key, pTmpObj->ToNumber()->GetFloat() );
			}
			break;
		case OBJ_TYPE_STRING:
			pNewDict->SetAtString( key, pTmpObj->ToString()->GetString() );
			break;
		case OBJ_TYPE_NAME:
			pNewDict->SetAtName( key, pTmpObj->ToName()->GetString() );
			break;
		case OBJ_TYPE_ARRAY:
			pNewDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), creator, pNewDict->GetObjNum(), pNewDict->GetObjNum(), pMgr ) );
			break;
		case OBJ_TYPE_DICTIONARY:
			pNewDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), creator, pNewDict->GetObjNum(), pNewDict->GetGenNum(), pMgr ) );
			break;
		case OBJ_TYPE_REFERENCE:
			{
				HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), creator, pMgr );
				pNewDict->SetAtReference( key, refNum );
				break;
			}
		default:
			break;
		}
	}
	return pNewDict;
}

HE_DWORD CloneIndirectObject( CHE_PDF_Reference * pRefObj, CHE_PDF_Creator * creator, IHE_ObjectCloneMgr * pMgr/* = NULL */ )
{
	if ( pRefObj == NULL || creator == NULL )
	{
		return 0;
	}
	if ( pMgr != NULL && pMgr->IsCloned( pRefObj->GetRefNum() ) == TRUE )
	{
		return pMgr->GetMapObjNum( pRefObj->GetRefNum() );
	}
	CHE_PDF_Object * pObj = pRefObj->GetRefObj();
	HE_DWORD newObject = 0;
	switch ( pObj->GetType() )
	{
	case OBJ_TYPE_NULL:
		newObject = creator->AppendIndirectObj_Null()->GetObjNum();
		break; 
	case OBJ_TYPE_BOOLEAN:
		newObject = creator->AppendIndirectObj_Boolean( pObj->ToBoolean()->GetValue() )->GetObjNum();
		break;
	case OBJ_TYPE_NUMBER:
		if ( pObj->ToNumber()->IsInteger() )
		{
			newObject = creator->AppendIndirectObj_Number( pObj->ToNumber()->GetInteger() )->GetObjNum();
		}else{
			newObject = creator->AppendIndirectObj_Number( pObj->ToNumber()->GetFloat() )->GetObjNum();
		}
		break;
	case OBJ_TYPE_STRING:
		newObject = creator->AppendIndirectObj_String( pObj->ToString()->GetString() )->GetObjNum();
		break;
	case OBJ_TYPE_NAME:
		newObject = creator->AppendIndirectObj_Name( pObj->ToName()->GetString() )->GetObjNum();
		break;
	case OBJ_TYPE_ARRAY:
		{
			CHE_PDF_Array * pNewArray = creator->AppendIndirectObj_Array();
			CHE_PDF_Array * pArray = pObj->ToArray();
			CHE_PDF_Object * pTmpObj = NULL;
			for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
			{
				pTmpObj = pArray->GetElement( i );
				switch ( pTmpObj->GetType() )
				{
				case OBJ_TYPE_NULL:
					pNewArray->Append( CHE_PDF_Null::Create( pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
					break;
				case OBJ_TYPE_BOOLEAN:
					pNewArray->Append( CHE_PDF_Boolean::Create( pTmpObj->ToBoolean()->GetValue(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
					break;
				case OBJ_TYPE_NUMBER:
					if ( pTmpObj->ToNumber()->IsInteger() )
					{
						pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetInteger(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
					}else{
						pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetFloat(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
					}
					break;
				case OBJ_TYPE_STRING:
					pNewArray->Append( CHE_PDF_String::Create( pTmpObj->ToString()->GetString(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
					break;
				case OBJ_TYPE_NAME:
					pNewArray->Append( CHE_PDF_Name::Create( pTmpObj->ToName()->GetString(), pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
					break;
				case OBJ_TYPE_ARRAY:
					pNewArray->Append( CloneDirectArrayObj( pTmpObj->ToArray(), creator, pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					pNewArray->Append( CloneDirectDictObj( pTmpObj->ToDict(), creator, pNewArray->GetObjNum(), pNewArray->GetGenNum() )  );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), creator, pMgr );
						pNewArray->Append( CHE_PDF_Reference::Create( refNum, pNewArray->GetObjNum(), pNewArray->GetGenNum() ) );
						break;
					}
				default:
					break;
				}
			}
			newObject = pNewArray->GetObjNum();
			break;
		}
	case OBJ_TYPE_DICTIONARY:
		{
			CHE_PDF_Dictionary * pNewDict = creator->AppendIndirectObj_Dict();
			CHE_PDF_Dictionary * pDict = pObj->ToDict();
			CHE_PDF_Object * pTmpObj = NULL;
			CHE_ByteString key;
			for ( HE_DWORD i = 0; i < pDict->GetCount(); i++ )
			{
				pTmpObj = pDict->GetElementByIndex( i );
				pDict->GetKeyByIndex( i, key );
				switch ( pTmpObj->GetType() )
				{
				case OBJ_TYPE_NULL:
					pNewDict->SetAtNull( key );
					break;
				case OBJ_TYPE_BOOLEAN:
					pNewDict->SetAtBoolean( key, pTmpObj->ToBoolean()->GetValue() );
					break;
				case OBJ_TYPE_NUMBER:
					if ( pTmpObj->ToNumber()->IsInteger() )
					{
						pNewDict->SetAtInteger( key, pTmpObj->ToNumber()->GetInteger() );
					}else{
						pNewDict->SetAtFloatNumber( key, pTmpObj->ToNumber()->GetFloat() );
					}
					break;
				case OBJ_TYPE_STRING:
					pNewDict->SetAtString( key, pTmpObj->ToString()->GetString() );
					break;
				case OBJ_TYPE_NAME:
					pNewDict->SetAtName( key, pTmpObj->ToName()->GetString() );
					break;
				case OBJ_TYPE_ARRAY:
					pNewDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), creator, pNewDict->GetObjNum(), pNewDict->GetObjNum() ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					pNewDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), creator, pNewDict->GetObjNum(), pNewDict->GetGenNum() ) );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), creator, pMgr );
						pNewDict->SetAtReference( key, refNum );
						break;
					}
				default:
					break;
				}
			}
			newObject = pNewDict->GetObjNum();
			break;
		}
	case OBJ_TYPE_STREAM:
		{
			CHE_PDF_Stream * pNewStream = creator->AppendIndirectObj_Stream();
			CHE_PDF_Dictionary * pNewDict = CloneDirectDictObj( pObj->ToStream()->GetDict(), creator, pNewStream->GetObjNum(), pNewStream->GetGenNum(), pMgr );
			pNewStream->SetDict( pNewDict );
			HE_LPBYTE pbuffer = new unsigned char[pObj->ToStream()->GetRawSize()];
			pObj->ToStream()->GetRawData( 0, pbuffer, pObj->ToStream()->GetRawSize() );
			pNewStream->SetRawData( pbuffer, pObj->ToStream()->GetRawSize() );
			delete [] pbuffer;
			pbuffer = NULL;
			newObject = pNewStream->GetObjNum();
			break;
		}
	case OBJ_TYPE_REFERENCE:
		{
			newObject = CloneIndirectObject( pObj->ToReference(), creator, pMgr );
			break;
		}
	default:
		break;
	}
	if ( pMgr )
	{
		pMgr->SetMap( pObj->GetObjNum(), newObject );
	}
	return newObject;
}

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

	for ( HE_DWORD i = 0; i < parser.GetPageCount()/*/10*/; i++ )
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
			if ( key == "Type" || key == "Parent" /*|| key == "MediaBox"*/ )
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
				pNewPageDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), &creator, pNewPageDict->GetObjNum(), pNewPageDict->GetObjNum(), &ObjCloneMgr ) );
				break;
			case OBJ_TYPE_DICTIONARY:
				pNewPageDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), &creator, pNewPageDict->GetObjNum(), pNewPageDict->GetGenNum(), &ObjCloneMgr ) );
				break;
			case OBJ_TYPE_REFERENCE:
				{
					HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), &creator, &ObjCloneMgr );
					pNewPageDict->SetAtReference( key, refNum );
					break;
				}
			default:
				break;
			}
		}
	}

// 	CHE_PDF_Object * pObj = pFirstPageDict->GetElement( "Contents" );
// 	HE_DWORD objNum = CloneIndirectObject( pObj->ToReference(), &creator );
// 	pNewPageDict->SetAtReference( "Contents", objNum );
// 
// 	pObj = pFirstPageDict->GetElement( "Resources" );
// 	objNum = CloneIndirectObject( pObj->ToReference(), &creator );
// 	pNewPageDict->SetAtReference( "Resources", objNum );
// 
// 	pObj = pFirstPageDict->GetElement( "Annots" );
// 	objNum = CloneIndirectObject( pObj->ToReference(), &creator );
// 	pNewPageDict->SetAtReference( "Annots", objNum );
	

	IHE_Write * pWrite = HE_CreateFileWrite( "D:\\newPdfPage.pdf" );

	creator.Save( pWrite );


// 	for ( HE_DWORD i = 0; i < pFirstPageDict->GetCount(); i++ )
// 	{
// 		if (  )
// 		{
// 		}
// 	}
}

