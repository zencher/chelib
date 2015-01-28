#include "stdafx.h"
#include "CppUnitTest.h"

#include "../../../../include/pdf/che_pdf_file.h"
#include "../../../../include/pdf/che_pdf_font.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(PDF_Type0_Font)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			// TODO:  在此输入测试代码
			CHE_PDF_File file;
			IHE_Read * pRead = HE_CreateFileRead("c:\\Users\\ruijie\\desktop\\Test1.pdf");
			file.Open(pRead);

			CHE_PDF_Font* pFont = NULL;

			PDF_RefInfo refInfo;
			refInfo.objNum = 1;
			refInfo.genNum = 0;
			CHE_PDF_ObjectPtr obj = file.GetObject(refInfo);

			if (obj)
			{
				pFont = CHE_PDF_Font::Create( obj->GetDictPtr() );
			}
			int x = 0;
			int y = x;
			
		}

	};
}