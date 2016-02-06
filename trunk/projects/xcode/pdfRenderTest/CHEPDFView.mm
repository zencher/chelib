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
        displayView = [[CHEPDFMainView alloc] initWithFrameAndParentView:frame parentView:self];
        [self setDocumentView:displayView];
        [self setAutohidesScrollers:YES];
        [self setHasVerticalScroller:YES];
        [self setHasHorizontalScroller:YES];
        [self setBorderType:NSNoBorder];
        [self setBackgroundColor:[NSColor lightGrayColor]];
    }
    return self;
}


-(BOOL) loadFile:(NSString*)filePath
{
    [self closeFile];

    PdfDocumentData * docData = [[PdfDocumentData alloc] initWithFilePath:filePath];
    [displayView newDocument:docData];
    return TRUE;
}

-(void) closeFile
{
    [displayView closeDocument];
}

-(void)nextPage
{
    [displayView nextPage];
}

-(void)prevPage
{
    [displayView prevPage];
}

-(void)rotate
{
    [displayView rotate];
}

-(id)mainView
{
    return displayView;
}

@end
