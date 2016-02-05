//
//  CHEPDFView.h
//  chelib
//
//  Created by Frank Zeng on 14-4-3.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CHEPDFMainView.h"

@interface CHEPDFView : NSScrollView
{
    CHEPDFMainView * displayView;
}

-(BOOL)loadFile:(NSString*)filePath;

-(void)closeFile;

-(void)nextPage;

-(void)prevPage;

-(void)rotate;

-(id)mainView;

@end
