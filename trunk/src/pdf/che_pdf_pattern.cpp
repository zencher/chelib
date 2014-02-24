#include "../../include/pdf/che_pdf_pattern.h"

CHE_PDF_TilingPtr CHE_PDF_Tiling::Create( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator/*= NULL*/ )
{

	CHE_PDF_TilingPtr ptr;
	if ( rootObjPtr && rootObjPtr->GetType() == OBJ_TYPE_REFERENCE && pComponentMgr != NULL )
	{
		CHE_PDF_ObjectPtr objPtr = rootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
		if ( objPtr )
		{
			if ( pAllocator == NULL )
			{
				pAllocator = GetDefaultAllocator();
			}
			CHE_PDF_Tiling * pTiling = pAllocator->New<CHE_PDF_Tiling>( rootObjPtr, pComponentMgr, pAllocator );
			if ( pTiling->IsError() )
			{
				pAllocator->Delete( pTiling );
				pTiling = NULL;
			}else{
				ptr.Reset( pTiling );
			}
		}
	}
	return ptr;
}

CHE_PDF_TilingPtr CHE_PDF_Tiling::Convert( const CHE_PDF_ComponentPtr & componentPtr )
{
	CHE_PDF_TilingPtr ptr;
	if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_Tiling )
	{
		ptr.Reset( componentPtr.GetPointer() );
	}
	return ptr;
}

CHE_PDF_Tiling::CHE_PDF_Tiling( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator/*= NULL*/ )
	: CHE_PDF_Component(COMPONENT_TYPE_Tiling, rootObjPtr, pAllocator), mbColored(FALSE), mTilingType(1), mXSetp(100), mYSetp(100)
{
	CHE_PDF_ObjectPtr objPtr = mRootObjPtr;
	objPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
	if ( objPtr )
	{
		CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
		CHE_PDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
		if ( dictPtr )
		{
			CHE_PDF_ObjectPtr objPtr;
			CHE_PDF_NamePtr namePtr;
			CHE_PDF_NumberPtr numberPtr;
			CHE_PDF_ArrayPtr arrayPtr;
			objPtr = dictPtr->GetElement( "Type", OBJ_TYPE_NAME );
			if ( objPtr )
			{
				namePtr = objPtr->GetNamePtr();
				if ( namePtr->GetString() != "Pattern" )
				{
					SetError( COMPONENT_ERROR_CONSTRUCTION );
					return;
				}
			}

			objPtr = dictPtr->GetElement( "PatternType", OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				HE_INT32 patternType = objPtr->GetNumberPtr()->GetInteger();
				if ( patternType != 1 )
				{
					SetError( COMPONENT_ERROR_CONSTRUCTION );
					return;
				}
			}

			objPtr = dictPtr->GetElement( "PaintType", OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				HE_INT32 paintType = objPtr->GetNumberPtr()->GetInteger();
				if ( paintType == 1 )
				{
					mbColored = TRUE;
				}else if ( paintType == 2 )
				{
					mbColored = FALSE;
				}else{
					SetError( COMPONENT_ERROR_CONSTRUCTION );
					return;
				}
			}

			objPtr = dictPtr->GetElement( "TilingType", OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mTilingType = objPtr->GetNumberPtr()->GetInteger();
			}

			objPtr = dictPtr->GetElement( "XStep", OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mXSetp = objPtr->GetNumberPtr()->GetInteger();
			}

			objPtr = dictPtr->GetElement( "YStep", OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				mYSetp = objPtr->GetNumberPtr()->GetInteger();
			}

			objPtr = dictPtr->GetElement( "BBox", OBJ_TYPE_ARRAY );
			if ( objPtr )
			{
				arrayPtr = objPtr->GetArrayPtr();
				arrayPtr->GetRect( mBBox );
			}

			objPtr = dictPtr->GetElement( "Matrix", OBJ_TYPE_ARRAY );
			if ( objPtr )
			{
				arrayPtr = objPtr->GetArrayPtr();
				arrayPtr->GetMatrix( mMatrix );
			}
			CHE_PDF_ContentListBuilder::ParseContentStream( stmPtr, mContentList, pComponentMgr, pAllocator );
		}
	}
}