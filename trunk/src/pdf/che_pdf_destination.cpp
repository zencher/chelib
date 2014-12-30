#include "../../include/pdf/che_pdf_destination.h"

CHE_PDF_DestinationPtr CHE_PDF_Destination::Create(const CHE_PDF_ArrayPtr & destArray, CHE_Allocator * pAllocator /*= NULL*/)
{
	CHE_PDF_DestinationPtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	if ( destArray )
	{
		ptr.Reset(pAllocator->New<CHE_PDF_Destination>(destArray, pAllocator));
	}
	return ptr;
}

CHE_PDF_Destination::CHE_PDF_Destination(const CHE_PDF_ArrayPtr & arr, CHE_Allocator * pAllocator/*= NULL*/)
	: CHE_PDF_Component(COMPONENT_TYPE_Destination, arr, pAllocator), mDestType(DESTINATION_NONE), 
	mLeft(0.0), mRight(0.0), mTop(0.0), mBottom(0.0), mZoom(0.0)
{
	if ( arr  )
	{
		CHE_PDF_ObjectPtr objPtr = arr->GetElement( 0, OBJ_TYPE_REFERENCE );
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
			CHE_ByteString str = objPtr->GetNamePtr()->GetString();
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