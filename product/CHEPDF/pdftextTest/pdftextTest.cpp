#include "../pdftext/pdftext.h"
#include <stdio.h>
#include <windows.h>

int main( int argc, char * argv[] )
{
	if ( argc < 2 )
	{
		printf( "no param!\n" );
		return 0;
	}

	PDFDocument doc = CHEPDF_OpenDocument( argv[1] ); //这里可以换成打开的文件路径，也可以在工程的调试属性中设置
	if ( doc )
	{

// 		int iPageCount = CHEPDF_GetPageCount( doc );
// 		char tmpStr[1024];
// 		for ( int i = 0; i < iPageCount; ++i )
// 		{
// 			printf( "Rendering Page : %d\n", i+1 );
// 			PDFPage page = CHEPDF_GetPage( doc, i ); //打开第一页
// 			PDFBitmap bitmap = CHEPDF_RenderPage( page, 3 );
// 			if ( bitmap )
// 			{
// 				sprintf_s( tmpStr, "%s_%03d.bmp", argv[1], i );
// 				CHEPDF_SaveBitmapToFile( bitmap, tmpStr );
// 				CHEPDF_CloseBitmap( bitmap );
// 			}
// 			CHEPDF_ClosePage( page );
// 		}
// 		CHEPDF_CloseDocument( doc );

// 		int iPageCount = CHEPDF_GetPageCount( doc );
// 		int index = 0;
// 		char tmpStr[1024];
// 		for ( int i = 1; i < 2; ++i )
// 		{
// 			PDFPage page = CHEPDF_GetPage( doc, i );
// 			PDFPageContent content = CHEPDF_GetPageContent( page ); //获取页面的内容
// 			if ( content )
// 			{
// 				int index = 0;
// 				char tmpStr[1024];
// 				PDFPageText text = CHEPDF_GetFirstPageText( content );
// 				while ( text )
// 				{
// 					PDFBitmap bitmap = CHEPDF_RenderText( text ); //以10倍于原始大小渲染词到位图中
// 					if ( bitmap )
// 					{
// 						sprintf_s( tmpStr, "d:\\text%03d.bmp", index );
// 						CHEPDF_SaveBitmapToFile( bitmap, tmpStr );
// 						CHEPDF_CloseBitmap( bitmap );
// 					}
// 					++index;
// 					text = CHEPDF_GetNextPageText( content );
// 				}
// 			}
// 			CHEPDF_ReleasePageContent( content );
// 			CHEPDF_ClosePage( page );
// 		}


		PDFPage page = CHEPDF_GetPage( doc, 34 );
		PDFPageContent content = CHEPDF_GetPageContent( page ); //获取页面的内容
		PDFPageWordSet wordset = CHEPDF_GetPageWordSet( content, 2.5 ); //对页面内容进行排版运行，计算出单词保存在wordset中
		if ( wordset )
		{
			PDFPageWord tmpWord = CHEPDF_GetFirstPageWord( wordset ); //获取第一个词
			int index = 0;
			char tmpStr[1024];
			WCHAR unicodeText[1024];
			unsigned int uLength = 0;
					
			while ( tmpWord )
			{	
				printf( "text %d\n", index );

				PDFRect rect = CHEPDF_GetWordBox( tmpWord );	//获取词的大小范围
				printf( "text rect = left : %.2f, bottom : %.2f, width : %.2f, height : %.2f\n", rect.left, rect.bottom, rect.width, rect.height );

				if ( CHEPDF_IsWordSymbolic( tmpWord ) == PDF_WORD_NOTSYMBOLIC )	//判断该词是不是符号
				{
					//该词不是符号，则要输出文本内容
					uLength = CHEPDF_GetWordLength( tmpWord );
					if ( uLength > 0 )
					{
						printf( "text length %d\n", uLength );
						CHEPDF_GetWordUnicodes( tmpWord, unicodeText, 1024 ); //获得词中的unicode编码的文本内容
						WideCharToMultiByte( CP_ACP, 0, unicodeText, -1, tmpStr, 1024, NULL, NULL );
						printf( "text : %s\n", tmpStr );
					}
				}

				PDFBitmap bitmap = CHEPDF_RenderWord( tmpWord, 3.0 ); //以10倍于原始大小渲染词到位图中
				if ( bitmap )
				{
					sprintf_s( tmpStr, "d:\\text%03d.bmp", index );
					CHEPDF_SaveBitmapToFile( bitmap, tmpStr );
					CHEPDF_CloseBitmap( bitmap );
				}
				++index;
				tmpWord = CHEPDF_GetNextPageWord( wordset ); //从wordset中获得下一个词
			}
		}



	}

	return 0;
}

