#include "stdafx.h"
#include "CppUnitTest.h"

#include "../../../../include/pdf/che_pdf_file.h"
#include "../../../../include/pdf/che_pdf_colorspace.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ColorSpace
{		
	TEST_CLASS(PDF_ColorSpace)
	{
	public:
		TEST_METHOD(CreateDeviceGray)
		{
			CHE_PDF_ColorSpacePtr cs = CHE_PDF_ColorSpace::CreateDeviceGray();
			Assert::IsTrue(cs.operator bool(), L"CreateDeviceRGB Failed!");

			PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_GRAY == type, L"ColorSpace Type Return Error!");

			uint32 c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(1, c, L"DeviceRGB ColorSpace Component Count Error!");
		}

		TEST_METHOD(CreateDeviceRGB)
		{
			CHE_PDF_ColorSpacePtr cs = CHE_PDF_ColorSpace::CreateDeviceRGB();
			Assert::IsTrue(cs.operator bool(), L"CreateDeviceRGB Failed!");

			PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_RGB == type, L"ColorSpace Type Return Error!");

			uint32 c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(3, c, L"DeviceRGB ColorSpace Component Count Error!");
		}

		TEST_METHOD(CreateDeviceCMYK)
		{
			CHE_PDF_ColorSpacePtr cs = CHE_PDF_ColorSpace::CreateDeviceCMYK();
			Assert::IsTrue(cs.operator bool(), L"CreateDeviceRGB Failed!");

			PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_CMYK == type, L"ColorSpace Type Return Error!");

			uint32 c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(4, c, L"DeviceRGB ColorSpace Component Count Error!");
		}

		TEST_METHOD(Create)
		{
			CHE_PDF_ColorSpacePtr cs = CHE_PDF_ColorSpace::Create( "DeviceGray" );
			Assert::IsTrue(cs.operator bool(), L"Create with DeviceGray Failed!");

			PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_GRAY == type, L"ColorSpace Type Return Error!");

			uint32 c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(1, c, L"DeviceGray ColorSpace Component Count Error!");

			cs = CHE_PDF_ColorSpace::Create("DeviceRGB");
			Assert::IsTrue(cs.operator bool(), L"Create with DeviceRGB Failed!");

			type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_RGB == type, L"ColorSpace Type Return Error!");

			c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(3, c, L"DeviceRGB ColorSpace Component Count Error!");

			cs = CHE_PDF_ColorSpace::Create("DeviceCMYK");
			Assert::IsTrue(cs.operator bool(), L"Create with DeviceCMYK Failed!");

			type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_CMYK == type, L"ColorSpace Type Return Error!");

			c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(4, c, L"DeviceCMYK ColorSpace Component Count Error!");

			cs = CHE_PDF_ColorSpace::Create("G");
			Assert::IsTrue(cs.operator bool(), L"Create with G Failed!");

			type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_GRAY == type, L"ColorSpace Type Return Error!");

			c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(1, c, L"DeviceGray ColorSpace Component Count Error!");

			cs = CHE_PDF_ColorSpace::Create("RGB");
			Assert::IsTrue(cs.operator bool(), L"Create with RGB Failed!");

			type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_RGB == type, L"ColorSpace Type Return Error!");

			c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(3, c, L"DeviceRGB ColorSpace Component Count Error!");

			cs = CHE_PDF_ColorSpace::Create("CMYK");
			Assert::IsTrue(cs.operator bool(), L"Create with CMYK Failed!");

			type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_CMYK == type, L"ColorSpace Type Return Error!");

			c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(4, c, L"DeviceCMYK ColorSpace Component Count Error!");

			cs = CHE_PDF_ColorSpace::Create("CalCMYK");
			Assert::IsTrue(cs.operator bool(), L"Create with CalCMYK Failed!");

			type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_DEVICE_CMYK == type, L"ColorSpace Type Return Error!");

			c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(4, c, L"DeviceCMYK ColorSpace Component Count Error!");

			cs = CHE_PDF_ColorSpace::Create("Pattern");
			Assert::IsTrue(cs.operator bool(), L"Create with Pattern Failed!");

			type = cs->GetColorSpaceType();
			Assert::IsTrue(COLORSPACE_SPECIAL_PATTERN == type, L"ColorSpace Type Return Error!");

			c = cs->GetComponentCount();
			Assert::AreEqual<uint32>(0, c, L"Pattern ColorSpace Component Count Error!");
		}
	};
}



