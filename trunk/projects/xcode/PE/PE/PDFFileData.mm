//
//  PDFFileData.m
//  chelib
//
//  Created by zencher on 14/12/27.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//
#import "PDFFileData.h"

@implementation PDFFileData

-(id)initWithReadInf:(IHE_Read*)read
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
        
        fileReadInf = read;
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
                    }
                }
            }
        }
    }
    return self;
}

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
                        pageContents.resize( pageCount );
                        
                        for ( HE_ULONG i = 0 ; i < pageCount; ++i )
                        {
                            pageContents[i] = NULL;
                        }
                    }
                }
            }
        }
    }
    return self;
}

-(CHE_PDF_ContentObjectList*)getPageContent:(unsigned int)index
{
    if ( index < pageCount )
    {
        if ( pageContents[index] != NULL )
        {
            return pageContents[index];
        }else{
            CHE_PDF_Page * pdfPage = pdfPageTree->GetPage( index );
            if ( pdfPage )
            {
                CHE_PDF_DictionaryPtr pageDict = pdfPage->GetPageDict();
                CHE_PDF_DictionaryPtr resDict = pdfPage->GetResourcesDict();
                pageContents[index] = allocator->New<CHE_PDF_ContentObjectList>( resDict, allocator );
                CHE_PDF_ContentListBuilder::ParsePageContent( pageDict, *pageContents[index], pdfComponentMgr, allocator );
                if ( pageContents[index] )
                {
                    return pageContents[index];
                }
            }
            pdfPageTree->ReleasePage( pdfPage );
        }
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