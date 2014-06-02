//
//  CHEPDFView.m
//  chelib
//
//  Created by Frank Zeng on 14-4-3.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import "CHEPDFView.h"

@implementation CHEPDFView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        displayView = [[CHEPDFMainView alloc] initWithFrame:frame];
        [displayView setParentScrollView:self];
        [self setAutohidesScrollers:YES];
        [self setHasVerticalScroller:YES];
        [self setHasHorizontalScroller:YES];
        [self setBorderType:NSNoBorder];
        [self setDocumentView:displayView];
        [self setPostsFrameChangedNotifications:YES];
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
}

-(BOOL) loadFile:(NSString*)filePath
{
    [displayView closeDocument];

    PdfDocumentData * docData = [[PdfDocumentData alloc] initWithFilePath:filePath];
    [displayView newDocument:docData];
    return TRUE;
}

-(void) closeFile
{
    [displayView closeDocument];
}

@end
