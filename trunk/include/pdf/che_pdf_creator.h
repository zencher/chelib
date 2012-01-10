#ifndef _CHE_PDF_CREATOR_H_
#define _CHE_PDF_CREATOR_H_

#include "../che_base.h"
#include "che_pdf_define.h"
#include "che_pdf_objects.h"
#include "che_pdf_parser.h"
#include "che_pdf_collector.h"

class CHE_PDF_Creator : public CHE_Object
{
public:
	CHE_PDF_Creator( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Creator();

	/**************   Document Operations   *************/

 	HE_BOOL					NewDocument();
	HE_BOOL					SetVersion( PDF_VERSION version );
	HE_BOOL					SetDocumentInfo( PDF_DOCUMENT_INFO infoType, CHE_ByteString & str );
	HE_BOOL					SetDocumentParam();
	HE_BOOL					Save( IHE_Write * pWrite );

	HE_BOOL					SaveUpdate( IHE_Write * pWrite, CHE_PDF_Parser * pParser );


	/**************   Page Operations   **************/

	CHE_PDF_Dictionary*		NewPage( HE_DWORD width, HE_DWORD height );

	HE_DWORD				GetPageCount() const { return m_arrPageDict.GetCount(); }
	CHE_PDF_Dictionary*		GetPageDict( HE_DWORD pageIndex ) const { return (CHE_PDF_Dictionary*)m_arrPageDict.GetItem( pageIndex ); }

	//�����ֵ�����ͽ��д������ܵ���ӵ�ҳ����ֵ䡣�����壬״̬��xobject�ȵ���Դ��//����ֵ�������κ���Դ���ֵ䣬����xobjcet stream���ֵ�
	HE_BOOL					SetAsPageResource( HE_DWORD pageIndex, CHE_PDF_Dictionary * pDict );

	//HE_BOOL SetAsPageContent( HE_DWORD pageIndex, CHE_PDF_Stream * pStream );
	//HE_BOOL SetAsPageContent( HE_DWORD pageIndex, CHE_PDF_Array * pArray );

	CHE_PDF_Stream*			AddStreamAsPageContents( HE_DWORD pageIndex );

	/************* Document Level Resources Operations *************/

// 	CHE_PDF_Dictionary*		AddType1Font_Standard14( HE_BYTE fontType, PDF_FONT_ENCODING encoding = FONT_ENCODING_WINANSI );
// 	
// 	/* AddType1Font ���һ��Type1 ���壬�������в���ȷ��������ص���Ϣ���������Widths, */
// 	/* FontDescriptor, ToUnicode ����Ϣ���������Ƚ���Щ��������ĵ����Ա��ڴ��������ֵ��ʱ�� */
// 	/* ������ȷ������Щ���� */
// 	CHE_PDF_Dictionary *	AddType1Font(	const CHE_ByteString & baseFont, HE_DWORD firstChar = 0, HE_DWORD lastChar = 255,
// 											PDF_FONT_ENCODING encoding = FONT_ENCODING_WINANSI, 
// 											/*const*/ CHE_PDF_Array * pWidths = NULL, /*const*/ CHE_PDF_Dictionary * pFontDescriptor = NULL,
// 											/*const*/ CHE_PDF_Stream * pToUnicode = NULL );
// 
// 	/* AddTrueTypeFont ���һ��TrueType ���壬�������в���ȷ��������ص���Ϣ���������Widths, */
// 	/* FontDescriptor, ToUnicode ����Ϣ���������Ƚ���Щ��������ĵ����Ա��ڴ��������ֵ��ʱ�� */
// 	/* ������ȷ������Щ���� */
// 	CHE_PDF_Dictionary*		AddTrueTypeFont(	const CHE_ByteString & baseFont, HE_DWORD firstChar = 0, HE_DWORD lastChar = 255,
// 												PDF_FONT_ENCODING encoding = FONT_ENCODING_WINANSI, 
// 												/*const*/ CHE_PDF_Array * pWidths = NULL, /*const*/ CHE_PDF_Dictionary * pFontDescriptor = NULL,
// 												/*const*/ CHE_PDF_Stream * pToUnicode = NULL );
// 
// 	/* AddTrueTypeFont ��һ�������ļ�ȥ��ȡ��ص�������Ϣ������һ�������ֵ䣬���뵽�ĵ��� */
// 	CHE_PDF_Dictionary*		AddTrueTypeFont( const char * pFontFile, HE_BOOL bEmbed = FALSE );


	CHE_PDF_Dictionary *	AddDictionary() { return AppendIndirectObj_Dict()->GetObj()->ToDict(); }
	CHE_PDF_Stream *		AddStream() { return AppendIndirectObj_Stream()->GetObj()->ToStream(); }

	CHE_PDF_IndirectObject*	AppendIndirectObj_Null();
	CHE_PDF_IndirectObject*	AppendIndirectObj_Boolean( HE_BOOL value );
	CHE_PDF_IndirectObject*	AppendIndirectObj_Number( HE_INT32 value );
	CHE_PDF_IndirectObject*	AppendIndirectObj_Number( HE_FLOAT value );
	CHE_PDF_IndirectObject*	AppendIndirectObj_String( CHE_ByteString & str );
	CHE_PDF_IndirectObject*	AppendIndirectObj_Name( CHE_ByteString & str );
	CHE_PDF_IndirectObject*	AppendIndirectObj_Array();
	CHE_PDF_IndirectObject*	AppendIndirectObj_Dict();
	CHE_PDF_IndirectObject*	AppendIndirectObj_Stream();

	HE_BOOL					SetEncryption(	const CHE_ByteString id, const CHE_ByteString userPassword, const CHE_ByteString ownerPassword,
											HE_BYTE algorithm, HE_BYTE keyLength, HE_BYTE revision,  HE_BOOL bMetaData, HE_DWORD pValue );

private:
	HE_VOID					ResetCreator();

	HE_VOID					ResetObjIndex() { m_dwObjNumIndex = 0; }
	HE_VOID					IncreaseObjIndex() { m_dwObjNumIndex++; }
	HE_VOID					DecreaseObjIndex() { m_dwObjNumIndex--; }
	HE_DWORD				GetObjIndex() { return m_dwObjNumIndex; }

	HE_VOID					OutPutObject( CHE_PDF_Object * pObj, IHE_Write * pWrite );

private:
	HE_BOOL					m_bNewDocument;	//�Ƿ�ʼ��һ��PDF�ļ������ı�־

	PDF_VERSION				m_Version;

	HE_DWORD				m_dwPageObjNum;
	HE_DWORD				m_dwCatalogObjNum;
	HE_DWORD				m_dwEncryptObjNum;
	HE_DWORD				m_dwInfoObjNum;
	CHE_PDF_Dictionary *	m_pCatalogDict;
	CHE_PDF_Dictionary *	m_pPagesDict;
	CHE_PDF_Dictionary *	m_pEncryptDict;
	CHE_PDF_Dictionary *	m_pInfoDict;

	CHE_PDF_Encrypt *		m_pEncrypt;

	HE_DWORD				m_dwObjNumIndex;

	CHE_PtrArray			m_arrPageDict;	//���ҳ���ֵ䣬���ڿ��ٷ���
	CHE_PDF_Collector		m_collector;	//��Ӷ�����ռ�������ʱ���
};

#endif