//
//  WindowController.m
//  chelib
//
//  Created by zencher on 14/12/27.
//  Copyright (c) 2014年 com.peroit. All rights reserved.
//

#import "MyWinController.h"
#import "Document.h"
#import "MyWindow.h"

@implementation MyWinController

- (id)init {
    self = [super initWithWindowNibName:@"Window"];
    if (self)
    {
    }
    return self;
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
    MyWindow * mywin = (MyWindow*)[self window];
    [mywin load:[[self document] getPdfDocumentData]];
}

@end
