#ifndef _CHE_PDF_CONTENTSTRING_H_
#define _CHE_PDF_CONTENTSTRING_H_

#include "../che_datastructure.h"

#include "che_pdf_contentobjs.h"

class CHE_PDF_ContentString
{
public:



	/*
	*	@brief ����ɫ�������ΪPDFָ���ַ���
	*/
	static HE_BOOL ColorToBuf( const CHE_PDF_Color & color, CHE_DynBuffer & buf );



	/*
	*	@brief ����ɫ�ռ��������ΪPDFָ���ַ���
	*/
	static HE_BOOL ColorSpaceToBuf( const CHE_PDF_ColorSpace & colorSpace, CHE_DynBuffer & buf );



	/*
	*	@brief ��PDFҳ�������Ϣ���ΪPDFָ���ַ���
	*/
	static HE_BOOL PdfMatrixToBuf( const CHE_PDF_Matrix & matrix, CHE_DynBuffer & buf );



	/*
	*	@brief ���·������Ĺ���ָ��
	*/
	static HE_BOOL PathDataToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf );



	/*
	*	@brief ��·�������������������ΪPDFָ���ַ���
	*/
	static HE_BOOL PathToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf );



	/*
	*	@brief ����ı������ʼָ��
	*/
	static HE_VOID TextBlockBeginToBuf( CHE_DynBuffer & buf );



	/*
	*	@brief ����ı���������ָ��
	*/
	static HE_VOID TextBlockEndToBuf( CHE_DynBuffer & buf );



	/*
	*	@brief ����ı������PDFָ���ַ���
	*/
	static HE_BOOL TextToBuf( const CHE_PDF_Text * pText, CHE_DynBuffer & buf );



	/*
	*	@brief ���������ͼ������PDFָ���ַ���
	*/
	static HE_BOOL RefImageToBuf( const CHE_PDF_RefImage * pImage, CHE_DynBuffer & buf );



	/*
	*	@brief �������ͼ������PDFָ���ַ���
	*/
	static HE_BOOL InlineImageToBuf( const CHE_PDF_InlineImage * pImage, CHE_DynBuffer & buf );



	/*
	*	@brief ���Shading�����PDFָ���ַ���
	*/
	static HE_BOOL ShadingToBuf( const CHE_PDF_Shading * pShading, CHE_DynBuffer & buf );



	/*
	*	@brief ���Mark Content�����PDFָ���ַ���
	*/
	static HE_BOOL MarkToBuf( const CHE_PDF_Mark * pMark, CHE_DynBuffer & buf );



	/*
	*	@brief ���Form XObject�����PDFָ���ַ���
	*/
	static HE_BOOL FormToBuf( const CHE_PDF_Form * pForm, CHE_DynBuffer & buf );



	/*
	*	@brief ����ı�ͼ��״ָ̬��
	*/
	static HE_BOOL TextStateToBuf(	const CHE_PDF_GState * pTextState, CHE_DynBuffer & buf );



	/*
	*	@brief �Ƚ������ı�ͼ��״̬�Ĳ��죬��������ָ��ʹ�õ�ǰͼ��״̬ΪĿ��ͼ��״̬
	*/
	static HE_BOOL TextStateToBuf(	const CHE_PDF_GState * pCurGSData, const CHE_PDF_GState * pTargetGSData, CHE_DynBuffer & buf );



	/*
	*	@brief ���ExtGStateͼ��״ָ̬��
	*/
	static HE_BOOL ExtGStateToBuf( const CHE_PDF_ExtGState * pExtGState, CHE_DynBuffer & buf );



	/*
	*	@brief �Ƚ�����ExtGStateͼ��״̬�Ĳ��죬��������ָ��ʹ�õ�ǰͼ��״̬ΪĿ��ͼ��״̬
	*/
	static HE_BOOL ExtGStateToBuf( const CHE_PDF_ExtGState * pCurExtGState, const CHE_PDF_ExtGState * pTargetExtGState, CHE_DynBuffer & buf );



	/*
	*	@brief ���ClipStateͼ��״ָ̬��
	*   @remark ��Ϊclip�����ı�clip����path clip��������һ��������ͼ��״̬����Ҳ���ܵ��µ�ǰͼ��״̬����ջ���߸ı�
	*/
	static HE_BOOL ClipStateToBuf( CHE_PDF_Matrix & curMatrix, CHE_PDF_ClipState * pClipState, CHE_DynBuffer & buf, HE_BOOL bInTextBlock = FALSE );



	/*
	*	@brief �Ƚ�����ClipStateͼ��״̬�Ĳ��죬��������ָ��ʹ�õ�ǰͼ��״̬ΪĿ��ͼ��״̬
	*   @remark ��Ϊclip�����ı�clip����path clip��������һ��������ͼ��״̬����Ҳ���ܵ��µ�ǰͼ��״̬����ջ���߸ı�
	*/
	static HE_BOOL ClipStateToBuf(	CHE_PDF_Matrix & curMatrix, CHE_PDF_ClipState * pCurClipState,
									const CHE_PDF_ClipState * pTargetClipState, CHE_DynBuffer & buf, HE_BOOL bInTextBlock = FALSE );



	/*
	*	@brief ���������ͼ��״̬�����ָ���Ҫ��ʱ�̽���ͼ��״̬��ջ�ͳ�ջ����
	*/
	static HE_BOOL GStateToBuf( CHE_PDF_GState * & pGSData, CHE_DynBuffer & buf, CHE_Stack<CHE_PDF_GState*> & gstack, HE_BOOL bInTextBlock = FALSE );



	/*
	*	@brief �����Ƚ�ͼ��״̬�Ĳ��죬��������ָ��ʹ�õ�ǰͼ��״̬ΪĿ��ͼ��״̬����Ҫ��ʱ�̽���ͼ��״̬��ջ�ͳ�ջ����
	*/
	static HE_BOOL GStateToBuf(	CHE_PDF_GState * & pCurGSData, CHE_PDF_GState * & pTargetGSData, CHE_DynBuffer & buf, CHE_Stack<CHE_PDF_GState*> & gstack, HE_BOOL bInTextBlock = FALSE );
}; 

#endif