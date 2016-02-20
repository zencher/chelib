//
//  CHEPDFPageView.m
//  chelib
//
//  Created by zencher on 16/2/19.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import "CHEPDFPageView.h"



@implementation CHEPDFPageMainView

-(id)initWithFrame:(NSRect)frame
        ParentView:(id)parent
          RootView:(id)root;
{
    self = [super initWithFrame:frame];
    if (self)
    {
        parentScrollView = parent;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(parentScrollViewFrameChanged)
                                                     name:NSViewFrameDidChangeNotification
                                                   object:parentScrollView/*nil*/];
        //change object from parentScrollView to nil, can fix the contentSize not stable in a view size changing process. But Why?
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
    [[NSColor lightGrayColor] set];
    NSRectFill( dirtyRect );
    if ( pdfDocData )
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
    if ( pdfDocData )
    {
        PDFVIEW_PAGE_MODE pageMode = [pdfDocData getPageMode];
        if ( pageMode == PAGE_MODE_SINGLE || pageMode == PAGE_MODE_DOUBLE )
        {
            CGFloat deltaY = [theEvent deltaY];
            if ( deltaY > 0 )
            {
                [self prevPage];
            }else{
                [self nextPage];
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
    if ( pdfDocData )
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
        [pdfDocData setViewFrame:contentSize.width height:contentSize.height];
        [pdfDocData updateLayout];
        
        frame.origin.x = 0;
        frame.origin.y = 0;
        frame.size = [pdfDocData getContentSize];
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
    
    //NSRect leftFrame = [leftView frame];
    
    visableRect = [parentScrollView documentVisibleRect];
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    if ( context )
    {
        HE_UINT32 rotate = 0;
        switch ( [pdfDocData getRotateMode] )
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
        
        HE_PDF_PAGE_RANGE range = [pdfDocData getCurPageRange];
        for ( HE_ULONG i = range.pageStart ; i < range.pageStart + range.pageCount; ++i )
        {
            pageRectInView = [pdfDocData getPageRectInView:i];
            if ( CGRectIntersectsRect( rect, pageRectInView ) )
            {
                CGContextSaveGState( context );
                CGContextAddRect( context, rect );
                CGContextClip( context );
                
                [self drawPageBorderAndShadow:context bound:pageRectInView];
                
                CHE_Rect pageRect = [pdfDocData getPageRect:i];
                
                CHE_PDF_Renderer render( context );
                render.SetPosition( pageRectInView.origin.x, pageRectInView.origin.y );
                render.SetPatternOffset( 0/*leftFrame.size.width + 1*/,  visableRect.size.height - frame.size.height );
                
                /*NSLog(@"visable(%f, %f, %f, %f)", visableRect.origin.x, visableRect.origin.y,
                 visableRect.origin.x + visableRect.size.width,
                 visableRect.origin.y + visableRect.size.height);
                 NSLog(@"page(%f, %f, %f, %f)", pageRectInView.origin.x, pageRectInView.origin.y,
                 pageRectInView.origin.x + pageRectInView.size.width,
                 pageRectInView.origin.y + pageRectInView.size.height);
                 NSLog(@"frame(%f, %f, %f, %f)", frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
                 NSLog(@"offsetX %f, offsetY %f", 0.0f, visableRect.size.height - frame.size.height);*/
                
                render.Render( *[pdfDocData getPageContent:i], pageRect, rotate, [pdfDocData getPageScaleInViwe:i], 72, 72 );
                
                CGContextRestoreGState( context );
            }
        }
    }
    
    //NSLog(@"%f, %f", [parentScrollView horizontalLineScroll], [parentScrollView verticalLineScroll]);
}

-(void)showDocument:(PdfDocumentData *)docData
{
    NSRect frame;
    NSRect pframe;
    NSSize contentSize;
    
    pdfDocData = docData;
    pframe = [parentScrollView frame];
    contentSize = [NSScrollView contentSizeForFrameSize:pframe.size horizontalScrollerClass:[[parentScrollView horizontalScroller] class] verticalScrollerClass:[[parentScrollView verticalScroller] class] borderType:[parentScrollView borderType] controlSize:NSRegularControlSize scrollerStyle:[parentScrollView scrollerStyle]];
    
    [pdfDocData setViewFrame:contentSize.width height:contentSize.height];
    
    [pdfDocData setScale:1.0f];
    [pdfDocData setRotateMode:ROTATE_MODE_0];
    [pdfDocData setZoomMode:ZOOM_MODE_FIT];
    //[pdfDocument setZoomMode:ZOOM_MODE_FIX];
    //[pdfDocument setPageMode:PAGE_MODE_SINGLE];
    //[pdfDocument setPageMode:PAGE_MODE_DOUBLE];
    [pdfDocData setPageMode:PAGE_MODE_SINGLE_SCROLL];
    //[pdfDocument setPageMode:PAGE_MODE_DOUBLE_SCROLL];
    [pdfDocData updateLayout];
    
    
    frame.origin.x = 0;
    frame.origin.y = 0;
    oldContentSize = frame.size = [pdfDocData getContentSize];
    [self setFrame:frame];
}

-(void)closeDocument
{
    pdfDocData = nil;
    
    //NSRect rect;
    //[self setFrame:rect];
    [self setNeedsDisplay:YES];
}

-(void)nextPage
{
    [pdfDocData nextPage];
    [pdfDocData updateLayout];
    [self setNeedsDisplay:YES];
}

-(void)prevPage
{
    [pdfDocData prePage];
    [pdfDocData updateLayout];
    [self setNeedsDisplay:YES];
}

-(void)rotate
{
    NSRect frame;
    NSSize contentSize;
    NSRect visible = [parentScrollView documentVisibleRect];
    CGFloat rate;
    CGFloat positionY;
    [pdfDocData rotate];
    contentSize = [parentScrollView contentSize];
    [pdfDocData setViewFrame:contentSize.width height:contentSize.height];
    [pdfDocData updateLayout];
    
    frame.origin.x = 0;
    frame.origin.y = 0;
    frame.size = [pdfDocData getContentSize];
    [self setFrame:frame];
    rate = visible.origin.y / oldContentSize.height;
    positionY = rate * frame.size.height;
    oldContentSize = frame.size;
    
    [self setFrame:frame];
    [self scrollPoint:NSMakePoint(0, positionY)];
    [self setNeedsDisplay:YES];
}

@end



@implementation CHEPDFPageView

-(id)initWithFrame:(NSRect)frameRect
          RootView:(id)root
{
    self = [super initWithFrame:frameRect];
    if (self) {
        mainView = [[CHEPDFPageMainView alloc] initWithFrame:frameRect ParentView:self RootView:root];
        [self setDocumentView:mainView];
        [self setAutohidesScrollers:YES];
        [self setHasVerticalScroller:YES];
        [self setHasHorizontalScroller:YES];
        [self setBorderType:NSNoBorder];
        [self setBackgroundColor:[NSColor lightGrayColor]];
    }
    return self;
}

-(void)showDocument:(PdfDocumentData*)docData
{
    [mainView showDocument:docData];
}

-(void)closeDocument
{
    [mainView closeDocument];
}

-(void)nextPage
{
    [mainView nextPage];
}

-(void)prevPage
{
    [mainView prevPage];
}

-(void)rotate
{
    [mainView rotate];
}

@end
