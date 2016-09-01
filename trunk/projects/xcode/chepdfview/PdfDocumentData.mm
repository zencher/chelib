//
//  PdfDocumentData.m
//  chelib
//
//  Created by Frank Zeng on 13-7-25.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import "PdfDocumentData.h"



class IHE_NSDataRead : public IHE_Read
{
public:
    IHE_NSDataRead(NSData * data, CHE_Allocator * pAllocator = NULL);
    
    ~IHE_NSDataRead();
    
    size_t	GetSize();
    
    size_t	ReadBlock( void * buffer, size_t offset, size_t size );
    
    BYTE	ReadByte( size_t offset );
    
    void    Release();
    
private:
    NSData *    mpData;
};

IHE_NSDataRead::IHE_NSDataRead(NSData * data, CHE_Allocator * pAllocator)
 :IHE_Read(pAllocator)
{
    mpData = data;
}

IHE_NSDataRead::~IHE_NSDataRead()
{
}

size_t IHE_NSDataRead::GetSize()
{
    return [mpData length];
}

size_t IHE_NSDataRead::ReadBlock( void * buffer, size_t offset, size_t size )
{
    [mpData getBytes:buffer range:NSMakeRange(offset, size)];
    return size;
}

BYTE IHE_NSDataRead::ReadByte( size_t offset )
{
    BYTE byte;
    [mpData getBytes:&byte range:NSMakeRange(offset, 1)];
    return byte;
}

void IHE_NSDataRead::Release()
{
}



@implementation PdfDocumentData

-(id)initWithNSData:(NSData*)data
{
    self = [super init];
    if (self) {
        pageCount       = 0;
        fileReadInf     = NULL;
        pdfFile         = NULL;
        pdfDocument     = NULL;
        pdfPageTree     = NULL;
        
        allocator = GetDefaultAllocator();
        
        fileReadInf = allocator->New<IHE_NSDataRead>(data, allocator);// HE_CreateFileRead( [path cStringUsingEncoding:NSUTF8StringEncoding] );
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
                        
                        pdfThumbnailLayout = new CHE_PDF_ThumbnailPageLayout;
                        
                        if ( pdfPageLayout )
                        {
                            for ( int i = 0; i < pdfPageTree->GetPageCount(); ++i)
                            {
                                CHE_PDF_Page * pdfPage = pdfPageTree->GetPage( i );
                                if ( pdfPage )
                                {
                                    CHE_Rect rect = pdfPage->GetPageRect();
                                    uint32 rotate = pdfPage->GetRotate() % 360;
                                    if ( rotate == 90 || rotate == 270 )
                                    {
                                        pdfPageLayout->AddPageSize(rect.height, rect.width);
                                        pdfThumbnailLayout->AddPageSize(rect.height, rect.width);
                                    }else{
                                        pdfPageLayout->AddPageSize(rect.width, rect.height);
                                        pdfThumbnailLayout->AddPageSize(rect.width, rect.height);
                                    }
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
                                    uint32 rotate = pdfPage->GetRotate() % 360;
                                    if ( rotate == 90 || rotate == 270 )
                                    {
                                        pdfPageLayout->AddPageSize(rect.height, rect.width);
                                        pdfThumbnailLayout->AddPageSize(rect.height, rect.width);
                                    }else{
                                        pdfPageLayout->AddPageSize(rect.width, rect.height);
                                        pdfThumbnailLayout->AddPageSize(rect.width, rect.height);
                                    }
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
    if (pdfThumbnailLayout) {
        delete pdfThumbnailLayout;
        pdfThumbnailLayout = NULL;
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

-(CHE_PDF_ContentObjectList*)getPageContent:(size_t)index
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

-(size_t)getPageCount
{
    return pdfPageTree->GetPageCount();
}

-(int32)getPageRotate:(size_t)index
{
    uint32 rotate = 0;
    if ( index < pageCount )
    {
        CHE_PDF_Page * pdfPage = pdfPageTree->GetPage( index );
        if ( pdfPage )
        {
            rotate = pdfPage->GetRotate();
        }
    }
    return rotate;
}

-(CHE_Rect)getPageRect:(size_t)index
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

-(void)setThumbnailViewFrame:(CGFloat)w height:(CGFloat)h
{
    pdfThumbnailLayout->SetViewSize(w, h);
}

-(void)updateLayout
{
    pdfPageLayout->UpdatePageInViewRectInfo();
}

-(void)updateThumbnailLayout
{
    pdfThumbnailLayout->UpdatePageInViewRectInfo();
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

-(NSRect)getPageRectInView:(size_t)pageIndex
{
    NSRect rect;
    CHE_Page_Rect pageRect = pdfPageLayout->GetPageRectInView(pageIndex);
    rect.origin.x = pageRect.left;
    rect.origin.y = pageRect.top;
    rect.size.width = pageRect.Width();
    rect.size.height = pageRect.Height();
    return rect;
}

-(CGSize)getThumbnailContentSize
{
    HE_PDF_PAGE_SIZE size = pdfThumbnailLayout->GetContentSize();
    CGSize sizeRet;
    sizeRet.width = size.width;
    sizeRet.height = size.height;
    return sizeRet;
}

-(NSRect)getPageRectInThumbnailView:(size_t)pageIndex
{
    NSRect rect;
    CHE_Page_Rect pageRect = pdfThumbnailLayout->GetPageRectInView(pageIndex);
    rect.origin.x = pageRect.left;
    rect.origin.y = pageRect.top;
    rect.size.width = pageRect.Width();
    rect.size.height = pageRect.Height();
    return rect;
}

-(CGFloat)getPageScaleInThumbnailView:(size_t)pageIndex
{
    return pdfThumbnailLayout->GetPageScaleInView(pageIndex);
}

-(CGFloat)getPageScaleInViwe:(size_t)pageIndex
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
