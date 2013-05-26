using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace pdftexttestCSharp
{
    struct PDFPosition
    {
        public Single x;
        public Single y;
    }

    struct PDFRect
    {
        public Single left;
        public Single bottom;
        public Single width;
        public Single height;
    };

    struct PDFMatrix
    {
        public Single a;
        public Single b;
        public Single c;
        public Single d;
        public Single e;
        public Single f;
    };
    
    class CPdftextAdapter
    {
        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_OpenDocument(System.String file);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_CloseDocument(System.IntPtr document);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern int CHEPDF_GetPageCount(System.IntPtr document);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_GetPage( System.IntPtr document, uint index);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern void CHEPDF_ClosePage(System.IntPtr page);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern PDFRect CHEPDF_GetPageBox(System.IntPtr page);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_GetPageContent(System.IntPtr page);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern void CHEPDF_ReleasePageContent(System.IntPtr content);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_GetFirstPageText(System.IntPtr content);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_GetNextPageText(System.IntPtr content);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern PDFPosition CHEPDF_GetTextPosition(System.IntPtr text);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern PDFMatrix CHEPDF_GetTextMatrix(System.IntPtr text);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern uint CHEPDF_GetTextLength(System.IntPtr text);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static unsafe extern int CHEPDF_GetTextUnicodes(System.IntPtr text, char* pBuf, uint bufSize);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern PDFRect CHEPDF_GetTextBox(System.IntPtr text);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_RenderText(System.IntPtr text, System.Single sclae=1f);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_GetPageChar(System.IntPtr text, uint index);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern void CHEPDF_ClosePageChar(System.IntPtr textChar);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern PDFMatrix CHEPDF_GetCharMatirx(System.IntPtr textChar);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern PDFPosition CHEPDF_GetCharPosition(System.IntPtr textChar);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static unsafe extern int CHEPDF_GetCharUnicode(System.IntPtr textChar, char* coedeRet);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern PDFRect CHEPDF_GetCharBox(System.IntPtr textChar);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_RenderChar(System.IntPtr textChar, System.Single sclae=1f);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern void CHEPDF_CloseBitmap(System.IntPtr bitmap);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern uint CHEPDF_GetBitmapWidth(System.IntPtr bitmap);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern uint CHEPDF_GetBitmapHeight(System.IntPtr bitmap);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern uint CHEPDF_GetBitmapDataSize(System.IntPtr bitmap);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static unsafe extern uint CHEPDF_GetBitmapData(System.IntPtr bitmap, byte* pbuf, uint bufSize);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern int CHEPDF_SaveBitmapToFile(System.IntPtr bitmap, System.String file);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_GetPageWordSet(System.IntPtr content, System.Single threshold=0.5f);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_GetFirstPageWord(System.IntPtr wordset);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr	CHEPDF_GetNextPageWord(System.IntPtr wordset);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern void CHEPDF_ReleasePageWordSet(System.IntPtr wordset);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern PDFRect CHEPDF_GetWordBox(System.IntPtr word);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern int CHEPDF_IsWordSymbolic(System.IntPtr word);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern uint CHEPDF_GetWordLength(System.IntPtr word);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static unsafe extern int CHEPDF_GetWordUnicodes(System.IntPtr word, char* pBuf, uint bufSize);

        [DllImport("pdftext.dll", CharSet = CharSet.Ansi)]
        public static extern System.IntPtr CHEPDF_RenderWord(System.IntPtr word, System.Single sclae=1f);
    }
 
    class Program
    {
        static void Main(string[] args)
        {

            System.IntPtr document = CPdftextAdapter.CHEPDF_OpenDocument(/*args[0]*/ "e:\\999.pdf" /* "d:\\pdffile.pdf" */ ); //打开pdf文件
            if (document.ToInt32()!=0)
            {
                int pageCount = CPdftextAdapter.CHEPDF_GetPageCount(document);
                Console.WriteLine("PageCount : {0}", pageCount);

                System.IntPtr page = CPdftextAdapter.CHEPDF_GetPage(document, 0);
                if (page.ToInt32()!=0)
                {
                    System.IntPtr content = CPdftextAdapter.CHEPDF_GetPageContent(page);
                    if (content.ToInt32()!=0)
                    {
                        System.IntPtr wordset = CPdftextAdapter.CHEPDF_GetPageWordSet(content, 2.5f);
                        if (wordset.ToInt32()!=0)
                        {
                            uint index = 0;
                            System.IntPtr word = CPdftextAdapter.CHEPDF_GetFirstPageWord(wordset);
                            while(word.ToInt32()!=0)
                            {
                                if (CPdftextAdapter.CHEPDF_IsWordSymbolic(word) == 0) //0 = PDF_WORD_NOTSYMBOLIC 1 = PDF_WORD_SYMBOLIC
                                {
                                    uint textLength = CPdftextAdapter.CHEPDF_GetWordLength(word);
                                    string str = "";
                                    unsafe
                                    {
                                        char[] unicodes = new char[textLength + 1];
                                        fixed (char* pUnicodes = &unicodes[0])
                                        {
                                            CPdftextAdapter.CHEPDF_GetWordUnicodes(word, pUnicodes, textLength + 1);
                                        }
                                        str = new string(unicodes);
                                        Console.WriteLine("text : {0}", str);
                                    }
                                }

                                System.IntPtr bitmap = CPdftextAdapter.CHEPDF_RenderWord(word, 5f);
                                if (bitmap.ToInt32() != 0)
                                {
                                    uint bitmapSize = CPdftextAdapter.CHEPDF_GetBitmapDataSize(bitmap);
                                    unsafe
                                    {
                                        byte[] data = new byte[bitmapSize];
                                        fixed( byte * pbuf = &data[0] )
                                        {
                                            CPdftextAdapter.CHEPDF_GetBitmapData(bitmap, pbuf, bitmapSize);
                                        }
                                    }

                                    System.String file = "d:\\bitmap";
                                    file += index;
                                    file += ".bmp";
                                    CPdftextAdapter.CHEPDF_SaveBitmapToFile(bitmap, file);
                                    CPdftextAdapter.CHEPDF_CloseBitmap(bitmap);
                                }
                                index++;
                                word = CPdftextAdapter.CHEPDF_GetNextPageWord(wordset);
                            }
                            CPdftextAdapter.CHEPDF_ReleasePageWordSet(wordset);
                        }
                        CPdftextAdapter.CHEPDF_ReleasePageContent(content);
                    }
                    CPdftextAdapter.CHEPDF_ClosePage(page);
                }
                CPdftextAdapter.CHEPDF_CloseDocument(document);
            }
        }
    }
}
