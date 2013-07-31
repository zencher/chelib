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
        data = [[PdfDocumentData alloc] initWithFilePath:@"/Users/zencher/Desktop/test/test2.pdf"];
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{    
    CGContextRef contextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextMoveToPoint( contextRef , 0, 0 );
    CGContextAddLineToPoint( contextRef, 100, 100 );
    CGContextStrokePath( contextRef );
    
    /*CHE_Bitmap bitmap;
    bitmap.Create( 100 , 100, BITMAP_DEPTH_32BPP, BITMAP_DIRECTION_DOWN );
    bitmap.Fill( 0xFF225566 );
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGDataProviderRef dataRef = CGDataProviderCreateWithData( NULL, bitmap.GetBuffer(), 100 * 100 * 4, NULL );
    
    CGImageRef imageRef = CGImageCreate( 100, 100, 8, 32, 100 * 4 , NULL, kCGImageAlphaFirst | kCGBitmapByteOrderDefault, dataRef, NULL, false, kCGRenderingIntentDefault );
    
    CHE_Matrix matrix( 100, 0, 0, 100, 0, 0 );
    CHE_Rect rect;
    rect.left = 0;
    rect.bottom = 0;
    rect.width = 1;
    rect.height = 1;
    rect = matrix.Transform( rect );

    CGContextDrawImage( contextRef, CGRectMake( rect.left, rect.bottom, rect.width, rect.height ), imageRef );*/

    
    //CGImageRef imageRef = CGBitmapContextCreateImage( drawer.GetContextRef() );
    //CGRect rect = CGRectMake( 0 , 0, drawer.GetWidth(), drawer.GetHeight() );
    //CGContextDrawImage( contextRef , rect, imageRef );
}

@end
