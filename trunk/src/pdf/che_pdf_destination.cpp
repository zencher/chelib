#include "../../include/pdf/che_pdf_destination.h"

namespace chelib {

CPDF_DestinationPtr CPDF_Destination::Create(const CPDF_ArrayPtr & destArray, CAllocator * pAllocator /*= nullptr*/)
{
	CPDF_DestinationPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	if ( destArray )
	{
		ptr.Reset(pAllocator->New<CPDF_Destination>(destArray, pAllocator));
	}
	return ptr;
}

CPDF_Destination::CPDF_Destination(const CPDF_ArrayPtr & arr, CAllocator * pAllocator/*= nullptr*/)
	: CPDF_Component(COMPONENT_TYPE_Destination, arr, pAllocator), mDestType(DESTINATION_NONE), 
	mLeft(0.0), mRight(0.0), mTop(0.0), mBottom(0.0), mZoom(0.0)
{
	if ( arr  )
	{
		CPDF_ObjectPtr objPtr = arr->GetElement( 0, OBJ_TYPE_REFERENCE );
		if ( objPtr )
		{
			mPageRef = objPtr->GetRefPtr();
		}else{
			objPtr = arr->GetElement(0, OBJ_TYPE_NUMBER);
			if ( objPtr )
			{
				mPageIndex = objPtr->GetNumberPtr()->GetInteger();
			}
		}
		objPtr = arr->GetElement( 1, OBJ_TYPE_NAME );
		if ( objPtr )
		{
			CByteString str = objPtr->GetNamePtr()->GetString();
			if ( str == "XYZ" )
			{
				mDestType = DESTINATION_PAGE_XYZ;
			}else if ( str == "Fit" )
			{
				mDestType = DESTINATION_PAGE_FIT;
			}else if ( str == "FitH" )
			{
				mDestType = DESTINATION_PAGE_FITH;
			}else if ( str == "FitV" )
			{
				mDestType = DESTINATION_PAGE_FITV;
			}else if ( str == "FitR" )
			{
				mDestType = DESTINATION_PAGE_FITR;
			}else if ( str == "FitB" )
			{
				mDestType = DESTINATION_PAGE_FITB;
			}else if ( str == "FitBH" )
			{
				mDestType = DESTINATION_PAGE_FITBH;
			}else if ( str == "FitBV" )
			{
				mDestType = DESTINATION_PAGE_FITBV;
			}
		}
		switch ( mType )
		{
		case DESTINATION_PAGE_XYZ:
			{
				objPtr = arr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mLeft = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arr->GetElement( 3, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mTop = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arr->GetElement( 4, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mZoom = objPtr->GetNumberPtr()->GetFloat();
				}
				break;
			}
		case DESTINATION_PAGE_FITH:
		case DESTINATION_PAGE_FITBH:
			{
				objPtr = arr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mTop = objPtr->GetNumberPtr()->GetFloat();
				}
				break;
			}
		case DESTINATION_PAGE_FITV:
		case DESTINATION_PAGE_FITBV:
			{
				objPtr = arr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mLeft = objPtr->GetNumberPtr()->GetFloat();
				}
				break;
			}
		case DESTINATION_PAGE_FITR:
			{
				objPtr = arr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mLeft = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arr->GetElement( 3, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mBottom = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arr->GetElement( 4, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mRight = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arr->GetElement( 5, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mTop = objPtr->GetNumberPtr()->GetFloat();
				}
				break;
			}
		case DESTINATION_PAGE_FIT:
		case DESTINATION_PAGE_FITB:
		default:
			break;
		}
	}
}

}//namespace