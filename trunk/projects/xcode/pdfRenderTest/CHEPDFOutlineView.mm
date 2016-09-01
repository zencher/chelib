//
//  CHEPDFOutlineView.m
//  chelib
//
//  Created by zencher on 16/2/5.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import "CHEPDFOutlineView.h"



@implementation CHEPDFOutlineScrollView

-(id)initWithFrameAndParentView:(NSRect)frame
                     parentView:(id)view;
{
    self = [super initWithFrame:frame];
    if (self)
    {
        outlineView = [[NSOutlineView alloc] initWithFrame:frame];
        [outlineView setSelectionHighlightStyle:NSTableViewSelectionHighlightStyleSourceList];
        [outlineView setColumnAutoresizingStyle:NSTableViewUniformColumnAutoresizingStyle];
        [outlineView setHeaderView:nil];
        
        NSTableColumn *c = [[NSTableColumn alloc] initWithIdentifier: @"Column"];
        [c setEditable: NO];
        //[c setMinWidth:frameRect.size.width];
        //[c setMaxWidth:frameRect.size.width];
        [c setResizingMask:NSTableColumnAutoresizingMask];
        [outlineView addTableColumn: c];
        [outlineView setOutlineTableColumn:c];
        [outlineView sizeLastColumnToFit];
        
        [self setDocumentView:outlineView];
        [self setAutohidesScrollers:YES];
        [self setHasVerticalScroller:YES];
        [self setHasHorizontalScroller:NO];
        
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
}

-(void) setDataSource:(id)source
{
    if (outlineView) {
        [outlineView setDataSource:source];
        [outlineView sizeLastColumnToFit];
    }
}

-(void)parentScrollViewFrameChanged
{
    [self setFrame:[parentScrollView frame]];
}


@end



@implementation CHEPDFOutlineView

-(id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self) {
        scrollView = [[CHEPDFOutlineScrollView alloc] initWithFrameAndParentView:frameRect parentView:self];
        [scrollView setBackgroundColor:[NSColor clearColor]];
        [self addSubview:scrollView];
    }
    return self;
}

-(void) setDataSource:(id)source
{
    if (scrollView) {
        [scrollView setDataSource:source];
    }
}

@end