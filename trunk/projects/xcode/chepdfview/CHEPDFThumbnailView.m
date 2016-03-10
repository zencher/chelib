//
//  CHEPDFThumbnailView.m
//  chelib
//
//  Created by zencher on 16/3/6.
//  Copyright © 2016年 com.peroit. All rights reserved.
//

#import "CHEPDFThumbnailView.h"

@implementation CHEPDFThumbnailMainView

- (id) initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self) {
        
    }
    return self;
}

@end


@implementation CHEPDFThumbnailScrollView

- (id) initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self) {
        mainView = [[CHEPDFThumbnailMainView alloc] initWithFrame:frameRect];
        [self addSubview:mainView];
    }
    return self;
}

@end

@implementation CHEPDFThumbnailView

- (id) initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self)
    {
        scrollView = [[CHEPDFThumbnailScrollView alloc] initWithFrame:frameRect];
        [self addSubview:scrollView];
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // Drawing code here.
}

@end
