//
//  chelibTests.m
//  chelibTests
//
//  Created by zencher on 15/1/15.
//  Copyright (c) 2015年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#include "che_pdf_colorspace.h"

@interface colorspace : XCTestCase

@end

@implementation colorspace

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testCreateDeviceGray
{
    CHE_PDF_ColorSpacePtr cs = CHE_PDF_ColorSpace::CreateDeviceGray();
    
    XCTAssertTrue( cs, @"CHE_PDF_ColorSpace::CreateDeviceCMYK return null" );
    
    PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
    
    XCTAssertTrue( type == COLORSPACE_DEVICE_GRAY, @"CHE_PDF_ColorSpace::GetColorSpaceType return not right" );
    
    HE_UINT32 c = cs->GetComponentCount();
    
    XCTAssertTrue( c == 1, @"CHE_PDF_ColorSpace::GetComponentCount return not right" );
}

- (void)testCreateDeviceRGB
{
    CHE_PDF_ColorSpacePtr cs = CHE_PDF_ColorSpace::CreateDeviceRGB();
   
    XCTAssertTrue( cs, @"CHE_PDF_ColorSpace::CreateDeviceRGB return null" );
    
    PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
    
    XCTAssertTrue( type == COLORSPACE_DEVICE_RGB, @"CHE_PDF_ColorSpace::GetColorSpaceType return not right" );
    
    HE_UINT32 c = cs->GetComponentCount();
    
    XCTAssertTrue( c == 3, @"CHE_PDF_ColorSpace::GetComponentCount return not right" );
}

- (void)testCreateDeviceCMYK
{
    CHE_PDF_ColorSpacePtr cs = CHE_PDF_ColorSpace::CreateDeviceCMYK();
    
    XCTAssertTrue( cs, @"CHE_PDF_ColorSpace::CreateDeviceCMYK return null" );
    
    PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
    
    XCTAssertTrue( type == COLORSPACE_DEVICE_CMYK, @"CHE_PDF_ColorSpace::GetColorSpaceType return not right" );
    
    HE_UINT32 c = cs->GetComponentCount();
    
    XCTAssertTrue( c == 4, @"CHE_PDF_ColorSpace::GetComponentCount return not right" );
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
