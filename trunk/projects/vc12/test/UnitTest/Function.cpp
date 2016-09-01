#include "stdafx.h"
#include "CppUnitTest.h"

#include <vector>
#include "../../../../include/pdf/che_pdf_file.h"
#include "../../../../include/pdf/che_pdf_function.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Function
{
	TEST_CLASS(PDF_PostScript_Function)
	{
	public:
		TEST_METHOD(Create)
		{
			CHE_PDF_File file;
			CHE_PDF_ObjectPtr obj;
			CHE_PDF_ArrayPtr array;
			CHE_PDF_DictionaryPtr dict;
			CHE_PDF_StreamPtr stm;
			CHE_PDF_FunctionPtr fun;
			std::vector<FLOAT> inputs;
			std::vector<FLOAT> outputs;

			file.CreateStreamObject(stm);
			dict = stm->GetDictPtr();
			dict->SetInteger("FunctionType", 4);
			array = CHE_PDF_Array::Create(GetDefaultAllocator());
			array->Append(CHE_PDF_Number::Create(0));
			array->Append(CHE_PDF_Number::Create(100));
			dict->SetArray("Domain", array);
			array = CHE_PDF_Array::Create();
			array->Append(CHE_PDF_Number::Create(0));
			array->Append(CHE_PDF_Number::Create(200));
			dict->SetArray("Range", array);
			stm->SetRawData((PBYTE)"{ false { 100 add } {1 add } ifelse }", 38);

			fun = CHE_PDF_Function::Create(stm);
			inputs.push_back(50);
			fun->Calculate(inputs, outputs);
		}
	};
}