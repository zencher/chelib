#include "../../include/pdf/che_pdf_pattern.h"

namespace chelib {

CPDF_ShadingPtr CPDF_Shading::Create( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator /*= nullptr*/ )
{
    CPDF_ShadingPtr ptr;
    
    if ( pAllocator == nullptr )
    {
        pAllocator = GetDefaultAllocator();
    }
    
    if ( rootObjPtr && rootObjPtr->GetType() == OBJ_TYPE_REFERENCE && pComponentMgr != nullptr )
    {
        CPDF_ObjectPtr objPtr = rootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_DICTIONARY );
        if ( objPtr )
        {
            CPDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
            objPtr = dictPtr->GetElement("ShadingType", OBJ_TYPE_NUMBER);
            if (objPtr)
            {
                CPDF_Shading * pShading = nullptr;
                PDF_SHADING_TYPE type = (PDF_SHADING_TYPE)objPtr->GetNumberPtr()->GetInteger();
                switch(type)
                {
                case SHADING_TYPE_FunctionBase: break;
                case SHADING_TYPE_Axial:
                    pShading = pAllocator->New<CPDF_Shading_Axial>(rootObjPtr, pComponentMgr, pAllocator);
                    break;
                case SHADING_TYPE_Radial:
                    pShading = pAllocator->New<CPDF_Shading_Radial>(rootObjPtr, pComponentMgr, pAllocator);
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
                    pShading = nullptr;
                }else{
                    ptr.Reset( pShading );
                }
            }
        }
    }
    return ptr;
}

CPDF_ShadingPtr CPDF_Shading::Convert( const CPDF_ComponentPtr & componentPtr )
{
    CPDF_ShadingPtr ptr;
    if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_Shading )
    {
        ptr.Reset( componentPtr.GetPointer() );
    }
    return ptr;
}

CPDF_Shading::CPDF_Shading(const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator/*= nullptr*/)
    : CPDF_Component(COMPONENT_TYPE_Shading, rootObjPtr, pAllocator), mShadingType(SHADING_TYPE_FunctionBase)
{
    CPDF_ObjectPtr objPtr = mRootObject;
    objPtr = objPtr->GetRefPtr()->GetRefObj(OBJ_TYPE_DICTIONARY);
    if (objPtr)
    {
        CPDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
        objPtr = dictPtr->GetElement("ShadingType", OBJ_TYPE_NUMBER);
        if (objPtr)
        {
            mShadingType = (PDF_SHADING_TYPE)objPtr->GetNumberPtr()->GetInteger();
        }
        objPtr = dictPtr->GetElement("ColorSpace");
        if (objPtr)
        {
            mColorSpace = CPDF_ColorSpace::Create(objPtr, pAllocator);
        }
        objPtr = dictPtr->GetElement("Background", OBJ_TYPE_ARRAY);
        if (objPtr)
        {
            CPDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
            for(size_t i = 0; i < arrPtr->GetCount(); ++i)
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

CPDF_AxialShadingPtr CPDF_Shading::GetAixalShadingPtr() const
{
    CPDF_AxialShadingPtr ptr;
    if ( mShadingType == SHADING_TYPE_Axial )
    {
        ptr.Reset( (CPDF_Shading_Axial*)this );
    }
    return ptr;
}

CPDF_RadialShadingPtr CPDF_Shading::GetRadialShadingPtr() const
{
    CPDF_RadialShadingPtr ptr;
    if ( mShadingType == SHADING_TYPE_Radial )
    {
        ptr.Reset( (CPDF_Shading_Radial*)this );
    }
    return ptr;

}

CPDF_Shading_Axial::CPDF_Shading_Axial( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator/*= nullptr*/)
 : CPDF_Shading(rootObjPtr, pComponentMgr, pAllocator), mStartExtend(false), mEndExtend(false)
{
    CPDF_ObjectPtr objPtr = mRootObject;
    objPtr = objPtr->GetRefPtr()->GetRefObj(OBJ_TYPE_DICTIONARY);
    if (objPtr)
    {
        CPDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
        objPtr = dictPtr->GetElement("Function");
        if (objPtr)
        {
            //objPtr = CPDF_Reference::Create (25, 0, objPtr->GetRefPtr()->GetFile());
            mFunction = CPDF_Function::Create(objPtr, pAllocator);
        }
        objPtr = dictPtr->GetElement("Coords", OBJ_TYPE_ARRAY);
        if (objPtr)
        {
            CPDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
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
            CPDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
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

CPDF_Shading_Radial::CPDF_Shading_Radial( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator/*= nullptr*/)
 : CPDF_Shading(rootObjPtr, pComponentMgr, pAllocator), mStartRadius(0), mEndRadius(0), mStartExtend(false), mEndExtend(false)
{
    CPDF_ObjectPtr objPtr = mRootObject;
    objPtr = objPtr->GetRefPtr()->GetRefObj(OBJ_TYPE_DICTIONARY);
    if (objPtr)
    {
        CPDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
        objPtr = dictPtr->GetElement("Function");
        if (objPtr)
        {
            mFunction = CPDF_Function::Create(objPtr, pAllocator);
        }
        objPtr = dictPtr->GetElement("Coords", OBJ_TYPE_ARRAY);
        if (objPtr)
        {
            CPDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
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
            CPDF_ArrayPtr arrPtr = objPtr->GetArrayPtr();
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


CPDF_TilingPtr CPDF_Tiling::Create( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator/*= nullptr*/ )
{
	CPDF_TilingPtr ptr;
	if ( rootObjPtr && rootObjPtr->GetType() == OBJ_TYPE_REFERENCE && pComponentMgr != nullptr )
	{
		CPDF_ObjectPtr objPtr = rootObjPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
		if ( objPtr )
		{
			if ( pAllocator == nullptr )
			{
				pAllocator = GetDefaultAllocator();
			}
			CPDF_Tiling * pTiling = pAllocator->New<CPDF_Tiling>( rootObjPtr, pComponentMgr, pAllocator );
			if ( pTiling->IsError() )
			{
				pAllocator->Delete( pTiling );
				pTiling = nullptr;
			}else{
				ptr.Reset( pTiling );
			}
		}
	}
	return ptr;
}

CPDF_TilingPtr CPDF_Tiling::Convert( const CPDF_ComponentPtr & componentPtr )
{
	CPDF_TilingPtr ptr;
	if ( componentPtr && componentPtr->GetType() == COMPONENT_TYPE_Tiling )
	{
		ptr.Reset( componentPtr.GetPointer() );
	}
	return ptr;
}

CPDF_Tiling::CPDF_Tiling( const CPDF_ObjectPtr & rootObjPtr, CPDF_ComponentMgr * pComponentMgr, CAllocator * pAllocator/*= nullptr*/ )
	: CPDF_Component(COMPONENT_TYPE_Tiling, rootObjPtr, pAllocator), mbColored(false), mTilingType(1), mXSetp(100), mYSetp(100)
{
	CPDF_ObjectPtr objPtr = mRootObject;
	objPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
	if ( objPtr )
	{
		CPDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
		CPDF_DictionaryPtr dictPtr = stmPtr->GetDictPtr();
		if ( dictPtr )
		{
			CPDF_ObjectPtr objPtr;
			CPDF_NamePtr namePtr;
			CPDF_NumberPtr numberPtr;
			CPDF_ArrayPtr arrayPtr;
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
				int32 patternType = objPtr->GetNumberPtr()->GetInteger();
				if ( patternType != 1 )
				{
					SetError( COMPONENT_ERROR_CONSTRUCTION );
					return;
				}
			}

			objPtr = dictPtr->GetElement( "PaintType", OBJ_TYPE_NUMBER );
			if ( objPtr )
			{
				int32 paintType = objPtr->GetNumberPtr()->GetInteger();
				if ( paintType == 1 )
				{
					mbColored = TRUE;
				}else if ( paintType == 2 )
				{
					mbColored = false;
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
			CPDF_ContentListBuilder::ParseContentStream( stmPtr, mContentList, pComponentMgr, pAllocator );
		}
	}
}

}//namespace