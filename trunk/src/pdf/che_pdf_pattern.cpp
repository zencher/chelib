#include "../../include/pdf/che_pdf_pattern.h"


CHE_PDF_ShadingPtr CHE_PDF_Shading::Create( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator /*= NULL*/ )
{
    CHE_PDF_ShadingPtr ptr;
    
    if ( pAllocator == NULL )
    {
        pAllocator = GetDefaultAllocator();
    }
    
    if ( rootObjPtr && rootObjPtr->GetType() == OBJ_TYPE_REFERENCE && pComponentMgr != NULL )
    {
        CHE_PDF_ObjectPtr objPtr = rootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
        if ( objPtr )
        {
            CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
            objPtr = dictPtr->GetElement("ShadingType", OBJ_TYPE_NUMBER);
            if (objPtr)
            {
                CHE_PDF_Shading * pShading = NULL;
                PDF_SHADING_TYPE type = (PDF_SHADING_TYPE)objPtr->GetNumberPtr()->GetInteger();
                switch(type)
                {
                case SHADING_TYPE_FunctionBase: break;
                case SHADING_TYPE_Axial:
                    pShading = pAllocator->New<CHE_PDF_Shading_Axial>(rootObjPtr, pComponentMgr, pAllocator);
                    break;
                case SHADING_TYPE_Radial:
                    pShading = pAllocator->New<CHE_PDF_Shading_Radial>(rootObjPtr, pComponentMgr, pAllocator);
                    break;
                case SHADING_TYPE_FreeFormGouraudShadedTriangleMesh: break;
                case SHADING_TYPE_LatticeFormGouraudShadedTriangleMesh: break;
                case SHADING_TYPE_CoonsPatchMesh: break;
                case SHADING_TYPE_TensorProductPatchMesh: break;
                default: break;
                }
                
                if ( pShading && pShading->IsError() )
                {
                    pAllocator->Delete( pShading );
                    pShading = NULL;
                }else{
                    ptr.Reset( pShading );
                }
            }
        }
    }
    return ptr;
}

CHE_PDF_ShadingPtr CHE_PDF_Shading::Convert( const CHE_PDF_ComponentPtr & componentPtr )
{
    CHE_PDF_ShadingPtr ptr;
    if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_Shading )
    {
        ptr.Reset( componentPtr.GetPointer() );
    }
    return ptr;
}

CHE_PDF_Shading::CHE_PDF_Shading(const CHE_PDF_ObjectPtr & rootObjPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator/*= NULL*/)
    : CHE_PDF_Component(COMPONENT_TYPE_Shading, rootObjPtr, pAllocator), mShadingType(SHADING_TYPE_FunctionBase)
{
    CHE_PDF_ObjectPtr objPtr = mRootObject;
    objPtr = objPtr->GetRefPtr()->GetRefObj(OBJ_TYPE_DICTIONARY);
    if (objPtr)
    {
        CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
        objPtr = dictPtr->GetElement("ShadingType", OBJ_TYPE_NUMBER);
        if (objPtr)
        {
            mShadingType = (PDF_SHADING_TYPE)objPtr->GetNumberPtr()->GetInteger();
        }
        objPtr = dictPtr->GetElement("ColorSpace");
        if (objPtr)
        {
            mColorSpace = CHE_PDF_ColorSpace::Create(objPtr, pAllocator);
        }
        objPtr = dictPtr->GetElement("Background", OBJ_TYPE_ARRAY);
        if (objPtr)
        {
            CHE_PDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
            for(HE_ULONG i = 0; i < arrPtr->GetCount(); ++i)
            {
                objPtr = arrPtr->GetElement(i, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mBackgroundColor.Push(objPtr->GetNumberPtr()->GetFloat());
                }
            }
        }
    }
}

CHE_PDF_AxialShadingPtr CHE_PDF_Shading::GetAixalShadingPtr() const
{
    CHE_PDF_AxialShadingPtr ptr;
    if ( mShadingType == SHADING_TYPE_Axial )
    {
        ptr.Reset( (CHE_PDF_Shading_Axial*)this );
    }
    return ptr;
}

CHE_PDF_RadialShadingPtr CHE_PDF_Shading::GetRadialShadingPtr() const
{
    CHE_PDF_RadialShadingPtr ptr;
    if ( mShadingType == SHADING_TYPE_Radial )
    {
        ptr.Reset( (CHE_PDF_Shading_Radial*)this );
    }
    return ptr;

}

CHE_PDF_Shading_Axial::CHE_PDF_Shading_Axial( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator/*= NULL*/)
 : CHE_PDF_Shading(rootObjPtr, pComponentMgr, pAllocator), mStartExtend(FALSE), mEndExtend(FALSE)
{
    CHE_PDF_ObjectPtr objPtr = mRootObject;
    objPtr = objPtr->GetRefPtr()->GetRefObj(OBJ_TYPE_DICTIONARY);
    if (objPtr)
    {
        CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
        objPtr = dictPtr->GetElement("Function");
        if (objPtr)
        {
            //objPtr = CHE_PDF_Reference::Create (25, 0, objPtr->GetRefPtr()->GetFile());
            mFunction = CHE_PDF_Function::Create(objPtr, pAllocator);
        }
        objPtr = dictPtr->GetElement("Coords", OBJ_TYPE_ARRAY);
        if (objPtr)
        {
            CHE_PDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
            if (arrPtr->GetCount() >= 4)
            {
                objPtr = arrPtr->GetElement(0, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mStart.x = objPtr->GetNumberPtr()->GetFloat();
                }
                objPtr = arrPtr->GetElement(1, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mStart.y = objPtr->GetNumberPtr()->GetFloat();
                }
                objPtr = arrPtr->GetElement(2, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mEnd.x = objPtr->GetNumberPtr()->GetFloat();
                }
                objPtr = arrPtr->GetElement(3, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mEnd.y = objPtr->GetNumberPtr()->GetFloat();
                }
            }
        }
        objPtr = dictPtr->GetElement("Extend", OBJ_TYPE_ARRAY);
        if (objPtr)
        {
            CHE_PDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
            if (arrPtr->GetCount() >= 2)
            {
                objPtr = arrPtr->GetElement(0, OBJ_TYPE_BOOLEAN);
                if (objPtr)
                {
                    mStartExtend = objPtr->GetBooleanPtr()->GetValue();
                }
                objPtr = arrPtr->GetElement(1, OBJ_TYPE_BOOLEAN);
                if (objPtr)
                {
                    mEndExtend = objPtr->GetBooleanPtr()->GetValue();
                }
            }
        }
    }
}

CHE_PDF_Shading_Radial::CHE_PDF_Shading_Radial( const CHE_PDF_ObjectPtr & rootObjPtr, CHE_PDF_ComponentMgr * pComponentMgr, CHE_Allocator * pAllocator/*= NULL*/)
 : CHE_PDF_Shading(rootObjPtr, pComponentMgr, pAllocator), mStartRadius(0), mEndRadius(0), mStartExtend(FALSE), mEndExtend(FALSE)
{
    CHE_PDF_ObjectPtr objPtr = mRootObject;
    objPtr = objPtr->GetRefPtr()->GetRefObj(OBJ_TYPE_DICTIONARY);
    if (objPtr)
    {
        CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
        objPtr = dictPtr->GetElement("Function");
        if (objPtr)
        {
            mFunction = CHE_PDF_Function::Create(objPtr, pAllocator);
        }
        objPtr = dictPtr->GetElement("Coords", OBJ_TYPE_ARRAY);
        if (objPtr)
        {
            CHE_PDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
            if (arrPtr->GetCount() >= 6)
            {
                objPtr = arrPtr->GetElement(0, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mStart.x = objPtr->GetNumberPtr()->GetFloat();
                }
                objPtr = arrPtr->GetElement(1, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mStart.y = objPtr->GetNumberPtr()->GetFloat();
                }
                objPtr = arrPtr->GetElement(2, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mStartRadius = objPtr->GetNumberPtr()->GetFloat();
                }
                objPtr = arrPtr->GetElement(3, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mEnd.x = objPtr->GetNumberPtr()->GetFloat();
                }
                objPtr = arrPtr->GetElement(4, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mEnd.y = objPtr->GetNumberPtr()->GetFloat();
                }
                objPtr = arrPtr->GetElement(5, OBJ_TYPE_NUMBER);
                if (objPtr)
                {
                    mEndRadius = objPtr->GetNumberPtr()->GetFloat();
                }            }
        }
        objPtr = dictPtr->GetElement("Extend", OBJ_TYPE_ARRAY);
        if (objPtr)
        {
            CHE_PDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
            if (arrPtr->GetCount() >= 2)
            {
                objPtr = arrPtr->GetElement(0, OBJ_TYPE_BOOLEAN);
                if (objPtr)
                {
                    mStartExtend = objPtr->GetBooleanPtr()->GetValue();
                }
                objPtr = arrPtr->GetElement(1, OBJ_TYPE_BOOLEAN);
                if (objPtr)
                {
                    mEndExtend = objPtr->GetBooleanPtr()->GetValue();
                }
            }
        }
    }
}


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
	CHE_PDF_ObjectPtr objPtr = mRootObject;
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