#ifndef _PDF_TEXT_DLL_H_
#define _PDF_TEXT_DLL_H_

typedef	int		PDFStatus;
typedef	void *	PDFDocument;
typedef	void *	PDFPage;
typedef	void *	PDFPageContent;
typedef	void *	PDFPageText;
typedef	void *	PDFPageChar;
typedef void *	PDFBitmap;
typedef void *	PDFPageWordSet;
typedef void *	PDFPageWord;

struct PDFPosition
{
	float x;
	float y;
};

struct PDFRect
{
	float left;
	float bottom;
	float width;
	float height;
};

struct PDFMatrix
{
	float a;
	float b;
	float c;
	float d;
	float e;
	float f;
};

#define				PDF_STATUS_OK				0
#define				PDF_STATUS_ERROR			-1
#define				PDF_STATUS_PARAM_ERR		-2
#define				PDF_STATUS_NOTEXT_ERR		-3
#define				PDF_STATUS_BUFFER_TOO_SMALL	-4

#define				PDF_WORD_NOTSYMBOLIC		0
#define				PDF_WORD_SYMBOLIC			1


/*
//	��������CHEPDF_OpenDocument
//
//  ���ܣ���һ��PDF�ļ�����ĵ�����
//
//  ������pFilePath - PDF�ļ����ڵ�·�����ļ���
//
//  ����ֵ��һ���ĵ�����
//
//  ��ע�����ص��ĵ�������ΪNULL����ʾ���ĵ�ʧ��
//        ���ص��ĵ�����ΪNULL����ʾ�򿪳ɹ�
//		  �ڲ�ʹ�ø��ĵ������ʱ����Ҫ����CHEPDF_CloseDocument��
//		  ���ĵ������ͷ�
*/
PDFDocument CHEPDF_OpenDocument( const char * pFilePath );

/*
//	��������CHEPDF_CloseDocument
//
//  ���ܣ��ر�һ���򿪵��ĵ������ͷ������Դ
//
//  ������doc - PDF�ĵ�����
//
//  ����ֵ��void
//
//  ��ע����
*/
void CHEPDF_CloseDocument( PDFDocument doc );

/*
//	��������CHEPDF_GetPageCount
//
//  ���ܣ���ȡһ��PDF�ĵ���ҳ����
//
//  ������doc - PDF�ĵ�����
//
//  ����ֵ����ʾҳ����������
//
//  ��ע����
*/
int CHEPDF_GetPageCount( PDFDocument doc );

/*
//	��������CHEPDF_GetPage
//
//  ���ܣ���PDF�ĵ������л��һ��PDFҳ�����
//
//  ������doc - PDF�ĵ�����
//		  index - ҳ������ֵ����ʾҪ�򿪵�ҳ�棬0��ʾ��һҳ���Դ�����
//
//  ����ֵ��һ��ҳ�����
//
//  ��ע�����ص��ĵ�������ΪNULL����ʾ��ҳ��ʧ��
//        ���ص��ĵ�����ΪNULL����ʾ��ҳ��ɹ�
//		  �ڲ�ʹ�ø��ĵ������ʱ����Ҫ����CHEPDF_ClosePage��
//		  ��ҳ������ͷ�
*/
PDFPage	CHEPDF_GetPage( PDFDocument doc, unsigned int index );

/*
//	��������CHEPDF_ClosePage
//
//  ���ܣ��ر�һ��ҳ������ͷ������Դ
//
//  ������page - PDFҳ�����
//
//  ����ֵ��void
//
//  ��ע����
*/
void CHEPDF_ClosePage( PDFPage page );

/*
//	��������CHEPDF_GetPageBox
//
//  ���ܣ���ȡһ��ҳ������ҳ���С
//
//  ������page - PDFҳ�����
//
//  ����ֵ��һ����ʾҳ���С�ľ�������PDFRect
//
//  ��ע��ҳ���С��PDFҳ�浥λ��ʾ
*/
PDFRect	CHEPDF_GetPageBox( PDFPage page );

/*
//	��������CHEPDF_GetPageContent
//
//  ���ܣ�����ҳ�����ݣ����ҳ�����ݶ���
//
//  ������page - PDFҳ�����
//
//  ����ֵ��һ��ҳ�����ݶ���
//
//  ��ע��ҳ�����ݶ�������ҳ��������Ϣ
//		  ���ص����ݶ�����ΪNULL����ʾ������ʧ��
//        ���ص����ݶ���ΪNULL����ʾ�����ݳɹ�
//		  �ڲ�ʹ�ø����ݶ����ʱ����Ҫ����CHEPDF_ReleasePageContent
//		  �������ݶ����ͷ�
//		  
*/
PDFPageContent CHEPDF_GetPageContent( PDFPage page );

/*
//	��������CHEPDF_ReleasePageContent
//
//  ���ܣ��Ƿ�ҳ�����ݶ���
//
//  ������content - PDFҳ�����ݶ���
//
//  ����ֵ��void
//
//  ��ע����
*/
void CHEPDF_ReleasePageContent( PDFPageContent content );

/*
//	��������CHEPDF_GetFirstPageText
//
//  ���ܣ���ҳ�����ݶ����л�ȡ��һ���ı�����
//
//  ������content - PDFҳ�����ݶ���
//
//  ����ֵ��һ���ı�����
//
//  ��ע�����ص��ı�������ΪNULL����ʾ���ı�ʧ��
//        ���ص��ı�����ΪNULL����ʾ���ı��ɹ�
//		  �ı�������Ҫ�ͷ�
*/
PDFPageText	CHEPDF_GetFirstPageText( PDFPageContent content );

/*
//	��������CHEPDF_GetNextPageText
//
//  ���ܣ���ҳ�����ݶ����л�ȡ��һ���ı�����
//
//  ������content - PDFҳ�����ݶ���
//
//  ����ֵ��һ���ı�����
//
//  ��ע�����ص��ı�������ΪNULL����ʾ���ı�ʧ��
//        ���ص��ı�����ΪNULL����ʾ���ı��ɹ�
//		  ���ݶ���ά���˻�ȡ������ֵ��Ϣ��ֻҪ��ͣ����
//		  CHEPDF_GetNextPageText��ֱ������ֵΪNULL���Ϳ������
//		  ҳ���������ı�����ı���
//		  �ı�������Ҫ�ͷ�
*/
PDFPageText	CHEPDF_GetNextPageText( PDFPageContent content );

/*
//	��������CHEPDF_GetTextPosition
//
//  ���ܣ���ȡ�ı�������ҳ���е�λ����Ϣ��ҳ�浥λ��
//
//  ������text - PDF�ı�����
//
//  ����ֵ����ʾλ����Ϣ�����ݽṹPDFPosition
//
//  ��ע��λ����Ϣ�е�������ҳ�浥λ��ʾ������Ϊҳ������
//		  PDFҳ�������ԭ����ҳ�����·���x�����������ң�y������������
*/
PDFPosition CHEPDF_GetTextPosition( PDFPageText text );

/*
//	��������CHEPDF_GetTextMatrix
//
//  ���ܣ���ȡ�ı�����ľ�����Ϣ
//
//  ������text - PDF�ı�����
//
//  ����ֵ����ʾ��������ݽṹPDFMatrix
//
//  ��ע����
*/
PDFMatrix CHEPDF_GetTextMatrix( PDFPageText text );

/*
//	��������CHEPDF_GetTextLength
//
//  ���ܣ���ȡ�ı�����İ������ַ����ĳ���
//
//  ������text - PDF�ı�����
//
//  ����ֵ����ʾ�ַ������ȵ�����ֵ
//
//  ��ע����
*/
unsigned int CHEPDF_GetTextLength( PDFPageText text );

/*
//	��������CHEPDF_GetTextUnicodes
//
//  ���ܣ���ȡ�ı�����İ������ַ������ݣ�Unicode�룩
//
//  ������text - PDF�ı�����
//		  pBuf - һ��������ȡ�ı��ַ����Ļ�����
//		  bufSize - �������Ĵ�С
//
//  ����ֵ��PDF_STATUS_OK - ��ȡ�ɹ�
//			PDF_STATUS_PARAM_ERR - ��������
//			PDF_STATUS_NOTEXT_ERR - ���ı�����û�а����ַ�����Ϣ
//			PDF_STATUS_BUFFER_TOO_SMALL - ������̫С���Ų����ַ�����Ϣ
//
//  ��ע����
*/
PDFStatus CHEPDF_GetTextUnicodes( PDFPageText text, wchar_t * pBuf, unsigned int bufSize );

/*
//	��������CHEPDF_GetTextBox
//
//  ���ܣ���ȡһ���ı�����Ĵ�С
//
//  ������text - PDF�ı�����
//
//  ����ֵ��һ����ʾ�ı���С�ľ�������PDFRect
//
//  ��ע����С��PDFҳ�浥λ��ʾ
*/
PDFRect CHEPDF_GetTextBox( PDFPageText text );

/*
//	��������CHEPDF_RenderText
//
//  ���ܣ���һ���ı�������Ⱦ��λͼ
//
//  ������text - PDF�ı�����
//		  scale - ����ϵ��
//
//  ����ֵ��һ��λͼ���ݶ���
//
//  ��ע�����ص�λͼ������ΪNULL����ʾ��Ⱦʧ��
//		  ���ص�λͼ���ݲ�ΪNULL����ʾ��Ⱦ�ɹ�
//		  ��Ⱦ��λͼ���ݶ����ڲ�ʹ��֮����Ҫ����
//		  CHEPDF_CloseBitmap�����ͷ�
*/
PDFBitmap CHEPDF_RenderText( PDFPageText text, float sclae = 1 );

/*
//	��������CHEPDF_GetPageChar
//
//  ���ܣ���һ���ı������л��һ���ַ�����
//
//  ������text - PDF�ı�����
//		  index - ����ֵ����0��ʼ��
//
//  ����ֵ��һ���ַ�����
//
//  ��ע�����ص��ַ�������ΪNULL����ʾʧ��
//		  ���ص��ַ�����ΪNULL����ʾ�ɹ�
//		  �������ʹ���ַ������ˣ���Ҫ����
//		  CHEPDF_ClosePageChar���ַ������ͷ�
//		  �ı������������ַ�����
*/
PDFPageChar CHEPDF_GetPageChar( PDFPageText text, unsigned int index );

/*
//	��������CHEPDF_ClosePageChar
//
//  ���ܣ��ر�һ���ַ�����
//
//  ������textChar - PDF�ַ�����
//
//  ����ֵ��void
//
//  ��ע����
*/
void CHEPDF_ClosePageChar( PDFPageChar textChar );

/*
//	��������CHEPDF_GetCharMatirx
//
//  ���ܣ���ȡһ���ַ�����ľ���
//
//  ������textChar - PDF�ַ�����
//
//  ����ֵ��һ����ʾ��������ݽṹPDFMatrix
//
//  ��ע����
*/
PDFMatrix CHEPDF_GetCharMatirx( PDFPageChar textChar );

/*
//	��������CHEPDF_GetCharPosition
//
//  ���ܣ���ȡ�ַ�������ҳ���е�λ����Ϣ��ҳ�浥λ��
//
//  ������textChar - PDF�ַ�����
//
//  ����ֵ����ʾλ����Ϣ�����ݽṹPDFPosition
//
//  ��ע��λ����Ϣ�е�������ҳ�浥λ��ʾ������Ϊҳ������
//		  PDFҳ�������ԭ����ҳ�����·���x�����������ң�y������������
*/
PDFPosition CHEPDF_GetCharPosition( PDFPageChar textChar );

/*
//	��������CHEPDF_GetCharUnicode
//
//  ���ܣ���ȡ�ַ������Unicode����
//
//  ������textChar - PDF�ַ�����
//		  UnicodeRet - һ��whar_tָ�룬���ڷ���unicode����
//
//  ����ֵ��PDF_STATUS_OK - ��ȡ�ɹ�
//			PDF_STATUS_PARAM_ERR - ��������
//			PDF_STATUS_NOTEXT_ERR - ���ı�����û�а����ַ�����Ϣ
//
//  ��ע����
*/
PDFStatus CHEPDF_GetCharUnicode( PDFPageChar textChar, wchar_t * UnicodeRet );

/*
//	��������CHEPDF_GetCharBox
//
//  ���ܣ���ȡһ���ַ�����Ĵ�С
//
//  ������textChar - PDF�ַ�����
//
//  ����ֵ��һ����ʾ�ַ���С�ľ�������PDFRect
//
//  ��ע����С��PDFҳ�浥λ��ʾ
*/
PDFRect	CHEPDF_GetCharBox( PDFPageChar textChar );

/*
//	��������CHEPDF_RenderChar
//
//  ���ܣ���һ���ַ�������Ⱦ��λͼ
//
//  ������textChar - PDF�ַ�����
//		  scale - ����ϵ��
//
//  ����ֵ��һ��λͼ���ݶ���
//
//  ��ע�����ص�λͼ������ΪNULL����ʾ��Ⱦʧ��
//		  ���ص�λͼ���ݲ�ΪNULL����ʾ��Ⱦ�ɹ�
//		  ��Ⱦ��λͼ���ݶ����ڲ�ʹ��֮����Ҫ����
//		  CHEPDF_CloseBitmap�����ͷ�
*/
PDFBitmap CHEPDF_RenderChar( PDFPageChar textChar, float sclae = 1 );

/*
//	��������CHEPDF_CloseBitmap
//
//  ���ܣ��ر�һ��λͼ����
//
//  ������bitmap - λͼ����
//
//  ����ֵ��void
//
//  ��ע����
*/
void CHEPDF_CloseBitmap( PDFBitmap bitmap );

/*
//	��������CHEPDF_GetBitmapWidth
//
//  ���ܣ���ȡһ��λͼ����Ŀ�ȣ�����ֵ��
//
//  ������bitmap - λͼ����
//
//  ����ֵ����ʾ��ȵ�����ֵ
//
//  ��ע����
*/
unsigned int CHEPDF_GetBitmapWidth( PDFBitmap bitmap );

/*
//	��������CHEPDF_GetBitmapHeight
//
//  ���ܣ���ȡһ��λͼ����ĸ߶ȣ�����ֵ��
//
//  ������bitmap - λͼ����
//
//  ����ֵ����ʾ�߶ȵ�����ֵ
//
//  ��ע����
*/
unsigned int CHEPDF_GetBitmapHeight( PDFBitmap bitmap );

/*
//	��������CHEPDF_SaveBitmapToFile
//
//  ���ܣ���λͼ���ݱ���Ϊһ��BMP�ļ�
//
//  ������bitmap - λͼ����
//		  filePath - �ļ�·�����ļ���
//
//  ����ֵ��PDF_STATUS_OK - �ɹ�
//			PDF_STATUS_ERROR - ʧ��
//
//  ��ע����
*/
PDFStatus CHEPDF_SaveBitmapToFile( PDFBitmap bitmap, char * filePath );

/*
//	��������CHEPDF_GetPageWordSet
//
//  ���ܣ���ҳ�������е��ı�������һ����ֵ���е��ʷָ�����
//		  ������������һ��PDFPageWordSet��
//
//  ������content - ���ݶ���
//		  threshold - ��ֵ���ַ�����ļ�����������ֵ�ͻᱻ��Ϊ2�����ʣ�
//
//  ����ֵ�����浥�����н���ĵ��ʼ��϶���
//
//  ��ע�����صĵ��ʼ��϶���ΪNULL��ʾʧ��
//		  ���صĵ��ʼ��϶���ΪNULL��ʾ�ɹ�
//		  ������ʹ��������ʼ��϶����ʱ����Ҫ
//		  ����CHEPDF_ReleasePageWordSet�����ͷ�
*/
PDFPageWordSet CHEPDF_GetPageWordSet( PDFPageContent content, float threshold = 0.5 );

/*
//	��������CHEPDF_GetFirstPageWord
//
//  ���ܣ��ӵ��ʼ��϶����л�õ�һ������
//
//  ������wordset - ���ʼ��϶���
//
//  ����ֵ��һ�����ʶ���
//
//  ��ע�����صĵ��ʶ���ΪNULL��ʾʧ��
//		  ���صĵ��ʶ���ΪNULL��ʾ�ɹ�
*/
PDFPageWord CHEPDF_GetFirstPageWord( PDFPageWordSet wordset );

/*
//	��������CHEPDF_GetNextPageWord
//
//  ���ܣ��ӵ��ʼ��϶����л����һ������
//
//  ������wordset - ���ʼ��϶���
//
//  ����ֵ��һ�����ʶ���
//
//  ��ע�����صĵ��ʶ���ΪNULL��ʾʧ��
//		  ���صĵ��ʶ���ΪNULL��ʾ�ɹ�
//		  ���ʼ��϶����ڲ�ά����������Ϣ��ֻҪ���ϵĵ���
//		  CHEPDF_GetNextPageWord��֪������ֵΪNULL������
//		  ��ɶ��ڵ��ʵı���
*/
PDFPageWord	CHEPDF_GetNextPageWord( PDFPageWordSet wordset );

/*
//	��������CHEPDF_ReleasePageWordSet
//
//  ���ܣ��ͷ�һ�����ʼ��϶���
//
//  ������wordset - ���ʼ��϶���
//
//  ����ֵ��void
//
//  ��ע����
*/
void CHEPDF_ReleasePageWordSet( PDFPageWordSet wordset );

/*
//	��������CHEPDF_GetWordBox
//
//  ���ܣ���ȡһ�����ʶ���Ĵ�С
//
//  ������word - PDF���ʶ���
//
//  ����ֵ��һ����ʾ�ַ���С�ľ�������PDFRect
//
//  ��ע����С��PDFҳ�浥λ��ʾ
*/
PDFRect CHEPDF_GetWordBox( PDFPageWord word );

/*
//	��������CHEPDF_IsWordSymbolic
//
//  ���ܣ��ж�һ�������Ƿ��������
//		 ��Ҳ���ǲ��ܹ�����ַ����룬ֻ����Ⱦ���鿴�ģ�
//
//  ������word - PDF���ʶ���
//
//  ����ֵ��PDF_STATUS_PARAM_ERR - ��������
//			PDF_WORD_SYMBOLIC - �����а������ţ�ֻ����Ⱦ����ʾ
//			PDF_WORD_NOTSYMBOLIC - �����в��������ţ�������ȡ�ַ�����
//
//  ��ע����
*/
PDFStatus CHEPDF_IsWordSymbolic( PDFPageWord word );

/*
//	��������CHEPDF_GetWordLength
//
//  ���ܣ���ȡ���ʶ���İ������ַ����ĳ���
//
//  ������word - PDF���ʶ���
//
//  ����ֵ����ʾ�ַ������ȵ�����ֵ
//
//  ��ע����
*/
unsigned int CHEPDF_GetWordLength( PDFPageWord word );

/*
//	��������CHEPDF_GetWordUnicodes
//
//  ���ܣ���ȡ���ʶ���İ������ַ������ݣ�Unicode�룩
//
//  ������word - PDF���ʶ���
//		  pBuf - һ��������ȡ�ı��ַ����Ļ�����
//		  bufSize - �������Ĵ�С
//
//  ����ֵ��PDF_STATUS_OK - ��ȡ�ɹ�
//			PDF_STATUS_PARAM_ERR - ��������
//			PDF_STATUS_NOTEXT_ERR - ���ı�����û�а����ַ�����Ϣ
//			PDF_STATUS_BUFFER_TOO_SMALL - ������̫С���Ų����ַ�����Ϣ
//
//  ��ע����
*/
PDFStatus CHEPDF_GetWordUnicodes( PDFPageWord word, wchar_t * pBuf, unsigned int bufSize );

/*
//	��������CHEPDF_RenderWord
//
//  ���ܣ���һ�����ʶ�����Ⱦ��λͼ
//
//  ������word - PDF���ʶ���
//		  scale - ����ϵ��
//
//  ����ֵ��һ��λͼ���ݶ���
//
//  ��ע�����ص�λͼ������ΪNULL����ʾ��Ⱦʧ��
//		  ���ص�λͼ���ݲ�ΪNULL����ʾ��Ⱦ�ɹ�
//		  ��Ⱦ��λͼ���ݶ����ڲ�ʹ��֮����Ҫ����
//		  CHEPDF_CloseBitmap�����ͷ�
*/
PDFBitmap CHEPDF_RenderWord( PDFPageWord word, float sclae = 1 );

/*
//	��������CHEPDF_RenderPage
//
//  ���ܣ���һ�����ʶ�����Ⱦ��λͼ
//
//  ������page - PDFҳ�����
//		  scale - ����ϵ��
//
//  ����ֵ��һ��λͼ���ݶ���
//
//  ��ע�����ص�λͼ������ΪNULL����ʾ��Ⱦʧ��
//		  ���ص�λͼ���ݲ�ΪNULL����ʾ��Ⱦ�ɹ�
//		  ��Ⱦ��λͼ���ݶ����ڲ�ʹ��֮����Ҫ����
//		  CHEPDF_CloseBitmap�����ͷ�
*/
PDFBitmap CHEPDF_RenderPage( PDFPage page, float sclae = 1 );

#endif