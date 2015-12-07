//
//  CHEPDFImpView.m
//  chelib
//
//  Created by Frank Zeng on 14-4-3.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import "CHEPDFMainView.h"

@implementation CHEPDFMainView

@synthesize pageSpaceX  = _pageSpaceX;
@synthesize pageSpaceY  = _pageSpaceY;
@synthesize scale       = _scale;
@synthesize pageMode    = _pageMode;
@synthesize zoomMode    = _zoomMode;
@synthesize rotateMode  = _rotateMode;

-(id)initWithFrameAndParentView:(NSRect)frame
                         parentView:(id)view;
{
    self = [super initWithFrame:frame];
    if (self)
    {
        parentScrollView = view;
        
        _pageSpaceY = 20;
        _pageSpaceX = 20;
        _scale = 2;
        _pageMode = PAGE_MODE_SINGLE_SCROLL;
        _zoomMode = ZOOM_MODE_ACTUAL;
        _rotateMode = ROTATE_MODE_0;
        
        allPageWidth        = 0;
        allPageHeight       = 0;
        biggestPageWidth    = 0;
        smallestPageWidth   = 0;
        biggestPageHeight   = 0;
        smallestPageHeight  = 0;
        
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
    NSLog(@"draw...");
    if ( pdfDocument )
    {
        [self drawPages:dirtyRect];
    }
}

- (void)mouseDown:(NSEvent *)theEvent
{
    //self->mouseDownPt = [theEvent locationInWindow];
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
    NSRect frame;
    frame.origin.x = 0;
    frame.origin.y = 0;
    frame.size = [self getViewContentSize];
    [self setFrame:frame];
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
    NSLog( @"view:x=%f,y=%f,w=%f,h=%f", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height );
    
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    if ( context )
    {
        CGRect pageRectInView = CGRectMake(0, 0, 0, 0);
        NSRect scrollFrame = [parentScrollView frame];
        
        CHE_Rect pageRect;
        HE_FLOAT tmpY = 0;
        HE_ULONG count = [pdfDocument getPageCount];
        for ( HE_UINT32 i = 0; i < count; ++i )
        {
            if ( _zoomMode == ZOOM_MODE_FIT_HEIGHT )
            {
                
            }else if ( _zoomMode == ZOOM_MODE_FIT_WIDTH )
            {
                
            }
            
            pageRect = [pdfDocument getPageRect:i];
            pageRect.width *= _scale;
            pageRect.height *= _scale;
            
            if ( _rotateMode == ROTATE_MODE_90 || _rotateMode == ROTATE_MODE_270 )
            {
                HE_FLOAT tmp = pageRect.width;
                pageRect.width = pageRect.height;
                pageRect.height = tmp;
            }
            
            switch ( _pageMode )
            {
                case PAGE_MODE_SINGLE:
                    break;
                case PAGE_MODE_DOUBLE:
                    break;
                case PAGE_MODE_SINGLE_SCROLL:
                {
                    tmpY += _pageSpaceY;
                    pageRectInView.origin.y = tmpY;
                    pageRectInView.size.width = pageRect.width;
                    pageRectInView.size.height = pageRect.height;
                    tmpY += pageRectInView.size.height;
                    break;
                }
                case PAGE_MODE_DOUBLE_SCROLL:
                {
                    break;
                }
                default:
                    break;
            }
            
            if ( scrollFrame.size.width-15 >= pageRectInView.size.width )
            {
                pageRectInView.origin.x = 15 + ( scrollFrame.size.width-15 - pageRectInView.size.width - _pageSpaceX * 2 ) / 2;
            }
            
            if ( CGRectIntersectsRect( rect, pageRectInView ) )
            {
                CGContextSaveGState( context );
                
                CGContextAddRect( context, rect );
                CGContextClip( context );
                
                [self drawPageBorderAndShadow:context bound:pageRectInView];

                CHE_Rect pageRect = [pdfDocument getPageRect:i];
                CHE_PDF_Renderer render( context );
                render.SetPosition( pageRectInView.origin.x, pageRectInView.origin.y );
                render.Render( *[pdfDocument getPageContent:i], pageRect, 0, _scale, 72, 72 );
                
                CGContextRestoreGState( context );
                
                NSLog( @"draw page %d", i+1);
            }
        }
    }
}

-(void)newDocument:(PdfDocumentData*)doc
{
    pdfDocument = doc;
    
    [self parseAllPageSizeInfo];
    
    NSRect frame;
    frame.origin.x = 0;
    frame.origin.y = 0;
    frame.size = [self getViewContentSize];
    [self setFrame:frame];
    [self setNeedsDisplay:YES];
}

-(void)closeDocument
{
    pdfDocument = nil;
    NSRect rect;
    [self setFrame:rect];
    [self setNeedsDisplay:YES];
}

-(void)parseAllPageSizeInfo
{
    allPageWidth        = 0;
    allPageHeight       = 0;
    biggestPageWidth    = 0;
    smallestPageWidth   = 0;
    biggestPageHeight   = 0;
    smallestPageHeight  = 0;
    
    HE_ULONG pageCount = [pdfDocument getPageCount];
    for ( HE_UINT32 i = 0; i < pageCount; ++i )
    {
        CHE_Rect rect = [pdfDocument getPageRect:i];
        allPageHeight += rect.height;
        allPageWidth += rect.width;
        
        if ( i == 0 )
        {
            biggestPageHeight   = rect.height;
            smallestPageHeight  = rect.height;
            biggestPageWidth    = rect.width;
            smallestPageHeight  = rect.height;
        }else{
            if ( rect.width > biggestPageWidth )
            {
                biggestPageWidth = rect.width;
            }
            if ( rect.width < smallestPageWidth )
            {
                smallestPageWidth = rect.width;
            }
            if ( rect.height > biggestPageHeight )
            {
                biggestPageHeight = rect.height;
            }
            if ( rect.height > smallestPageHeight )
            {
                biggestPageHeight = rect.height;
            }
        }
    }
}

-(CGSize)getViewContentSize
{
    NSRect scrollFrame = [parentScrollView frame];
    CGSize sizeRet = scrollFrame.size;
    
    //if ( _zoomMode == ZOOM_MODE_FIT_HEIGHT )
    //{
    //
    //}else if ( _zoomMode == ZOOM_MODE_FIT_WIDTH )
    //
    //    }
    
    CGSize pages;

    switch ( _pageMode )
    {
        case PAGE_MODE_SINGLE:
            break;
        case PAGE_MODE_DOUBLE:
            break;
        case PAGE_MODE_SINGLE_SCROLL:
        {
            if ( _rotateMode == ROTATE_MODE_0 || _rotateMode == ROTATE_MODE_180 )
            {
                pages.width = biggestPageWidth * _scale + _pageSpaceX * 2;
                pages.height = allPageHeight * _scale + ( [pdfDocument getPageCount] + 1 ) * _pageSpaceY;
            }else{
                pages.width = biggestPageHeight * _scale + _pageSpaceX * 2;
                pages.height = allPageWidth * _scale + ( [pdfDocument getPageCount] + 1 ) * _pageSpaceY;
            }
            break;
        }
        case PAGE_MODE_DOUBLE_SCROLL:
        {
            break;
        }
        default:
            break;
    }

    if ( scrollFrame.size.width-15 >= pages.width )
    {
        sizeRet.width = scrollFrame.size.width-15;
    }else{
        sizeRet.width = pages.width;
    }
    if ( scrollFrame.size.height >= pages.height )
    {
        sizeRet.height = scrollFrame.size.height;
    }else{
        sizeRet.height = pages.height;
    }
    
    return sizeRet;
}


//获得当前页面排版属性下面的所有页面的大小
-(CGRect)getAllPagesRectInView
{
    //pageRect = [pdfDocument getPageRect:i];
    //pageRectInView = pageRect;
    //pageRectInView.width *= _scale;
    //pageRectInView.height *= _scale;
    //pageRectInView.bottom = 0;
    //pageRectInView.left = 0;
    //pageRectInView.bottom += totalHeight;
    //pageRectInView.bottom += _pageSpaceY * (i+1);
    //totalHeight += pageRectInView.height;
    
    //if ( viewFrame.size.width-15 >= pageRectInView.width )
    //{
    //    pageRectInView.left = 15 + ( viewFrame.size.width-15 - pageRectInView.width - _pageSpaceX * 2 ) / 2;
    //}
    //if ( viewFrame.size.height >= allPageSize.height * _scale )
    //{
    //    pageRect.bottom += (viewFrame.size.height - allPageSize.height * _scale)/2;
    //}
    
    CGRect rectRet = CGRectMake( 0, 0, 0, 0 );
    NSRect scrollFrame = [parentScrollView bounds];
    
    if ( _zoomMode == ZOOM_MODE_FIT_HEIGHT )
    {
    
    }else if ( _zoomMode == ZOOM_MODE_FIT_WIDTH )
    {
        
    }
    
    switch ( _pageMode )
    {
        case PAGE_MODE_SINGLE:
            break;
        case PAGE_MODE_DOUBLE:
            break;
        case PAGE_MODE_SINGLE_SCROLL:
        {
            if ( _rotateMode == ROTATE_MODE_0 || _rotateMode == ROTATE_MODE_180 )
            {
                rectRet.size.width = biggestPageWidth * _scale + _pageSpaceX * 2;
                rectRet.size.height = allPageHeight * _scale + ( [pdfDocument getPageCount] + 2 ) * _pageSpaceY;
            }else{
                rectRet.size.width = biggestPageHeight * _scale + _pageSpaceX * 2;
                rectRet.size.height = allPageWidth * _scale + ( [pdfDocument getPageCount] + 2 ) * _pageSpaceY;
            }
            break;
        }
        case PAGE_MODE_DOUBLE_SCROLL:
        {
            break;
        }
        default:
            break;
    }

    /*if ( scrollFrame.size.width-15 >= pages.width )
    {
        sizeRet.width = scrollFrame.size.width-15;
    }else{
        sizeRet.width = pages.width;
    }
    if ( scrollFrame.size.height >= pages.height )
    {
        sizeRet.height = scrollFrame.size.height;
    }else{
        sizeRet.height = pages.height;
    }*/
    
    return rectRet;
}

@end
