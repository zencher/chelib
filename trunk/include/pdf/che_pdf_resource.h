#ifndef _CHE_PDF_RESOURCE_H_
#define _CHE_PDF_RESOURCE_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_pages.h"

#define PDFFONT_UNKNOWN			0
#define PDFFONT_TYPE0			1
#define PDFFONT_TYPE1			2
#define PDFFONT_TRUETYPE		3
#define PDFFONT_TYPE3			4

#define PDFENCODING_STANDARD	1
#define PDFENCODING_WINANSI		2
#define	PDFENCODING_MACROMAN	3
#define PDFENCODING_MACEXPERT	4
#define PDFENCODING_PDFDOC		5
#define PDFENCODING_GBK			6
#define PDFENCODING_OTHER		7

class CHE_PDF_FontCharCodeMgr : public CHE_Object
{
public:
	CHE_PDF_FontCharCodeMgr( CHE_PDF_Page * pPage, CHE_PDF_Dictionary * pFontDict );
	~CHE_PDF_FontCharCodeMgr();

	HE_WCHAR	GetUnicode( HE_WCHAR wch );

	HE_BYTE		GetEncodingType() { return m_EncodingType; }
	HE_BYTE		GetFontType() { return m_FontType; }
	HE_BOOL		IsDefaultEncoding() { return m_bDefaultEncoding; }

private:
	CHE_PDF_Page *			m_pPage;
	CHE_PDF_Dictionary *	m_pFontDict;
	HE_WCHAR *				m_pUnicodeTable;

	HE_BYTE					m_EncodingType;
	HE_BYTE					m_FontType;
	HE_BOOL					m_bDefaultEncoding;
};

// #define PDFFONT_TYPE1			1
// #define PDFFONT_TRUETYPE			2
// #define PDFFONT_TYPE3			3
// #define PDFFONT_CIDFONT			4
// 
// class CHE_PDF_Font : public CHE_Object
// {
// public:
// 	static CHE_PDF_Font*		CreateFontF( CHE_PDF_Document* pDoc, CHE_PDF_Dictionary* pFontDict );
// 
// 	virtual ~CHE_PDF_Font();
// 	
// 	HE_DWORD					GetFontType() const { return m_FontType; }
// 
// 	CHE_ByteString				GetFontTypeName() const;
// 	
// 	CHE_ByteString				GetBaseFont() const { return m_BaseFont; }
// 
// //	CPDF_Type1Font*			GetType1Font() const { return m_FontType == PDFFONT_TYPE1 ? (CPDF_Type1Font*)(void*)this : NULL; }
// //	/** Get a truetype font. */
// //	CPDF_TrueTypeFont*		GetTrueTypeFont() const { return m_FontType == PDFFONT_TRUETYPE ? (CPDF_TrueTypeFont*)(void*)this : NULL; }
// //	/** Get a CID font. */
// //	CPDF_CIDFont*			GetCIDFont() const { return (m_FontType == PDFFONT_CIDFONT) ? (CPDF_CIDFont*)(void*)this : NULL; }
// //	/** Get a type3 font. */
// //	CPDF_Type3Font*			GetType3Font() const { return (m_FontType == PDFFONT_TYPE3) ? (CPDF_Type3Font*)(void*)this : NULL; }
// //
// //	HE_BOOL						IsEmbedded() const { return m_FontType == PDFFONT_TYPE3 || m_pFontFile != NULL; }
// //
// //	CHE_PDF_Dictionary*			GetFontDict() const { return m_pFontDict; }
// //
// //	HE_BOOL						IsStandardFont() const;
// //
// //	CFX_CharMap*				GetCharMap() { return m_pCharMap; }
// 
// protected:
// 	CHE_PDF_Font();
// 	HE_DWORD				m_FontType;
// 	CHE_ByteString			m_BaseFont;
// 	CHE_PDF_Dictionary*		m_pFontDict;
// };
// 
// 
// class CPDF_FontEncoding : public CFX_Object
// {
// public:
// 	/** Default constructor. */
// 	CPDF_FontEncoding();
// 	/**
// 	* Construct with a predefined encoding.
// 	*
// 	* @param[in] PredefinedEncoding		The input predefined encoding.
// 	*/
// 	CPDF_FontEncoding(int PredefinedEncoding);
// 	
// 	/**
// 	* Load encoding from a PDF encoding object.
// 	*
// 	* @param[in] pEncoding		The PDF object of encoding.
// 	*/
// 	void					LoadEncoding(CPDF_Object* pEncoding);
// 	/**
// 	* Check whether the encoding is identical with another encoding.
// 	*
// 	* @param[in] pAnother		The another encoding.
// 	* @return Non-zero means identical, otherwise not identical. 
// 	*/
// 	FX_BOOL					IsIdentical(CPDF_FontEncoding* pAnother) const;
// 	
// 	/**
// 	* Get the unicode from a charcode.
// 	*
// 	* @param[in] charcode		The input charcode.
// 	* @return The unicode of the charcode.
// 	*/
// 	FX_WCHAR				UnicodeFromCharCode(FX_BYTE charcode) const { return m_Unicodes[charcode]; }
// 	/**
// 	* Get the charcode from a unicode.
// 	*
// 	* @param[in] unicode		The input unicode.
// 	* @return The charcode of the unicode. -1 for not found.
// 	*/
// 	int						CharCodeFromUnicode(FX_WCHAR unicode) const;
// 	
// 	/**
// 	* Set the unicode of a charcode.
// 	*
// 	* @param[in] charcode		Input a charcode.
// 	* @param[in] unicode		The new unicode value for the charcode.
// 	*/
// 	void					SetUnicode(FX_BYTE charcode, FX_WCHAR unicode) { m_Unicodes[charcode] = unicode; }
// 	
// 	/**
// 	* Realize the font encoding in a PDF encoding object. Can be NULL is not needed.
// 	* @return A new PDF encoding object corresponding to the encoding table.
// 	*/
// 	CPDF_Object*			Realize();
// 	
// public:
// 	/** The encoding table. */
// 	HE_WCHAR				m_Unicodes[256];
// };

#endif