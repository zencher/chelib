#include "../../include/pdf/che_pdf_file.h"
#include "../../include/pdf/che_pdf_creator.h"

CHE_PDF_File::CHE_PDF_File( CHE_Allocator * pAllocator )
	: CHE_Object(pAllocator), mpParser(NULL), mVersion(PDF_VERSION_1_7), mObjCollector(pAllocator), mXRefTable(pAllocator) {}

CHE_PDF_File::~CHE_PDF_File()
{
	Close();
}

HE_BOOL CHE_PDF_File::Open( IHE_Read * pRead )
{
	if ( pRead == NULL )
	{
		return FALSE;
	}
	if ( mpParser )
	{
		mpParser->GetAllocator()->Delete( mpParser );
		mObjCollector.ReleaseObj();
		mObjCollector.Clear();
		mXRefTable.Clear();
	}
	mpParser = CHE_PDF_Parser::Create( this, pRead, &mXRefTable, GetAllocator() );
	if ( mpParser )
	{
		mVersion = mpParser->GetPDFVersion();
		return TRUE;
	}
	return FALSE;
}

HE_VOID CHE_PDF_File::Close()
{
	mObjCollector.ReleaseObj();
	mObjCollector.Clear();
	mXRefTable.Clear();
	if ( mpParser )
	{
		mpParser->GetAllocator()->Delete( mpParser );
		mpParser = NULL;
	}
}

HE_BOOL CHE_PDF_File::Save( IHE_Write * pWrite )
{
	if ( pWrite == NULL )
	{
		return FALSE;
	}
	CHE_PDF_Creator * pCreator = CHE_PDF_Creator::Create( pWrite, GetAllocator() );
	if ( pCreator == NULL )
	{
		return FALSE;
	}

	HE_DWORD objCount = 0;
	HE_DWORD offset = 0;
	CHE_PDF_XREF_Entry entry;
	CHE_PDF_XREF_Table xref( GetAllocator() );
	HE_PDF_RefInfo refInfo;
	CHE_PDF_IndirectObject * pInObj = NULL;

	pCreator->OutPutFileHead( GetPDFVersion() );

	entry.ObjNum = 0;
	entry.Field1 = 0;
	entry.Field2 = 65535;
	entry.Type = XREF_ENTRY_TYPE_FREE;

	xref.Add( entry );
	++objCount;

	mXRefTable.MoveFirst();
	while( !mXRefTable.IsEOF() )
	{
		if ( mXRefTable.GetCurNode( entry ) && entry.GetObjNum() != 0 )
		{
			refInfo.objNum = entry.GetObjNum();
			refInfo.genNum = 0;
			pInObj = GetInObject( refInfo );
			if ( pInObj )
			{
				if ( pInObj->GetObj()->GetType() == OBJ_TYPE_STREAM )
				{
					CHE_PDF_Dictionary * pDict = pInObj->GetObj()->ToStream()->GetDict();
					CHE_PDF_Object * pObj = NULL;
					CHE_PDF_Name * pName = NULL;
					CHE_PDF_ObjectCollector objCollecor;
					pObj = pDict->GetElement( "Type", OBJ_TYPE_NAME, objCollecor );
					if ( pObj )
					{
						pName = pObj->ToName();
						if ( pName )
						{
							if ( pName->GetString() == "ObjStm" )
							{
								mXRefTable.MoveNext();
								continue;
							}
						}
					}
				}
				offset = pCreator->OutPutInObject( pInObj );
				entry.Type = XREF_ENTRY_TYPE_COMMON;
				entry.ObjNum = refInfo.objNum;
				entry.Field1 = offset;
				entry.Field2 = 0;
				xref.Add( entry );
				++objCount;
			}
		}
		mXRefTable.MoveNext();
	}

	offset = pCreator->OutPutXRefTable( xref );
	CHE_PDF_Object * pObj = NULL;
	CHE_PDF_Dictionary * pDict = mXRefTable.GetTrailer();
	CHE_PDF_Dictionary * pNewDict = CHE_PDF_Dictionary::Create( GetAllocator() );
	pObj = pDict->GetElement( "Root" );
	if ( pObj )
	{
		pNewDict->SetAtObj( "Root", pObj->Clone() );
	}
	pObj = pDict->GetElement( "Info" );
	if ( pObj )
	{
		pNewDict->SetAtObj( "Info", pObj->Clone() );
	}
	pNewDict->SetAtInteger( "Size", objCount );
	pCreator->OutPutTailerDict( pNewDict );
	pCreator->OutPutFileTailer( offset );
	pNewDict->Release();
	return TRUE;
}

HE_BOOL CHE_PDF_File::Authenticate( const CHE_ByteString & password ) const
{
	if ( mpParser )
	{
		return mpParser->Authenticate( password );
	}
	return FALSE;
}

CHE_PDF_Object * CHE_PDF_File::GetObject( const HE_PDF_RefInfo & refInfo )
{
	CHE_PDF_IndirectObject * pInObj = GetInObject( refInfo );
	if ( pInObj )
	{
		return pInObj->GetObj()->Clone();
	}
	return NULL;
}

CHE_PDF_IndirectObject * CHE_PDF_File::GetInObject( const HE_PDF_RefInfo & refInfo )
{
	CHE_PDF_IndirectObject * pInObj = mObjCollector.GetInObj( refInfo );
	if ( pInObj )
	{
		return pInObj;
	}
	if ( mpParser == NULL )
	{
		return NULL;
	}
	CHE_PDF_Object * pObj = NULL;
	CHE_PDF_XREF_Entry entry;
	if ( mXRefTable.Get( refInfo.objNum, entry ) )
	{
		if ( entry.GetType() == XREF_ENTRY_TYPE_COMMON )
		{
			mpParser->SetReadPos( entry.GetOffset() );
			pObj = mpParser->GetObject();
			if ( pObj )
			{
				pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, this, GetAllocator() );
				mObjCollector.Add( pInObj );
				return pInObj;
			}
		}else{
			HE_PDF_RefInfo stmRefInfo;
			stmRefInfo.objNum = entry.GetParentObjNum();
			pInObj = mObjCollector.GetInObj( stmRefInfo );
			if ( pInObj == NULL )
			{
				return NULL;
			}
			CHE_PDF_Object * pTmpObj = pInObj->GetObj();
			if ( pTmpObj == NULL || pTmpObj->GetType() != OBJ_TYPE_STREAM )
			{
				return NULL;
			}
			pObj = mpParser->GetObjectInObjStm( pTmpObj->ToStream(), refInfo, entry.GetIndex() );
			if ( pObj )
			{
				pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, this, GetAllocator() );
				mObjCollector.Add( pInObj );
				return pInObj;
			}
		}
	}
	return NULL;
}

HE_BOOL CHE_PDF_File::ChangeObject( const HE_PDF_RefInfo & refInfo, CHE_PDF_Object * pObj )
{
	if ( pObj == NULL )
	{
		return FALSE;
	}
	CHE_PDF_IndirectObject * pInObj = GetInObject( refInfo );
	if ( pInObj )
	{
		pInObj->mpObj = pObj;
		CHE_PDF_XREF_Entry entry;
		entry.ObjNum = refInfo.objNum;
		entry.Type = XREF_ENTRY_TYPE_NEW;
		mXRefTable.Update( refInfo.objNum, entry );
		return TRUE;
	}
	return FALSE;
}

HE_DWORD CHE_PDF_File::GetFileSize() const
{
	if ( mpParser )
	{
		return mpParser->GetFileSize();
	}
	return 0;
}

PDF_VERSION CHE_PDF_File::GetPDFVersion() const
{
	return mVersion;
}

CHE_PDF_Dictionary * CHE_PDF_File::GetTrailerDict() const
{
	return mXRefTable.GetTrailer();
}

CHE_PDF_Dictionary * CHE_PDF_File::GetRootDict()
{
	CHE_PDF_ObjectCollector objCollector( GetAllocator() );
	CHE_PDF_Object * pObj = mXRefTable.GetTrailer()->GetElement( "Root", OBJ_TYPE_DICTIONARY, objCollector );
	if ( pObj )
	{
		objCollector.PopAll();
		return pObj->ToDict();
	}
	return NULL;
}

CHE_PDF_Dictionary * CHE_PDF_File::GetInfoDict()
{
	CHE_PDF_ObjectCollector objCollector( GetAllocator() );
	CHE_PDF_Object * pObj = mXRefTable.GetTrailer()->GetElement( "Info", OBJ_TYPE_DICTIONARY, objCollector );
	if ( pObj )
	{
		objCollector.PopAll();
		return pObj->ToDict();
	}
	return NULL;
}

CHE_PDF_Array * CHE_PDF_File::GetIDArray()
{
	CHE_PDF_ObjectCollector objCollector( GetAllocator() );
	CHE_PDF_Object * pObj = mXRefTable.GetTrailer()->GetElement( "ID", OBJ_TYPE_ARRAY, objCollector );
	if ( pObj )
	{
		objCollector.PopAll();
		return pObj->ToArray();
	}
	return NULL;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Null()
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Null * pObj = CHE_PDF_Null::Create( GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Boolean( HE_BOOL value )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Boolean * pObj = CHE_PDF_Boolean::Create( value, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Number( HE_INT32 value )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Number( HE_FLOAT value )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_String( const CHE_ByteString & str )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_String * pObj = CHE_PDF_String::Create( str, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Name( const CHE_ByteString & str )
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Name * pObj = CHE_PDF_Name::Create( str, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Array()
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Array * pObj = CHE_PDF_Array::Create( GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Dict()
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Dictionary * pObj = CHE_PDF_Dictionary::Create( GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}

CHE_PDF_IndirectObject*	CHE_PDF_File::CreateInObj_Stream()
{
	CHE_PDF_XREF_Entry entry;
	mXRefTable.AddNewEntry( entry );
	CHE_PDF_Stream * pObj = CHE_PDF_Stream::Create( entry.ObjNum, 0 );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( entry.ObjNum, 0, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}
// 	else{
// 		ReleaseRefInfo( refInfo );
// 	}
	return pInObj;
}