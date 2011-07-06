#ifndef _CHE_PDF_CREATOR_H_
#define _CHE_PDF_CREATOR_H_

#include "../che_base.h"
#include "che_pdf_collector.h"
#include "che_pdf_objects.h"

#define FONT_TYPE1_STANDARD14_TIMES_ROMAN			0
#define FONT_TYPE1_STANDARD14_TIMES_BOLD			1
#define FONT_TYPE1_STANDARD14_TIMES_ITALIC			2
#define FONT_TYPE1_STANDARD14_TIMES_BOLDITALIC		3
#define FONT_TYPE1_STANDARD14_HELVETICA				4
#define FONT_TYPE1_STANDARD14_HELVETICA_BOLD		5
#define FONT_TYPE1_STANDARD14_HELVETICA_OBILQUE		6
#define FONT_TYPE1_STANDARD14_HELVETICA_BOLDOBILQUE	7
#define FONT_TYPE1_STANDARD14_COURIER				8
#define FONT_TYPE1_STANDARD14_COURIER_BOLD			9
#define FONT_TYPE1_STANDARD14_COURIER_OBILQUE		10
#define FONT_TYPE1_STANDARD14_COURIER_BOLDOBILQUE	11
#define FONT_TYPE1_STANDARD14_SYMBOL				12
#define FONT_TYPE1_STANDARD14_ZAPFDINGBATS			13

#define ENCODING_WINANSIENCODING					1
#define ENCODING_MACROMANENCODING					2
#define ENCODING_MACEXPERTENCODING					3

#define DOCUMENT_INFO_TITLE							0
#define DOCUMENT_INFO_AUTHOR						1
#define DOCUMENT_INFO_SUBJECT						2
#define DOCUMENT_INFO_KEYWORDS						3
#define DOCUMENT_INFO_CREATOR						4
#define DOCUMENT_INFO_PRODUCER						5
#define DOCUMENT_INFO_CREATIONDATE					6
#define DOCUMENT_INFO_MODDATE						7
#define DOCUMENT_INFO_TRAPPED						8

class CHE_PDF_Creator : public CHE_Object
{
public:
	CHE_PDF_Creator( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Creator();

	/**************   Document Operations   *************/

 	HE_BOOL					NewDocument();
	HE_BOOL					SetDocumentInfo( HE_BYTE infoType, CHE_ByteString & str );
	HE_BOOL					SetDocumentParam();
	HE_BOOL					Save( IHE_Write * pWrite );


	/**************   Page Operations   **************/

	CHE_PDF_Dictionary*		NewPage( HE_DWORD width, HE_DWORD height );

	HE_DWORD				GetPageCount() const { return m_arrPageDict.GetCount(); }
	CHE_PDF_Dictionary*		GetPageDict( HE_DWORD pageIndex ) const { return (CHE_PDF_Dictionary*)m_arrPageDict.GetItem( pageIndex ); }

	//依据字典的类型进行处理，智能的添加到页面的字典。（字体，状态、xobject等等资源）//这个字典可以是任何资源的字典，包括xobjcet stream的字典
	HE_BOOL					SetAsPageResource( HE_DWORD pageIndex, const CHE_PDF_Dictionary * pDict );

	//HE_BOOL SetAsPageContent( HE_DWORD pageIndex, CHE_PDF_Stream * pStream );
	//HE_BOOL SetAsPageContent( HE_DWORD pageIndex, CHE_PDF_Array * pArray );

	CHE_PDF_Stream*			AddStreamAsPageContents( HE_DWORD pageIndex );

	/************* Document Level Resources Operations *************/

	CHE_PDF_Dictionary*		AddType1Font_Standard14( HE_BYTE fontType, HE_BYTE Encoding = ENCODING_WINANSIENCODING );
	
	/* AddType1Font 添加一个Type1 字体，必须自行并正确的设置相关的信息，如果包含Widths, */
	/* FontDescriptor, ToUnicode 等信息，必须首先将这些对象插入文档，以便在创建字体字典的时候 */
	/* 可以正确引用这些对象 */
	CHE_PDF_Dictionary *	AddType1Font(	const CHE_ByteString & baseFont, HE_DWORD firstChar = 0, HE_DWORD lastChar = 255,
											HE_BYTE Encoding = ENCODING_WINANSIENCODING, 
											const CHE_PDF_Array * pWidths = NULL, const CHE_PDF_Dictionary * pFontDescriptor = NULL,
											const CHE_PDF_Stream * pToUnicode = NULL );

	/* AddTrueTypeFont 添加一个TrueType 字体，必须自行并正确的设置相关的信息，如果包含Widths, */
	/* FontDescriptor, ToUnicode 等信息，必须首先将这些对象插入文档，以便在创建字体字典的时候 */
	/* 可以正确引用这些对象 */
	CHE_PDF_Dictionary*		AddTrueTypeFont(	const CHE_ByteString & baseFont, HE_DWORD firstChar = 0, HE_DWORD lastChar = 255,
												HE_BYTE Encoding = ENCODING_WINANSIENCODING, 
												const CHE_PDF_Array * pWidths = NULL, const CHE_PDF_Dictionary * pFontDescriptor = NULL,
												const CHE_PDF_Stream * pToUnicode = NULL );

	/* AddTrueTypeFont 从一个字体文件去获取相关的字体信息并创建一个字体字典，加入到文档中 */
	CHE_PDF_Dictionary*		AddTrueTypeFont( const char * pFontFile, HE_BOOL bEmbed = FALSE );


	CHE_PDF_Dictionary *	AddDictionary() { return AppendIndirectObj_Dict(); }
	CHE_PDF_Stream *		AddStream() { return AppendIndirectObj_Stream(); }

private:
	HE_VOID					ResetCreator();

	HE_VOID					ResetObjIndex() { m_dwObjNumIndex = 0; }
	HE_VOID					IncreaseObjIndex() { m_dwObjNumIndex++; }
	HE_VOID					DecreaseObjIndex() { m_dwObjNumIndex--; }
	HE_DWORD				GetObjIndex() { return m_dwObjNumIndex; }

	CHE_PDF_Null*			AppendIndirectObj_Null();
	CHE_PDF_Boolean*		AppendIndirectObj_Boolean( HE_BOOL value );
	CHE_PDF_Number*			AppendIndirectObj_Number( HE_INT32 value );
	CHE_PDF_Number*			AppendIndirectObj_Number( HE_FLOAT value );
	CHE_PDF_String*			AppendIndirectObj_String( CHE_ByteString & str );
	CHE_PDF_Name*			AppendIndirectObj_Name( CHE_ByteString & str );
	CHE_PDF_Array*			AppendIndirectObj_Array();
	CHE_PDF_Dictionary*		AppendIndirectObj_Dict();
	CHE_PDF_Stream*			AppendIndirectObj_Stream();

	HE_VOID					OutPutObject( CHE_PDF_Object * pObj, IHE_Write * pWrite );

private:
	HE_BOOL					m_bNewDocument;	//是否开始了一个PDF文件创建的标志

	CHE_PDF_Dictionary *	m_pCatalogDict;
	CHE_PDF_Dictionary *	m_pPagesDict;
	CHE_PDF_Dictionary *	m_pEncryptDict;
	CHE_PDF_Dictionary *	m_pInfoDict;

	HE_DWORD				m_dwObjNumIndex;

	CHE_PtrArray			m_arrPageDict;	//存放页面字典，便于快速访问
	CHE_PDF_Collector		m_collector;	//间接对象的收集器。暂时存放
};

#endif