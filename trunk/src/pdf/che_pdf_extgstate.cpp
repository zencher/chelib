#include "../../include/pdf/che_pdf_extgstate.h"


CHE_PDF_ExtGStatePtr CHE_PDF_ExtGState::Create( const CHE_PDF_ObjectPtr & objPtr, CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_ExtGStatePtr ptr;
	if ( objPtr )
	{
		if ( pAllocator == nullptr )
		{
			pAllocator = GetDefaultAllocator();
		}

		if ( objPtr->GetType() == OBJ_TYPE_REFERENCE  )
		{
			CHE_PDF_ObjectPtr tmpObjPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
			if ( tmpObjPtr )
			{
				ptr.Reset( pAllocator->New<CHE_PDF_ExtGState>( tmpObjPtr, pAllocator ) );
			}
		}else if ( objPtr->GetType() == OBJ_TYPE_DICTIONARY )
		{
			ptr.Reset( pAllocator->New<CHE_PDF_ExtGState>( objPtr, pAllocator ) );
		}
	}
	return ptr;
}


CHE_PDF_ExtGStatePtr CHE_PDF_ExtGState::Convert( const CHE_PDF_ComponentPtr & componetPtr )
{
	CHE_PDF_ExtGStatePtr ptr;
	if ( componetPtr && componetPtr->GetType() == COMPONENT_TYPE_ExtGState )
	{
		ptr.Reset( componetPtr.GetPointer() );
	}
	return ptr;
}


CHE_PDF_ExtGState::CHE_PDF_ExtGState( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_Allocator * pAllocator /*= nullptr*/ )
	:CHE_PDF_Component( COMPONENT_TYPE_ExtGState, rootObjPtr, pAllocator )
{
	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_DictionaryPtr dictPtr = mRootObject->GetDictPtr();

	CHE_PDF_ExtGStateItem item;

	objPtr = dictPtr->GetElement( "LW", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_LW;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "LJ", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_LJ;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "LC", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_LC;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "ML", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_ML;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "D", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_D;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "RI", OBJ_TYPE_NAME );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_RI;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "OP", OBJ_TYPE_BOOLEAN );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_OP;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "op", OBJ_TYPE_BOOLEAN );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_op;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "OPM", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_OPM;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "Font", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_OP;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "BG" );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_BG;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "BG2" );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_BG2;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "UCR" );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_UCR;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "UCR2" );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_UCR2;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "TR" );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_TR;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "TR2" );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_TR2;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "HT" );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_HT;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "FL", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_FL;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "SM", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_SM;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}
	
	objPtr = dictPtr->GetElement( "SM", OBJ_TYPE_BOOLEAN );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_SA;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}
	
	objPtr = dictPtr->GetElement( "BM", OBJ_TYPE_NAME );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_BM;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}else{
		objPtr = dictPtr->GetElement( "BM", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			item.type = PDF_EXTGSTATE_BM;
			item.objPtr = objPtr;
			mStateItems.push_back( item );
		}
	}

	objPtr = dictPtr->GetElement( "SMask", OBJ_TYPE_NAME );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_SMask;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}else{
		objPtr = dictPtr->GetElement( "SMask", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			item.type = PDF_EXTGSTATE_SMask;
			item.objPtr = objPtr;
			mStateItems.push_back( item );
		}
	}

	objPtr = dictPtr->GetElement( "CA", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_CA;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "ca", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_ca;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "AIS", OBJ_TYPE_BOOLEAN );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_AIS;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}

	objPtr = dictPtr->GetElement( "TK", OBJ_TYPE_BOOLEAN );
	if ( objPtr )
	{
		item.type = PDF_EXTGSTATE_TK;
		item.objPtr = objPtr;
		mStateItems.push_back( item );
	}
}