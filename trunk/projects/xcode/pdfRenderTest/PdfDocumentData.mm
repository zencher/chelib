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
        
        allocator = GetDefaultAllocator();
        
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
                        
                        pdfPageLayout = new CHE_PDF_PageLayout;
                        
                        if ( pdfPageLayout )
                        {
                            for ( int i = 0; i < pdfPageTree->GetPageCount(); ++i)
                            {
                                CHE_PDF_Page * pdfPage = pdfPageTree->GetPage( i );
                                if ( pdfPage )
                                {
                                    CHE_Rect rect = pdfPage->GetPageRect();
                                    pdfPageLayout->AddPageSize( rect.width, rect.height );
                                    
                                }
                            }
                        }
                    }
                }
            }
        }
        
        pdfoutlineRoot = allocator->New<CHE_PDF_OutlineItem>(allocator);
        pdfoutlineRoot->mpFirst = NULL;
        pdfoutlineRoot->mpLast = NULL;
    }
    return self;
}

-(void)dealloc
{
    if (pdfPageLayout) {
        delete pdfPageLayout;
        pdfPageLayout = NULL;
    }
    if (pdfDocument) {
        pdfDocument->GetAllocator()->Delete(pdfDocument);
        pdfDocument = NULL;
    }
    if (pdfFile) {
        pdfFile->GetAllocator()->Delete(pdfFile);
        pdfFile = NULL;
    }
    if (fileReadInf) {
        HE_DestoryIHERead(fileReadInf);
        fileReadInf = NULL;
    }
}

-(CHE_PDF_ContentObjectList*)getPageContent:(HE_ULONG)index
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

-(CHE_Rect)getPageRect:(HE_ULONG)index
{
    CHE_Rect rect;
    if ( index < pageCount )
    {
        CHE_PDF_Page * pdfPage = pdfPageTree->GetPage( index );
        if ( pdfPage )
        {
            rect = pdfPage->GetPageRect();
        }
    }
    return rect;
}

-(void)setPageMode:(PDFVIEW_PAGE_MODE)mode
{
    switch ( mode ) {
        case PAGE_MODE_SINGLE:
            pdfPageLayout->SetPageMode( PAGE_SINGLE );
            break;
        case PAGE_MODE_DOUBLE:
            pdfPageLayout->SetPageMode( PAGE_DOUBLE );
            break;
        case PAGE_MODE_SINGLE_SCROLL:
            pdfPageLayout->SetPageMode( PAGE_SINGLE_SCROLL );
            break;
        case PAGE_MODE_DOUBLE_SCROLL:
            pdfPageLayout->SetPageMode( PAGE_DOUBLE_SCROLL );
            break;
        default:
            break;
    }
}

-(PDFVIEW_PAGE_MODE)getPageMode
{
    HE_PDF_VIEW_PAGE_MODE mode = pdfPageLayout->GetPageMode();
    switch ( mode ) {
        case PAGE_SINGLE:
            return PAGE_MODE_SINGLE;
        case PAGE_DOUBLE:
            return PAGE_MODE_DOUBLE;
        case PAGE_SINGLE_SCROLL:
            return PAGE_MODE_SINGLE_SCROLL;
        case PAGE_DOUBLE_SCROLL:
            return PAGE_MODE_DOUBLE_SCROLL;
        default:
            break;
    }
    return PAGE_MODE_SINGLE;
}

-(void)setZoomMode:(PDFVIEW_ZOOM_MODE)mode
{
    switch ( mode ) {
        case ZOOM_MODE_FIX:
            pdfPageLayout->SetZoomMode( ZOOM_FIX );
            break;
        case ZOOM_MODE_FIT:
            pdfPageLayout->SetZoomMode( ZOOM_FIT );
            break;
        default:
            break;
    }
}

-(PDFVIEW_ZOOM_MODE)getZoomMode
{
    HE_PDF_VIEW_ZOOM_MODE mode = pdfPageLayout->GetZoomMode();
    switch ( mode ) {
        case ZOOM_FIX:
            return ZOOM_MODE_FIX;
        case ZOOM_FIT:
            return ZOOM_MODE_FIT;
        default:
            break;
    }
    return ZOOM_MODE_FIX;
}


-(void)setRotateMode:(PDFVIEW_ROTATE_MODE)mode
{
    switch ( mode ) {
        case ROTATE_MODE_0:
            pdfPageLayout->SetRotateMode( ROTATE_0 );
            break;
        case ROTATE_MODE_90:
            pdfPageLayout->SetRotateMode( ROTATE_90 );
            break;
        case ROTATE_MODE_180:
            pdfPageLayout->SetRotateMode( ROTATE_180 );
            break;
        case ROTATE_MODE_270:
            pdfPageLayout->SetRotateMode( ROTATE_270 );
            break;
        default:
            break;
    }
}

-(PDFVIEW_ROTATE_MODE)getRotateMode
{
    HE_PDF_VIEW_ROTATE_MODE mode = pdfPageLayout->GetRotateMode();
    switch ( mode ) {
        case ROTATE_0:
            return ROTATE_MODE_0;
        case ROTATE_90:
            return ROTATE_MODE_90;
        case ROTATE_180:
            return ROTATE_MODE_180;
        case ROTATE_270:
            return ROTATE_MODE_270;
        default:
            break;
    }
    return ROTATE_MODE_0;
}

-(void)setScale:(CGFloat)scale
{
    pdfPageLayout->SetScale( scale );
}

-(CGFloat)getScale
{
    return pdfPageLayout->GetScale();
}

-(void)setViewFrame:(CGFloat)w height:(CGFloat)h
{
    pdfPageLayout->SetViewSize( w, h );
}

-(void)updateLayout
{
    pdfPageLayout->UpdatePageInViewRectInfo();
}

-(void)nextPage
{
    HE_PDF_PAGE_RANGE range = pdfPageLayout->GetCurPageRange();
    if ( pdfPageLayout->GetPageMode() == PAGE_SINGLE )
    {
        if ( range.pageStart + 1 < pdfPageTree->GetPageCount() )
        {
            pdfPageLayout->SetCurPageRange( range.pageStart + 1, 1 );
        }
    }else if ( pdfPageLayout->GetPageMode() == PAGE_DOUBLE )
    {
        if ( range.pageStart + 2 < pdfPageTree->GetPageCount() )
        {
            pdfPageLayout->SetCurPageRange( range.pageStart + 2, 2 );
        }else if ( range.pageStart + 1 < pdfPageTree->GetPageCount() )
        {
            pdfPageLayout->SetCurPageRange( range.pageStart + 1, 1 );
        }
    }
}

-(void)prePage
{
    HE_PDF_PAGE_RANGE range = pdfPageLayout->GetCurPageRange();
    if ( pdfPageLayout->GetPageMode() == PAGE_SINGLE )
    {
        if ( range.pageStart != 0 )
        {
            pdfPageLayout->SetCurPageRange( range.pageStart - 1, 1 );
        }
    }else if ( pdfPageLayout->GetPageMode() == PAGE_DOUBLE )
    {
        if ( range.pageStart >= 2 )
        {
            pdfPageLayout->SetCurPageRange( range.pageStart - 2, 2 );
        }
    }
}

-(void)rotate
{
    HE_PDF_VIEW_ROTATE_MODE mode = pdfPageLayout->GetRotateMode();
    if ( mode == ROTATE_0 )
    {
        pdfPageLayout->SetRotateMode( ROTATE_90 );
    }else if ( mode == ROTATE_90 )
    {
        pdfPageLayout->SetRotateMode( ROTATE_180 );
    }else if ( mode == ROTATE_180 )
    {
        pdfPageLayout->SetRotateMode( ROTATE_270 );
    }else{
        pdfPageLayout->SetRotateMode( ROTATE_0 );
    }
}

-(CGSize)getContentSize
{
    HE_PDF_PAGE_SIZE size = pdfPageLayout->GetContentSize();
    CGSize sizeRet;
    sizeRet.width = size.width;
    sizeRet.height = size.height;
    return sizeRet;
}

-(NSRect)getPageRectInView:(HE_ULONG)pageIndex
{
    NSRect rect;
    CHE_Page_Rect pageRect = pdfPageLayout->GetPageRectInView(pageIndex);
    
    rect.origin.x = pageRect.left;
    rect.origin.y = pageRect.top;
    rect.size.width = pageRect.Width();
    rect.size.height = pageRect.Height();
    
    return rect;
}

-(CGFloat)getPageScaleInViwe:(HE_ULONG)pageIndex
{
    return pdfPageLayout->GetPageScaleInView(pageIndex);
}

-(HE_PDF_PAGE_RANGE)getCurPageRange
{
    return pdfPageLayout->GetCurPageRange();
}

-(CHE_PDF_Outline*)getOutline
{
    return pdfDocument->GetOutline();
}

-(CHE_PDF_OutlineItem*)getOutlineRoot
{
    CHE_PDF_Outline * outline = pdfDocument->GetOutline();
    if (outline) {
        pdfoutlineRoot->mpFirst = outline->First();
        pdfoutlineRoot->mpLast = outline->Last();
        pdfoutlineRoot->mTitle = "Outline";
    }else{
        pdfoutlineRoot->mpFirst = NULL;
        pdfoutlineRoot->mpLast = NULL;
    }
    return pdfoutlineRoot;
}

@end
