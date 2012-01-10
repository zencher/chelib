#ifndef _CHE_PDF_DEFINE_H_
#define _CHE_PDF_DEFINE_H_

enum PDF_VERSION 
{
	PDF_VERSION_UNKNOWN = 0,
	PDF_VERSION_1_0 = 10,
	PDF_VERSION_1_1 = 11,
	PDF_VERSION_1_2 = 12,
	PDF_VERSION_1_3 = 13,
	PDF_VERSION_1_4 = 14,
	PDF_VERSION_1_5 = 15,
	PDF_VERSION_1_6 = 16,
	PDF_VERSION_1_7 = 17
};

enum PDF_OBJ_TYPE
{
	OBJ_TYPE_INVALID	= 0x00,
	OBJ_TYPE_NULL		= 0x01,
	OBJ_TYPE_BOOLEAN	= 0x02,
	OBJ_TYPE_NUMBER		= 0x03,
	OBJ_TYPE_STRING		= 0x04,
	OBJ_TYPE_NAME		= 0x05,
	OBJ_TYPE_ARRAY		= 0x06,
	OBJ_TYPE_DICTIONARY	= 0x07,
	OBJ_TYPE_STREAM		= 0x08,
	OBJ_TYPE_REFERENCE	= 0x09,
	OBJ_TYPE_
};

enum PDF_STREAM_FILTER
{
	STREAM_FILTER_NULL			= 0x00,
	STREAM_FILTER_HEX			= 0x01,
	STREAM_FILTER_ASCII85		= 0x02,
	STREAM_FILTER_FLATE			= 0x03,
	STREAM_FILTER_LZW			= 0x04,
	STREAM_FILTER_RLE			= 0x05
};

enum PDF_XREF_ENTRY_TYPE
{
	XREF_ENTRY_TYPE_FREE		= 0x00,
	XREF_ENTRY_TYPE_COMMON		= 0x01,
	XREF_ENTRY_TYPE_COMPRESSED	= 0x02

};

enum PDF_OBJ_STATUS
{
	OBJ_STATUS_LOAD				= 0x00,
	OBJ_STATUS_UPDATE			= 0x01,
	//OBJ_STATUS_NEW			= 0x02,
	OBJ_STATUS_UNLOAD			= 0x02
};

enum PDF_PARSE_WORD
{
	PARSE_WORD_UNKNOWN	= 0x00,
	PARSE_WORD_INTEGER	= 0x01,
	PARSE_WORD_FLOAT	= 0x02,
	PARSE_WORD_STRING	= 0x03,
	PARSE_WORD_NAME		= 0x04,
	PARSE_WORD_DICT_B	= 0x05,
	PARSE_WORD_DICT_E	= 0x06,
	PARSE_WORD_ARRAY_B	= 0x07,
	PARSE_WORD_ARRAY_E	= 0x08
};

enum PDF_COLORSPACE_TYPE
{
	COLORSAPCE_DEVICE	= 0x00,
	COLORSPACE_CIEBASE	= 0x01,
	COLORSPACE_SPECIAL	= 0x02
};

enum PDF_COLORSPACE
{
	COLORSAPCE_DEVICE_GRAY			= 0x01,
	COLORSAPCE_DEVICE_RGB			= 0x02,
	COLORSAPCE_DEVICE_CMYK			= 0x03,
	COLORSAPCE_CIEBASE_CALCRAY		= 0x04,
	COLORSAPCE_CIEBASE_CALRGB		= 0x05,
	COLORSAPCE_CIEBASE_CALLAB		= 0x06,
	COLORSAPCE_CIEBASE_ICCBASED		= 0x07,
	COLORSAPCE_SPECIAL_PATTERN		= 0x08,
	COLORSAPCE_SPECIAL_INDEXED		= 0x09,
	COLORSAPCE_SPECIAL_SEPARATION	= 0x0A,
	COLORSAPCE_SPECAIL_DEVICEN		= 0x0B,
};

enum PDF_PATH_TYPE
{
	PATH_NOOP			= 0x00,
	PATH_STROKE			= 0x01,
	PATH_FILL			= 0x02,
	PATH_FILLSTROKE		= 0x03,
	PATH_NOOP_CLIP		= 0x04,
	PATH_STROKE_CLIP	= 0x05,
	PATH_FILL_CLIP		= 0x06,
	PATH_FILLSTROKE_CLIP= 0x07
};

// enum PDF_FILL_MODE
// {
// 	FILL_MODE_NOZERO	= 0x00,
// 	FILL_MODE_EVERODD	= 0x01
// };

enum PDF_ORDER_TYPE
{
	ORDER_UNKNOWN		= 0x00,
	ORDER_INIT_STATE	= 0x01,

	//General graphics state
	ORDER_LINE_WIDTH	= 0x02,
	ORDER_LINE_CAP		= 0x03,
	ORDER_LINE_JION		= 0x04,
	ORDER_MITER_LIMIT	= 0x05,
	ORDER_DASH_PATTERN	= 0x06,
	ORDER_INTENT		= 0x07,
	ORDER_FALTNESS		= 0x08,
	ORDER_SET_STATE		= 0x09,

	//Special graphics state
	ORDER_PUSH_STATE	= 0x0A,
	ORDER_POP_STATE		= 0x0B,
	ORDER_MATRIX		= 0x0C,

	//Color
	ORDER_STROKE_COLOR	= 0x0D,
	ORDER_FILL_COLOR	= 0x0E
};

enum PDF_FONT_TYPE
{
	FONT_TYPE0			= 0x00,
	FONT_TYPE1			= 0x01,
	FONT_TRUETYPE		= 0x02,
	FONT_TYPE3			= 0x03
};

enum PDF_FONT_TYPE1_STANDARD14
{
	FONT_TYPE1_STANDARD14_TIMES_ROMAN			= 0x00,
	FONT_TYPE1_STANDARD14_TIMES_BOLD			= 0x01,
	FONT_TYPE1_STANDARD14_TIMES_ITALIC			= 0x02,
	FONT_TYPE1_STANDARD14_TIMES_BOLDITALIC		= 0x03,
	FONT_TYPE1_STANDARD14_HELVETICA				= 0x04,
	FONT_TYPE1_STANDARD14_HELVETICA_BOLD		= 0x05,
	FONT_TYPE1_STANDARD14_HELVETICA_OBILQUE		= 0x06,
	FONT_TYPE1_STANDARD14_HELVETICA_BOLDOBILQUE	= 0x07,
	FONT_TYPE1_STANDARD14_COURIER				= 0x08,
	FONT_TYPE1_STANDARD14_COURIER_BOLD			= 0x09,
	FONT_TYPE1_STANDARD14_COURIER_OBILQUE		= 0x0a,
	FONT_TYPE1_STANDARD14_COURIER_BOLDOBILQUE	= 0x0b,
	FONT_TYPE1_STANDARD14_SYMBOL				= 0x0c,
	FONT_TYPE1_STANDARD14_ZAPFDINGBATS			= 0x0d
};

enum PDF_FONT_ENCODING
{
	FONT_ENCODING_NONE				= 0x00,
	FONT_ENCODING_STANDARD			= 0x01,
	FONT_ENCODING_PDFDOC			= 0x02,
	FONT_ENCODING_WINANSI			= 0x03,
	FONT_ENCODING_MACROMAN			= 0x04,
	FONT_ENCODING_MACEXPERT			= 0x05,
	FONT_ENCODING_GBK				= 0x06,
	FONT_ENCODING_UCS2				= 0x07,
	FONT_ENCODING_UTF16				= 0x08,
	FONT_ENCODING_SELFDEF			= 0x09,
	FONT_ENCODING_OTHER				= 0x0A
};

enum PDF_DOCUMENT_INFO
{
	DOCUMENT_INFO_TITLE				= 0x00,
	DOCUMENT_INFO_AUTHOR			= 0x01,
	DOCUMENT_INFO_SUBJECT			= 0x02,
	DOCUMENT_INFO_KEYWORDS			= 0x03,
	DOCUMENT_INFO_CREATOR			= 0x04,
	DOCUMENT_INFO_PRODUCER			= 0x05,
	DOCUMENT_INFO_CREATIONDATE		= 0x06,
	DOCUMENT_INFO_MODDATE			= 0x07,
	DOCUMENT_INFO_TRAPPED			= 0x08
};

#endif