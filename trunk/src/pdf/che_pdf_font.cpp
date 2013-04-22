#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_font.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_contentobjs.h"

FT_Library gFTLibrary = NULL;

IHE_SystemFontMgr * gpSystemFontMgr = NULL;

const HE_WCHAR	gPdfDocEncoding[256] = {
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009,
	0x000A, 0x000B,	0x000C, 0x000D,	0x000E, 0x000F, 0x0010, 0x0011, 0x0012, 0x0013,
	0x0014, 0x0015, 0x0017, 0x0017, 0x02D8, 0x02C7, 0x02C6, 0x02D9, 0x02DD, 0x02DB,
	0x02DA, 0x02DC,	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
	0x0028, 0x0029, 0x002A,	0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B,
	0x003C, 0x003D, 0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
	0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059,
	0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062, 0x0063,
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D,
	0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x0000, 0x2022, 0x2020,
	0x2021, 0x2026, 0x2014, 0x2013, 0x0192, 0x2044, 0x2039, 0x203A, 0x2212, 0x2030,
	0x201E, 0x201C, 0x201D, 0x2018, 0x2019, 0x201A, 0x2122, 0xFB01, 0xFB02, 0x0141,
	0x0152, 0x0160, 0x0178, 0x017D, 0x0131, 0x0142, 0x0153, 0x0161, 0x017E, 0x0000,
	0x20AC, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9,
	0x00AA, 0x00AB, 0x00AC, 0x0000, 0x00AE, 0x00AF, 0x00B0, 0x00B1, 0x00B2, 0x00B3,
	0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD,
	0x00BE, 0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1,
	0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB,
	0x00DC, 0x00DD, 0x00DE, 0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5,
	0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9,
	0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF };
const HE_WCHAR	gWinAnsiEncoding[256] = {	
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009,
	0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012, 0x0013,
	0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D,
	0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
	0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B,
	0x003C, 0x003D, 0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
	0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059,
	0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062, 0x0063,
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D,
	0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F, 0x20AC, 0x0000,
	0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039,
	0x0152, 0x0000, 0x017D, 0x0000, 0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022,
	0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x0000, 0x017E, 0x0178,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9,
	0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 0x00B0, 0x00B1, 0x00B2, 0x00B3,
	0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD,
	0x00BE, 0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1,
	0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB,
	0x00DC, 0x00DD, 0x00DE, 0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5,
	0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9,
	0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF };
const HE_WCHAR gMacRomanEncoding[256] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 
	0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012, 0x0013, 
	0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 
	0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 
	0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 
	0x003C, 0x003D, 0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 
	0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 
	0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062, 0x0063, 
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 
	0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F, 0x00C4, 0x00C5, 
	0x00C7, 0x00C9, 0x00D1, 0x00D6, 0x00DC, 0x00E1, 0x00E0, 0x00E2, 0x00E4, 0x00E3, 
	0x00E5, 0x00E7, 0x00E9, 0x00E8, 0x00EA, 0x00EB, 0x00ED, 0x00EC, 0x00EE, 0x00EF, 
	0x00F1, 0x00F3, 0x00F2, 0x00F4, 0x00F6, 0x00F5, 0x00FA, 0x00F9, 0x00FB, 0x00FC, 
	0x2020, 0x00B0, 0x00A2, 0x00A3, 0x00A7, 0x2022, 0x00B6, 0x00DF, 0x00AE, 0x00A9, 
	0x2122, 0x00B4, 0x00A8, 0x2260, 0x00C6, 0x00D8, 0x221E, 0x00B1, 0x2264, 0x2265, 
	0x00A5, 0x00B5, 0x2202, 0x2211, 0x220F, 0x03C0, 0x222B, 0x00AA, 0x00BA, 0x03A9, 
	0x00E6, 0x00F8, 0x00BF, 0x00A1, 0x00AC, 0x221A, 0x0192, 0x2248, 0x2206, 0x00AB, 
	0x00BB, 0x2026, 0x00A0, 0x00C0, 0x00C3, 0x00D5, 0x0152, 0x0153, 0x2013, 0x2014, 
	0x201C, 0x201D, 0x2018, 0x2019, 0x00F7, 0x25CA, 0x00FF, 0x0178, 0x2044, 0x20AC, 
	0x2039, 0x203A, 0xFB01, 0xFB02, 0x2021, 0x00B7, 0x201A, 0x201E, 0x2030, 0x00C2, 
	0x00CA, 0x00C1, 0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF, 0x00CC, 0x00D3, 0x00D4, 
	0xF8FF, 0x00D2, 0x00DA, 0x00DB, 0x00D9, 0x0131, 0x02C6, 0x02DC, 0x00AF, 0x02D8, 
	0x02D9, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7 };
const HE_WCHAR gMacExpertEncoding[256] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0020, 0xF721, 0xF6F8, 0xF7A2, 0xF724, 0xF6E4, 0xF726, 0xF7B4,
	0x207D, 0x207E, 0x2025, 0x2024, 0x002C, 0x002D, 0x002E, 0x2044, 0xF730, 0xF731,
	0xF732, 0xF733, 0xF734, 0xF735, 0xF736, 0xF737, 0xF738, 0xF739, 0x003A, 0x003B,
	0x0000, 0xF6DE, 0x0000, 0xF73F, 0x0000, 0x0000, 0x0000, 0x0000, 0xF7F0, 0x0000,
	0x0000, 0x00BC, 0x00BD, 0x00BE, 0x215B, 0x215C, 0x215D, 0x215E, 0x2153, 0x2154,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFB00, 0xFB01, 0xFB02, 0xFB03,
	0xFB04, 0x208D, 0x0000, 0x208E, 0xF6F6, 0xF6E5, 0xF760, 0xF761, 0xF762, 0xF763,
	0xF764, 0xF765, 0xF766, 0xF767, 0xF768, 0xF769, 0xF76A, 0xF76B, 0xF76C, 0xF76D,
	0xF76E, 0xF76F, 0xF770, 0xF771, 0xF772, 0xF773, 0xF774, 0xF775, 0xF776, 0xF777,
	0xF778, 0xF779, 0xF77A, 0x20A1, 0xF6DC, 0xF6DD, 0xF6FE, 0x0000, 0x0000, 0xF6E9,
	0xF6E0, 0x0000, 0x0000, 0x0000, 0x0000, 0xF7E1, 0xF7E0, 0xF7E2, 0xF7E4, 0xF7E3,
	0xF7E5, 0xF7E7, 0xF7E9, 0xF7E8, 0xF7EA, 0xF7EB, 0xF7ED, 0xF7EC, 0xF7EE, 0xF7EF,
	0xF7F1, 0xF7F3, 0xF7F2, 0xF7F4, 0xF7F6, 0xF7F5, 0xF7FA, 0xF7F9, 0xF7FB, 0xF7FC,
	0x0000, 0x2078, 0x2084, 0x2083, 0x2086, 0x2088, 0x2087, 0xF6FD, 0x0000, 0xF6DF,
	0x2082, 0x0000, 0xF7A8, 0x0000, 0xF6F5, 0xF6F0, 0x2085, 0x0000, 0xF6E1, 0xF6E7,
	0xF7FD, 0x0000, 0xF6E3, 0x0000, 0x0000, 0xF7FE, 0x0000, 0x2089, 0x2080, 0xF6FF,
	0xF7E6, 0xF7F8, 0xF7BF, 0x2081, 0xF6F9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0xF7B8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xF6FA, 0x2012, 0xF6E6,
	0x0000, 0x0000, 0x0000, 0x0000, 0xF7A1, 0x0000, 0xF7FF, 0x0000, 0x00B9, 0x00B2,
	0x00B3, 0x2074, 0x2075, 0x2076, 0x2077, 0x2079, 0x2070, 0x0000, 0xF6EC, 0xF6F1,
	0xF6F3, 0x0000, 0x0000, 0xF6ED, 0xF6F2, 0xF6EB, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0xF6EE, 0xF6FB, 0xF6F4, 0xF7AF, 0xF6EA, 0x207F, 0xF6EF, 0xF6E2, 0xF6E8,
	0xF6F7, 0xF6FC, 0x0000, 0x0000, 0x0000, 0x0000 };
const HE_WCHAR gStandardEncoding[256] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x2019,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B,
	0x003C, 0x003D, 0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
	0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059,
	0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062, 0x0063,
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D,
	0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x00A1, 0x00A2, 0x00A3, 0x2044, 0x00A5, 0x0192, 0x00A7, 0x00A4, 0x0027,
	0x201C, 0x00AB, 0x2039, 0x203A, 0xFB01, 0xFB02, 0x0000, 0x2013, 0x2020, 0x2021,
	0x00B7, 0x0000, 0x00B6, 0x2022, 0x201A, 0x201E, 0x201D, 0x00BB, 0x2026, 0x2030,
	0x0000, 0x00BF, 0x0000, 0x0060, 0x00B4, 0x02C6, 0x02DC, 0x00AF, 0x02C9, 0x02D8,
	0x02D9, 0x00A8, 0x0000, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7, 0x2014, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00C6, 0x0000, 0x00AA, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0141, 0x00D8, 0x0152, 0x00BA, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x00E6, 0x0000, 0x0000, 0x0000, 0x0131, 0x0000, 0x0000, 0x0142, 0x00F8,
	0x0153, 0x00DF, 0x0000, 0x0000, 0x0000, 0x0000 };
const HE_WCHAR gSymbolEncoding[256] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0020, 0x0021, 0x2200, 0x0023, 0x2203, 0x0025, 0x0026, 0x220B,
	0x0028, 0x0029, 0x2217, 0x002B, 0x002C, 0x2212, 0x002E, 0x002F, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B,
	0x003C, 0x003D, 0x003E, 0x003F, 0x2245, 0x0391, 0x0392, 0x03A7, 0x0394, 0x0395,
	0x03A6, 0x0393, 0x0397, 0x0399, 0x03D1, 0x039A, 0x039B, 0x039C, 0x039D, 0x039F,
	0x03A0, 0x0398, 0x03A1, 0x03A3, 0x03A4, 0x03A5, 0x03C2, 0x03A9, 0x039E, 0x03A8,
	0x0396, 0x005B, 0x2234, 0x005D, 0x22A5, 0x005F, 0xF8E5, 0x03B1, 0x03B2, 0x03C7,
	0x03B4, 0x03B5, 0x03C6, 0x03B3, 0x03B7, 0x03B9, 0x03D5, 0x03BA, 0x03BB, 0x03BC,
	0x03BD, 0x03BF, 0x03C0, 0x03B8, 0x03C1, 0x03C3, 0x03C4, 0x03C5, 0x03D6, 0x03C9,
	0x03BE, 0x03C8, 0x03B6, 0x007B, 0x007C, 0x007D, 0x223C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x20AC, 0x03D2, 0x2032, 0x2264, 0x2044, 0x221E, 0x0192, 0x2663, 0x2666, 0x2665, 
	0x2660, 0x2194, 0x2190, 0x2191, 0x2192, 0x2193, 0x00B0, 0x00B1, 0x2033, 0x2265, 
	0x00D7, 0x221D, 0x2202, 0x2022, 0x00F7, 0x2260, 0x2261, 0x2248, 0x2026, 0xF8E6, 
	0xF8E7, 0x21B5, 0x2135, 0x2111, 0x211C, 0x2118, 0x2297, 0x2295, 0x2205, 0x2229, 
	0x222A, 0x2283, 0x2287, 0x2284, 0x2282, 0x2286, 0x2208, 0x2209, 0x2220, 0x2207, 
	0xF6DA, 0xF6D9, 0xF6DB, 0x220F, 0x221A, 0x22C5, 0x00AC, 0x2227, 0x2228, 0x21D4, 
	0x21D0, 0x21D1, 0x21D2, 0x21D3, 0x25CA, 0x2329, 0xF8E8, 0xF8E9, 0xF8EA, 0x2211, 
	0xF8EB, 0xF8EC, 0xF8ED, 0xF8EE, 0xF8EF, 0xF8F0, 0xF8F1, 0xF8F2, 0xF8F3, 0xF8F4, 
	0x0000, 0x232A, 0x222B, 0x2320, 0xF8F5, 0x2321, 0xF8F6, 0xF8F7, 0xF8F8, 0xF8F9, 
	0xF8FA, 0xF8FB, 0xF8FC, 0xF8FD, 0xF8FE, 0x0000 };
const HE_WCHAR gPdfZapfDingbatsEncoding[256] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0020, 0x2701, 0x2702, 0x2703, 0x2704, 0x260E, 0x2706, 0x2707,  
	0x2708, 0x2709, 0x261B, 0x261E, 0x270C, 0x270D, 0x270E, 0x270F, 0x2710, 0x2711,
	0x2712, 0x2713, 0x2714, 0x2715, 0x2716, 0x2717, 0x2718, 0x2719, 0x271A, 0x271B,
	0x271C, 0x271D, 0x271E, 0x271F, 0x2720, 0x2721, 0x2722, 0x2723, 0x2724, 0x2725,
	0x2726, 0x2727, 0x2605, 0x2729, 0x272A, 0x272B, 0x272C, 0x272D, 0x272E, 0x272F,
	0x2730, 0x2731, 0x2732, 0x2733, 0x2734, 0x2735, 0x2736, 0x2737, 0x2738, 0x2739,
	0x273A, 0x273B, 0x273C, 0x273D, 0x273E, 0x273F, 0x2740, 0x2741, 0x2742, 0x2743,
	0x2744, 0x2745, 0x2746, 0x2747, 0x2748, 0x2749, 0x274A, 0x274B, 0x25CF, 0x274D,
	0x25A0, 0x274F, 0x2750, 0x2751, 0x2752, 0x25B2, 0x25BC, 0x25C6, 0x2756, 0x25D7,
	0x2758, 0x2759, 0x275A, 0x275B, 0x275C, 0x275D, 0x275E, 0x0000, 0xF8D7, 0xF8D8,
	0xF8D9, 0xF8DA, 0xF8DB, 0xF8DC, 0xF8DD, 0xF8DE, 0xF8DF, 0xF8E0, 0xF8E1, 0xF8E2,
	0xF8E3, 0xF8E4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x2761, 0x2762, 0x2763, 0x2764, 0x2765, 0x2766, 0x2767, 0x2663, 0x2666,
	0x2665, 0x2660, 0x2460, 0x2461, 0x2462, 0x2463, 0x2464, 0x2465, 0x2466, 0x2467,
	0x2468, 0x2469, 0x2776, 0x2777, 0x2778, 0x2779, 0x277A, 0x277B, 0x277C, 0x277D,
	0x277E, 0x277F, 0x2780, 0x2781, 0x2782, 0x2783, 0x2784, 0x2785, 0x2786, 0x2787,
	0x2788, 0x2789, 0x278A, 0x278B, 0x278C, 0x278D, 0x278E, 0x278F, 0x2790, 0x2791,
	0x2792, 0x2793, 0x2794, 0x2192, 0x2194, 0x2195, 0x2798, 0x2799, 0x279A, 0x279B,
	0x279C, 0x279D, 0x279E, 0x279F, 0x27A0, 0x27A1, 0x27A2, 0x27A3, 0x27A4, 0x27A5,
	0x27A6, 0x27A7, 0x27A8, 0x27A9, 0x27AA, 0x27AB, 0x27AC, 0x27AD, 0x27AE, 0x27AF,
	0x0000, 0x27B1, 0x27B2, 0x27B3, 0x27B4, 0x27B5, 0x27B6, 0x27B7, 0x27B8, 0x27B9,
	0x27BA, 0x27BB, 0x27BC, 0x27BD, 0x27BE, 0x0000 };


HE_BOOL HE_GetCodeFromName( CHE_ByteString & name, HE_BYTE encoding, HE_BYTE & ret )
{
	if ( encoding == FONT_ENCODING_STANDARD )
	{
		switch ( name[0] )
		{
		case 'A':
			if ( name == "A" )
			{
				ret = 0x41;
				return TRUE;
			}else if ( name == "AE" )
			{
				ret = 0xE1;
				return TRUE;
			}else if ( name == "Aacute" )
			{
				return FALSE;
			}else if ( name == "Acircumflex" )
			{
				return FALSE;
			}else if ( name == "Adieresis" )
			{
				return FALSE;
			}else if ( name == "Agrave" )
			{
				return FALSE;
			}else if ( name == "Aring" )
			{
				return FALSE;
			}else if ( name == "Atilde" )
			{
				return FALSE;
			}
			break;
		case 'B':
			ret = 0x42;
			return TRUE;
			break;
		case 'C':
			if ( name == "C" )
			{
				ret = 0x43;
				return TRUE;
			}else if ( name == "Ccedilla" )
			{
				return FALSE;
			}
			break;
		case 'D':
			ret = 0x44;
			return TRUE;
			break;
		case 'E':
			if ( name == "E" )
			{
				ret = 0x45;
				return TRUE;
			}else if ( name == "Eacute" )
			{
				return FALSE;
			}else if ( name == "Ecircumflex" )
			{
				return FALSE;
			}else if ( name == "Edieresis" )
			{
				return FALSE;
			}else if ( name == "Egrave" )
			{
				return FALSE;
			}else if ( name == "Eth" )
			{
				return FALSE;
			}else if ( name == "Euro" )
			{
				return FALSE;
			}
			break;
		case 'F':
			ret = 0x46;
			return TRUE;
			break;
		case 'G':
			ret = 0x47;
			return TRUE;
			break;
		case 'H':
			ret = 0x48;
			return TRUE;
			break;
		case 'I':
			if ( name == "I" )
			{
				ret = 0x49;
				return TRUE;
			}else if ( name == "Iacute" )
			{
				return FALSE;
			}else if ( name == "Icircumflex" )
			{
				return FALSE;
			}else if ( name == "Idieresis" )
			{
				return FALSE;
			}else if ( name == "Igrave" )
			{
				return FALSE;
			}
			break;
		case 'J':
			ret = 0x4A;
			return TRUE;
			break;
		case 'K':
			ret = 0x4B;
			return TRUE;
			break;
		case 'L':
			if ( name == "L" )
			{
				ret = 0x4C;
				return TRUE;
			}else if ( name == "Lslash" )
			{
				ret = 0xE8;
				return TRUE;
			}
			break;
		case 'M':
			ret = 0x4D;
			return TRUE;
			break;
		case 'N':
			if ( name == "N" )
			{
				ret = 0x4E;
				return TRUE;
			}else if ( name == "Ntilde" )
			{
				return FALSE;
			}
			break;
		case 'O':
			if ( name == "O" )
			{
				ret = 0x4F;
				return TRUE;
			}else if ( name == "OE" )
			{
				ret = 0xEA;
				return TRUE;
			}else if ( name == "Oacute" )
			{
				return FALSE;
			}else if ( name == "Ocircumflex" )
			{
				return FALSE;
			}else if ( name == "Odieresis" )
			{
				return FALSE;
			}else if ( name == "Ograve" )
			{
				return FALSE;
			}else if ( name == "Oslash" )
			{
				ret = 0xE9;
				return FALSE;
			}else if ( name == "Otilde" )
			{
				return FALSE;
			}
			break;
		case 'P':
			ret = 0x50;
			return TRUE;
			break;
		case 'Q':
			ret = 0x51;
			return TRUE;
			break;
		case 'R':
			ret = 0x52;
			return TRUE;
			break;
		case 'S':
			if ( name == "S" )
			{
				ret = 0x53;
				return TRUE;
			}else if ( name == "Scaron" )
			{
				return FALSE;
			}
			break;
		case 'T':
			if ( name == "T" )
			{
				ret = 0x54;
				return TRUE;
			}else if ( name == "Thorn" )
			{
				return FALSE;
			}
			break;
		case 'U':
			if ( name == "U" )
			{
				ret = 0x55;
				return TRUE;
			}else if ( name == "Uacute" )
			{
				return FALSE;
			}else if ( name == "Ucircumflex" )
			{
				return FALSE;
			}else if ( name == "Udieresis" )
			{
				return FALSE;
			}else if ( name == "Ugrave" )
			{
				return FALSE;
			}
			break;
		case 'V':
			ret = 0x56;
			return TRUE;
			break;
		case 'W':
			ret = 0x57;
			return TRUE;
			break;
		case 'X':
			ret = 0x58;
			return TRUE;
			break;
		case 'Y':
			if ( name == "Y" )
			{
				ret = 0x59;
				return TRUE;
			}else if ( name == "Yacute" )
			{
				return FALSE;
			}else if ( name == "Ydieresis" )
			{
				return FALSE;
			}
			break;
		case 'Z':
			if ( name == "Z" )
			{
				ret = 0x5A;
			}else if ( name == "Zcaron" )
			{
				return FALSE;
			}
			break;
		case 'a':
			if ( name == "a" )
			{
				ret = 0x61;
				return TRUE;
			}else if ( name == "aacute" )
			{
				return FALSE;
			}else if ( name == "acircumflex" )
			{
				return FALSE;
			}else if ( name == "acute" )
			{
				ret = 0xC2;
				return TRUE;
			}else if ( name == "adieresis" )
			{
				return FALSE;
			}else if ( name == "ae" )
			{
				ret = 0xF1;
				return TRUE;
			}else if ( name == "agrave" )
			{
				return FALSE;
			}else if ( name == "ampersand" )
			{
				ret = 0x26;
				return TRUE;
			}else if ( name == "aring" )
			{
				return FALSE;
			}else if ( name == "asciicircum" )
			{
				ret = 0x5E;
				return TRUE;
			}else if ( name == "asciitidle" )
			{
				ret = 0x7E;
				return TRUE;
			}else if ( name == "asterisk" )
			{
				ret = 0x2A;
				return TRUE;
			}else if ( name == "at" )
			{
				ret = 0x40;
				return TRUE;
			}else if ( name == "atilde" )
			{
				return FALSE;
			}
			break;
		case 'b':
			if ( name == "b" )
			{
				ret = 0x62;
				return TRUE;
			}else if ( name == "backslash" )
			{
				ret = 0x5C;
				return TRUE;
			}else if ( name == "bar" )
			{
				ret = 0x7C;
				return TRUE;
			}else if ( name == "braceleft" )
			{
				ret = 0x7B;
				return TRUE;
			}else if ( name == "braceright" )
			{
				ret = 0x7D;
				return TRUE;
			}else if ( name == "bracketleft" )
			{
				ret = 0x5B;
				return TRUE;
			}else if ( name == "bracketright" )
			{
				ret = 0x5D;
				return TRUE;
			}else if ( name == "breve" )
			{
				ret = 0xC6;
				return TRUE;
			}else if ( name == "brokenbar" )
			{
				return FALSE;
			}else if ( name == "bullet" )
			{
				ret = 0xB7;
				return TRUE;
			}
			break;
		case 'c':
			if ( name == "c" )
			{
				ret = 0x63;
				return TRUE;
			}else if ( name == "caron" )
			{
				ret = 0xCF;
				return TRUE;
			}else if ( name == "ccedilla" )
			{
				return FALSE;
			}else if ( name == "cedilla" )
			{
				ret = 0xCB;
				return TRUE;
			}else if ( name == "cent" )
			{
				ret = 0xA2;
				return TRUE;
			}else if ( name == "circumflex" )
			{
				ret = 0xC3;
				return TRUE;
			}else if ( name == "colon" )
			{
				ret = 0x3A;
				return TRUE;
			}else if ( name == "comma" )
			{
				ret = 0x2C;
				return TRUE;
			}else if ( name == "copyright" )
			{
				return FALSE;
			}else if ( name == "currency" )
			{
				ret = 0xA8;
				return TRUE;
			}
			break;
		case 'd':
			if ( name == "d" )
			{
				ret = 0x64;
				return TRUE;
			}else if ( name == "dagger" )
			{
				ret = 0xB2;
				return TRUE;
			}else if ( name == "daggerdbl" )
			{
				ret = 0xB3;
				return TRUE;
			}else if ( name == "degree" )
			{
				return FALSE;
			}else if ( name == "dieresis" )
			{
				ret = 0xC8;
				return TRUE;
			}else if ( name == "divide" )
			{
				return FALSE;
			}else if ( name == "dollar" )
			{
				ret = 0x24;
				return TRUE;
			}else if ( name == "dotaccent" )
			{
				ret = 0xC7;
				return TRUE;
			}else if ( name == "dotlessi" )
			{
				ret = 0xF5;
				return TRUE;
			}
			break;
		case 'e':
			if ( name == "e" )
			{
				ret = 0x65;
				return TRUE;
			}else if ( name == "eacute" )
			{
				return FALSE;
			}else if ( name == "ecircumflex" )
			{
				return FALSE;
			}else if ( name == "edieresis" )
			{
				return FALSE;
			}else if ( name == "egrave" )
			{
				return FALSE;
			}else if ( name == "eight" )
			{
				ret = 0x38;
				return TRUE;
			}else if ( name == "ellipsis" )
			{
				ret = 0xBC;
				return TRUE;
			}else if ( name == "emdash" )
			{
				ret = 0xD0;
				return TRUE;
			}else if ( name == "endash" )
			{
				ret = 0xB1;
				return TRUE;
			}else if ( name == "equal" )
			{
				ret = 0x3D;
				return TRUE;
			}else if ( name == "eth" )
			{
				return FALSE;
			}else if ( name == "exclam" )
			{
				ret = 0x21;
				return TRUE;
			}else if ( name == "exclamdown" )
			{
				ret = 0xA1;
				return TRUE;
			}
			break;
		case 'f':
			if ( name == "f" )
			{
				ret = 0x66;
				return TRUE;
			}else if ( name == "fi" )
			{
				ret = 0xAE;
				return TRUE;
			}else if ( name == "five" )
			{
				ret = 0x35;
				return TRUE;
			}else if ( name == "fl" )
			{
				ret = 0xAF;
				return TRUE;
			}else if ( name == "florin" )
			{
				ret = 0xA6;
				return TRUE;
			}else if ( name == "four" )
			{
				ret = 0x34;
				return TRUE;
			}else if ( name == "fraction" )
			{
				ret = 0xA4;
				return TRUE;
			}
			break;
		case 'g':
			if ( name == "g" )
			{
				ret = 0x67;
				return TRUE;
			}else if ( name == "germandbls" )
			{
				ret = 0xFB;
				return TRUE;
			}else if ( name == "grave" )
			{
				ret = 0xC1;
				return TRUE;
			}else if ( name == "greater" )
			{
				ret = 0x3E;
				return TRUE;
			}else if ( name == "guillemotleft" )
			{
				ret = 0xAB;
				return TRUE;
			}else if ( name == "guillemotright" )
			{
				ret = 0xBB;
				return TRUE;
			}else if ( name == "guilsinglleft" )
			{
				ret = 0xAC;
				return TRUE;
			}else if ( name == "guilsinglright" )
			{
				ret = 0xAD;
				return TRUE;
			}
			break;
		case 'h':
			if ( name == "h" )
			{
				ret = 0x68;
				return TRUE;
			}else if ( name == "hungarumlaut" )
			{
				ret = 0xCD;
				return TRUE;
			}else if ( name == "hyphen" )
			{
				ret = 0x2D;
				return TRUE;
			}
			break;
		case 'i':
			if ( name == "i" )
			{
				ret = 0x69;
				return TRUE;
			}else if ( name == "iacute" )
			{
				return FALSE;
			}else if ( name == "icircumflex" )
			{
				return FALSE;
			}else if ( name == "idieresis" )
			{
				return FALSE;
			}else if ( name == "igrave" )
			{
				return FALSE;
			}
			break;
		case 'j':
			ret = 0x6A;
			return TRUE;
			break;
		case 'k':
			ret = 0x6B;
			return TRUE;
			break;
		case 'l':
			if ( name == "l" )
			{
				ret = 0x6C;
				return TRUE;
			}else if ( name == "less" )
			{
				ret = 0x3C;
				return TRUE;
			}else if ( name == "logicalnot" )
			{
				return FALSE;
			}else if ( name == "lslash" )
			{
				ret = 0xF8;
				return TRUE;
			}
			break;
		case 'm':
			if ( name == "m" )
			{
				ret = 0x6D;
				return TRUE;
			}else if ( name == "macron" )
			{
				ret = 0xC5;
				return TRUE;
			}else if ( name == "minus" )
			{
				return FALSE;
			}else if ( name == "mu" )
			{
				return FALSE;
			}else if ( name == "multiply" )
			{
				return FALSE;
			}
			break;
		case 'n':
			if ( name == "n" )
			{
				ret = 0x6E;
				return TRUE;
			}else if ( name == "nine" )
			{
				ret = 0x39;
				return TRUE;
			}else if ( name == "ntilde" )
			{
				return FALSE;
			}else if ( name == "numbersign" )
			{
				ret = 0x23;
				return TRUE;
			}
			break;
		case 'o':
			if ( name == "o" )
			{
				ret = 0x6F;
				return TRUE;
			}else if ( name == "oacute" )
			{
				return FALSE;
			}else if ( name == "ocircumflex" )
			{
				return FALSE;
			}else if ( name == "odieresis" )
			{
				return FALSE;
			}else if ( name == "oe" )
			{
				ret = 0xFA;
				return TRUE;
			}else if ( name == "ogonek" )
			{
				ret = 0xCE;
				return TRUE;
			}else if ( name == "ograve" )
			{
				return FALSE;
			}else if ( name == "one" )
			{
				ret = 0x31;
				return TRUE;
			}else if ( name == "onehalf" )
			{
				return FALSE;
			}else if ( name == "onequarter" )
			{
				return FALSE;
			}else if ( name == "onesuperior" )
			{
				return FALSE;
			}else if ( name == "ordfeminine" )
			{
				ret = 0xE3;
				return TRUE;
			}else if ( name == "ordmasculine" )
			{
				ret = 0xEB;
				return TRUE;
			}else if ( name == "oslash" )
			{
				ret = 0xF9;
				return TRUE;
			}else if ( name == "otilde" )
			{
				return FALSE;
			}
			break;
		case 'p':
			if ( name == "p" )
			{
				ret = 0x70;
				return TRUE;
			}else if ( name == "paragraph" )
			{
				ret = 0xB6;
				return TRUE;
			}else if ( name == "parenleft" )
			{
				ret = 0x28;
				return TRUE;
			}else if ( name == "parenright" )
			{
				ret = 0x29;
				return TRUE;
			}else if ( name == "percent" )
			{
				ret = 0x25;
				return TRUE;
			}else if ( name == "period" )
			{
				ret = 0x2E;
				return TRUE;
			}else if ( name == "periodcentered" )
			{
				ret = 0xB4;
				return TRUE;
			}else if ( name == "perthousand" )
			{
				ret = 0xBD;
				return TRUE;
			}else if ( name == "plus" )
			{
				ret = 0x2B;
				return TRUE;
			}else if ( name == "plusminus" )
			{
				return FALSE;
			}
			break;
		case 'q':
			if ( name == "q" )
			{
				ret = 0x71;
				return TRUE;
			}else if ( name == "question" )
			{
				ret = 0x3F;
				return TRUE;
			}else if ( name == "questiondown" )
			{
				ret = 0xBF;
				return TRUE;
			}else if ( name == "quotedbl" )
			{
				ret = 0x22;
				return TRUE;
			}else if ( name == "quotedblbase" )
			{
				ret = 0xB9;
				return TRUE;
			}else if ( name == "quotedblleft" )
			{
				ret = 0xAA;
				return TRUE;
			}else if ( name == "quotedblright" )
			{
				ret = 0xBA;
				return TRUE;
			}else if ( name == "quoteleft" )
			{
				ret = 0x60;
				return TRUE;
			}
			else if ( name == "quoteright" )
			{
				ret = 0x27;
				return TRUE;
			}else if ( name == "quotesinglbase" )
			{
				ret = 0xB8;
				return TRUE;
			}else if ( name == "quotesingle" )
			{
				ret = 0xA9;
				return TRUE;
			}
			break;
		case 'r':
			if ( name == "r" )
			{
				ret = 0x72;
				return TRUE;
			}else if ( name == "registered" )
			{
				return FALSE;
			}else if ( name == "ring" )
			{
				ret = 0xCA;
				return TRUE;
			}
			break;
		case 's':
			if ( name == "s" )
			{
				ret = 0x73;
				return TRUE;
			}else if ( name == "scaron" )
			{
				return FALSE;
			}else if ( name == "section" )
			{
				ret = 0xA7;
				return TRUE;
			}else if ( name == "semicolon" )
			{
				ret = 0x3B;
				return TRUE;
			}else if ( name == "seven" )
			{
				ret = 0x37;
				return TRUE;
			}else if ( name == "six" )
			{
				ret = 0x36;
				return TRUE;
			}else if ( name == "slash" )
			{
				ret = 0x2F;
				return TRUE;
			}else if ( name == "space" )
			{
				ret = 0x20;
				return TRUE;
			}
			else if ( name == "sterling" )
			{
				ret = 0xA3;
				return TRUE;
			}
			break;
		case 't':
			if ( name == "t" )
			{
				ret = 0x74;
				return TRUE;
			}else if ( name == "thorn" )
			{
				return FALSE;
			}else if ( name == "three" )
			{
				ret = 0x33;
				return TRUE;
			}else if ( name == "threequarters" )
			{
				return FALSE;
			}else if ( name == "threesuperior" )
			{
				return FALSE;
			}else if ( name == "tilde" )
			{
				ret = 0xC4;
				return TRUE;
			}else if ( name == "trademark" )
			{
				return FALSE;
			}else if ( name == "two" )
			{
				ret = 0x32;
				return TRUE;
			}
			else if ( name == "twosuperior" )
			{
				return FALSE;
			}
			break;
		case 'u':
			if ( name == "u" )
			{
				ret = 0x75;
				return TRUE;
			}else if ( name == "uacute" )
			{
				return FALSE;
			}else if ( name == "ucircumflex" )
			{
				return FALSE;
			}else if ( name == "udieresis" )
			{
				return FALSE;
			}else if ( name == "ugrave" )
			{
				return FALSE;
			}else if ( name == "underscore" )
			{
				ret = 0x5F;
				return TRUE;
			}
			break;
		case 'v':
			ret = 0x76;
			return TRUE;
			break;
		case 'w':
			ret = 0x77;
			return TRUE;
			break;
		case 'x':
			ret = 0x78;
			return TRUE;
			break;
		case 'y':
			if ( name == "y" )
			{
				ret = 0x79;
				return TRUE;
			}else if ( name == "yacute" )
			{
				return FALSE;
			}else if ( name == "ydieresis" )
			{
				return FALSE;
			}else if ( name == "yen" )
			{
				ret = 0xA5;
				return TRUE;
			}
			break;
		case 'z':
			if ( name == "z" )
			{
				ret = 0x7A;
				return TRUE;
			}else if ( name == "zcaron" )
			{
				return FALSE;
			}else if ( name == "zero" )
			{
				ret = 0x30;
				return TRUE;
			}
			break;
		}
	}else if ( encoding == FONT_ENCODING_WINANSI )
	{
		switch ( name[0] )
		{
		case 'A':
			if ( name == "A" )
			{
				ret = 0x41;
				return TRUE;
			}else if ( name == "AE" )
			{
				ret = 0xC6;
				return TRUE;
			}else if ( name == "Aacute" )
			{
				ret = 0xC1;
				return TRUE;
			}else if ( name == "Acircumflex" )
			{
				ret = 0xC2;
				return TRUE;
			}else if ( name == "Adieresis" )
			{
				ret = 0xC4;
				return TRUE;
			}else if ( name == "Agrave" )
			{
				ret = 0xC0;
				return TRUE;
			}else if ( name == "Aring" )
			{
				ret = 0xC5;
				return TRUE;
			}else if ( name == "Atilde" )
			{
				ret = 0xC3;
				return TRUE;
			}
			break;
		case 'B':
			ret = 0x42;
			return TRUE;
			break;
		case 'C':
			if ( name == "C" )
			{
				ret = 0x43;
				return TRUE;
			}else if ( name == "Ccedilla" )
			{
				ret = 0xC7;
				return TRUE;
			}
			break;
		case 'D':
			ret = 0x44;
			return TRUE;
			break;
		case 'E':
			if ( name == "E" )
			{
				ret = 0x45;
				return TRUE;
			}else if ( name == "Eacute" )
			{
				ret = 0xC9;
				return TRUE;
			}else if ( name == "Ecircumflex" )
			{
				ret = 0xCA;
				return TRUE;
			}else if ( name == "Edieresis" )
			{
				ret = 0xCB;
				return TRUE;
			}else if ( name == "Egrave" )
			{
				ret = 0xC8;
				return TRUE;
			}else if ( name == "Eth" )
			{
				ret = 0xD0;
				return TRUE;
			}else if ( name == "Euro" )
			{
				ret = 0x80;
				return TRUE;
			}
			break;
		case 'F':
			ret = 0x46;
			return TRUE;
			break;
		case 'G':
			ret = 0x47;
			return TRUE;
			break;
		case 'H':
			ret = 0x48;
			return TRUE;
			break;
		case 'I':
			if ( name == "I" )
			{
				ret = 0x49;
				return TRUE;
			}else if ( name == "Iacute" )
			{
				ret = 0xCD;
				return TRUE;
			}else if ( name == "Icircumflex" )
			{
				ret = 0xCE;
				return TRUE;
			}else if ( name == "Idieresis" )
			{
				ret = 0xCF;
				return TRUE;
			}else if ( name == "Igrave" )
			{
				ret = 0xCC;
				return TRUE;
			}
			break;
		case 'J':
			ret = 0x4A;
			return TRUE;
			break;
		case 'K':
			ret = 0x4B;
			return TRUE;
			break;
		case 'L':
			if ( name == "L" )
			{
				ret = 0x4C;
				return TRUE;
			}else if ( name == "Lslash" )
			{
				return FALSE;
			}
			break;
		case 'M':
			ret = 0x4D;
			return TRUE;
			break;
		case 'N':
			if ( name == "N" )
			{
				ret = 0x4E;
				return TRUE;
			}else if ( name == "Ntilde" )
			{
				ret = 0xD1;
				return TRUE;
			}
			break;
		case 'O':
			if ( name == "O" )
			{
				ret = 0x4F;
				return TRUE;
			}else if ( name == "OE" )
			{
				ret = 0x8C;
				return TRUE;
			}else if ( name == "Oacute" )
			{
				ret = 0xD3;
				return TRUE;
			}else if ( name == "Ocircumflex" )
			{
				ret = 0xD4;
				return TRUE;
			}else if ( name == "Odieresis" )
			{
				ret = 0xD6;
				return TRUE;
			}else if ( name == "Ograve" )
			{
				ret = 0xD2;
				return TRUE;
			}else if ( name == "Oslash" )
			{
				ret = 0xD8;
				return TRUE;
			}else if ( name == "Otilde" )
			{
				ret = 0xD5;
				return TRUE;
			}
			break;
		case 'P':
			ret = 0x50;
			return TRUE;
			break;
		case 'Q':
			ret = 0x51;
			return TRUE;
			break;
		case 'R':
			ret = 0x52;
			return TRUE;
			break;
		case 'S':
			if ( name == "S" )
			{
				ret = 0x53;
				return TRUE;
			}else if ( name == "Scaron" )
			{
				ret = 0x8A;
				return TRUE;
			}
			break;
		case 'T':
			if ( name == "T" )
			{
				ret = 0x54;
				return TRUE;
			}else if ( name == "Thorn" )
			{
				ret = 0xDE;
				return TRUE;
			}
			break;
		case 'U':
			if ( name == "U" )
			{
				ret = 0x55;
				return TRUE;
			}else if ( name == "Uacute" )
			{
				ret = 0xDA;
				return TRUE;
			}else if ( name == "Ucircumflex" )
			{
				ret = 0xDB;
				return TRUE;
			}else if ( name == "Udieresis" )
			{
				ret = 0xDC;
				return TRUE;
			}else if ( name == "Ugrave" )
			{
				ret = 0xD9;
				return TRUE;
			}
			break;
		case 'V':
			ret = 0x56;
			return TRUE;
			break;
		case 'W':
			ret = 0x57;
			return TRUE;
			break;
		case 'X':
			ret = 0x58;
			return TRUE;
			break;
		case 'Y':
			if ( name == "Y" )
			{
				ret = 0x59;
				return TRUE;
			}else if ( name == "Yacute" )
			{
				ret = 0xDD;
				return TRUE;
			}else if ( name == "Ydieresis" )
			{
				ret = 0x9F;
				return TRUE;
			}
			break;
		case 'Z':
			if ( name == "Z" )
			{
				ret = 0x5A;
			}else if ( name == "Zcaron" )
			{
				ret = 0x8E;
				return TRUE;
			}
			break;
		case 'a':
			if ( name == "a" )
			{
				ret = 0x61;
				return TRUE;
			}else if ( name == "aacute" )
			{
				ret = 0xE1;
				return TRUE;
			}else if ( name == "acircumflex" )
			{
				ret = 0xE2;
				return TRUE;
			}else if ( name == "acute" )
			{
				ret = 0xB4;
				return TRUE;
			}else if ( name == "adieresis" )
			{
				ret = 0xE4;
				return TRUE;
			}else if ( name == "ae" )
			{
				ret = 0xE6;
				return TRUE;
			}else if ( name == "agrave" )
			{
				ret = 0xE0;
				return TRUE;
			}else if ( name == "ampersand" )
			{
				ret = 0x26;
				return TRUE;
			}else if ( name == "aring" )
			{
				ret = 0xE5;
				return TRUE;
			}else if ( name == "asciicircum" )
			{
				ret = 0x5E;
				return TRUE;
			}else if ( name == "asciitidle" )
			{
				ret = 0x7E;
				return TRUE;
			}else if ( name == "asterisk" )
			{
				ret = 0x2A;
				return TRUE;
			}else if ( name == "at" )
			{
				ret = 0x40;
				return TRUE;
			}else if ( name == "atilde" )
			{
				ret = 0xE3;
				return TRUE;
			}
			break;
		case 'b':
			if ( name == "b" )
			{
				ret = 0x62;
				return TRUE;
			}else if ( name == "backslash" )
			{
				ret = 0x5C;
				return TRUE;
			}else if ( name == "bar" )
			{
				ret = 0x7C;
				return TRUE;
			}else if ( name == "braceleft" )
			{
				ret = 0x7B;
				return TRUE;
			}else if ( name == "braceright" )
			{
				ret = 0x7D;
				return TRUE;
			}else if ( name == "bracketleft" )
			{
				ret = 0x5B;
				return TRUE;
			}else if ( name == "bracketright" )
			{
				ret = 0x5D;
				return TRUE;
			}else if ( name == "breve" )
			{
				return FALSE;
			}else if ( name == "brokenbar" )
			{
				ret = 0xA6;
				return TRUE;
			}else if ( name == "bullet" )
			{
				ret = 0x95;
				return TRUE;
			}
			break;
		case 'c':
			if ( name == "c" )
			{
				ret = 0x63;
				return TRUE;
			}else if ( name == "caron" )
			{
				return FALSE;
			}else if ( name == "ccedilla" )
			{
				ret = 0xE7;
				return TRUE;
			}else if ( name == "cedilla" )
			{
				ret = 0xB8;
				return TRUE;
			}else if ( name == "cent" )
			{
				ret = 0xA2;
				return TRUE;
			}else if ( name == "circumflex" )
			{
				ret = 0x88;
				return TRUE;
			}else if ( name == "colon" )
			{
				ret = 0x3A;
				return TRUE;
			}else if ( name == "comma" )
			{
				ret = 0x2C;
				return TRUE;
			}else if ( name == "copyright" )
			{
				ret = 0xA9;
				return TRUE;
			}else if ( name == "currency" )
			{
				ret = 0xA4;
				return TRUE;
			}
			break;
		case 'd':
			if ( name == "d" )
			{
				ret = 0x64;
				return TRUE;
			}else if ( name == "dagger" )
			{
				ret = 0x86;
				return TRUE;
			}else if ( name == "daggerdbl" )
			{
				ret = 0x87;
				return TRUE;
			}else if ( name == "degree" )
			{
				ret = 0xB0;
				return TRUE;
			}else if ( name == "dieresis" )
			{
				ret = 0xA8;
				return TRUE;
			}else if ( name == "divide" )
			{
				ret = 0xF7;
				return TRUE;
			}else if ( name == "dollar" )
			{
				ret = 0x24;
				return TRUE;
			}else if ( name == "dotaccent" )
			{
				return FALSE;
			}else if ( name == "dotlessi" )
			{
				return FALSE;
			}
			break;
		case 'e':
			if ( name == "e" )
			{
				ret = 0x65;
				return TRUE;
			}else if ( name == "eacute" )
			{
				ret = 0xE9;
				return TRUE;
			}else if ( name == "ecircumflex" )
			{
				ret = 0xEA;
				return TRUE;
			}else if ( name == "edieresis" )
			{
				ret = 0xEB;
				return TRUE;
			}else if ( name == "egrave" )
			{
				ret = 0xE8;
				return TRUE;
			}else if ( name == "eight" )
			{
				ret = 0x38;
				return TRUE;
			}else if ( name == "ellipsis" )
			{
				ret = 0x85;
				return TRUE;
			}else if ( name == "emdash" )
			{
				ret = 0x97;
				return TRUE;
			}else if ( name == "endash" )
			{
				ret = 0x96;
				return TRUE;
			}else if ( name == "equal" )
			{
				ret = 0x3D;
				return TRUE;
			}else if ( name == "eth" )
			{
				ret = 0xF0;
				return TRUE;
			}else if ( name == "exclam" )
			{
				ret = 0x21;
				return TRUE;
			}else if ( name == "exclamdown" )
			{
				ret = 0xA1;
				return TRUE;
			}
			break;
		case 'f':
			if ( name == "f" )
			{
				ret = 0x66;
				return TRUE;
			}else if ( name == "fi" )
			{
				return FALSE;
			}else if ( name == "five" )
			{
				ret = 0x35;
				return TRUE;
			}else if ( name == "fl" )
			{
				return FALSE;
			}else if ( name == "florin" )
			{
				ret = 0x83;
				return TRUE;
			}else if ( name == "four" )
			{
				ret = 0x34;
				return TRUE;
			}else if ( name == "fraction" )
			{
				return FALSE;
			}
			break;
		case 'g':
			if ( name == "g" )
			{
				ret = 0x67;
				return TRUE;
			}else if ( name == "germandbls" )
			{
				ret = 0xDF;
				return TRUE;
			}else if ( name == "grave" )
			{
				ret = 0x60;
				return TRUE;
			}else if ( name == "greater" )
			{
				ret = 0x3E;
				return TRUE;
			}else if ( name == "guillemotleft" )
			{
				ret = 0xAB;
				return TRUE;
			}else if ( name == "guillemotright" )
			{
				ret = 0xBB;
				return TRUE;
			}else if ( name == "guilsinglleft" )
			{
				ret = 0x8B;
				return TRUE;
			}else if ( name == "guilsinglright" )
			{
				ret = 0x9B;
				return TRUE;
			}
			break;
		case 'h':
			if ( name == "h" )
			{
				ret = 0x68;
				return TRUE;
			}else if ( name == "hungarumlaut" )
			{
				return FALSE;
			}else if ( name == "hyphen" )
			{
				ret = 0x2D;
				return TRUE;
			}
			break;
		case 'i':
			if ( name == "i" )
			{
				ret = 0x69;
				return TRUE;
			}else if ( name == "iacute" )
			{
				ret = 0xED;
				return TRUE;
			}else if ( name == "icircumflex" )
			{
				ret = 0xEE;
				return TRUE;
			}else if ( name == "idieresis" )
			{
				ret = 0xEF;
				return TRUE;
			}else if ( name == "igrave" )
			{
				ret = 0xEC;
				return TRUE;
			}
			break;
		case 'j':
			ret = 0x6A;
			return TRUE;
			break;
		case 'k':
			ret = 0x6B;
			return TRUE;
			break;
		case 'l':
			if ( name == "l" )
			{
				ret = 0x6C;
				return TRUE;
			}else if ( name == "less" )
			{
				ret = 0x3C;
				return TRUE;
			}else if ( name == "logicalnot" )
			{
				ret = 0xAC;
				return TRUE;
			}else if ( name == "lslash" )
			{
				return FALSE;
			}
			break;
		case 'm':
			if ( name == "m" )
			{
				ret = 0x6D;
				return FALSE;
			}else if ( name == "macron" )
			{
				ret = 0xAF;
				return TRUE;
			}else if ( name == "minus" )
			{
				return FALSE;
			}else if ( name == "mu" )
			{
				ret = 0xB5;
				return TRUE;
			}else if ( name == "multiply" )
			{
				ret = 0xD7;
				return TRUE;
			}
			break;
		case 'n':
			if ( name == "n" )
			{
				ret = 0x6E;
				return TRUE;
			}else if ( name == "nine" )
			{
				ret = 0x39;
				return TRUE;
			}else if ( name == "ntilde" )
			{
				ret = 0xF1;
				return TRUE;
			}else if ( name == "numbersign" )
			{
				ret = 0x23;
				return TRUE;
			}
			break;
		case 'o':
			if ( name == "o" )
			{
				ret = 0x6F;
				return TRUE;
			}else if ( name == "oacute" )
			{
				ret = 0xF3;
				return TRUE;
			}else if ( name == "ocircumflex" )
			{
				ret = 0xF4;
				return TRUE;
			}else if ( name == "odieresis" )
			{
				ret = 0xF6;
				return TRUE;
			}else if ( name == "oe" )
			{
				ret = 0x9C;
				return TRUE;
			}else if ( name == "ogonek" )
			{
				return FALSE;
			}else if ( name == "ograve" )
			{
				ret = 0xF2;
				return TRUE;
			}else if ( name == "one" )
			{
				ret = 0x31;
				return TRUE;
			}else if ( name == "onehalf" )
			{
				ret = 0xBD;
				return TRUE;
			}else if ( name == "onequarter" )
			{
				ret = 0xBC;
				return TRUE;
			}else if ( name == "onesuperior" )
			{
				ret = 0xB9;
				return TRUE;
			}else if ( name == "ordfeminine" )
			{
				ret = 0xAA;
				return TRUE;
			}else if ( name == "ordmasculine" )
			{
				ret = 0xBA;
				return TRUE;
			}else if ( name == "oslash" )
			{
				ret = 0xF8;
				return TRUE;
			}else if ( name == "otilde" )
			{
				ret = 0xF6;
				return TRUE;
			}
			break;
		case 'p':
			if ( name == "p" )
			{
				ret = 0x70;
				return TRUE;
			}else if ( name == "paragraph" )
			{
				ret = 0xB6;
				return TRUE;
			}else if ( name == "parenleft" )
			{
				ret = 0x28;
				return TRUE;
			}else if ( name == "parenright" )
			{
				ret = 0x29;
				return TRUE;
			}else if ( name == "percent" )
			{
				ret = 0x25;
				return TRUE;
			}else if ( name == "period" )
			{
				ret = 0x2E;
				return TRUE;
			}else if ( name == "periodcentered" )
			{
				ret = 0xB7;
				return TRUE;
			}else if ( name == "perthousand" )
			{
				ret = 0x89;
				return TRUE;
			}else if ( name == "plus" )
			{
				ret = 0x2B;
				return TRUE;
			}else if ( name == "plusminus" )
			{
				ret = 0xB1;
				return TRUE;
			}
			break;
		case 'q':
			if ( name == "q" )
			{
				ret = 0x71;
				return TRUE;
			}else if ( name == "question" )
			{
				ret = 0x3F;
				return TRUE;
			}else if ( name == "questiondown" )
			{
				ret = 0xBF;
				return TRUE;
			}else if ( name == "quotedbl" )
			{
				ret = 0x22;
				return TRUE;
			}else if ( name == "quotedblbase" )
			{
				ret = 0x84;
				return TRUE;
			}else if ( name == "quotedblleft" )
			{
				ret = 0x93;
				return TRUE;
			}else if ( name == "quotedblright" )
			{
				ret = 0x94;
				return TRUE;
			}else if ( name == "quoteleft" )
			{
				ret = 0x91;
				return TRUE;
			}
			else if ( name == "quoteright" )
			{
				ret = 0x92;
				return TRUE;
			}else if ( name == "quotesinglbase" )
			{
				ret = 0x82;
				return TRUE;
			}else if ( name == "quotesingle" )
			{
				ret = 0x27;
				return TRUE;
			}
			break;
		case 'r':
			if ( name == "r" )
			{
				ret = 0x72;
				return TRUE;
			}else if ( name == "registered" )
			{
				ret = 0xAE;
				return TRUE;
			}else if ( name == "ring" )
			{
				return FALSE;
			}
			break;
		case 's':
			if ( name == "s" )
			{
				ret = 0x73;
				return TRUE;
			}else if ( name == "scaron" )
			{
				ret = 0x9A;
				return TRUE;
			}else if ( name == "section" )
			{
				ret = 0xA7;
				return TRUE;
			}else if ( name == "semicolon" )
			{
				ret = 0x3B;
				return TRUE;
			}else if ( name == "seven" )
			{
				ret = 0x37;
				return TRUE;
			}else if ( name == "six" )
			{
				ret = 0x36;
				return TRUE;
			}else if ( name == "slash" )
			{
				ret = 0x2F;
				return TRUE;
			}else if ( name == "space" )
			{
				ret = 0x20;
				return TRUE;
			}
			else if ( name == "sterling" )
			{
				ret = 0xA3;
				return TRUE;
			}
			break;
		case 't':
			if ( name == "t" )
			{
				ret = 0x74;
				return TRUE;
			}else if ( name == "thorn" )
			{
				ret = 0xFE;
				return TRUE;
			}else if ( name == "three" )
			{
				ret = 0x33;
				return TRUE;
			}else if ( name == "threequarters" )
			{
				ret = 0xBE;
				return TRUE;
			}else if ( name == "threesuperior" )
			{
				ret = 0xB3;
				return TRUE;
			}else if ( name == "tilde" )
			{
				ret = 0x98;
				return TRUE;
			}else if ( name == "trademark" )
			{
				ret = 0x99;
				return TRUE;
			}else if ( name == "two" )
			{
				ret = 0x32;
				return TRUE;
			}
			else if ( name == "twosuperior" )
			{
				ret = 0xB2;
				return TRUE;
			}
			break;
		case 'u':
			if ( name == "u" )
			{
				ret = 0x75;
				return TRUE;
			}else if ( name == "uacute" )
			{
				ret = 0xFA;
				return TRUE;
			}else if ( name == "ucircumflex" )
			{
				ret = 0xFB;
				return TRUE;
			}else if ( name == "udieresis" )
			{
				ret = 0xFC;
				return TRUE;
			}else if ( name == "ugrave" )
			{
				ret = 0xF9;
				return TRUE;
			}else if ( name == "underscore" )
			{
				ret = 0x5F;
				return TRUE;
			}
			break;
		case 'v':
			ret = 0x76;
			return TRUE;
			break;
		case 'w':
			ret = 0x77;
			return TRUE;
			break;
		case 'x':
			ret = 0x78;
			return TRUE;
			break;
		case 'y':
			if ( name == "y" )
			{
				ret = 0x79;
				return TRUE;
			}else if ( name == "yacute" )
			{
				ret = 0xFD;
				return TRUE;
			}else if ( name == "ydieresis" )
			{
				ret = 0xFF;
				return TRUE;
			}else if ( name == "yen" )
			{
				ret = 0xA5;
				return TRUE;
			}
			break;
		case 'z':
			if ( name == "z" )
			{
				ret = 0x7A;
				return TRUE;
			}else if ( name == "zcaron" )
			{
				ret = 0x9E;
				return TRUE;
			}else if ( name == "zero" )
			{
				ret = 0x30;
				return TRUE;
			}
			break;
		}
	}else if ( encoding == FONT_ENCODING_MACROMAN || encoding == FONT_ENCODING_MACEXPERT )
	{
		switch ( name[0] )
		{
		case 'A':
			if ( name == "A" )
			{
				ret = 0x41;
				return TRUE;
			}else if ( name == "AE" )
			{
				ret = 0xAE;
				return TRUE;
			}else if ( name == "Aacute" )
			{
				ret = 0xE7;
				return TRUE;
			}else if ( name == "Acircumflex" )
			{
				ret = 0xE5;
				return TRUE;
			}else if ( name == "Adieresis" )
			{
				ret = 0x80;
				return TRUE;
			}else if ( name == "Agrave" )
			{
				ret = 0xCB;
				return TRUE;
			}else if ( name == "Aring" )
			{
				ret = 0x81;
				return TRUE;
			}else if ( name == "Atilde" )
			{
				ret = 0xCC;
				return TRUE;
			}
			break;
		case 'B':
			ret = 0x42;
			return TRUE;
			break;
		case 'C':
			if ( name == "C" )
			{
				ret = 0x43;
				return TRUE;
			}else if ( name == "Ccedilla" )
			{
				ret = 0x82;
				return TRUE;
			}
			break;
		case 'D':
			ret = 0x44;
			return TRUE;
			break;
		case 'E':
			if ( name == "E" )
			{
				ret = 0x45;
				return TRUE;
			}else if ( name == "Eacute" )
			{
				ret = 0x83;
				return TRUE;
			}else if ( name == "Ecircumflex" )
			{
				ret = 0xE6;
				return TRUE;
			}else if ( name == "Edieresis" )
			{
				ret = 0xE8;
				return TRUE;
			}else if ( name == "Egrave" )
			{
				ret = 0xE9;
				return TRUE;
			}else if ( name == "Eth" )
			{
				return FALSE;
			}else if ( name == "Euro" )
			{
				return FALSE;
			}
			break;
		case 'F':
			ret = 0x46;
			return TRUE;
			break;
		case 'G':
			ret = 0x47;
			return TRUE;
			break;
		case 'H':
			ret = 0x48;
			return TRUE;
			break;
		case 'I':
			if ( name == "I" )
			{
				ret = 0x49;
				return TRUE;
			}else if ( name == "Iacute" )
			{
				ret = 0xEA;
				return TRUE;
			}else if ( name == "Icircumflex" )
			{
				ret = 0xEB;
				return TRUE;
			}else if ( name == "Idieresis" )
			{
				ret = 0xEC;
				return TRUE;
			}else if ( name == "Igrave" )
			{
				ret = 0xED;
				return TRUE;
			}
			break;
		case 'J':
			ret = 0x4A;
			return TRUE;
			break;
		case 'K':
			ret = 0x4B;
			return TRUE;
			break;
		case 'L':
			if ( name == "L" )
			{
				ret = 0x4C;
				return TRUE;
			}else if ( name == "Lslash" )
			{
				return FALSE;
			}
			break;
		case 'M':
			ret = 0x4D;
			return TRUE;
			break;
		case 'N':
			if ( name == "N" )
			{
				ret = 0x4E;
				return TRUE;
			}else if ( name == "Ntilde" )
			{
				ret = 0x84;
				return TRUE;
			}
			break;
		case 'O':
			if ( name == "O" )
			{
				ret = 0x4F;
				return TRUE;
			}else if ( name == "OE" )
			{
				ret = 0xCE;
				return TRUE;
			}else if ( name == "Oacute" )
			{
				ret = 0xEE;
				return TRUE;
			}else if ( name == "Ocircumflex" )
			{
				ret = 0xEF;
				return TRUE;
			}else if ( name == "Odieresis" )
			{
				ret = 0x85;
				return TRUE;
			}else if ( name == "Ograve" )
			{
				ret = 0xF1;
				return TRUE;
			}else if ( name == "Oslash" )
			{
				ret = 0xAF;
				return TRUE;
			}else if ( name == "Otilde" )
			{
				ret = 0xCD;
				return TRUE;
			}
			break;
		case 'P':
			ret = 0x50;
			return TRUE;
			break;
		case 'Q':
			ret = 0x51;
			return TRUE;
			break;
		case 'R':
			ret = 0x52;
			return TRUE;
			break;
		case 'S':
			if ( name == "S" )
			{
				ret = 0x53;
				return TRUE;
			}else if ( name == "Scaron" )
			{
				return FALSE;
			}
			break;
		case 'T':
			if ( name == "T" )
			{
				ret = 0x54;
				return TRUE;
			}else if ( name == "Thorn" )
			{
				return FALSE;
			}
			break;
		case 'U':
			if ( name == "U" )
			{
				ret = 0x55;
				return TRUE;
			}else if ( name == "Uacute" )
			{
				ret = 0xF2;
				return TRUE;
			}else if ( name == "Ucircumflex" )
			{
				ret = 0xF3;
				return TRUE;
			}else if ( name == "Udieresis" )
			{
				ret = 0x86;
				return TRUE;
			}else if ( name == "Ugrave" )
			{
				ret = 0xF4;
				return TRUE;
			}
			break;
		case 'V':
			ret = 0x56;
			return TRUE;
			break;
		case 'W':
			ret = 0x57;
			return TRUE;
			break;
		case 'X':
			ret = 0x58;
			return TRUE;
			break;
		case 'Y':
			if ( name == "Y" )
			{
				ret = 0x59;
				return TRUE;
			}else if ( name == "Yacute" )
			{
				return FALSE;
			}else if ( name == "Ydieresis" )
			{
				ret = 0xD9;
				return TRUE;
			}
			break;
		case 'Z':
			if ( name == "Z" )
			{
				ret = 0x5A;
			}else if ( name == "Zcaron" )
			{
				return FALSE;
			}
			break;
		case 'a':
			if ( name == "a" )
			{
				ret = 0x61;
				return TRUE;
			}else if ( name == "aacute" )
			{
				ret = 0x87;
				return TRUE;
			}else if ( name == "acircumflex" )
			{
				ret = 0x89;
				return TRUE;
			}else if ( name == "acute" )
			{
				ret = 0xAB;
				return TRUE;
			}else if ( name == "adieresis" )
			{
				ret = 0x8A;
				return TRUE;
			}else if ( name == "ae" )
			{
				ret = 0xBE;
				return TRUE;
			}else if ( name == "agrave" )
			{
				ret = 0x88;
				return TRUE;
			}else if ( name == "ampersand" )
			{
				ret = 0x26;
				return TRUE;
			}else if ( name == "aring" )
			{
				ret = 0x8C;
				return TRUE;
			}else if ( name == "asciicircum" )
			{
				ret = 0x5E;
				return TRUE;
			}else if ( name == "asciitidle" )
			{
				ret = 0x7E;
				return TRUE;
			}else if ( name == "asterisk" )
			{
				ret = 0x2A;
				return TRUE;
			}else if ( name == "at" )
			{
				ret = 0x40;
				return TRUE;
			}else if ( name == "atilde" )
			{
				ret = 0x8B;
				return TRUE;
			}
			break;
		case 'b':
			if ( name == "b" )
			{
				ret = 0x62;
				return TRUE;
			}else if ( name == "backslash" )
			{
				ret = 0x5C;
				return TRUE;
			}else if ( name == "bar" )
			{
				ret = 0x7C;
				return TRUE;
			}else if ( name == "braceleft" )
			{
				ret = 0x7B;
				return TRUE;
			}else if ( name == "braceright" )
			{
				ret = 0x7D;
				return TRUE;
			}else if ( name == "bracketleft" )
			{
				ret = 0x5B;
				return TRUE;
			}else if ( name == "bracketright" )
			{
				ret = 0x5D;
				return TRUE;
			}else if ( name == "breve" )
			{
				ret = 0xF9;
				return TRUE;
			}else if ( name == "brokenbar" )
			{
				return FALSE;
			}else if ( name == "bullet" )
			{
				ret = 0xA5;
				return TRUE;
			}
			break;
		case 'c':
			if ( name == "c" )
			{
				ret = 0x63;
				return TRUE;
			}else if ( name == "caron" )
			{
				ret = 0xFF;
				return TRUE;
			}else if ( name == "ccedilla" )
			{
				ret = 0x8D;
				return TRUE;
			}else if ( name == "cedilla" )
			{
				ret = 0xFC;
				return TRUE;
			}else if ( name == "cent" )
			{
				ret = 0xA2;
				return TRUE;
			}else if ( name == "circumflex" )
			{
				ret = 0xF6;
				return TRUE;
			}else if ( name == "colon" )
			{
				ret = 0x3A;
				return TRUE;
			}else if ( name == "comma" )
			{
				ret = 0x2C;
				return TRUE;
			}else if ( name == "copyright" )
			{
				ret = 0xA9;
				return TRUE;
			}else if ( name == "currency" )
			{
				ret = 0xDB;
				return TRUE;
			}
			break;
		case 'd':
			if ( name == "d" )
			{
				ret = 0x64;
				return TRUE;
			}else if ( name == "dagger" )
			{
				ret = 0xA0;
				return TRUE;
			}else if ( name == "daggerdbl" )
			{
				ret = 0xB0;
				return TRUE;
			}else if ( name == "degree" )
			{
				ret = 0xA1;
				return TRUE;
			}else if ( name == "dieresis" )
			{
				ret = 0xAC;
				return TRUE;
			}else if ( name == "divide" )
			{
				ret = 0xD6;
				return TRUE;
			}else if ( name == "dollar" )
			{
				ret = 0x24;
				return TRUE;
			}else if ( name == "dotaccent" )
			{
				ret = 0xFA;
				return TRUE;
			}else if ( name == "dotlessi" )
			{
				ret = 0xF5;
				return TRUE;
			}
			break;
		case 'e':
			if ( name == "e" )
			{
				ret = 0x65;
				return TRUE;
			}else if ( name == "eacute" )
			{
				ret = 0x8E;
				return TRUE;
			}else if ( name == "ecircumflex" )
			{
				ret = 0x90;
				return TRUE;
			}else if ( name == "edieresis" )
			{
				ret = 0x91;
				return TRUE;
			}else if ( name == "egrave" )
			{
				ret = 0x8F;
				return TRUE;
			}else if ( name == "eight" )
			{
				ret = 0x38;
				return TRUE;
			}else if ( name == "ellipsis" )
			{
				ret = 0xC9;
				return TRUE;
			}else if ( name == "emdash" )
			{
				ret = 0xD1;
				return TRUE;
			}else if ( name == "endash" )
			{
				ret = 0xD0;
				return TRUE;
			}else if ( name == "equal" )
			{
				ret = 0x3D;
				return TRUE;
			}else if ( name == "eth" )
			{
				return FALSE;
			}else if ( name == "exclam" )
			{
				ret = 0x21;
				return TRUE;
			}else if ( name == "exclamdown" )
			{
				ret = 0xC1;
				return TRUE;
			}
			break;
		case 'f':
			if ( name == "f" )
			{
				ret = 0x66;
				return TRUE;
			}else if ( name == "fi" )
			{
				ret = 0xDE;
				return TRUE;
			}else if ( name == "five" )
			{
				ret = 0x35;
				return TRUE;
			}else if ( name == "fl" )
			{
				ret = 0xDF;
				return TRUE;
			}else if ( name == "florin" )
			{
				ret = 0xC4;
				return TRUE;
			}else if ( name == "four" )
			{
				ret = 0x34;
				return TRUE;
			}else if ( name == "fraction" )
			{
				ret = 0xDA;
				return TRUE;
			}
			break;
		case 'g':
			if ( name == "g" )
			{
				ret = 0x67;
				return TRUE;
			}else if ( name == "germandbls" )
			{
				ret = 0xA7;
				return TRUE;
			}else if ( name == "grave" )
			{
				ret = 0x60;
				return TRUE;
			}else if ( name == "greater" )
			{
				ret = 0x3E;
				return TRUE;
			}else if ( name == "guillemotleft" )
			{
				ret = 0xC7;
				return TRUE;
			}else if ( name == "guillemotright" )
			{
				ret = 0xC8;
				return TRUE;
			}else if ( name == "guilsinglleft" )
			{
				ret = 0xDC;
				return TRUE;
			}else if ( name == "guilsinglright" )
			{
				ret = 0xDD;
				return TRUE;
			}
			break;
		case 'h':
			if ( name == "h" )
			{
				ret = 0x68;
				return TRUE;
			}else if ( name == "hungarumlaut" )
			{
				ret = 0xFD;
				return TRUE;
			}else if ( name == "hyphen" )
			{
				ret = 0x2D;
				return TRUE;
			}
			break;
		case 'i':
			if ( name == "i" )
			{
				ret = 0x69;
				return TRUE;
			}else if ( name == "iacute" )
			{
				ret = 0x92;
				return TRUE;
			}else if ( name == "icircumflex" )
			{
				ret = 0x94;
				return TRUE;
			}else if ( name == "idieresis" )
			{
				ret = 0x95;
				return TRUE;
			}else if ( name == "igrave" )
			{
				ret = 0x93;
				return TRUE;
			}
			break;
		case 'j':
			ret = 0x6A;
			return TRUE;
			break;
		case 'k':
			ret = 0x6B;
			return TRUE;
			break;
		case 'l':
			if ( name == "l" )
			{
				ret = 0x6C;
				return TRUE;
			}else if ( name == "less" )
			{
				ret = 0x3C;
				return TRUE;
			}else if ( name == "logicalnot" )
			{
				ret = 0xC2;
				return TRUE;
			}else if ( name == "lslash" )
			{
				return FALSE;
			}
			break;
		case 'm':
			if ( name == "m" )
			{
				ret = 0x6D;
				return FALSE;
			}else if ( name == "macron" )
			{
				ret = 0xF8;
				return TRUE;
			}else if ( name == "minus" )
			{
				return FALSE;
			}else if ( name == "mu" )
			{
				ret = 0xB5;
				return TRUE;
			}else if ( name == "multiply" )
			{
				return FALSE;
			}
			break;
		case 'n':
			if ( name == "n" )
			{
				ret = 0x6E;
				return TRUE;
			}else if ( name == "nine" )
			{
				ret = 0x39;
				return TRUE;
			}else if ( name == "ntilde" )
			{
				ret = 0x96;
				return TRUE;
			}else if ( name == "numbersign" )
			{
				ret = 0x23;
				return TRUE;
			}
			break;
		case 'o':
			if ( name == "o" )
			{
				ret = 0x6F;
				return TRUE;
			}else if ( name == "oacute" )
			{
				ret = 0x97;
				return TRUE;
			}else if ( name == "ocircumflex" )
			{
				ret = 0x99;
				return TRUE;
			}else if ( name == "odieresis" )
			{
				ret = 0x9A;
				return TRUE;
			}else if ( name == "oe" )
			{
				ret = 0xCF;
				return TRUE;
			}else if ( name == "ogonek" )
			{
				ret = 0xFE;
				return TRUE;
			}else if ( name == "ograve" )
			{
				ret = 0x98;
				return TRUE;
			}else if ( name == "one" )
			{
				ret = 0x31;
				return TRUE;
			}else if ( name == "onehalf" )
			{
				return FALSE;
			}else if ( name == "onequarter" )
			{
				return FALSE;
			}else if ( name == "onesuperior" )
			{
				return FALSE;
			}else if ( name == "ordfeminine" )
			{
				ret = 0xBB;
				return TRUE;
			}else if ( name == "ordmasculine" )
			{
				ret = 0xBC;
				return TRUE;
			}else if ( name == "oslash" )
			{
				ret = 0xBF;
				return TRUE;
			}else if ( name == "otilde" )
			{
				ret = 0x9B;
				return TRUE;
			}
			break;
		case 'p':
			if ( name == "p" )
			{
				ret = 0x70;
				return TRUE;
			}else if ( name == "paragraph" )
			{
				ret = 0xA6;
				return TRUE;
			}else if ( name == "parenleft" )
			{
				ret = 0x28;
				return TRUE;
			}else if ( name == "parenright" )
			{
				ret = 0x29;
				return TRUE;
			}else if ( name == "percent" )
			{
				ret = 0x25;
				return TRUE;
			}else if ( name == "period" )
			{
				ret = 0x2E;
				return TRUE;
			}else if ( name == "periodcentered" )
			{
				ret = 0xE1;
				return TRUE;
			}else if ( name == "perthousand" )
			{
				ret = 0xE4;
				return TRUE;
			}else if ( name == "plus" )
			{
				ret = 0x2B;
				return TRUE;
			}else if ( name == "plusminus" )
			{
				ret = 0xB1;
				return TRUE;
			}
			break;
		case 'q':
			if ( name == "q" )
			{
				ret = 0x71;
				return TRUE;
			}else if ( name == "question" )
			{
				ret = 0x3F;
				return TRUE;
			}else if ( name == "questiondown" )
			{
				ret = 0xC0;
				return TRUE;
			}else if ( name == "quotedbl" )
			{
				ret = 0x22;
				return TRUE;
			}else if ( name == "quotedblbase" )
			{
				ret = 0xE3;
				return TRUE;
			}else if ( name == "quotedblleft" )
			{
				ret = 0xD2;
				return TRUE;
			}else if ( name == "quotedblright" )
			{
				ret = 0xD3;
				return TRUE;
			}else if ( name == "quoteleft" )
			{
				ret = 0xD4;
				return TRUE;
			}
			else if ( name == "quoteright" )
			{
				ret = 0xD5;
				return TRUE;
			}else if ( name == "quotesinglbase" )
			{
				ret = 0xE2;
				return TRUE;
			}else if ( name == "quotesingle" )
			{
				ret = 0x27;
				return TRUE;
			}
			break;
		case 'r':
			if ( name == "r" )
			{
				ret = 0x72;
				return TRUE;
			}else if ( name == "registered" )
			{
				ret = 0xA8;
				return TRUE;
			}else if ( name == "ring" )
			{
				ret = 0xFB;
				return TRUE;
			}
			break;
		case 's':
			if ( name == "s" )
			{
				ret = 0x73;
				return TRUE;
			}else if ( name == "scaron" )
			{
				return FALSE;
			}else if ( name == "section" )
			{
				ret = 0xA4;
				return TRUE;
			}else if ( name == "semicolon" )
			{
				ret = 0x3B;
				return TRUE;
			}else if ( name == "seven" )
			{
				ret = 0x37;
				return TRUE;
			}else if ( name == "six" )
			{
				ret = 0x36;
				return TRUE;
			}else if ( name == "slash" )
			{
				ret = 0x2F;
				return TRUE;
			}else if ( name == "space" )
			{
				ret = 0x20;
				return TRUE;
			}
			else if ( name == "sterling" )
			{
				ret = 0xA3;
				return TRUE;
			}
			break;
		case 't':
			if ( name == "t" )
			{
				ret = 0x74;
				return TRUE;
			}else if ( name == "thorn" )
			{
				ret = 0xFE;
				return TRUE;
			}else if ( name == "three" )
			{
				ret = 0x33;
				return TRUE;
			}else if ( name == "threequarters" )
			{
				return FALSE;
			}else if ( name == "threesuperior" )
			{
				return FALSE;
			}else if ( name == "tilde" )
			{
				ret = 0xF7;
				return TRUE;
			}else if ( name == "trademark" )
			{
				ret = 0xAA;
				return TRUE;
			}else if ( name == "two" )
			{
				ret = 0x32;
				return TRUE;
			}
			else if ( name == "twosuperior" )
			{
				return FALSE;
			}
			break;
		case 'u':
			if ( name == "u" )
			{
				ret = 0x75;
				return TRUE;
			}else if ( name == "uacute" )
			{
				ret = 0x9C;
				return TRUE;
			}else if ( name == "ucircumflex" )
			{
				ret = 0x9E;
				return TRUE;
			}else if ( name == "udieresis" )
			{
				ret = 0x9F;
				return TRUE;
			}else if ( name == "ugrave" )
			{
				ret = 0x9D;
				return TRUE;
			}else if ( name == "underscore" )
			{
				ret = 0x5F;
				return TRUE;
			}
			break;
		case 'v':
			ret = 0x76;
			return TRUE;
			break;
		case 'w':
			ret = 0x77;
			return TRUE;
			break;
		case 'x':
			ret = 0x78;
			return TRUE;
			break;
		case 'y':
			if ( name == "y" )
			{
				ret = 0x79;
				return TRUE;
			}else if ( name == "yacute" )
			{
				return FALSE;
			}else if ( name == "ydieresis" )
			{
				ret = 0xD8;
				return TRUE;
			}else if ( name == "yen" )
			{
				ret = 0xD4;
				return TRUE;
			}
			break;
		case 'z':
			if ( name == "z" )
			{
				ret = 0x7A;
				return TRUE;
			}else if ( name == "zcaron" )
			{
				return FALSE;
			}else if ( name == "zero" )
			{
				ret = 0x30;
				return TRUE;
			}
			break;
		}
	}else if ( encoding == FONT_ENCODING_PDFDOC )
	{
		switch ( name[0] )
		{
		case 'A':
			if ( name == "A" )
			{
				ret = 0x41;
				return TRUE;
			}else if ( name == "AE" )
			{
				ret = 0xC6;
				return TRUE;
			}else if ( name == "Aacute" )
			{
				ret = 0xC1;
				return TRUE;
			}else if ( name == "Acircumflex" )
			{
				ret = 0xC2;
				return TRUE;
			}else if ( name == "Adieresis" )
			{
				ret = 0xC4;
				return TRUE;
			}else if ( name == "Agrave" )
			{
				ret = 0xC0;
				return TRUE;
			}else if ( name == "Aring" )
			{
				ret = 0xC5;
				return TRUE;
			}else if ( name == "Atilde" )
			{
				ret = 0xC3;
				return TRUE;
			}
			break;
		case 'B':
			ret = 0x42;
			return TRUE;
			break;
		case 'C':
			if ( name == "C" )
			{
				ret = 0x43;
				return TRUE;
			}else if ( name == "Ccedilla" )
			{
				ret = 0xC7;
				return TRUE;
			}
			break;
		case 'D':
			ret = 0x44;
			return TRUE;
			break;
		case 'E':
			if ( name == "E" )
			{
				ret = 0x45;
				return TRUE;
			}else if ( name == "Eacute" )
			{
				ret = 0xC9;
				return TRUE;
			}else if ( name == "Ecircumflex" )
			{
				ret = 0xCA;
				return TRUE;
			}else if ( name == "Edieresis" )
			{
				ret = 0xCB;
				return TRUE;
			}else if ( name == "Egrave" )
			{
				ret = 0xC8;
				return TRUE;
			}else if ( name == "Eth" )
			{
				ret = 0xD0;
				return TRUE;
			}else if ( name == "Euro" )
			{
				ret = 0xA0;
				return TRUE;
			}
			break;
		case 'F':
			ret = 0x46;
			return TRUE;
			break;
		case 'G':
			ret = 0x47;
			return TRUE;
			break;
		case 'H':
			ret = 0x48;
			return TRUE;
			break;
		case 'I':
			if ( name == "I" )
			{
				ret = 0x49;
				return TRUE;
			}else if ( name == "Iacute" )
			{
				ret = 0xCD;
				return TRUE;
			}else if ( name == "Icircumflex" )
			{
				ret = 0xCE;
				return TRUE;
			}else if ( name == "Idieresis" )
			{
				ret = 0xCF;
				return TRUE;
			}else if ( name == "Igrave" )
			{
				ret = 0xCC;
				return TRUE;
			}
			break;
		case 'J':
			ret = 0x4A;
			return TRUE;
			break;
		case 'K':
			ret = 0x4B;
			return TRUE;
			break;
		case 'L':
			if ( name == "L" )
			{
				ret = 0x4C;
				return TRUE;
			}else if ( name == "Lslash" )
			{
				ret = 0x95;
				return TRUE;
			}
			break;
		case 'M':
			ret = 0x4D;
			return TRUE;
			break;
		case 'N':
			if ( name == "N" )
			{
				ret = 0x4E;
				return TRUE;
			}else if ( name == "Ntilde" )
			{
				ret = 0xD1;
				return TRUE;
			}
			break;
		case 'O':
			if ( name == "O" )
			{
				ret = 0x4F;
				return TRUE;
			}else if ( name == "OE" )
			{
				ret = 0x96;
				return TRUE;
			}else if ( name == "Oacute" )
			{
				ret = 0xD3;
				return TRUE;
			}else if ( name == "Ocircumflex" )
			{
				ret = 0xD4;
				return TRUE;
			}else if ( name == "Odieresis" )
			{
				ret = 0xD6;
				return TRUE;
			}else if ( name == "Ograve" )
			{
				ret = 0xD2;
				return TRUE;
			}else if ( name == "Oslash" )
			{
				ret = 0xD8;
				return TRUE;
			}else if ( name == "Otilde" )
			{
				ret = 0xD5;
				return TRUE;
			}
			break;
		case 'P':
			ret = 0x50;
			return TRUE;
			break;
		case 'Q':
			ret = 0x51;
			return TRUE;
			break;
		case 'R':
			ret = 0x52;
			return TRUE;
			break;
		case 'S':
			if ( name == "S" )
			{
				ret = 0x53;
				return TRUE;
			}else if ( name == "Scaron" )
			{
				ret = 0x97;
				return TRUE;
			}
			break;
		case 'T':
			if ( name == "T" )
			{
				ret = 0x54;
				return TRUE;
			}else if ( name == "Thorn" )
			{
				ret = 0xDE;
				return TRUE;
			}
			break;
		case 'U':
			if ( name == "U" )
			{
				ret = 0x55;
				return TRUE;
			}else if ( name == "Uacute" )
			{
				ret = 0xDA;
				return TRUE;
			}else if ( name == "Ucircumflex" )
			{
				ret = 0xDB;
				return TRUE;
			}else if ( name == "Udieresis" )
			{
				ret = 0xDC;
				return TRUE;
			}else if ( name == "Ugrave" )
			{
				ret = 0xD9;
				return TRUE;
			}
			break;
		case 'V':
			ret = 0x56;
			return TRUE;
			break;
		case 'W':
			ret = 0x57;
			return TRUE;
			break;
		case 'X':
			ret = 0x58;
			return TRUE;
			break;
		case 'Y':
			if ( name == "Y" )
			{
				ret = 0x59;
				return TRUE;
			}else if ( name == "Yacute" )
			{
				ret = 0xDD;
				return TRUE;
			}else if ( name == "Ydieresis" )
			{
				ret = 0x98;
				return TRUE;
			}
			break;
		case 'Z':
			if ( name == "Z" )
			{
				ret = 0x5A;
			}else if ( name == "Zcaron" )
			{
				ret = 0x99;
				return TRUE;
			}
			break;
		case 'a':
			if ( name == "a" )
			{
				ret = 0x61;
				return TRUE;
			}else if ( name == "aacute" )
			{
				ret = 0xE1;
				return TRUE;
			}else if ( name == "acircumflex" )
			{
				ret = 0xE2;
				return TRUE;
			}else if ( name == "acute" )
			{
				ret = 0xB4;
				return TRUE;
			}else if ( name == "adieresis" )
			{
				ret = 0xE4;
				return TRUE;
			}else if ( name == "ae" )
			{
				ret = 0xE6;
				return TRUE;
			}else if ( name == "agrave" )
			{
				ret = 0xE0;
				return TRUE;
			}else if ( name == "ampersand" )
			{
				ret = 0x26;
				return TRUE;
			}else if ( name == "aring" )
			{
				ret = 0xE5;
				return TRUE;
			}else if ( name == "asciicircum" )
			{
				ret = 0x5E;
				return TRUE;
			}else if ( name == "asciitidle" )
			{
				ret = 0x7E;
				return TRUE;
			}else if ( name == "asterisk" )
			{
				ret = 0x2A;
				return TRUE;
			}else if ( name == "at" )
			{
				ret = 0x40;
				return TRUE;
			}else if ( name == "atilde" )
			{
				ret = 0xE3;
				return TRUE;
			}
			break;
		case 'b':
			if ( name == "b" )
			{
				ret = 0x62;
				return TRUE;
			}else if ( name == "backslash" )
			{
				ret = 0x5C;
				return TRUE;
			}else if ( name == "bar" )
			{
				ret = 0x7C;
				return TRUE;
			}else if ( name == "braceleft" )
			{
				ret = 0x7B;
				return TRUE;
			}else if ( name == "braceright" )
			{
				ret = 0x7D;
				return TRUE;
			}else if ( name == "bracketleft" )
			{
				ret = 0x5B;
				return TRUE;
			}else if ( name == "bracketright" )
			{
				ret = 0x5D;
				return TRUE;
			}else if ( name == "breve" )
			{
				ret = 0x18;
				return TRUE;
			}else if ( name == "brokenbar" )
			{
				ret = 0xA6;
				return TRUE;
			}else if ( name == "bullet" )
			{
				ret = 0x80;
				return TRUE;
			}
			break;
		case 'c':
			if ( name == "c" )
			{
				ret = 0x63;
				return TRUE;
			}else if ( name == "caron" )
			{
				ret = 0x19;
				return TRUE;
			}else if ( name == "ccedilla" )
			{
				ret = 0xE7;
				return TRUE;
			}else if ( name == "cedilla" )
			{
				ret = 0xB8;
				return TRUE;
			}else if ( name == "cent" )
			{
				ret = 0xA2;
				return TRUE;
			}else if ( name == "circumflex" )
			{
				ret = 0x1A;
				return TRUE;
			}else if ( name == "colon" )
			{
				ret = 0x3A;
				return TRUE;
			}else if ( name == "comma" )
			{
				ret = 0x2C;
				return TRUE;
			}else if ( name == "copyright" )
			{
				ret = 0xA9;
				return TRUE;
			}else if ( name == "currency" )
			{
				ret = 0xA4;
				return TRUE;
			}
			break;
		case 'd':
			if ( name == "d" )
			{
				ret = 0x64;
				return TRUE;
			}else if ( name == "dagger" )
			{
				ret = 0x81;
				return TRUE;
			}else if ( name == "daggerdbl" )
			{
				ret = 0x82;
				return TRUE;
			}else if ( name == "degree" )
			{
				ret = 0xB0;
				return TRUE;
			}else if ( name == "dieresis" )
			{
				ret = 0xA8;
				return TRUE;
			}else if ( name == "divide" )
			{
				ret = 0xF7;
				return TRUE;
			}else if ( name == "dollar" )
			{
				ret = 0x24;
				return TRUE;
			}else if ( name == "dotaccent" )
			{
				ret = 0x1B;
				return TRUE;
			}else if ( name == "dotlessi" )
			{
				ret = 0x9A;
				return TRUE;
			}
			break;
		case 'e':
			if ( name == "e" )
			{
				ret = 0x65;
				return TRUE;
			}else if ( name == "eacute" )
			{
				ret = 0xE9;
				return TRUE;
			}else if ( name == "ecircumflex" )
			{
				ret = 0xEA;
				return TRUE;
			}else if ( name == "edieresis" )
			{
				ret = 0xEB;
				return TRUE;
			}else if ( name == "egrave" )
			{
				ret = 0xE8;
				return TRUE;
			}else if ( name == "eight" )
			{
				ret = 0x38;
				return TRUE;
			}else if ( name == "ellipsis" )
			{
				ret = 0x83;
				return TRUE;
			}else if ( name == "emdash" )
			{
				ret = 0x84;
				return TRUE;
			}else if ( name == "endash" )
			{
				ret = 0x85;
				return TRUE;
			}else if ( name == "equal" )
			{
				ret = 0x3D;
				return TRUE;
			}else if ( name == "eth" )
			{
				ret = 0xF0;
				return TRUE;
			}else if ( name == "exclam" )
			{
				ret = 0x21;
				return TRUE;
			}else if ( name == "exclamdown" )
			{
				ret = 0xA1;
				return TRUE;
			}
			break;
		case 'f':
			if ( name == "f" )
			{
				ret = 0x66;
				return TRUE;
			}else if ( name == "fi" )
			{
				ret = 0x93;
				return TRUE;
			}else if ( name == "five" )
			{
				ret = 0x35;
				return TRUE;
			}else if ( name == "fl" )
			{
				ret = 0x94;
				return TRUE;
			}else if ( name == "florin" )
			{
				ret = 0x86;
				return TRUE;
			}else if ( name == "four" )
			{
				ret = 0x34;
				return TRUE;
			}else if ( name == "fraction" )
			{
				ret = 0x95;
				return TRUE;
			}
			break;
		case 'g':
			if ( name == "g" )
			{
				ret = 0x67;
				return TRUE;
			}else if ( name == "germandbls" )
			{
				ret = 0xDF;
				return TRUE;
			}else if ( name == "grave" )
			{
				ret = 0x60;
				return TRUE;
			}else if ( name == "greater" )
			{
				ret = 0x3E;
				return TRUE;
			}else if ( name == "guillemotleft" )
			{
				ret = 0xAB;
				return TRUE;
			}else if ( name == "guillemotright" )
			{
				ret = 0xBB;
				return TRUE;
			}else if ( name == "guilsinglleft" )
			{
				ret = 0x88;
				return TRUE;
			}else if ( name == "guilsinglright" )
			{
				ret = 0x89;
				return TRUE;
			}
			break;
		case 'h':
			if ( name == "h" )
			{
				ret = 0x68;
				return TRUE;
			}else if ( name == "hungarumlaut" )
			{
				ret = 0x1C;
				return TRUE;
			}else if ( name == "hyphen" )
			{
				ret = 0x2D;
				return TRUE;
			}
			break;
		case 'i':
			if ( name == "i" )
			{
				ret = 0x69;
				return TRUE;
			}else if ( name == "iacute" )
			{
				ret = 0xED;
				return TRUE;
			}else if ( name == "icircumflex" )
			{
				ret = 0xEE;
				return TRUE;
			}else if ( name == "idieresis" )
			{
				ret = 0xEF;
				return TRUE;
			}else if ( name == "igrave" )
			{
				ret = 0xEC;
				return TRUE;
			}
			break;
		case 'j':
			ret = 0x6A;
			return TRUE;
			break;
		case 'k':
			ret = 0x6B;
			return TRUE;
			break;
		case 'l':
			if ( name == "l" )
			{
				ret = 0x6C;
				return TRUE;
			}else if ( name == "less" )
			{
				ret = 0x3C;
				return TRUE;
			}else if ( name == "logicalnot" )
			{
				ret = 0xAC;
				return TRUE;
			}else if ( name == "lslash" )
			{
				ret = 0x9B;
				return TRUE;
			}
			break;
		case 'm':
			if ( name == "m" )
			{
				ret = 0x6D;
				return FALSE;
			}else if ( name == "macron" )
			{
				ret = 0xAF;
				return TRUE;
			}else if ( name == "minus" )
			{
				ret = 0x8A;
				return TRUE;
			}else if ( name == "mu" )
			{
				ret = 0xB5;
				return TRUE;
			}else if ( name == "multiply" )
			{
				ret = 0xD7;
				return TRUE;
			}
			break;
		case 'n':
			if ( name == "n" )
			{
				ret = 0x6E;
				return TRUE;
			}else if ( name == "nine" )
			{
				ret = 0x39;
				return TRUE;
			}else if ( name == "ntilde" )
			{
				ret = 0xF1;
				return TRUE;
			}else if ( name == "numbersign" )
			{
				ret = 0x23;
				return TRUE;
			}
			break;
		case 'o':
			if ( name == "o" )
			{
				ret = 0x6F;
				return TRUE;
			}else if ( name == "oacute" )
			{
				ret = 0xF3;
				return TRUE;
			}else if ( name == "ocircumflex" )
			{
				ret = 0xF4;
				return TRUE;
			}else if ( name == "odieresis" )
			{
				ret = 0xF6;
				return TRUE;
			}else if ( name == "oe" )
			{
				ret = 0x9C;
				return TRUE;
			}else if ( name == "ogonek" )
			{
				ret = 0x1D;
				return TRUE;
			}else if ( name == "ograve" )
			{
				ret = 0xF2;
				return TRUE;
			}else if ( name == "one" )
			{
				ret = 0x31;
				return TRUE;
			}else if ( name == "onehalf" )
			{
				ret = 0xBD;
				return TRUE;
			}else if ( name == "onequarter" )
			{
				ret = 0xBC;
				return TRUE;
			}else if ( name == "onesuperior" )
			{
				ret = 0xB9;
				return TRUE;
			}else if ( name == "ordfeminine" )
			{
				ret = 0xAA;
				return TRUE;
			}else if ( name == "ordmasculine" )
			{
				ret = 0xBA;
				return TRUE;
			}else if ( name == "oslash" )
			{
				ret = 0xF8;
				return TRUE;
			}else if ( name == "otilde" )
			{
				ret = 0xF6;
				return TRUE;
			}
			break;
		case 'p':
			if ( name == "p" )
			{
				ret = 0x70;
				return TRUE;
			}else if ( name == "paragraph" )
			{
				ret = 0xB6;
				return TRUE;
			}else if ( name == "parenleft" )
			{
				ret = 0x28;
				return TRUE;
			}else if ( name == "parenright" )
			{
				ret = 0x29;
				return TRUE;
			}else if ( name == "percent" )
			{
				ret = 0x25;
				return TRUE;
			}else if ( name == "period" )
			{
				ret = 0x2E;
				return TRUE;
			}else if ( name == "periodcentered" )
			{
				ret = 0xB7;
				return TRUE;
			}else if ( name == "perthousand" )
			{
				ret = 0x8B;
				return TRUE;
			}else if ( name == "plus" )
			{
				ret = 0x2B;
				return TRUE;
			}else if ( name == "plusminus" )
			{
				ret = 0xB1;
				return TRUE;
			}
			break;
		case 'q':
			if ( name == "q" )
			{
				ret = 0x71;
				return TRUE;
			}else if ( name == "question" )
			{
				ret = 0x3F;
				return TRUE;
			}else if ( name == "questiondown" )
			{
				ret = 0xBF;
				return TRUE;
			}else if ( name == "quotedbl" )
			{
				ret = 0x22;
				return TRUE;
			}else if ( name == "quotedblbase" )
			{
				ret = 0x8C;
				return TRUE;
			}else if ( name == "quotedblleft" )
			{
				ret = 0x8D;
				return TRUE;
			}else if ( name == "quotedblright" )
			{
				ret = 0x8E;
				return TRUE;
			}else if ( name == "quoteleft" )
			{
				ret = 0x8F;
				return TRUE;
			}
			else if ( name == "quoteright" )
			{
				ret = 0x90;
				return TRUE;
			}else if ( name == "quotesinglbase" )
			{
				ret = 0x91;
				return TRUE;
			}else if ( name == "quotesingle" )
			{
				ret = 0x27;
				return TRUE;
			}
			break;
		case 'r':
			if ( name == "r" )
			{
				ret = 0x72;
				return TRUE;
			}else if ( name == "registered" )
			{
				ret = 0xAE;
				return TRUE;
			}else if ( name == "ring" )
			{
				ret = 0x1E;
				return TRUE;
			}
			break;
		case 's':
			if ( name == "s" )
			{
				ret = 0x73;
				return TRUE;
			}else if ( name == "scaron" )
			{
				ret = 0x9D;
				return TRUE;
			}else if ( name == "section" )
			{
				ret = 0xA7;
				return TRUE;
			}else if ( name == "semicolon" )
			{
				ret = 0x3B;
				return TRUE;
			}else if ( name == "seven" )
			{
				ret = 0x37;
				return TRUE;
			}else if ( name == "six" )
			{
				ret = 0x36;
				return TRUE;
			}else if ( name == "slash" )
			{
				ret = 0x2F;
				return TRUE;
			}else if ( name == "space" )
			{
				ret = 0x20;
				return TRUE;
			}
			else if ( name == "sterling" )
			{
				ret = 0xA3;
				return TRUE;
			}
			break;
		case 't':
			if ( name == "t" )
			{
				ret = 0x74;
				return TRUE;
			}else if ( name == "thorn" )
			{
				ret = 0xFE;
				return TRUE;
			}else if ( name == "three" )
			{
				ret = 0x33;
				return TRUE;
			}else if ( name == "threequarters" )
			{
				ret = 0xBE;
				return TRUE;
			}else if ( name == "threesuperior" )
			{
				ret = 0xB3;
				return TRUE;
			}else if ( name == "tilde" )
			{
				ret = 0x1F;
				return TRUE;
			}else if ( name == "trademark" )
			{
				ret = 0x92;
				return TRUE;
			}else if ( name == "two" )
			{
				ret = 0x32;
				return TRUE;
			}
			else if ( name == "twosuperior" )
			{
				ret = 0xB2;
				return TRUE;
			}
			break;
		case 'u':
			if ( name == "u" )
			{
				ret = 0x75;
				return TRUE;
			}else if ( name == "uacute" )
			{
				ret = 0xFA;
				return TRUE;
			}else if ( name == "ucircumflex" )
			{
				ret = 0xFB;
				return TRUE;
			}else if ( name == "udieresis" )
			{
				ret = 0xFC;
				return TRUE;
			}else if ( name == "ugrave" )
			{
				ret = 0xF9;
				return TRUE;
			}else if ( name == "underscore" )
			{
				ret = 0x5F;
				return TRUE;
			}
			break;
		case 'v':
			ret = 0x76;
			return TRUE;
			break;
		case 'w':
			ret = 0x77;
			return TRUE;
			break;
		case 'x':
			ret = 0x78;
			return TRUE;
			break;
		case 'y':
			if ( name == "y" )
			{
				ret = 0x79;
				return TRUE;
			}else if ( name == "yacute" )
			{
				ret = 0xFD;
				return TRUE;
			}else if ( name == "ydieresis" )
			{
				ret = 0xFF;
				return TRUE;
			}else if ( name == "yen" )
			{
				ret = 0xA5;
				return TRUE;
			}
			break;
		case 'z':
			if ( name == "z" )
			{
				ret = 0x7A;
				return TRUE;
			}else if ( name == "zcaron" )
			{
				ret = 0x9E;
				return TRUE;
			}else if ( name == "zero" )
			{
				ret = 0x30;
				return TRUE;
			}
			break;
		}
	}
	return FALSE;
}

HE_DWORD HE_HexStrToValue( CHE_ByteString & str )
{
	HE_DWORD valRet = 0;
	HE_BYTE tmpByte = 0;
	for ( HE_DWORD i = 0; i < str.GetLength(); i++ )
	{
		tmpByte = str[i];
		valRet = valRet << 8;
		valRet += tmpByte;
	}
	return valRet;
}


FT_Library HE_GetFTLibrary()
{
	if ( gFTLibrary == NULL )
	{
		HE_InitFTLibrary();
	}
	return gFTLibrary;
}


HE_VOID HE_InitFTLibrary()
{
	FT_Init_FreeType( &gFTLibrary );
}


HE_VOID HE_DestroyFTLibrary()
{
	if ( gFTLibrary != NULL )
	{
		FT_Done_FreeType( gFTLibrary );
	}
}


IHE_SystemFontMgr * HE_GetSystemFontMgr( CHE_Allocator * pAllocator /*= NULL*/ )
{
	if ( gpSystemFontMgr == NULL )
	{
		gpSystemFontMgr = IHE_SystemFontMgr::Create( pAllocator );
	}
	return gpSystemFontMgr;
}


CHE_PDF_Encoding::CHE_PDF_Encoding( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mType( FONT_ENCODING_NONE ), mbCodeTableRelease( FALSE ), mpCodeTable( NULL )
{
	//还需要完善对于Type3字体的Encoding的支持，还要添加字体类型的配合判断

	if ( !fontDict )
	{
		return;
	}

	CHE_PDF_ObjectPtr objPtr;

	objPtr = fontDict->GetElement( "Encoding", OBJ_TYPE_NAME );
	if ( !objPtr )
	{
		objPtr = fontDict->GetElement( "Encoding", OBJ_TYPE_DICTIONARY );
		if ( !objPtr )
		{
			objPtr = fontDict->GetElement( "Encoding", OBJ_TYPE_ARRAY );
		}
	}
	if ( !objPtr )
	{
		return;
	}

	CHE_ByteString	str;

	if ( objPtr->GetType() == OBJ_TYPE_NAME )
	{
		str = objPtr->GetNamePtr()->GetString();
		if ( str == "MacRomanEncoding" )
		{
			mType = FONT_ENCODING_MACROMAN;
			mpCodeTable = (HE_WCHAR*)gMacRomanEncoding;
		}else if ( str == "MacExpertEncoding" )
		{
			mType = FONT_ENCODING_MACEXPERT;
			mpCodeTable = (HE_WCHAR*)gMacExpertEncoding;
		}else if ( str == "WinAnsiEncoding" )
		{
			mType = FONT_ENCODING_WINANSI;
			mpCodeTable = (HE_WCHAR*)gWinAnsiEncoding;
		}else if ( str == "StandardEncoding" )
		{
			mType = FONT_ENCODING_STANDARD;
			mpCodeTable = (HE_WCHAR*)gStandardEncoding;
		}else if ( str == "PdfZapfDingbatsEncoding" )
		{
			mType = FONT_ENCODING_ZAPFDINGBAT;
			mpCodeTable = (HE_WCHAR*)gPdfZapfDingbatsEncoding;
		}else if ( str == "SymbolEncoding" )
		{
			mType = FONT_ENCODING_SYMBOL;
			mpCodeTable = (HE_WCHAR*)gSymbolEncoding;
		}else if ( str == "Identity-H" || str == "Identity-V" )
		{
			mType = FONT_ENCODING_IDENTITY;
		}else{
			//mType = FONT_ENCODING_WINANSI;
			mType = FONT_ENCODING_BUILDINCMAP;
		}
	}else if ( objPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
		mType = FONT_ENCODING_STANDARD;

		CHE_PDF_DictionaryPtr pEncodingDict = objPtr->GetDictPtr();
		objPtr = pEncodingDict->GetElement( "BaseEncoding", OBJ_TYPE_NAME );
		if ( objPtr )
		{
			str = objPtr->GetNamePtr()->GetString();
			if ( str == "MacRomanEncoding" )
			{
				mType = FONT_ENCODING_MACROMAN;
			}else if ( str == "MacExpertEncoding" )
			{
				mType = FONT_ENCODING_MACEXPERT;
			}else if ( str == "WinAnsiEncoding" )
			{
				mType = FONT_ENCODING_WINANSI;
			}else if ( str == "StandardEncoding" )
			{
				mType = FONT_ENCODING_STANDARD;
			}else if ( str == "PdfZapfDingbatsEncoding" )
			{
				mType = FONT_ENCODING_ZAPFDINGBAT;
			}else if ( str == "SymbolEncoding" )
			{
				mType = FONT_ENCODING_SYMBOL;
			}
		}else{
			mType = FONT_ENCODING_CUSTOM;
		}

		mpCodeTable = GetAllocator()->NewArray<HE_WCHAR>( 256 );
		for ( HE_DWORD i = 0; i < 256; ++i )
		{
			mpCodeTable[i] = 0;
		}
		mbCodeTableRelease = TRUE;

		switch ( mType )
		{ 
		case FONT_ENCODING_PDFDOC:
			memcpy( (HE_LPVOID)mpCodeTable, gPdfDocEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_STANDARD:
			memcpy( (HE_LPVOID)mpCodeTable, gStandardEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_MACROMAN:
			memcpy( (HE_LPVOID)mpCodeTable, gMacRomanEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_MACEXPERT:
			memcpy( (HE_LPVOID)mpCodeTable, gMacExpertEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_WINANSI:
			memcpy( (HE_LPVOID)mpCodeTable, gWinAnsiEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_ZAPFDINGBAT:
			memcpy( (HE_LPVOID)mpCodeTable, gPdfZapfDingbatsEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_SYMBOL:
			memcpy( (HE_LPVOID)mpCodeTable, gSymbolEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		default:
			break;
		}

		CHE_PDF_ArrayPtr pDifArray = pEncodingDict->GetElement( "Differences", OBJ_TYPE_ARRAY )->GetArrayPtr();
		if ( pDifArray )
		{
			HE_DWORD iCount = pDifArray->GetCount();
			HE_DWORD iIndex = 0;
			HE_BYTE tmpByte;
			CHE_PDF_ObjectPtr pObj;
			for ( HE_DWORD i = 0; i < iCount; i++ )
			{
				pObj = pDifArray->GetElement( i );
				if ( pObj->GetType() == OBJ_TYPE_NUMBER )
				{
					iIndex = pObj->GetNumberPtr()->GetInteger();
				}else if ( pObj->GetType() == OBJ_TYPE_NAME )
				{
					CHE_ByteString strTmp = pObj->GetNamePtr()->GetString();
					if ( HE_GetCodeFromName( strTmp, mType, tmpByte ) )
					{
						*(mpCodeTable + iIndex) = tmpByte;
						iIndex++;
					}
				}
			}
		}
	}
}


CHE_PDF_Encoding::~CHE_PDF_Encoding()
{
	if ( mbCodeTableRelease && mpCodeTable )
	{
		GetAllocator()->DeleteArray<HE_WCHAR>( mpCodeTable );
		mpCodeTable = NULL;
	}
}


PDF_FONT_ENCODING CHE_PDF_Encoding::GetType() const
{
	return mType;
}


HE_BOOL CHE_PDF_Encoding::GetUnicode( HE_BYTE charCode, HE_WCHAR & codeRet ) const
{
	if ( mpCodeTable )
	{
		codeRet =  mpCodeTable[charCode];
		return TRUE;
	}
	return FALSE;
}


CHE_PDF_FontDescriptor::CHE_PDF_FontDescriptor( const CHE_PDF_DictionaryPtr & fontDesDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mFlags( 0 ),  mItalicAngle( 0 ), mAscent( 0 ), mDescent( 0 ), 
	mCapHeight( 0 ), mXHeight( 0 ),mMissingWidth( 0 ), mEmbedded( FALSE ), mWMode( 0 )
{
	if ( fontDesDict )
	{
		CHE_PDF_ObjectPtr objPtr = fontDesDict->GetElement( "Flags", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mFlags = objPtr->GetNumberPtr()->GetInteger();
		}

		objPtr = fontDesDict->GetElement( "ItalicAngle", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mItalicAngle = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "Ascent", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mAscent = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "Descent", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mDescent = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "CapHeight", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mCapHeight = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "XHeight", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mXHeight = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "MissingWidth", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mMissingWidth = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "FontBBox", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
			if ( arrayPtr && arrayPtr->GetCount() >= 4 )
			{
				objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontBBox.left = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontBBox.bottom = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontBBox.width = objPtr->GetNumberPtr()->GetFloat() - mFontBBox.left;
				}
				objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontBBox.height = objPtr->GetNumberPtr()->GetFloat() - mFontBBox.bottom;
				}
			}
		}

		objPtr = fontDesDict->GetElement( "FontFile", OBJ_TYPE_REFERENCE );
		if ( ! objPtr )
		{
			objPtr = fontDesDict->GetElement( "FontFile2", OBJ_TYPE_REFERENCE );
			if ( !objPtr )
			{
				objPtr = fontDesDict->GetElement( "FontFile3", OBJ_TYPE_REFERENCE );
			}
		}
		if ( objPtr )
		{
			mEmbedded = TRUE;
			mEmbedFont = objPtr->GetRefPtr();
		}

		//todo 获取字体的WMode 只有CIDFont才有可能WMode为1，也就是垂直显示
	}
}


CHE_PDF_FontDescriptor::~CHE_PDF_FontDescriptor()
{

}


HE_BOOL CHE_PDF_FontDescriptor::IsFixedPitch() const
{
	return mFlags & 1;
}


HE_BOOL CHE_PDF_FontDescriptor::IsSerif() const
{
	return mFlags & 2;
}


HE_BOOL CHE_PDF_FontDescriptor::IsSymbolic() const
{
	return mFlags & 4;
}


HE_BOOL CHE_PDF_FontDescriptor::IsScript() const
{
	return mFlags & 8;
}


HE_BOOL	CHE_PDF_FontDescriptor::IsNonsymbolic() const
{
	return mFlags & 20;
}


HE_BOOL	CHE_PDF_FontDescriptor::IsItalic() const
{
	return mFlags & 40;
}


HE_BOOL	CHE_PDF_FontDescriptor::IsAllCap() const
{
	return mFlags & 10000;
}


HE_BOOL	CHE_PDF_FontDescriptor::IsSmallCap() const
{
	return mFlags & 20000;
}


HE_BOOL	CHE_PDF_FontDescriptor::IsForceBold() const
{
	return mFlags & 40000;
}


CHE_PDF_Font * CHE_PDF_Font::Create( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_Font * pFont = NULL;
	
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}

	if ( fontDict )
	{
		CHE_PDF_ObjectPtr objPtr = fontDict->GetElement( "Subtype", OBJ_TYPE_NAME );
		if ( objPtr )
		{
			CHE_ByteString str( pAllocator );
			str = objPtr->GetNamePtr()->GetString();
			if ( str == "Type0" )
			{
				pFont = pAllocator->New<CHE_PDF_Type0_Font>( fontDict, pAllocator );
			}
			else if ( str == "Type1"  )
			{
				pFont = pAllocator->New<CHE_PDF_Type1_Font>( fontDict, pAllocator );
			}
			else if ( str == "MMType1" )
			{
				pFont = pAllocator->New<CHE_PDF_MMType1_Font>( fontDict, pAllocator );
			}
			else if ( str == "Type3" )
			{
				pFont = pAllocator->New<CHE_PDF_Type3_Font>( fontDict, pAllocator );
			}
			else if ( str == "TrueType" )
			{
				pFont = pAllocator->New<CHE_PDF_TrueType_Font>( fontDict, pAllocator );
			}
		}
	}

	return pFont;
}


CHE_PDF_Font::CHE_PDF_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mType( FONT_TYPE1 ), mBaseFont( pAllocator ), mEncoding( fontDict, pAllocator ), mFontDict( fontDict ),
	mFace( NULL ), mpToUnicodeMap( NULL ), mpFontDescriptor( NULL ), mpEmbeddedFontFile( NULL ), mFontFileSize( 0 )
{
	CHE_PDF_ObjectPtr objPtr = mFontDict->GetElement( "Subtype", OBJ_TYPE_NAME );
	if ( objPtr )
	{
		CHE_ByteString str( GetAllocator() );
		str = objPtr->GetNamePtr()->GetString();
		if ( str == "Type0" )
		{
			mType = FONT_TYPE0;
		}else if ( str == "Type1"  )
		{
			mType = FONT_TYPE1;
		}else if ( str == "MMType1" )
		{
			mType = FONT_MMTYPE1;
		}else if ( str == "Type3" )
		{
			mType = FONT_TYPE3;
		}else if ( str == "TrueType" )
		{
			mType = FONT_TRUETYPE;
		}
	}

	objPtr = mFontDict->GetElement( "BaseFont", OBJ_TYPE_NAME );
	if ( objPtr )
	{
		mBaseFont = objPtr->GetNamePtr()->GetString();;
	}else{
		objPtr = mFontDict->GetElement( "FontName", OBJ_TYPE_NAME );
		if ( objPtr )
		{
			mBaseFont = objPtr->GetNamePtr()->GetString();
		}
	}

	objPtr = mFontDict->GetElement( "ToUnicode", OBJ_TYPE_STREAM );
	if ( objPtr )
	{
		mpToUnicodeMap = GetToUnicodeMap( objPtr->GetStreamPtr() );
	}

	objPtr = mFontDict->GetElement( "FontDescriptor", OBJ_TYPE_DICTIONARY );
	if ( objPtr )
	{
		mFontDescriptorDict = objPtr->GetDictPtr();
		mpFontDescriptor = GetAllocator()->New<CHE_PDF_FontDescriptor>( mFontDescriptorDict, GetAllocator() );
	}else{
		//Type0 Font
		objPtr = mFontDict->GetElement( "DescendantFonts", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			objPtr = objPtr->GetArrayPtr()->GetElement( 0, OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				objPtr = objPtr->GetDictPtr()->GetElement( "FontDescriptor", OBJ_TYPE_DICTIONARY );
				if ( objPtr )
				{
					mFontDescriptorDict = objPtr->GetDictPtr();
					mpFontDescriptor = GetAllocator()->New<CHE_PDF_FontDescriptor>( mFontDescriptorDict, GetAllocator() );
				}
			}
		}
		//No font descriptor dictionary
	}

	if ( mpFontDescriptor )
	{
		CHE_PDF_ReferencePtr refPtr = mpFontDescriptor->GetEmbeddedStream();
		if ( refPtr )
		{
			objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
			if ( objPtr )
			{
				CHE_PDF_StreamAcc stmAcc( GetAllocator() );
				CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
				stmAcc.Attach( stmPtr );
				if ( stmAcc.GetSize() > 0 )
				{
					mFontFileSize = stmAcc.GetSize();
					mpEmbeddedFontFile = GetAllocator()->NewArray<HE_BYTE>( mFontFileSize );
					memcpy( mpEmbeddedFontFile, stmAcc.GetData(), mFontFileSize );
				}
				stmAcc.Detach();
			}
		}

		if ( mpEmbeddedFontFile )
		{
			FT_Library ftlib = HE_GetFTLibrary();
			FT_New_Memory_Face( ftlib, mpEmbeddedFontFile, mFontFileSize, 0, &mFace );
		}
	}

	if ( mFace == NULL )
	{
		//type1 base 14 font
		if ( mType == FONT_TYPE1 || mType == FONT_MMTYPE1 )
		{
			HE_LPBYTE pBuf = NULL;
			HE_DWORD bufSize = 0;
			if ( HE_GetType1BaseFontFile( mBaseFont, pBuf, bufSize ) )
			{
				FT_Library ftlib = HE_GetFTLibrary();
				FT_New_Memory_Face( ftlib, pBuf, bufSize, 0, &mFace );
			}else if ( mpFontDescriptor )
			{
				//对于无法通过baseFont获得对于的文件的，应该通过某种其他的规则获得替代字体
				if ( HE_GetType1BaseFontFile( *mpFontDescriptor, pBuf, bufSize ) )
				{
					FT_Library ftlib = HE_GetFTLibrary();
					FT_New_Memory_Face( ftlib, pBuf, bufSize, 0, &mFace );
				}
			}
		}

		if ( mFace == NULL && mBaseFont.GetLength() > 0 )
		{
			IHE_SystemFontMgr * pSystemFontMgr = HE_GetSystemFontMgr( GetAllocator() );
			if ( pSystemFontMgr )
			{
				CHE_ByteString filePath( pAllocator );
				filePath = pSystemFontMgr->GetFontFilePath( mBaseFont );
				if ( filePath.GetLength() == 0 )
				{
					filePath = pSystemFontMgr->GetFontFilePath( "微软雅黑" );
					if ( filePath.GetLength() == 0 )
					{
						filePath = pSystemFontMgr->GetFontFilePath( "宋体" );
						if ( filePath.GetLength() == 0 )
						{
							filePath = pSystemFontMgr->GetFontFilePath( "黑体" );
						}
					}
				}

				FT_Library ftlib = HE_GetFTLibrary();
				FT_New_Face( ftlib, filePath.GetData(), 0, &mFace );
			}
		}
	}

	//处理一下字体的charmap
	FT_CharMap cmap = NULL;
	if ( mFace )
	{
		for ( HE_DWORD i = 0; i < mFace->num_charmaps; ++i )
		{
			FT_CharMap test = mFace->charmaps[i];
			if ( mType == FONT_TYPE1 )
			{
				if (test->platform_id == 7)
					cmap = test;
			}
			else if ( mType == FONT_TRUETYPE )
			{
				if ( test->platform_id == 1 && test->encoding_id == 0 )
					cmap = test;
				if ( test->platform_id == 3 && test->encoding_id == 1 )
					cmap = test;
				if ( mpFontDescriptor && mpFontDescriptor->IsSymbolic() && test->platform_id == 3 && test->encoding_id == 0 )
					cmap = test;
			}
		}
		if ( cmap )
		{
			FT_Set_Charmap( mFace, cmap );
		}
	}

	if ( mEncoding.GetType() == FONT_ENCODING_CUSTOM )
	{
		CHE_PDF_ObjectPtr objPtr;
		objPtr = fontDict->GetElement( "Encoding", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			CHE_PDF_ArrayPtr pDifArray = objPtr->GetDictPtr()->GetElement( "Differences", OBJ_TYPE_ARRAY )->GetArrayPtr();
			if ( pDifArray )
			{
				HE_DWORD iCount = pDifArray->GetCount();
				HE_DWORD iIndex = 0;
				HE_BYTE tmpByte;
				CHE_PDF_ObjectPtr pObj;
				for ( HE_DWORD i = 0; i < iCount; i++ )
				{
					pObj = pDifArray->GetElement( i );
					if ( pObj->GetType() == OBJ_TYPE_NUMBER )
					{
						iIndex = pObj->GetNumberPtr()->GetInteger();
					}else if ( pObj->GetType() == OBJ_TYPE_NAME )
					{
						CHE_ByteString strTmp = pObj->GetNamePtr()->GetString();
						HE_DWORD gid = FT_Get_Name_Index( mFace, (char *)strTmp.GetData() );
						if ( gid > 0 )
						{
							*(mEncoding.mpCodeTable + iIndex) = gid;
							iIndex++;
						}
					}
				}
			}
		}
	}
	 
}


CHE_PDF_Font::~CHE_PDF_Font()
{
	if ( mpFontDescriptor )
	{
		mpFontDescriptor->GetAllocator()->Delete<CHE_PDF_FontDescriptor>( mpFontDescriptor );
	}
	if ( mpToUnicodeMap )
	{
		mpToUnicodeMap->GetAllocator()->Delete<CHE_NumToPtrMap>( mpToUnicodeMap );
	}
	if ( mFace )
	{
		FT_Done_Face( mFace );
	}
	if ( mpEmbeddedFontFile )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( mpEmbeddedFontFile );
	}
}


PDF_FONT_TYPE CHE_PDF_Font::GetType() const
{
	return mType;
}


CHE_ByteString CHE_PDF_Font::GetBaseFont() const
{
	return mBaseFont;
}


PDF_FONT_ENCODING CHE_PDF_Font::GetEncodingType() const
{
	return mEncoding.GetType();
}


CHE_PDF_DictionaryPtr CHE_PDF_Font::GetFontDictPtr() const
{
	return mFontDict;
}


CHE_PDF_DictionaryPtr CHE_PDF_Font::GetFontDescriptorDictPtr() const
{
	return mFontDescriptorDict;
}


FT_Face CHE_PDF_Font::GetFTFace()
{
	return mFace;
}


HE_BOOL CHE_PDF_Font::GetGlyphId( HE_WCHAR charCode, HE_DWORD & codeRet ) const
{
	if ( mFace == NULL )
	{
		return FALSE;
	}

	if ( mEncoding.GetType() == FONT_ENCODING_CUSTOM )
	{
		BOOL bRet = FALSE;
		HE_WCHAR wcharRet;
		bRet = mEncoding.GetUnicode( charCode, wcharRet );
		codeRet = wcharRet;
		return bRet;
	}

	codeRet = FT_Get_Char_Index( mFace, charCode );
	if ( codeRet == 0 )
	{
		codeRet = FT_Get_Char_Index( mFace, 0xf000 + charCode );
		/* some chinese fonts only ship the similarly looking 0x2026 */
		if ( codeRet == 0 && charCode == 0x22ef )
			codeRet = FT_Get_Char_Index( mFace, 0x2026 );
	}
	if ( codeRet == 0 )
	{
		return FALSE;
	}
	return TRUE;
}


CHE_NumToPtrMap	* CHE_PDF_Font::GetToUnicodeMap( const CHE_PDF_StreamPtr & pToUnicodeStream )
{
	if ( ! pToUnicodeStream )
	{
		return NULL;
	}

	CHE_PDF_StreamAcc stmAcc( GetAllocator() );
	stmAcc.Attach( pToUnicodeStream );
	if ( stmAcc.GetSize() == 0 )
	{
		stmAcc.Detach();
		return NULL;
	}

	CHE_NumToPtrMap * tmpMap = GetAllocator()->New<CHE_NumToPtrMap>( GetAllocator() );
	IHE_Read * pFileRead = HE_CreateMemBufRead( (HE_BYTE*)(stmAcc.GetData()), stmAcc.GetSize(), GetAllocator() );
	CHE_PDF_SyntaxParser parser( NULL, GetAllocator() );
	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_DWORD lMaxIndex = 0;
	HE_DWORD lCodeCount = 0;
	parser.InitParser( pFileRead );
	while ( parser.GetWord( wordDes ) )
	{
		if ( wordDes.type == PARSE_WORD_INTEGER )
		{
			HE_DWORD lCount = wordDes.str.GetInteger();
			if ( parser.GetWord( wordDes ) == FALSE )
			{
				break;
			}
			HE_DWORD lIndex = 0;

			if ( wordDes.str == "beginbfchar" )
			{
				for ( HE_DWORD i = 0; i < lCount; i++ )
				{
					parser.GetWord( wordDes );
					lIndex = HE_HexStrToValue( wordDes.str );
					if ( lIndex > lMaxIndex )
					{
						lMaxIndex = lIndex;
					}
					parser.GetWord( wordDes );
					tmpMap->Append( lIndex, (HE_LPVOID)HE_HexStrToValue( wordDes.str ) );
					lCodeCount++;
				}
			}else if ( wordDes.str == "beginbfrange" )
			{
				for ( HE_DWORD j = 0; j < lCount; j++ )
				{
					HE_DWORD lIndexEnd = 0;
					HE_DWORD tmpValue = 0;
					HE_DWORD offset = 0;
					parser.GetWord( wordDes );
					lIndex = HE_HexStrToValue( wordDes.str );
					parser.GetWord( wordDes );
					lIndexEnd = HE_HexStrToValue( wordDes.str );
					offset = parser.GetPos();
					parser.GetWord( wordDes );
					if ( wordDes.type == PARSE_WORD_ARRAY_B )
					{
						parser.SetPos( offset );
						CHE_PDF_ArrayPtr pArray = parser.GetArrayPtr();
						if ( ! pArray )
						{
							continue;
						}
						CHE_PDF_StringPtr pStrObj;
						if ( lIndexEnd > lMaxIndex )
						{
							lMaxIndex = lIndexEnd;
						}
						for ( HE_DWORD i = lIndex ; i <= lIndexEnd; i++ )
						{
							pStrObj = pArray->GetElement( i - lIndex )->GetStringPtr();
							if ( ! pStrObj || pStrObj->GetType() != OBJ_TYPE_STRING )
							{
								continue;
							}
							CHE_ByteString strTmp = pStrObj->GetString();
							tmpValue = HE_HexStrToValue( strTmp );
							tmpMap->Append( i, (HE_LPVOID)tmpValue );
							lCodeCount++;
						}
					}else if ( wordDes.type == PARSE_WORD_STRING )
					{
						tmpValue = HE_HexStrToValue( wordDes.str );
						if ( lIndexEnd > lMaxIndex )
						{
							lMaxIndex = lIndexEnd;
						}
						for ( HE_DWORD i = lIndex ; i <= lIndexEnd; i++ )
						{
							tmpMap->Append( i, (HE_LPVOID)tmpValue );
							lCodeCount++;
							tmpValue++;
						}
					}
				}
			}
		}
	}
	pFileRead->Release();
	HE_DestoryIHERead( pFileRead );
	stmAcc.Detach();
	return tmpMap;
}


HE_DWORD CHE_PDF_Font::GetWMode() const
{
	if ( mpFontDescriptor )
	{
		return mpFontDescriptor->GetWMode();
	}
	return 0;
}


CHE_PDF_Type0_Font::CHE_PDF_Type0_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Font( fontDict, pAllocator ), mpCIDMap( NULL ), mpUnicodeMap( NULL )
{
	if ( mFontDict )
	{
		CHE_PDF_ObjectPtr objPtr = mFontDict->GetElement( "DescendantFonts", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			objPtr = objPtr->GetArrayPtr()->GetElement( 0, OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				mDescdtFontDict = objPtr->GetDictPtr();
			}
		}
			
		objPtr = mFontDict->GetElement( "Encoding", OBJ_TYPE_NAME );
		if ( objPtr )
		{
			CHE_ByteString str( pAllocator );
			str = objPtr->GetNamePtr()->GetString();
			if ( str == "Identity-H" || str == "Identity-V" )
			{
				//Load Custom Map (map to unicode)
				objPtr = mFontDict->GetElement( "ToUnicode", OBJ_TYPE_REFERENCE );
				if ( objPtr )
				{
				}else if ( mDescdtFontDict )
				{
					objPtr = mDescdtFontDict->GetElement( "CIDSystemInfo", OBJ_TYPE_DICTIONARY );
					if ( objPtr )
					{
						CHE_PDF_DictionaryPtr pCIDSystemInfoDict = objPtr->GetDictPtr();
						objPtr = pCIDSystemInfoDict->GetElement( "Registry", OBJ_TYPE_STRING );
						if ( objPtr )
						{
							CHE_PDF_StringPtr pSt = objPtr->GetStringPtr();
							CHE_ByteString cmapNuame( pAllocator );
							cmapNuame = pSt->GetString();
							cmapNuame += "-";
							objPtr = pCIDSystemInfoDict->GetElement( "Ordering", OBJ_TYPE_STRING );
							if ( objPtr )
							{
								pSt = objPtr->GetStringPtr();
								cmapNuame += pSt->GetString();
								if ( cmapNuame == "Adobe-CNS1" )
								{
									cmapNuame = "Adobe-CNS1-UCS2";
								}else if ( cmapNuame == "Adobe-GB1" )
								{
									cmapNuame = "Adobe-GB1-UCS2";
								}else if ( cmapNuame == "Adobe-Japan1" )
								{
									cmapNuame = "Adobe-Japan1-UCS2";
								}else if ( cmapNuame == "Adobe-Japan2" )
								{
									cmapNuame = "Adobe-Japan2-UCS2";
								}else if ( cmapNuame == "Adobe-Korea1" )
								{
									cmapNuame = "Adobe-Korea1-UCS2";
								}
								mpUnicodeMap = CHE_PDF_CMap::LoadBuildinCMap( cmapNuame, GetAllocator() );
							}
						}
					}
				}
			}else{
				mpCIDMap = CHE_PDF_CMap::LoadBuildinCMap( str, GetAllocator() );
			}
		}
	}	
}


HE_BOOL	CHE_PDF_Type0_Font::GetUnicode( HE_WCHAR charCode, HE_WCHAR & codeRet ) const
{
	if ( mpUnicodeMap )
	{
		HE_DWORD tmpCode = 0;
		if ( mpUnicodeMap->LookupCode( charCode, tmpCode ) )
		{
			codeRet = (HE_DWORD)( tmpCode );
			return TRUE;
		}
		return FALSE;
	}
	if ( mpToUnicodeMap )
	{
		
		codeRet = (HE_DWORD)( mpToUnicodeMap->GetItem( charCode ) );
		return TRUE;
	}
	return FALSE;
}


HE_FLOAT CHE_PDF_Type0_Font::GetWidth( const CHE_PDF_TextItem & item, const CHE_PDF_Matrix & matrix /*= CHE_PDF_Matrix()*/ ) const
{
	CHE_PDF_Matrix tmpMatrix;
	tmpMatrix.a = 0;
	tmpMatrix.b = 0;
	tmpMatrix.c = 0;
	tmpMatrix.d = 0;
	tmpMatrix.e = 0;
	tmpMatrix.f = 0;

	DWORD qureyVal = 0;
	if ( GetEncodingType() == FONT_ENCODING_NONE )
	{
		qureyVal = item.charCode;
	}else if ( GetEncodingType() == FONT_ENCODING_IDENTITY )
	{
		qureyVal = item.charCode;
	}else{
		qureyVal = item.cid;
	}

	//这里去读取W数据，可以优化一下
	if ( mFontDict )
	{
		CHE_PDF_ObjectPtr objPtr = mFontDict->GetElement( "DescendantFonts", OBJ_TYPE_ARRAY );
		if ( objPtr && objPtr->GetArrayPtr()->GetCount() > 0 )
		{
			objPtr = objPtr->GetArrayPtr()->GetElement( 0, OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
				objPtr = dictPtr->GetElement( "W", OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
					HE_BOOL bIndex = FALSE;
					HE_DWORD indexStart = 0;
					HE_DWORD indexEnd = 0;
					CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
					CHE_PDF_ArrayPtr tmpArrayPtr;
					for ( HE_DWORD i = 0; i < arrayPtr->GetCount(); ++i )
					{
						if ( bIndex == FALSE )
						{
							objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
							if ( objPtr )
							{
								indexStart = objPtr->GetNumberPtr()->GetInteger();
								indexEnd = 0;
								bIndex = TRUE;
							}
						}else{
							objPtr = arrayPtr->GetElement( i, OBJ_TYPE_ARRAY );
							if ( objPtr )
							{
								tmpArrayPtr = objPtr->GetArrayPtr();
								if ( qureyVal >= indexStart && qureyVal < indexStart + tmpArrayPtr->GetCount() )
								{
									objPtr = tmpArrayPtr->GetElement( qureyVal - indexStart, OBJ_TYPE_NUMBER );
									if ( objPtr )
									{
										return objPtr->GetNumberPtr()->GetFloat() / 1000;
									}
								}
								bIndex = FALSE;
							}else{
								objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
								indexEnd = objPtr->GetNumberPtr()->GetInteger();
								if ( qureyVal >= indexStart && qureyVal <= indexEnd )
								{
									objPtr = arrayPtr->GetElement( ++i, OBJ_TYPE_NUMBER );
									if ( objPtr )
									{
										return objPtr->GetNumberPtr()->GetFloat() / 1000;
									}
								}
								++i;
								bIndex = FALSE;
							}
						}
					}
				}else{
					objPtr = dictPtr->GetElement( "DW", OBJ_TYPE_NUMBER );
					if ( objPtr )
					{
						return objPtr->GetNumberPtr()->GetFloat() / 1000;
					}else{
						return 1.0;
					}
				}
			}
		}
	}

// 	if ( mFace )
// 	{
// 		FT_Set_Transform( mFace, NULL, NULL );
// 		FT_Error err = FT_Load_Glyph( mFace, item.gid, FT_LOAD_NO_SCALE );
// 		if ( err == 0 )
// 		{
// 			tmpMatrix.a = mFace->glyph->advance.x;
// 			tmpMatrix.d = mFace->glyph->advance.y;
// 		}
// 
// 		tmpMatrix.Concat( matrix );
// 
// 		return tmpMatrix.a * 1.0 / mFace->units_per_EM;
// 	}
	return 1;
}


HE_BOOL CHE_PDF_Type0_Font::GetCID( HE_WCHAR charCode, HE_DWORD & codeRet ) const
{
	if ( mpCIDMap )
	{
		return mpCIDMap->LookupCode( charCode, codeRet );
	}
	return FALSE;
}


CHE_PDF_Type0_Font::~CHE_PDF_Type0_Font()
{
	if ( mpCIDMap )
	{
		mpCIDMap->GetAllocator()->Delete<CHE_PDF_CMap>( mpCIDMap );
	}
	if ( mpUnicodeMap )
	{
		mpUnicodeMap->GetAllocator()->Delete<CHE_PDF_CMap>( mpUnicodeMap );
	}
}


CHE_PDF_Type1_Font::CHE_PDF_Type1_Font( const CHE_PDF_DictionaryPtr & pFontDcit, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Font( pFontDcit ), mFirstChar( 0 ), mLastChar( 255 )
{
	CHE_PDF_ObjectPtr objPtr = mFontDict->GetElement( "FirstChar", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		mFirstChar = objPtr->GetNumberPtr()->GetInteger();
	}

	objPtr = mFontDict->GetElement( "LastChar", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
 		mLastChar = objPtr->GetNumberPtr()->GetInteger();
	}

	for ( HE_DWORD i = 0; i < 255; ++i )
	{
		mCharWidths[i] = 0;
	}

	objPtr = mFontDict->GetElement( "Widths", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		if ( arrayPtr )
		{
			for ( HE_DWORD i = 0; i < arrayPtr->GetCount(); ++i )
			{
				objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
				if ( objPtr && mFirstChar-1+i >= 0 && mFirstChar-1+i  < 256 )
				{
					mCharWidths[mFirstChar-1+i] = objPtr->GetNumberPtr()->GetInteger();
				}
			}
		}
	}
}


CHE_PDF_Type1_Font::~CHE_PDF_Type1_Font()
{
}


HE_BOOL	CHE_PDF_Type1_Font::GetUnicode( HE_WCHAR charCode, HE_WCHAR & codeRet ) const
{
	if ( mEncoding.GetType() != FONT_ENCODING_CUSTOM )
	{
		if ( mEncoding.GetUnicode( (HE_BYTE)charCode, codeRet ) )
		{
			return TRUE;
		}
	}

	if ( mpToUnicodeMap )
	{
		codeRet = (HE_WCHAR)( mpToUnicodeMap->GetItem( charCode ) );
		return TRUE;
	}

	return FALSE;
}


HE_FLOAT CHE_PDF_Type1_Font::GetWidth( const CHE_PDF_TextItem & item, const CHE_PDF_Matrix & matrix /*= CHE_PDF_Matrix()*/ ) const
{
	CHE_PDF_Matrix tmpMatrix;
	tmpMatrix.a = 0;
	tmpMatrix.b = 0;
	tmpMatrix.c = 0;
	tmpMatrix.d = 0;
	tmpMatrix.e = 0;
	tmpMatrix.f = 0;

	if ( item.charCode > 0 && item.charCode < 256 )
	{
		tmpMatrix.a = mCharWidths[item.charCode-1];
		tmpMatrix.d = tmpMatrix.a;
		if ( ( tmpMatrix.a - 0 ) > FLT_EPSILON )
		{
			tmpMatrix.Concat( matrix );
			return tmpMatrix.a / 1000.0;
		}
	}
	if ( mFace )
	{
		FT_Set_Transform( mFace, NULL, NULL );
		FT_Error err = FT_Load_Glyph( mFace, item.gid, FT_LOAD_NO_SCALE );
		if ( err == 0 )
		{
			tmpMatrix.a = mFace->glyph->advance.x;
			tmpMatrix.d = mFace->glyph->advance.y;

			tmpMatrix.Concat( matrix );

			return tmpMatrix.a * 1.0 / mFace->units_per_EM;
		}
	}

	return 0;
}


CHE_PDF_MMType1_Font::CHE_PDF_MMType1_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Type1_Font( pFontDict, pAllocator )
{
}


CHE_PDF_MMType1_Font::~CHE_PDF_MMType1_Font()
{
}


CHE_PDF_TrueType_Font::CHE_PDF_TrueType_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Type1_Font( pFontDict, pAllocator )
{
}


CHE_PDF_TrueType_Font::~CHE_PDF_TrueType_Font()
{
}


CHE_PDF_Type3_Font::CHE_PDF_Type3_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Type1_Font( pFontDict, pAllocator )
{
	if ( pFontDict )
	{
		CHE_PDF_ObjectPtr objPtr = pFontDict->GetElement( "FontBBox", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			mFontBBox = objPtr->GetArrayPtr();
		}
		objPtr = pFontDict->GetElement( "FontMatrix", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
			if ( arrayPtr && arrayPtr->GetCount() >= 6 )
			{
				objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.a = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.b = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.c = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.d = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 4, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.e = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 5, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.f = objPtr->GetNumberPtr()->GetFloat();
				}
			}
		}
		objPtr = pFontDict->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			mResDict = objPtr->GetDictPtr();
		}

		objPtr = pFontDict->GetElement( "CharProcs", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			mCharProcsDict = objPtr->GetDictPtr();
		}

		if ( ! mCharProcsDict )
		{
			return;
		}
		
		CHE_PDF_ArrayPtr encodingDiffArray;
		objPtr = pFontDict->GetElement( "Encoding", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			objPtr = objPtr->GetDictPtr()->GetElement( "Differences", OBJ_TYPE_ARRAY );
			if ( objPtr )
			{
				encodingDiffArray = objPtr->GetArrayPtr();
			}
		}
		if ( encodingDiffArray )
		{
			HE_BYTE code = 0;
			for ( HE_DWORD i = 0; i < encodingDiffArray->GetCount(); ++i )
			{
				objPtr = encodingDiffArray->GetElement( i, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					code = (HE_BYTE)( objPtr->GetNumberPtr()->GetInteger() );
				}else{
					objPtr = encodingDiffArray->GetElement( i, OBJ_TYPE_NAME );
					if ( objPtr )
					{
						objPtr = mCharProcsDict->GetElement( objPtr->GetNamePtr()->GetString(), OBJ_TYPE_STREAM );
						if ( objPtr && code >= 0 && code <= 255 )
						{
							mCharProcsSet[code-1] = objPtr->GetStreamPtr();
						}
					}
					++code;
				}
			}
		}
	}
}


CHE_PDF_Type3_Font::~CHE_PDF_Type3_Font()
{
}


CHE_PDF_ArrayPtr CHE_PDF_Type3_Font::GetFontBBox() const
{
	return mFontBBox;
}


CHE_PDF_Matrix CHE_PDF_Type3_Font::GetFontMatrix() const
{
	return mFontMatrix;
}


CHE_PDF_DictionaryPtr CHE_PDF_Type3_Font::GetResDict() const
{
	return mResDict;
}


CHE_PDF_StreamPtr CHE_PDF_Type3_Font::GetCharProc( HE_BYTE index ) const
{
	if ( index >= mFirstChar && index <= mLastChar )
	{
		return mCharProcsSet[index];
	}
	return CHE_PDF_StreamPtr();
}