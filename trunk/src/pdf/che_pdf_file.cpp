#include "../../include/pdf/che_pdf_file.h"

CHE_PDF_File::CHE_PDF_File( CHE_Allocator * pAllocator )
	: CHE_Object(pAllocator), mpParser(NULL), mObjCollector(pAllocator), mXRefTable(pAllocator) {}

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
	CHE_PDF_IndirectObject * pInObj = mObjCollector.GetInObj( refInfo );
	if ( pInObj )
	{
		return pInObj->GetObj()->Clone();
	}
	if ( mpParser == NULL )
	{
		return NULL;
	}
	CHE_PDF_Object * pObj = NULL;
	CHE_PDF_XREF_Entry entry;
	if ( mXRefTable.GetEntry( refInfo.objNum, entry ) )
	{
		if ( entry.GetType() == XREF_ENTRY_TYPE_COMMON )
		{
			mpParser->SetReadPos( entry.GetOffset() );
			pObj = mpParser->GetObject();
			if ( pObj )
			{
				pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, this, GetAllocator() );
				mObjCollector.Add( pInObj );
				return pObj->Clone();
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
				return pObj->Clone();
			}
		}
	}
	return NULL;
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
	if ( mpParser )
	{
		return mpParser->GetPDFVersion();
	}
	return PDF_VERSION_1_7;
}

CHE_PDF_Dictionary * CHE_PDF_File::GetTrailerDict() const
{
	return mXRefTable.GetTrailer();
}

CHE_PDF_Dictionary * CHE_PDF_File::GetRootDict()
{
	if ( mpParser )
	{
		return mpParser->GetRootDict();
	}
	return NULL;
}

CHE_PDF_Dictionary * CHE_PDF_File::GetInfoDict()
{
	if ( mpParser )
	{
		return mpParser->GetInfoDict();
	}
	return NULL;
}

CHE_PDF_Array * CHE_PDF_File::GetIDArray()
{
	if ( mpParser )
	{
		return mpParser->GetIDArray();
	}
	return NULL;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Null()
{
	HE_PDF_RefInfo refInfo = TakeFreeRefIno();
	CHE_PDF_Null * pObj = CHE_PDF_Null::Create( GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj )
	{
		mObjCollector.Add( pInObj );
	}else{
		ReleaseRefInfo( refInfo );
	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Boolean( HE_BOOL value )
{
	HE_PDF_RefInfo refInfo = TakeFreeRefIno();
	CHE_PDF_Boolean * pObj = CHE_PDF_Boolean::Create( value, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}else{
		ReleaseRefInfo( refInfo );
	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Number( HE_INT32 value )
{
	HE_PDF_RefInfo refInfo = TakeFreeRefIno();
	CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}else{
		ReleaseRefInfo( refInfo );
	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Number( HE_FLOAT value )
{
	HE_PDF_RefInfo refInfo = TakeFreeRefIno();
	CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}else{
		ReleaseRefInfo( refInfo );
	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_String( const CHE_ByteString & str )
{
	HE_PDF_RefInfo refInfo = TakeFreeRefIno();
	CHE_PDF_String * pObj = CHE_PDF_String::Create( str, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}else{
		ReleaseRefInfo( refInfo );
	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Name( const CHE_ByteString & str )
{
	HE_PDF_RefInfo refInfo = TakeFreeRefIno();
	CHE_PDF_Name * pObj = CHE_PDF_Name::Create( str, GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}else{
		ReleaseRefInfo( refInfo );
	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Array()
{
	HE_PDF_RefInfo refInfo = TakeFreeRefIno();
	CHE_PDF_Array * pObj = CHE_PDF_Array::Create( GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}else{
		ReleaseRefInfo( refInfo );
	}
	return pInObj;
}

CHE_PDF_IndirectObject* CHE_PDF_File::CreateInObj_Dict()
{
	HE_PDF_RefInfo refInfo = TakeFreeRefIno();
	CHE_PDF_Dictionary * pObj = CHE_PDF_Dictionary::Create( GetAllocator() );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}else{
		ReleaseRefInfo( refInfo );
	}
	return pInObj;
}

CHE_PDF_IndirectObject*	CHE_PDF_File::CreateInObj_Stream()
{
	HE_PDF_RefInfo refInfo = TakeFreeRefIno();
	CHE_PDF_Stream * pObj = CHE_PDF_Stream::Create( refInfo.objNum, refInfo.genNum );
	CHE_PDF_IndirectObject * pInObj = GetAllocator()->New<CHE_PDF_IndirectObject>( refInfo.objNum, refInfo.genNum, pObj, (CHE_PDF_File*)NULL, GetAllocator() );
	if ( pInObj != NULL )
	{
		mObjCollector.Add( pInObj );
	}else{
		ReleaseRefInfo( refInfo );
	}
	return pInObj;
}

HE_PDF_RefInfo CHE_PDF_File::TakeFreeRefIno()
{
	HE_DWORD objNum = mXRefTable.GetMaxObjNum();
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = objNum;
	refInfo.genNum = 0;
	return refInfo;
}

HE_VOID	CHE_PDF_File::ReleaseRefInfo( const HE_PDF_RefInfo & /*refInfo*/ )
{

}