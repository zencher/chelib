#ifndef _CHE_PDF_CONTENTSTRING_H_
#define _CHE_PDF_CONTENTSTRING_H_

#include "../che_datastructure.h"

#include "che_pdf_contentobjs.h"

class CHE_PDF_ContentString
{
public:



	/*
	*	@brief 将颜色数据输出为PDF指令字符串
	*/
	static HE_BOOL ColorToBuf( const CHE_PDF_Color & color, CHE_DynBuffer & buf );



	/*
	*	@brief 将颜色空间数据输出为PDF指令字符串
	*/
	static HE_BOOL ColorSpaceToBuf( const CHE_PDF_ColorSpace & colorSpace, CHE_DynBuffer & buf );



	/*
	*	@brief 将PDF页面矩阵信息输出为PDF指令字符串
	*/
	static HE_BOOL PdfMatrixToBuf( const CHE_PDF_Matrix & matrix, CHE_DynBuffer & buf );



	/*
	*	@brief 输出路径对象的构造指令
	*/
	static HE_BOOL PathDataToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf );



	/*
	*	@brief 将路径对象数据完整的输出为PDF指令字符串
	*/
	static HE_BOOL PathToBuf( const CHE_PDF_Path * pPath, CHE_DynBuffer & buf );



	/*
	*	@brief 输出文本块对象开始指令
	*/
	static HE_VOID TextBlockBeginToBuf( CHE_DynBuffer & buf );



	/*
	*	@brief 输出文本块对象结束指令
	*/
	static HE_VOID TextBlockEndToBuf( CHE_DynBuffer & buf );



	/*
	*	@brief 输出文本对象的PDF指令字符串
	*/
	static HE_BOOL TextToBuf( const CHE_PDF_Text * pText, CHE_DynBuffer & buf );



	/*
	*	@brief 输出非内联图像对象的PDF指令字符串
	*/
	static HE_BOOL RefImageToBuf( const CHE_PDF_RefImage * pImage, CHE_DynBuffer & buf );



	/*
	*	@brief 输出内联图像对象的PDF指令字符串
	*/
	static HE_BOOL InlineImageToBuf( const CHE_PDF_InlineImage * pImage, CHE_DynBuffer & buf );



	/*
	*	@brief 输出Shading对象的PDF指令字符串
	*/
	static HE_BOOL ShadingToBuf( const CHE_PDF_Shading * pShading, CHE_DynBuffer & buf );



	/*
	*	@brief 输出Mark Content对象的PDF指令字符串
	*/
	static HE_BOOL MarkToBuf( const CHE_PDF_Mark * pMark, CHE_DynBuffer & buf );



	/*
	*	@brief 输出Form XObject对象的PDF指令字符串
	*/
	static HE_BOOL FormToBuf( const CHE_PDF_Form * pForm, CHE_DynBuffer & buf );



	/*
	*	@brief 输出文本图形状态指令
	*/
	static HE_BOOL TextStateToBuf(	const CHE_PDF_GState * pTextState, CHE_DynBuffer & buf );



	/*
	*	@brief 比较两个文本图形状态的差异，并输出相关指令使得当前图形状态为目标图形状态
	*/
	static HE_BOOL TextStateToBuf(	const CHE_PDF_GState * pCurGSData, const CHE_PDF_GState * pTargetGSData, CHE_DynBuffer & buf );



	/*
	*	@brief 输出ExtGState图形状态指令
	*/
	static HE_BOOL ExtGStateToBuf( const CHE_PDF_ExtGState * pExtGState, CHE_DynBuffer & buf );



	/*
	*	@brief 比较两个ExtGState图形状态的差异，并输出相关指令使得当前图形状态为目标图形状态
	*/
	static HE_BOOL ExtGStateToBuf( const CHE_PDF_ExtGState * pCurExtGState, const CHE_PDF_ExtGState * pTargetExtGState, CHE_DynBuffer & buf );



	/*
	*	@brief 输出ClipState图形状态指令
	*   @remark 因为clip对象（文本clip或者path clip）都存在一个独立的图形状态对象，也可能导致当前图形状态的入栈或者改变
	*/
	static HE_BOOL ClipStateToBuf( CHE_PDF_Matrix & curMatrix, CHE_PDF_ClipState * pClipState, CHE_DynBuffer & buf, HE_BOOL bInTextBlock = FALSE );



	/*
	*	@brief 比较两个ClipState图形状态的差异，并输出相关指令使得当前图形状态为目标图形状态
	*   @remark 因为clip对象（文本clip或者path clip）都存在一个独立的图形状态对象，也可能导致当前图形状态的入栈或者改变
	*/
	static HE_BOOL ClipStateToBuf(	CHE_PDF_Matrix & curMatrix, CHE_PDF_ClipState * pCurClipState,
									const CHE_PDF_ClipState * pTargetClipState, CHE_DynBuffer & buf, HE_BOOL bInTextBlock = FALSE );



	/*
	*	@brief 完整的输出图形状态对象的指令，必要的时刻进行图形状态入栈和出栈操作
	*/
	static HE_BOOL GStateToBuf( CHE_PDF_GState * & pGSData, CHE_DynBuffer & buf, CHE_Stack<CHE_PDF_GState*> & gstack, HE_BOOL bInTextBlock = FALSE );



	/*
	*	@brief 完整比较图形状态的差异，并输出相关指令使得当前图形状态为目标图形状态，必要的时刻进行图形状态入栈和出栈操作
	*/
	static HE_BOOL GStateToBuf(	CHE_PDF_GState * & pCurGSData, CHE_PDF_GState * & pTargetGSData, CHE_DynBuffer & buf, CHE_Stack<CHE_PDF_GState*> & gstack, HE_BOOL bInTextBlock = FALSE );
}; 

#endif