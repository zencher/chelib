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
        pageCount       = 0;
        fileReadInf     = NULL;
        pdfFile         = NULL;
        pdfDocument     = NULL;
        pdfPageTree     = NULL;
        pdfComponentMgr      = NULL;
        
        allocator = GetDefaultAllocator();
        pdfComponentMgr = allocator->New<CHE_PDF_ComponentMgr>( allocator );
        
        fileReadInf = HE_CreateFileRead( [path cStringUsingEncoding:NSUTF8StringEncoding] );
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
                    }
                }
            }
        }
    }
    return self;
}

-(CHE_PDF_ContentObjectList*)getPageContent:(unsigned int)index
{
    CHE_PDF_Page * pdfPage = pdfPageTree->GetPage( index );
    if ( pdfPage )
    {
        pdfPage->ParsePageContent( pdfDocument->GetComponentMgr() );
        CHE_PDF_ContentObjectList & list = pdfPage->GetPageContentList();
        return & list;
    }
    return NULL;
}

-(HE_ULONG)getPageCount
{
    return pdfPageTree->GetPageCount();
}

-(CHE_Rect)getPageRect:(unsigned int)index
{
    CHE_Rect rect;
    if ( index < pageCount )
    {
        CHE_PDF_Page * pdfPage = pdfPageTree->GetPage( index );
        if ( pdfPage )
        {
            rect = pdfPage->GetPageRect();
            pdfPageTree->ReleasePage( pdfPage );
        }
    }
    return rect;
}

@end
