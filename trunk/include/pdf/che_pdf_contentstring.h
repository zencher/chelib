#ifndef _CHE_PDF_CONTENTSTRING_H_
#define _CHE_PDF_CONTENTSTRING_H_

#include "che_pdf_contentobjs.h"

class CHE_PDF_ContentString
{
public:



	/*
	*	@brief ����ɫ�������ΪPDFָ���ַ���
	*/
	static bool ColorToBuf( const CHE_PDF_Color & color, CHE_DynBuffer & buf );



	/*
	*	@brief ����ɫ�ռ��������ΪPDFָ���ַ���
	*/
	static bool ColorSpaceToBuf( const CHE_PDF_ColorSpace & colorSpace, CHE_DynBuffer & buf );



	/*
	*	@brief ��PDFҳ�������Ϣ���ΪPDFָ���ַ���
	*/
	static bool PdfMatrixToBuf( const CHE_Matrix & matrix, CHE_DynBuffer & buf );



	/*
	*	@brief ���·������Ĺ���ָ��
	*/
	static bool PathDataToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf );



	/*
	*	@brief ��·�������������������ΪPDFָ���ַ���
	*/
	static bool PathToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf );



	/*
	*	@brief ����ı������ʼָ��
	*/
	static void TextBlockBeginToBuf( CHE_DynBuffer & buf );



	/*
	*	@brief ����ı���������ָ��
	*/
	static void TextBlockEndToBuf( CHE_DynBuffer & buf );



	/*
	*	@brief ����ı������PDFָ���ַ���
	*/
	static bool TextToBuf( const CHE_PDF_Text * pText, CHE_DynBuffer & buf );



	/*
	*	@brief ���������ͼ������PDFָ���ַ���
	*/
	static bool RefImageToBuf( const CHE_PDF_RefImage * pImage, CHE_DynBuffer & buf );



	/*
	*	@brief �������ͼ������PDFָ���ַ���
	*/
	static bool InlineImageToBuf( const CHE_PDF_InlineImage * pImage, CHE_DynBuffer & buf );



	/*
	*	@brief ���Shading�����PDFָ���ַ���
	*/
	static bool ShadingToBuf( const CHE_PDF_Shading * pShading, CHE_DynBuffer & buf );



	/*
	*	@brief ���Mark Content�����PDFָ���ַ���
	*/
	static bool MarkToBuf( const CHE_PDF_Mark * pMark, CHE_DynBuffer & buf );



	/*
	*	@brief ���Form XObject�����PDFָ���ַ���
	*/
	static bool FormToBuf( const CHE_PDF_Form * pForm, CHE_DynBuffer & buf );



	/*
	*	@brief ����ı�ͼ��״ָ̬��
	*/
	static bool TextStateToBuf(	const CHE_PDF_GState * pTextState, CHE_DynBuffer & buf, bool bNewBlock = false );



	/*
	*	@brief �Ƚ������ı�ͼ��״̬�Ĳ��죬��������ָ��ʹ�õ�ǰͼ��״̬ΪĿ��ͼ��״̬
	*/
	static bool TextStateToBuf(	const CHE_PDF_GState * pCurGSData, const CHE_PDF_GState * pTargetGSData, CHE_DynBuffer & buf, bool bNewBlock = false );



	/*
	*	@brief ���ExtGStateͼ��״ָ̬��
	*/
	static bool ExtGStateToBuf( const CHE_PDF_ExtGState * pExtGState, CHE_DynBuffer & buf );



	/*
	*	@brief �Ƚ�����ExtGStateͼ��״̬�Ĳ��죬��������ָ��ʹ�õ�ǰͼ��״̬ΪĿ��ͼ��״̬
	*/
	static bool ExtGStateToBuf( const CHE_PDF_ExtGState * pCurExtGState, const CHE_PDF_ExtGState * pTargetExtGState, CHE_DynBuffer & buf );



	/*
	*	@brief ���ClipStateͼ��״ָ̬��
	*   @remark ��Ϊclip�����ı�clip����path clip��������һ��������ͼ��״̬����Ҳ���ܵ��µ�ǰͼ��״̬����ջ���߸ı�
	*/
	static bool ClipStateToBuf( CHE_Matrix & curMatrix, CHE_PDF_ClipState * pClipState, CHE_DynBuffer & buf, bool bInTextBlock = false );



	/*
	*	@brief �Ƚ�����ClipStateͼ��״̬�Ĳ��죬��������ָ��ʹ�õ�ǰͼ��״̬ΪĿ��ͼ��״̬
	*   @remark ��Ϊclip�����ı�clip����path clip��������һ��������ͼ��״̬����Ҳ���ܵ��µ�ǰͼ��״̬����ջ���߸ı�
	*/
	static bool ClipStateToBuf(	CHE_Matrix & curMatrix, CHE_PDF_ClipState * pCurClipState,
									const CHE_PDF_ClipState * pTargetClipState, CHE_DynBuffer & buf, bool bInTextBlock = false );



	/*
	*	@brief ���������ͼ��״̬�����ָ���Ҫ��ʱ�̽���ͼ��״̬��ջ�ͳ�ջ����
	*/
	static bool GStateToBuf( CHE_PDF_GState * & pGSData, CHE_DynBuffer & buf, CHE_Stack<CHE_PDF_GState*> & gstack, bool bInTextBlock = false );



	/*
	*	@brief �����Ƚ�ͼ��״̬�Ĳ��죬��������ָ��ʹ�õ�ǰͼ��״̬ΪĿ��ͼ��״̬����Ҫ��ʱ�̽���ͼ��״̬��ջ�ͳ�ջ����
	*/
	static bool GStateToBuf(	CHE_PDF_GState * & pCurGSData, CHE_PDF_GState * & pTargetGSData, CHE_DynBuffer & buf, CHE_Stack<CHE_PDF_GState*> & gstack, bool bInTextBlock = false );
}; 

#endif