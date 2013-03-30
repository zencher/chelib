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
//	函数名：CHEPDF_OpenDocument
//
//  功能：打开一个PDF文件获得文档对象
//
//  参数：pFilePath - PDF文件所在的路径和文件名
//
//  返回值：一个文档对象
//
//  备注：返回的文档对象若为NULL，表示打开文档失败
//        返回的文档对象不为NULL，表示打开成功
//		  在不使用该文档对象的时候需要调用CHEPDF_CloseDocument将
//		  该文档对象释放
*/
PDFDocument CHEPDF_OpenDocument( const char * pFilePath );

/*
//	函数名：CHEPDF_CloseDocument
//
//  功能：关闭一个打开的文档对象，释放相关资源
//
//  参数：doc - PDF文档对象
//
//  返回值：void
//
//  备注：无
*/
void CHEPDF_CloseDocument( PDFDocument doc );

/*
//	函数名：CHEPDF_GetPageCount
//
//  功能：获取一个PDF文档的页面数
//
//  参数：doc - PDF文档对象
//
//  返回值：表示页面数的整数
//
//  备注：无
*/
int CHEPDF_GetPageCount( PDFDocument doc );

/*
//	函数名：CHEPDF_GetPage
//
//  功能：从PDF文档对象中获得一个PDF页面对象
//
//  参数：doc - PDF文档对象
//		  index - 页面索引值，表示要打开的页面，0表示第一页，以此类推
//
//  返回值：一个页面对象
//
//  备注：返回的文档对象若为NULL，表示打开页面失败
//        返回的文档对象不为NULL，表示打开页面成功
//		  在不使用该文档对象的时候需要调用CHEPDF_ClosePage将
//		  该页面对象释放
*/
PDFPage	CHEPDF_GetPage( PDFDocument doc, unsigned int index );

/*
//	函数名：CHEPDF_ClosePage
//
//  功能：关闭一个页面对象，释放相关资源
//
//  参数：page - PDF页面对象
//
//  返回值：void
//
//  备注：无
*/
void CHEPDF_ClosePage( PDFPage page );

/*
//	函数名：CHEPDF_GetPageBox
//
//  功能：获取一个页面对象的页面大小
//
//  参数：page - PDF页面对象
//
//  返回值：一个表示页面大小的矩形数据PDFRect
//
//  备注：页面大小以PDF页面单位表示
*/
PDFRect	CHEPDF_GetPageBox( PDFPage page );

/*
//	函数名：CHEPDF_GetPageContent
//
//  功能：解析页面内容，获得页面内容对象
//
//  参数：page - PDF页面对象
//
//  返回值：一个页面内容对象
//
//  备注：页面内容对象存放了页面内容信息
//		  返回的内容对象若为NULL，表示打开内容失败
//        返回的内容对象不为NULL，表示打开内容成功
//		  在不使用该内容对象的时候需要调用CHEPDF_ReleasePageContent
//		  将该内容对象释放
//		  
*/
PDFPageContent CHEPDF_GetPageContent( PDFPage page );

/*
//	函数名：CHEPDF_ReleasePageContent
//
//  功能：是否页面内容对象
//
//  参数：content - PDF页面内容对象
//
//  返回值：void
//
//  备注：无
*/
void CHEPDF_ReleasePageContent( PDFPageContent content );

/*
//	函数名：CHEPDF_GetFirstPageText
//
//  功能：从页面内容对象中获取第一个文本对象
//
//  参数：content - PDF页面内容对象
//
//  返回值：一个文本对象
//
//  备注：返回的文本对象若为NULL，表示打开文本失败
//        返回的文本对象不为NULL，表示打开文本成功
//		  文本对象不需要释放
*/
PDFPageText	CHEPDF_GetFirstPageText( PDFPageContent content );

/*
//	函数名：CHEPDF_GetNextPageText
//
//  功能：从页面内容对象中获取下一个文本对象
//
//  参数：content - PDF页面内容对象
//
//  返回值：一个文本对象
//
//  备注：返回的文本对象若为NULL，表示打开文本失败
//        返回的文本对象不为NULL，表示打开文本成功
//		  内容对象维护了获取的索引值信息，只要不停调用
//		  CHEPDF_GetNextPageText，直到返回值为NULL，就可以完成
//		  页面内容中文本对象的遍历
//		  文本对象不需要释放
*/
PDFPageText	CHEPDF_GetNextPageText( PDFPageContent content );

/*
//	函数名：CHEPDF_GetTextPosition
//
//  功能：获取文本对象在页面中的位置信息（页面单位）
//
//  参数：text - PDF文本对象
//
//  返回值：表示位置信息的数据结构PDFPosition
//
//  备注：位置信息中的数据以页面单位表示，坐标为页面坐标
//		  PDF页面的坐标原点在页面左下方，x轴正方向向右，y轴正方向向上
*/
PDFPosition CHEPDF_GetTextPosition( PDFPageText text );

/*
//	函数名：CHEPDF_GetTextMatrix
//
//  功能：获取文本对象的矩阵信息
//
//  参数：text - PDF文本对象
//
//  返回值：表示矩阵的数据结构PDFMatrix
//
//  备注：无
*/
PDFMatrix CHEPDF_GetTextMatrix( PDFPageText text );

/*
//	函数名：CHEPDF_GetTextLength
//
//  功能：获取文本对象的包含的字符串的长度
//
//  参数：text - PDF文本对象
//
//  返回值：表示字符串长度的整数值
//
//  备注：无
*/
unsigned int CHEPDF_GetTextLength( PDFPageText text );

/*
//	函数名：CHEPDF_GetTextUnicodes
//
//  功能：获取文本对象的包含的字符串数据（Unicode码）
//
//  参数：text - PDF文本对象
//		  pBuf - 一个用来获取文本字符串的缓冲区
//		  bufSize - 缓冲区的大小
//
//  返回值：PDF_STATUS_OK - 获取成功
//			PDF_STATUS_PARAM_ERR - 参数错误
//			PDF_STATUS_NOTEXT_ERR - 该文本对象没有包含字符串信息
//			PDF_STATUS_BUFFER_TOO_SMALL - 缓冲区太小而放不下字符串信息
//
//  备注：无
*/
PDFStatus CHEPDF_GetTextUnicodes( PDFPageText text, wchar_t * pBuf, unsigned int bufSize );

/*
//	函数名：CHEPDF_GetTextBox
//
//  功能：获取一个文本对象的大小
//
//  参数：text - PDF文本对象
//
//  返回值：一个表示文本大小的矩形数据PDFRect
//
//  备注：大小以PDF页面单位表示
*/
PDFRect CHEPDF_GetTextBox( PDFPageText text );

/*
//	函数名：CHEPDF_RenderText
//
//  功能：将一个文本对象渲染成位图
//
//  参数：text - PDF文本对象
//		  scale - 缩放系数
//
//  返回值：一个位图数据对象
//
//  备注：返回的位图数据若为NULL，表示渲染失败
//		  返回的位图数据不为NULL，表示渲染成功
//		  渲染的位图数据对象在不使用之后需要调用
//		  CHEPDF_CloseBitmap将其释放
*/
PDFBitmap CHEPDF_RenderText( PDFPageText text, float sclae = 1 );

/*
//	函数名：CHEPDF_GetPageChar
//
//  功能：从一个文本对象中获得一个字符对象
//
//  参数：text - PDF文本对象
//		  index - 索引值（从0开始）
//
//  返回值：一个字符对象
//
//  备注：返回的字符对象若为NULL，表示失败
//		  返回的字符对象不为NULL，表示成功
//		  如果不再使用字符对象了，需要调用
//		  CHEPDF_ClosePageChar将字符对象释放
//		  文本对象包含多个字符对象
*/
PDFPageChar CHEPDF_GetPageChar( PDFPageText text, unsigned int index );

/*
//	函数名：CHEPDF_ClosePageChar
//
//  功能：关闭一个字符对象
//
//  参数：textChar - PDF字符对象
//
//  返回值：void
//
//  备注：无
*/
void CHEPDF_ClosePageChar( PDFPageChar textChar );

/*
//	函数名：CHEPDF_GetCharMatirx
//
//  功能：获取一个字符对象的矩阵
//
//  参数：textChar - PDF字符对象
//
//  返回值：一个表示矩阵的数据结构PDFMatrix
//
//  备注：无
*/
PDFMatrix CHEPDF_GetCharMatirx( PDFPageChar textChar );

/*
//	函数名：CHEPDF_GetCharPosition
//
//  功能：获取字符对象在页面中的位置信息（页面单位）
//
//  参数：textChar - PDF字符对象
//
//  返回值：表示位置信息的数据结构PDFPosition
//
//  备注：位置信息中的数据以页面单位表示，坐标为页面坐标
//		  PDF页面的坐标原点在页面左下方，x轴正方向向右，y轴正方向向上
*/
PDFPosition CHEPDF_GetCharPosition( PDFPageChar textChar );

/*
//	函数名：CHEPDF_GetCharUnicode
//
//  功能：获取字符对象的Unicode编码
//
//  参数：textChar - PDF字符对象
//		  UnicodeRet - 一个whar_t指针，用于返回unicode编码
//
//  返回值：PDF_STATUS_OK - 获取成功
//			PDF_STATUS_PARAM_ERR - 参数错误
//			PDF_STATUS_NOTEXT_ERR - 该文本对象没有包含字符串信息
//
//  备注：无
*/
PDFStatus CHEPDF_GetCharUnicode( PDFPageChar textChar, wchar_t * UnicodeRet );

/*
//	函数名：CHEPDF_GetCharBox
//
//  功能：获取一个字符对象的大小
//
//  参数：textChar - PDF字符对象
//
//  返回值：一个表示字符大小的矩形数据PDFRect
//
//  备注：大小以PDF页面单位表示
*/
PDFRect	CHEPDF_GetCharBox( PDFPageChar textChar );

/*
//	函数名：CHEPDF_RenderChar
//
//  功能：将一个字符对象渲染成位图
//
//  参数：textChar - PDF字符对象
//		  scale - 缩放系数
//
//  返回值：一个位图数据对象
//
//  备注：返回的位图数据若为NULL，表示渲染失败
//		  返回的位图数据不为NULL，表示渲染成功
//		  渲染的位图数据对象在不使用之后需要调用
//		  CHEPDF_CloseBitmap将其释放
*/
PDFBitmap CHEPDF_RenderChar( PDFPageChar textChar, float sclae = 1 );

/*
//	函数名：CHEPDF_CloseBitmap
//
//  功能：关闭一个位图对象
//
//  参数：bitmap - 位图对象
//
//  返回值：void
//
//  备注：无
*/
void CHEPDF_CloseBitmap( PDFBitmap bitmap );

/*
//	函数名：CHEPDF_GetBitmapWidth
//
//  功能：获取一个位图对象的宽度（像素值）
//
//  参数：bitmap - 位图对象
//
//  返回值：表示宽度的整数值
//
//  备注：无
*/
unsigned int CHEPDF_GetBitmapWidth( PDFBitmap bitmap );

/*
//	函数名：CHEPDF_GetBitmapHeight
//
//  功能：获取一个位图对象的高度（像素值）
//
//  参数：bitmap - 位图对象
//
//  返回值：表示高度的整数值
//
//  备注：无
*/
unsigned int CHEPDF_GetBitmapHeight( PDFBitmap bitmap );

/*
//	函数名：CHEPDF_SaveBitmapToFile
//
//  功能：将位图数据保存为一个BMP文件
//
//  参数：bitmap - 位图对象
//		  filePath - 文件路径和文件名
//
//  返回值：PDF_STATUS_OK - 成功
//			PDF_STATUS_ERROR - 失败
//
//  备注：无
*/
PDFStatus CHEPDF_SaveBitmapToFile( PDFBitmap bitmap, char * filePath );

/*
//	函数名：CHEPDF_GetPageWordSet
//
//  功能：将页面内容中的文本对象按照一个阀值进行单词分割运算
//		  运算结果保存在一个PDFPageWordSet中
//
//  参数：content - 内容对象
//		  threshold - 阀值（字符对象的间隔超过这个阀值就会被分为2个单词）
//
//  返回值：保存单词运行结果的单词集合对象
//
//  备注：返回的单词集合对象为NULL表示失败
//		  返回的单词集合对象不为NULL表示成功
//		  当不再使用这个单词集合对象的时候需要
//		  调用CHEPDF_ReleasePageWordSet进行释放
*/
PDFPageWordSet CHEPDF_GetPageWordSet( PDFPageContent content, float threshold = 0.5 );

/*
//	函数名：CHEPDF_GetFirstPageWord
//
//  功能：从单词集合对象中获得第一个单词
//
//  参数：wordset - 单词集合对象
//
//  返回值：一个单词对象
//
//  备注：返回的单词对象为NULL表示失败
//		  返回的单词对象不为NULL表示成功
*/
PDFPageWord CHEPDF_GetFirstPageWord( PDFPageWordSet wordset );

/*
//	函数名：CHEPDF_GetNextPageWord
//
//  功能：从单词集合对象中获得下一个单词
//
//  参数：wordset - 单词集合对象
//
//  返回值：一个单词对象
//
//  备注：返回的单词对象为NULL表示失败
//		  返回的单词对象不为NULL表示成功
//		  单词集合对象内部维护了索引信息，只要不断的调用
//		  CHEPDF_GetNextPageWord，知道返回值为NULL，就能
//		  完成对于单词的遍历
*/
PDFPageWord	CHEPDF_GetNextPageWord( PDFPageWordSet wordset );

/*
//	函数名：CHEPDF_ReleasePageWordSet
//
//  功能：释放一个单词集合对象
//
//  参数：wordset - 单词集合对象
//
//  返回值：void
//
//  备注：无
*/
void CHEPDF_ReleasePageWordSet( PDFPageWordSet wordset );

/*
//	函数名：CHEPDF_GetWordBox
//
//  功能：获取一个单词对象的大小
//
//  参数：word - PDF单词对象
//
//  返回值：一个表示字符大小的矩形数据PDFRect
//
//  备注：大小以PDF页面单位表示
*/
PDFRect CHEPDF_GetWordBox( PDFPageWord word );

/*
//	函数名：CHEPDF_IsWordSymbolic
//
//  功能：判断一个单词是否包含符号
//		 （也就是不能够获得字符编码，只能渲染来查看的）
//
//  参数：word - PDF单词对象
//
//  返回值：PDF_STATUS_PARAM_ERR - 参数错误
//			PDF_WORD_SYMBOLIC - 单词中包含符号，只能渲染来显示
//			PDF_WORD_NOTSYMBOLIC - 单词中不包含符号，可以提取字符编码
//
//  备注：无
*/
PDFStatus CHEPDF_IsWordSymbolic( PDFPageWord word );

/*
//	函数名：CHEPDF_GetWordLength
//
//  功能：获取单词对象的包含的字符串的长度
//
//  参数：word - PDF单词对象
//
//  返回值：表示字符串长度的整数值
//
//  备注：无
*/
unsigned int CHEPDF_GetWordLength( PDFPageWord word );

/*
//	函数名：CHEPDF_GetWordUnicodes
//
//  功能：获取单词对象的包含的字符串数据（Unicode码）
//
//  参数：word - PDF单词对象
//		  pBuf - 一个用来获取文本字符串的缓冲区
//		  bufSize - 缓冲区的大小
//
//  返回值：PDF_STATUS_OK - 获取成功
//			PDF_STATUS_PARAM_ERR - 参数错误
//			PDF_STATUS_NOTEXT_ERR - 该文本对象没有包含字符串信息
//			PDF_STATUS_BUFFER_TOO_SMALL - 缓冲区太小而放不下字符串信息
//
//  备注：无
*/
PDFStatus CHEPDF_GetWordUnicodes( PDFPageWord word, wchar_t * pBuf, unsigned int bufSize );

/*
//	函数名：CHEPDF_RenderWord
//
//  功能：将一个单词对象渲染成位图
//
//  参数：word - PDF单词对象
//		  scale - 缩放系数
//
//  返回值：一个位图数据对象
//
//  备注：返回的位图数据若为NULL，表示渲染失败
//		  返回的位图数据不为NULL，表示渲染成功
//		  渲染的位图数据对象在不使用之后需要调用
//		  CHEPDF_CloseBitmap将其释放
*/
PDFBitmap CHEPDF_RenderWord( PDFPageWord word, float sclae = 1 );

/*
//	函数名：CHEPDF_RenderPage
//
//  功能：将一个单词对象渲染成位图
//
//  参数：page - PDF页面对象
//		  scale - 缩放系数
//
//  返回值：一个位图数据对象
//
//  备注：返回的位图数据若为NULL，表示渲染失败
//		  返回的位图数据不为NULL，表示渲染成功
//		  渲染的位图数据对象在不使用之后需要调用
//		  CHEPDF_CloseBitmap将其释放
*/
PDFBitmap CHEPDF_RenderPage( PDFPage page, float sclae = 1 );

#endif