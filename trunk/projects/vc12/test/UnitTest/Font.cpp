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
		
		TEST_METHOD(Decode)
		{
			CHE_PDF_File file;
			IHE_Read * pRead = HE_CreateFileRead("D:\\PDFFiles\\Test\\FontTest\\Test1.pdf");
			file.Open(pRead);

			//3,11,19,26,33
			PDF_RefInfo refInfo;
			refInfo.objNum = 3;
			refInfo.genNum = 0;
			CHE_PDF_ObjectPtr obj = file.GetObject(refInfo);
			CHE_PDF_Font * pFont1 = CHE_PDF_Font::Create(obj->GetDictPtr());

			HE_WCHAR tcode[63] = {	11588,	8817,	6857,	1002,	2344,	842,	17944,	1688,	987,	955,
									2290,	1692,	1293,	1030,	983,	1399,	10831,	1699,	19353,	1702, 
									21898,	11188,	4301,	2000,	1881,	7247,	2155,	6199,	2520,	2183, 
									21915,	2311,	7296,	2305,	2678,	3160,	8513,	3326,	3768,	11304,
									4300,	4469,	4878,	17001,	12535,	4914,	6101,	13302,	6974,	7528, 
									10088,	11967,	10550,	21901,	10840,	12497,	10951,	12091,	13692,	16658, 
									18992,	19063,	21897 };

			HE_WCHAR tucs[63] = {	30740,	27969,	26009,	20154,	21496,	19994,	37096,	20840,	20139,	20107,
									21442,	20844,	20445,	20182,	20135,	20551,	29983,	20851,	38505,	20854,
									65289,	30340,	23453,	21152,	21033,	26399,	21307,	25351,	21672,	21335,
									65306,	21463,	26448,	21457,	21830,	22312,	27665,	22478,	22920,	30456,
									23452,	23621,	24030,	36153,	31687,	24066,	25253,	32454,	26126,	26680,
									29240,	31119,	29702,	65292,	29992,	31649,	30103,	31243,	32844,	35810,
									38144,	38215,	65288 };

			HE_ULONG tcid[63] = {	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
									0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
									0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
									0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
									0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
									0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
									0,	0,	0 };

			HE_ULONG tgid[63] = {	11588,	8817,	6857,	1002,	2344,	842,	17944,	1688,	987,	955,
									2290,	1692,	1293,	1030,	983,	1399,	10831,	1699,	19353,	1702, 
									21898,	11188,	4301,	2000,	1881,	7247,	2155,	6199,	2520,	2183, 
									21915,	2311,	7296,	2305,	2678,	3160,	8513,	3326,	3768,	11304,
									4300,	4469,	4878,	17001,	12535,	4914,	6101,	13302,	6974,	7528, 
									10088,	11967,	10550,	21901,	10840,	12497,	10951,	12091,	13692,	16658, 
									18992,	19063,	21897 };
 
			HE_BOOL bREt = false;
			HE_WCHAR ucs = 0;
			HE_ULONG gid = 0;
			HE_ULONG cid = 0;
			for ( int i = 0; i <= 62; ++i )
			{
				bREt = false;
				ucs = 0;
				gid = 0;
				cid = 0;
				bREt = pFont1->Decode(tcode[i], ucs, gid, cid);
				Assert::IsTrue( bREt, L"Font1 Decode return false!");
				Assert::AreEqual<HE_WCHAR>(ucs, tucs[i], L"Font1 Unicode get wrong!");
				Assert::AreEqual<HE_ULONG>(gid, tgid[i], L"Font1 GlyphId get wrong!");
				Assert::AreEqual<HE_ULONG>(cid, tcid[i], L"Font1 CID get wrong!");
			}
			
		}

	};
}