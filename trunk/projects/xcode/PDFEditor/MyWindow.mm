//
//  MyWindow.m
//  chelib
//
//  Created by zencher on 16/3/6.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import "MyWindow.h"

@implementation MyWindow

- (instancetype)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag
{
    self = [super initWithContentRect:contentRect styleMask:aStyle backing:bufferingType defer:false];
    if (self) {
        pdfView = [[CHEPDFEditView alloc] initWithFrame:contentRect];
        [self setContentView:pdfView];
        [self setAllowsConcurrentViewDrawing:YES];
    }
    return self;
}

- (instancetype)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag screen:(nullable NSScreen *)screen
{
    self = [super initWithContentRect:contentRect styleMask:aStyle backing:bufferingType defer:flag screen:screen];
    if (self) {
        pdfView = [[CHEPDFEditView alloc] init];
        [self setContentView:pdfView];
        [self setAllowsConcurrentViewDrawing:YES];
    }
    return self;
}

- (void) load:(PdfDocumentData*)data
{
    [self setContentView:pdfView];
    [pdfView load:data];
}

- (IBAction)onRotate:(id)sender
{
    [pdfView rotate];
}

- (IBAction)showThumbnail:(id)sender {
    [pdfView showThumbnailView];
    //[pdfView showLeftBar:YES];
    //[pdfView showRightBar:YES];
}

- (IBAction)showBookmark:(id)sender {
    [pdfView showBookmarkView];
    //[pdfView showLeftBar:NO];
    //[pdfView showRightBar:NO];
}

@end
