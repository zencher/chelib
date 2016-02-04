//
//  CHEPDFImpView.m
//  chelib
//
//  Created by Frank Zeng on 14-4-3.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import "CHEPDFMainView.h"

@implementation CHEPDFMainView

-(id)initWithFrameAndParentView:(NSRect)frame
                         parentView:(id)view;
{
    self = [super initWithFrame:frame];
    if (self)
    {
        parentScrollView = view;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(parentScrollViewFrameChanged)
                                                     name:NSViewFrameDidChangeNotification
                                                   object:nil];
        //change object from parentScrollView to nil, can fix the contentSize not stable in a view size changing process. But Why?
    }
    return self;
}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

-(BOOL)isFlipped
{
    return YES;
}

- (void)drawRect:(NSRect)dirtyRect
{
    [[NSColor lightGrayColor] set];
    NSRectFill( dirtyRect );
    if ( pdfDocument )
    {
        [self drawPages:dirtyRect];
    }
}

- (void)mouseDown:(NSEvent *)theEvent
{
    //self->mouseDownPt = [theEvent locationInWindow];
    
    //[self setScale:[self scale] + 0.2];
    
    /*if ( _zoomMode == ZOOM_MODE_FIT_WIDTH || _zoomMode == ZOOM_MODE_FIT_HEIGHT )
    {
        NSRect frame;
        frame.origin.x = 0;
        frame.origin.y = 0;
        frame.size = [self getViewContentSize];
        [self setFrame:frame];
        
    }*/
}

- (void)scrollWheel:(NSEvent *)theEvent
{
    if ( pdfDocument )
    {
        PDFVIEW_PAGE_MODE pageMode = [pdfDocument getPageMode];
        if ( pageMode == PAGE_MODE_SINGLE || pageMode == PAGE_MODE_DOUBLE )
        {
            CGFloat deltaY = [theEvent deltaY];
            if ( deltaY > 0 )
            {
                [self prevPage];
                //[pdfDocument prePage];
                //[pdfDocument updateLayout];
                //[self setNeedsDisplay:YES];
            }else{
                [self nextPage];
                //[pdfDocument nextPage];
                //[pdfDocument updateLayout];
                //[self setNeedsDisplay:YES];
            }
            return;
        }
    }
    [super scrollWheel:theEvent];
    //NSLog( @"deltaX %f, deltaY %f, deltaZ %f", [theEvent deltaX], [theEvent deltaY], [theEvent deltaZ] );
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    //NSPoint currentLocation = [theEvent locationInWindow];
    //self->pageOffset.width = currentLocation.x - self->mouseDownPt.x;
    //self->pageOffset.height = currentLocation.y - self->mouseDownPt.y;
    //[self setNeedsDisplay:YES];
}

-(void)parentScrollViewFrameChanged
{
    if ( pdfDocument )
    {
        NSRect frame;
        NSSize contentSize;
        NSRect visible = [parentScrollView documentVisibleRect];
        CGFloat rate;
        CGFloat positionY;

        //NSRect pframe;
        //pframe = [parentScrollView frame];
        //contentSize = [NSScrollView contentSizeForFrameSize:pframe.size horizontalScrollerClass:[[parentScrollView horizontalScroller] class] verticalScrollerClass:[[parentScrollView verticalScroller] class] borderType:[parentScrollView borderType] controlSize:NSRegularControlSize scrollerStyle:[parentScrollView scrollerStyle]];
        
        contentSize = [parentScrollView contentSize];
        [pdfDocument setViewFrame:contentSize.width height:contentSize.height];
        [pdfDocument updateLayout];
        
        frame.origin.x = 0;
        frame.origin.y = 0;
        frame.size = [pdfDocument getContentSize];
        rate = visible.origin.y / oldContentSize.height;
        positionY = rate * frame.size.height;
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
    //NSLog( @"view:x=%f,y=%f,w=%f,h=%f", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height );
    NSRect visableRect;
    NSRect pageRectInView;
    NSRect frame = [self frame];
    visableRect = [parentScrollView documentVisibleRect];
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    if ( context )
    {
        HE_UINT32 rotate = 0;
        switch ( [pdfDocument getRotateMode] )
        {
            case ROTATE_MODE_0:
                rotate = 0;
                break;
            case ROTATE_MODE_90:
                rotate = 270;
                break;
            case ROTATE_MODE_180:
                rotate = 180;
                break;
            case ROTATE_MODE_270:
                rotate = 90;
            default:
                break;
        }
        
        HE_PDF_PAGE_RANGE range = [pdfDocument getCurPageRange];
        for ( HE_ULONG i = range.pageStart ; i < range.pageStart + range.pageCount; ++i )
        {
            pageRectInView = [pdfDocument getPageRectInView:i];
            if ( CGRectIntersectsRect( rect, pageRectInView ) )
            {
                CGContextSaveGState( context );
                CGContextAddRect( context, rect );
                CGContextClip( context );
                
                [self drawPageBorderAndShadow:context bound:pageRectInView];
                
                CHE_Rect pageRect = [pdfDocument getPageRect:i];
                
                CHE_PDF_Renderer render( context );
                render.SetPosition( pageRectInView.origin.x, pageRectInView.origin.y );
                render.SetPatternOffset( 0,  visableRect.size.height - frame.size.height );
                
                /*NSLog(@"visable(%f, %f, %f, %f)", visableRect.origin.x, visableRect.origin.y,
                      visableRect.origin.x + visableRect.size.width,
                      visableRect.origin.y + visableRect.size.height);
                NSLog(@"page(%f, %f, %f, %f)", pageRectInView.origin.x, pageRectInView.origin.y,
                      pageRectInView.origin.x + pageRectInView.size.width,
                      pageRectInView.origin.y + pageRectInView.size.height);
                NSLog(@"frame(%f, %f, %f, %f)", frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
                NSLog(@"offsetX %f, offsetY %f", 0.0f, visableRect.size.height - frame.size.height);*/
                
                render.Render( *[pdfDocument getPageContent:i], pageRect, rotate, [pdfDocument getPageScaleInViwe:i], 72, 72 );
                
                CGContextRestoreGState( context );
            }
        }
    }
    
    //NSLog(@"%f, %f", [parentScrollView horizontalLineScroll], [parentScrollView verticalLineScroll]);
}

-(void)newDocument:(PdfDocumentData*)doc
{
    NSRect frame;
    NSRect pframe;
    NSSize contentSize;
    
    pdfDocument = doc;
    pframe = [parentScrollView frame];
    contentSize = [NSScrollView contentSizeForFrameSize:pframe.size horizontalScrollerClass:[[parentScrollView horizontalScroller] class] verticalScrollerClass:[[parentScrollView verticalScroller] class] borderType:[parentScrollView borderType] controlSize:NSRegularControlSize scrollerStyle:[parentScrollView scrollerStyle]];
    
    [pdfDocument setViewFrame:contentSize.width height:contentSize.height];

    [pdfDocument setScale:1.0f];
    [pdfDocument setRotateMode:ROTATE_MODE_0];
    [pdfDocument setZoomMode:ZOOM_MODE_FIT];
    //[pdfDocument setZoomMode:ZOOM_MODE_FIX];
    //[pdfDocument setPageMode:PAGE_MODE_SINGLE];
    //[pdfDocument setPageMode:PAGE_MODE_DOUBLE];
    [pdfDocument setPageMode:PAGE_MODE_SINGLE_SCROLL];
    //[pdfDocument setPageMode:PAGE_MODE_DOUBLE_SCROLL];
    [pdfDocument updateLayout];
    
    
    frame.origin.x = 0;
    frame.origin.y = 0;
    oldContentSize = frame.size = [pdfDocument getContentSize];
    [self setFrame:frame];
}

-(void)closeDocument
{
    if ( pdfDocument )
    {
        [pdfDocument dealloc];
        pdfDocument = nil;
    }
    NSRect rect;
    [self setFrame:rect];
    [self setNeedsDisplay:YES];
}

-(void)nextPage
{
    [pdfDocument nextPage];
    [pdfDocument updateLayout];
    [self setNeedsDisplay:YES];
}

-(void)prevPage
{
    [pdfDocument prePage];
    [pdfDocument updateLayout];
    [self setNeedsDisplay:YES];
}

-(void)rotate
{
    NSRect frame;
    NSSize contentSize;
    NSRect visible = [parentScrollView documentVisibleRect];
    CGFloat rate;
    CGFloat positionY;
    [pdfDocument rotate];
    contentSize = [parentScrollView contentSize];
    [pdfDocument setViewFrame:contentSize.width height:contentSize.height];
    [pdfDocument updateLayout];
    
    frame.origin.x = 0;
    frame.origin.y = 0;
    frame.size = [pdfDocument getContentSize];
    [self setFrame:frame];
    rate = visible.origin.y / oldContentSize.height;
    positionY = rate * frame.size.height;
    oldContentSize = frame.size;
    
    [self setFrame:frame];
    [self scrollPoint:NSMakePoint(0, positionY)];
    [self setNeedsDisplay:YES];
}

@end