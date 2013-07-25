//
//  PdfDocumentData.m
//  chelib
//
//  Created by Frank Zeng on 13-7-25.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import "PdfDocumentData.h"

@implementation PdfDocumentData

-(id)initWithFilePath:(NSString *)path
{
    self = [super init];
    if ( self )
    {
        pageIndex       = 0;
        pageCount       = 0;
        fileReadInf     = NULL;
        pdfFile         = NULL;
        pdfDocument     = NULL;
        pdfPageTree     = NULL;
        pdfPage         = NULL;
        pdfFontMgr      = NULL;
        pdfDrawer       = NULL;
        
        allocator = GetDefaultAllocator();
        pdfFontMgr = allocator->New<CHE_PDF_FontMgr>( allocator );
        pdfDrawer = allocator->New<CHE_GraphicsDrawer>( 1, 1 );
        
        fileReadInf = HE_CreateFileRead( [path cStringUsingEncoding:NSASCIIStringEncoding] );
        if ( fileReadInf )
        {
            pdfFile = allocator->New<CHE_PDF_File>( allocator );
            if ( pdfFile && pdfFile->Open( fileReadInf ) )
            {
                pdfDocument = CHE_PDF_Document::CreateDocument( pdfFile );
                if ( pdfDocument )
                {
                    pdfPageTree = pdfDocument->GetPageTree();
                    if ( pdfPageTree )
                    {
                        pageCount = pdfPageTree->GetPageCount();
                        pageContents.resize( pageCount );
                        
                        for ( HE_ULONG i = 0 ; i < pageCount; ++i )
                        {
                            pageContents[i] = NULL;
                        }
                        
                        pdfPage = pdfPageTree->GetPage( pageIndex );
                        if ( pdfPage )
                        {
                            CHE_PDF_DictionaryPtr pageDict = pdfPage->GetPageDict();
                            CHE_PDF_DictionaryPtr resDict = pdfPage->GetResourcesDict();
                            if ( pageContents[pageIndex] == NULL )
                            {
                                pageContents[pageIndex] = allocator->New<CHE_PDF_ContentObjectList>( resDict, allocator );
                                GetPageContent( pageDict, pageContents[pageIndex], pdfFontMgr, allocator );
                                //CHE_Rect rect = pdfPage->GetMediaBox();
                                //CHE_PDF_Renderer::Render( contentList, *pdfDrawer, rect );
                                //pdfDrawer->SaveToFile( "/Users/zencher/sdf.png" );
                            }
                        }
                    }
                }
            }
        }
    }
    return self;
}

@end
