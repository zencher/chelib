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
                                                   object:parentScrollView];
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
    //NSLog(@"draw...");
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
        NSRect pframe;
        NSSize contentSize;
        
        pframe = [parentScrollView frame];
        //contentSize = [NSScrollView contentSizeForFrameSize:pframe.size horizontalScrollerClass:[[parentScrollView horizontalScroller] class] verticalScrollerClass:[[parentScrollView verticalScroller] class] borderType:[parentScrollView borderType] controlSize:NSRegularControlSize scrollerStyle:[parentScrollView scrollerStyle]];
        
        contentSize = [parentScrollView contentSize];
        [pdfDocument setViewFrame:contentSize.width height:contentSize.height];
        [pdfDocument updateLayout];
        
        frame.origin.x = 0;
        frame.origin.y = 0;
        frame.size = [pdfDocument getContentSize];
        [self setFrame:frame];

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
    
    NSRect pageRectInView;
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
                render.Render( *[pdfDocument getPageContent:i], pageRect, rotate, [pdfDocument getPageScaleInViwe:i], 72, 72 );
                
                CGContextRestoreGState( context );
            }
        }
    }
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
    [pdfDocument setRotateMode:ROTATE_MODE_90];
    //[pdfDocument setZoomMode:ZOOM_MODE_FIT];
    [pdfDocument setZoomMode:ZOOM_MODE_FIX];
    [pdfDocument setPageMode:PAGE_MODE_SINGLE];
    //[pdfDocument setPageMode:PAGE_MODE_SINGLE_SCROLL];
    [pdfDocument updateLayout];
    
    frame.origin.x = 0;
    frame.origin.y = 0;
    frame.size = [pdfDocument getContentSize];
    [self setFrame:frame];
}

-(void)closeDocument
{
    pdfDocument = nil;
    NSRect rect;
    [self setFrame:rect];
    [self setNeedsDisplay:YES];
}

@end