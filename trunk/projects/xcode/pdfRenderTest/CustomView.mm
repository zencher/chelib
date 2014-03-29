//
//  CustomView.m
//  chelib
//
//  Created by Frank Zeng on 13-5-31.
//  Copyright (c) 2013年 com.peroit. All rights reserved.
//

#import "CustomView.h"

@implementation CustomView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
    }
    return self;
}

-(BOOL)isFlipped
{
    return YES;
}

- (void)drawRect:(NSRect)dirtyRect
{
    //NSRect bounds = [self bounds];
    //[[NSColor whiteColor] set];
    //NSRectFill( [self bounds] );
    
    NSLog(@"draw...");
    
    if ( pdfDocument )
    {
        CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
        if ( context )
        {
            CHE_PDF_Renderer render( context );
            CHE_Rect rect = [pdfDocument getPageRect:pageIndex];
            render.Render( *[pdfDocument getPageContent:pageIndex], rect, 0, 1, 96, 96 );
        }
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

-(void)newDocument:(PdfDocumentData*)doc
{
    pdfDocument = doc;
    pageIndex = 0;
    CHE_Rect rect = [pdfDocument getPageRect:pageIndex];
    NSRect frame;
    frame.size.width = 96.0 * rect.width / 72;
    frame.size.height = 96.0 * rect.height / 72;
    frame.origin.x = 96.0 * rect.left / 72;
    frame.origin.y = 96.0 * rect.bottom / 72;
    [self setFrame:frame];
    [self setNeedsDisplay:YES];
}

-(void)closeDocument
{
    pdfDocument = nil;
    pageIndex = 0;
    NSRect rect;
    [self setFrame:rect];
    [self setNeedsDisplay:YES];
}

- (void)onPrePage
{
    if ( pageIndex > 0 )
    {
        pageIndex--;
    }
    CHE_Rect rect = [pdfDocument getPageRect:pageIndex];
    NSRect frame;
    frame.size.width = 96.0 * rect.width / 72;
    frame.size.height = 96.0 * rect.height / 72;
    frame.origin.x = 96.0 * rect.left / 72;
    frame.origin.y = 96.0 * rect.bottom / 72;
    [self setFrame:frame];
    [self setNeedsDisplay:YES];
}

- (void)onNextPage
{
    pageIndex++;
    CHE_Rect rect = [pdfDocument getPageRect:pageIndex];
    NSRect frame;
    frame.size.width = 96.0 * rect.width / 72;
    frame.size.height = 96.0 * rect.height / 72;
    frame.origin.x = 96.0 * rect.left / 72;
    frame.origin.y = 96.0 * rect.bottom / 72;
    [self setFrame:frame];
    [self setNeedsDisplay:YES];
}

@end
