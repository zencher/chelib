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
        // Initialization code here.
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{    
//    CGContextRef contextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
//    CGImageRef imageRef = CGBitmapContextCreateImage( drawer.GetContextRef() );
//    CGRect rect = CGRectMake( 0 , 0, drawer.GetWidth(), drawer.GetHeight() );
//    CGContextDrawImage( contextRef , rect, imageRef );
}

@end
