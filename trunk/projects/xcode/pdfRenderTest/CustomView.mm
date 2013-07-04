//
//  CustomView.m
//  chelib
//
//  Created by Frank Zeng on 13-5-31.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import "CustomView.h"

#include "../../../include/pdf/che_pdf_file.h"
#include "../../../include/pdf/che_pdf_document.h"
#include "../../../include/pdf/che_pdf_contents.h"
#include "../../../include/pdf/che_pdf_contentobjs.h"
#include "../../../include/pdf/che_pdf_renderer_macosx.h"

@implementation CustomView

CHE_GraphicsDrawer drawer( 1, 1 );
CHE_PDF_FontMgr fontMgr;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        // Initialization code here.
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    IHE_Read * pFileRead = HE_CreateFileRead( "/Users/zencher/Desktop/test/test14.pdf" );
    if ( pFileRead )
    {
        CHE_PDF_File pdfFile;
        if ( pdfFile.Open( pFileRead ) )
        {
            CHE_PDF_Document * pDocument = CHE_PDF_Document::CreateDocument( &pdfFile );
            if ( pDocument )
            {
                CHE_PDF_PageTree * pPageTree = pDocument->GetPageTree();
                if ( pPageTree )
                {
                    for( size_t i = 0; i < pPageTree->GetPageCount(); ++i )
                    {
                        CHE_PDF_Page * pPage = pPageTree->GetPage( i );
                        if ( pPage )
                        {
                            CHE_PDF_DictionaryPtr pageDict = pPage->GetPageDict();
                            CHE_PDF_ContentObjectList contentList;
                            
                            GetPageContent( pageDict, &contentList, &fontMgr );
                            
                            CHE_Rect rect = pPage->GetMediaBox();
                            CHE_PDF_Renderer::Render( contentList, drawer, rect );
                        }
                    }
                }
            }
        }
    }
    
    CGContextRef contextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    
    CGImageRef imageRef = CGBitmapContextCreateImage( drawer.GetContextRef() );
    
    CGRect rect = CGRectMake( 0 , 0, drawer.GetWidth(), drawer.GetHeight() );
    
    CGContextDrawImage( contextRef , rect, imageRef );
    
}

@end
