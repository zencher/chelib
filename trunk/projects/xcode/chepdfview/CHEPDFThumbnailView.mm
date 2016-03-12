//
//  CHEPDFThumbnailView.m
//  chelib
//
//  Created by zencher on 16/3/6.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import "CHEPDFThumbnailView.h"

@implementation CHEPDFThumbnailMainView

- (id)initWithFrame:(NSRect)frame
        parentView:(id)parent
{
    self = [super initWithFrame:frame];
    if (self) {
        parentScrollView = parent;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(viewFrameChanged)
                                                     name:NSViewFrameDidChangeNotification
                                                   object:parentScrollView];
    }
    return self;
}

- (BOOL)isFlipped
{
    return YES;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)viewFrameChanged
{
    if ( pdfDocData )
    {
        NSRect frame;
        NSSize contentSize;
    
        contentSize = [parentScrollView contentSize];

        [pdfDocData setThumbnailViewFrame:contentSize.width height:contentSize.height];
        [pdfDocData updateThumbnailLayout];
        
        frame.origin.x = 0;
        frame.origin.y = 0;
        frame.size = [pdfDocData getThumbnailContentSize];
        [self setFrame:frame];
        [self setNeedsDisplay:YES];
    }
}

- (void)setDocumentData:(PdfDocumentData *)doc
{
    NSRect frame;
    NSSize contentSize;
    
    pdfDocData = doc;
    contentSize = [parentScrollView contentSize];
    
    [pdfDocData setThumbnailViewFrame:contentSize.width height:contentSize.height];
    [pdfDocData updateThumbnailLayout];
    
    frame.origin.x = 0;
    frame.origin.y = 0;
    frame.size = [pdfDocData getThumbnailContentSize];
    [self setFrame:frame];
}

- (void)drawPages:(NSRect)rect
{
    NSRect visableRect;
    NSRect pageRectInView;
    NSRect frame = [self frame];
    visableRect = [parentScrollView documentVisibleRect];
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    if ( context )
    {
        HE_UINT32 rotate = 0;
        HE_PDF_PAGE_RANGE range = [pdfDocData getCurPageRange];
        for ( HE_ULONG i = range.pageStart ; i < range.pageStart + range.pageCount; ++i )
        {      
            pageRectInView = [pdfDocData getPageRectInThumbnailView:i];
            if ( CGRectIntersectsRect( rect, pageRectInView ) )
            {
                CGContextSaveGState( context );
                [self drawPageBorderAndShadow:context bound:pageRectInView];
                CHE_Rect pageRect = [pdfDocData getPageRect:i];
                CHE_PDF_Renderer render( context );
                render.SetPosition( pageRectInView.origin.x, pageRectInView.origin.y );
                render.SetPatternOffset( 0/*leftFrame.size.width + 1*/,  visableRect.size.height - frame.size.height );
                render.Render( *[pdfDocData getPageContent:i], pageRect, rotate, [pdfDocData getPageScaleInThumbnailView:i], 72, 72 );
                CGContextRestoreGState( context );
            }
        }
    }
}

- (void)drawPageBorderAndShadow:(CGContextRef)context bound:(NSRect)rect
{
    CGContextSaveGState( context );
    CGContextSetShadow( context, CGSizeMake( 1, 1 ), 10 );
    CGContextSetRGBFillColor( context, 1, 1, 1, 1 );
    CGContextFillRect( context, rect );
    CGContextRestoreGState( context );
}

- (void)drawRect:(NSRect)dirtyRect
{
    [[NSColor lightGrayColor] set];
    NSRectFill( dirtyRect );
    if ( pdfDocData )
    {
        [self drawPages:dirtyRect];
    }
}

@end


@implementation CHEPDFThumbnailView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        mainView = [[CHEPDFThumbnailMainView alloc] initWithFrame:frame parentView:self];
        [self setDocumentView:mainView];
        [self setAutohidesScrollers:YES];
        [self setHasVerticalScroller:YES];
        [self setHasHorizontalScroller:NO];
        [self setBorderType:NSNoBorder];
        [self setBackgroundColor:[NSColor lightGrayColor]];
    }
    return self;
}

- (void)setDocumentData:(PdfDocumentData *)doc
{
    [mainView setDocumentData:doc];
}

@end