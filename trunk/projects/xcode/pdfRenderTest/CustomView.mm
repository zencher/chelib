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
        pdfDocument = [[PdfDocumentData alloc] initWithFilePath:@"/Users/zencher/PDFResearch/Files/澳洲史.pdf"];
        pageIndex = 0;
        mouseDownPt.x = 0;
        mouseDownPt.y = 0;
        pageOffset.width = 0;
        pageOffset.height = 0;
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    NSRect bounds = [self bounds];
    
    [[NSColor whiteColor] set];
    NSRectFill( [self bounds] );
    
    NSLog(@"draw...");
    
    /*CGRect pageRect = CGRectMake( 0 + pageOffset.width, bounds.size.height - [pdfDocument getDrawer]->GetHeight() + pageOffset.height, [pdfDocument getDrawer]->GetWidth(), [pdfDocument getDrawer]->GetHeight() );
    if ( bounds.size.width > [pdfDocument getDrawer]->GetWidth() )
    {
        pageRect.origin.x = bounds.size.width - [pdfDocument getDrawer]->GetWidth();
        pageRect.origin.x /= 2;
    }
    if ( bounds.size.height > [pdfDocument getDrawer]->GetHeight() )
    {
        pageRect.origin.y = bounds.size.height - [pdfDocument getDrawer]->GetHeight();
        pageRect.origin.y /= 2;
    }*/
    
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    if ( context )
    {
        CHE_PDF_Renderer render( context );
        render.Render( *[pdfDocument getPageContent:pageIndex] );
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

- (IBAction)onPreviousPage:(id)sender
{
    if ( pageIndex > 0 ) {
        pageIndex--;
    }
    [self setNeedsDisplay:YES];
}

- (IBAction)onNextPage:(id)sender
{
    pageIndex++;
    [self setNeedsDisplay:YES];
}

- (IBAction)onFileOpen:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    //[panel setDirectory:NSHomeDirectory()]; // Set panel's default directory.
    [panel setAllowedFileTypes:[NSImage imageFileTypes]]; // Set what kind of file to select.
    // More panel configure code.
    [panel beginSheetModalForWindow:[self window] completionHandler:
    (^(NSInteger result)
    {
        if(result == NSOKButton)
        {
            NSArray *fileURLs = [panel URLs];
        }
    })];
}

@end
