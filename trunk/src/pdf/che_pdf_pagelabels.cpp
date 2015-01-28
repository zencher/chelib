#include "../../Include/pdf/che_pdf_pagelabels.h"

CHE_PDF_PageLabels::CHE_PDF_PageLabels(const CHE_PDF_DictionaryPtr & dict, CHE_Allocator * pAllocator/*= NULL*/)
: CHE_Object(pAllocator), mpNumberTree(NULL)
{
	if ( dict )
	{
		mpNumberTree = GetAllocator()->New<CHE_PDF_NumberTree>(dict, GetAllocator());
		mpNumberTree->ParseAll();

		HE_INT32 num = 0;
		CHE_PDF_PageLabel label;
		CHE_PDF_ObjectPtr objPtr;
		CHE_PDF_DictionaryPtr dictPtr;
		objPtr = mpNumberTree->First(num);
		while ( objPtr && objPtr->GetType() == OBJ_TYPE_DICTIONARY )
		{
			dictPtr = objPtr->GetDictPtr();
			if ( GetLabel(dictPtr, label) )
			{
				mLabelsInfo.insert(make_pair(num, label));
			}
			objPtr = mpNumberTree->Next(num);
		}
	}
}

CHE_PDF_PageLabels::~CHE_PDF_PageLabels()
{
	if ( mpNumberTree )
	{
		mpNumberTree->GetAllocator()->Delete(mpNumberTree);
		mpNumberTree = NULL;
	}
}

HE_BOOL CHE_PDF_PageLabels::GetLabel(const CHE_PDF_DictionaryPtr & dict, CHE_PDF_PageLabel & label)
{
	CHE_PDF_ObjectPtr objPtr = dict->GetElement("S", OBJ_TYPE_NAME);
	if ( objPtr )
	{
		CHE_ByteString str = objPtr->GetNamePtr()->GetString();
		switch (StringToDWORD(str))
		{
		case A('D'): label.stype = LABEL_DEC_NUM; break;
		case A('R'): label.stype = LABEL_UPPER_ROMAN_NUM; break;
		case A('r'): label.stype = LABEL_LOWER_ROMAN_NUM; break;
		case A('A'): label.stype = LABEL_UPPER_LETTER; break;
		case A('a'): label.stype = LABEL_LOWER_LETTER; break;
		default: return false;
		} 
	}

	objPtr = dict->GetElement("P", OBJ_TYPE_NAME);
	if ( objPtr )
	{
		label.prefix = objPtr->GetNamePtr()->GetString();
	}

	objPtr = dict->GetElement("St", OBJ_TYPE_NUMBER);
	if (objPtr)
	{
		label.start = objPtr->GetNumberPtr()->GetInteger();
	}
	return true;
}