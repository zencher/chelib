#include "../../Include/pdf/che_pdf_pagelabel.h"

namespace chelib {

CPDF_PageLabels::CPDF_PageLabels(const CPDF_DictionaryPtr & dict, CAllocator * pAllocator/*= nullptr*/)
: CObject(pAllocator), mpNumberTree(nullptr)
{
	if ( dict )
	{
		mpNumberTree = GetAllocator()->New<CPDF_NumberTree>(dict, GetAllocator());
		mpNumberTree->ParseAll();

		int32 num = 0;
		CPDF_PageLabel label;
		CPDF_ObjectPtr objPtr;
		CPDF_DictionaryPtr dictPtr;
		objPtr = mpNumberTree->First(num);
		while ( objPtr && objPtr->GetType() == OBJ_TYPE_DICTIONARY )
		{
			dictPtr = objPtr->GetDictPtr();
			if ( GetLabel(dictPtr, label) )
			{
                mLabelsInfo.insert(std::make_pair(num, label));
			}
			objPtr = mpNumberTree->Next(num);
		}
	}
}

CPDF_PageLabels::~CPDF_PageLabels()
{
	if ( mpNumberTree )
	{
		mpNumberTree->GetAllocator()->Delete(mpNumberTree);
		mpNumberTree = nullptr;
	}
}

bool CPDF_PageLabels::GetLabel(const CPDF_DictionaryPtr & dict, CPDF_PageLabel & label)
{
	CPDF_ObjectPtr objPtr = dict->GetElement("S", OBJ_TYPE_NAME);
	if ( objPtr )
	{
		CByteString str = objPtr->GetNamePtr()->GetString();
		switch (StringToUINT32(str))
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

}//namespace