//
//  CHEPDFThumbnailView.m
//  chelib
//
//  Created by zencher on 16/3/6.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import "CHEPDFThumbnailView.h"

@implementation CHEPDFThumbnailMainView

-(id)initWithFrame:(NSRect)frame
        parentView:(id)parent
{
    self = [super initWithFrame:frame];
    if (self)
    {
        //[self setCanDrawConcurrently:YES];
        parentScrollView = parent;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(parentScrollViewFrameChanged)
                                                     name:NSViewFrameDidChangeNotification
                                                   object:parentScrollView];
    }
    return self;
}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

-(BOOL)isFlipped
{
    return YES;
}

- (void)drawRect:(NSRect)dirtyRect
{
    if ( pdfDocData )
    {
        [self drawPages:dirtyRect];
    }
}

-(void)parentScrollViewFrameChanged
{
    if ( pdfDocData )
    {
        NSRect visible = [parentScrollView documentVisibleRect];
        NSRect pframe = [parentScrollView frame];
        NSSize contentSize = [NSScrollView contentSizeForFrameSize:pframe.size horizontalScrollerClass:[[parentScrollView horizontalScroller] class] verticalScrollerClass:[[parentScrollView verticalScroller] class] borderType:[parentScrollView borderType] controlSize:NSRegularControlSize scrollerStyle:[parentScrollView scrollerStyle]];
        //NSSize contentSize = pframe.size;
        
        [pdfDocData setThumbnailViewFrame:contentSize.width height:contentSize.height];
        [pdfDocData updateThumbnailLayout];
        
        NSRect frame;
        frame.origin.x = 0;
        frame.origin.y = 0;
        frame.size = [pdfDocData getThumbnailContentSize];

        CGFloat rate = visible.origin.y / oldContentSize.height;
        CGFloat positionY = rate * frame.size.height;
        oldContentSize = frame.size;
        
        [self setFrame:frame];
        [self scrollPoint:NSMakePoint(0, positionY)];
    }
}

//绘制页面的边框和阴影效果
-(void)drawPageBorderAndShadow:(CGContextRef)context bound:(NSRect)rect
{
    CGContextSaveGState( context );
    CGContextSetShadow( context, CGSizeMake( 1, 1 ), 10 );
    CGContextSetRGBFillColor( context, 1, 1, 1, 1 );
    CGContextFillRect( context, rect );
    CGContextRestoreGState( context );
}

-(void)drawPages:(NSRect)rect
{
    NSRect visableRect;
    NSRect pageRectInView;
    NSRect pageRectWithShadowInView;
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
            pageRectWithShadowInView = pageRectInView;
            pageRectWithShadowInView.origin.x -= 10;
            pageRectWithShadowInView.origin.y -= 10;
            pageRectWithShadowInView.size.width += 10;
            pageRectWithShadowInView.size.height += 30;
            
            if ( CGRectIntersectsRect( rect, pageRectWithShadowInView ) )
            {
                [self drawPageBorderAndShadow:context bound:pageRectInView];
                
                CGContextSaveGState( context );
                CHE_Rect pageRect = [pdfDocData getPageRect:i];
                CHE_PDF_Renderer render( context );
                render.SetPosition( pageRectInView.origin.x, pageRectInView.origin.y );
                render.SetPatternOffset( 0/*leftFrame.size.width + 1*/,  visableRect.size.height - frame.size.height );
                
                rotate = [pdfDocData getPageRotate:i] % 360;
                rotate = 360 - rotate;

                render.Render( *[pdfDocData getPageContent:i], pageRect, rotate, [pdfDocData getPageScaleInThumbnailView:i], 72, 72 );
                CGContextRestoreGState( context );
                
                NSString * str = [[NSString alloc] initWithFormat:@"%ld", i + 1];
                NSMutableParagraphStyle *style = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
                [style setAlignment:NSCenterTextAlignment];
                NSDictionary *attr = [NSDictionary dictionaryWithObject:style forKey:NSParagraphStyleAttributeName];
                pageRectInView.origin.y  += pageRectInView.size.height + 5;
                pageRectInView.size.height = 30;
                [str drawInRect:pageRectInView withAttributes:attr];
            }
        }
    }
}

-(void)setDocumentData:(PdfDocumentData *)docData
{
    NSRect frame;
    NSRect pframe;
    NSSize contentSize;
    
    pdfDocData = docData;
    pframe = [parentScrollView frame];
    contentSize = [NSScrollView contentSizeForFrameSize:pframe.size horizontalScrollerClass:[[parentScrollView horizontalScroller] class] verticalScrollerClass:[[parentScrollView verticalScroller] class] borderType:[parentScrollView borderType] controlSize:NSRegularControlSize scrollerStyle:[parentScrollView scrollerStyle]];
    
    [pdfDocData setThumbnailViewFrame:contentSize.width height:contentSize.height];
    [pdfDocData updateThumbnailLayout];
    
    frame.origin.x = 0;
    frame.origin.y = 0;
    oldContentSize = frame.size = [pdfDocData getThumbnailContentSize];
    [self setFrame:frame];
}

@end



@implementation CHEPDFThumbnailScrollView

-(id)initWithFrame:(NSRect)frameRect
        parentView:(id)parent
{
    self = [super initWithFrame:frameRect];
    if (self) {
        mainView = [[CHEPDFThumbnailMainView alloc] initWithFrame:frameRect parentView:self];
        [self setDocumentView:mainView];
        [self setAutohidesScrollers:YES];
        [self setHasVerticalScroller:YES];
        [self setHasHorizontalScroller:NO];
        [self setDrawsBackground:NO];
        [self setBorderType:NSNoBorder];
        
        parentView = parent;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(parentViewFrameChanged)
                                                     name:NSViewFrameDidChangeNotification
                                                   object:parentView];
    }
    return self;
}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

-(void)parentViewFrameChanged
{
    [self setFrame:[parentView frame]];
}

-(void)setDocumentData:(PdfDocumentData*)docData
{
    [mainView setDocumentData:docData];
}

@end



@implementation CHEPDFThumbnailView

-(id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self) {
        scrollView = [[CHEPDFThumbnailScrollView alloc] initWithFrame:frameRect parentView:self];
        [self addSubview:scrollView];
    }
    return self;
}

-(void)setDocumentData:(PdfDocumentData*)docData
{
    [scrollView setDocumentData:docData];
}

@end