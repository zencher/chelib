//
//  chelibTests.m
//  chelibTests
//
//  Created by zencher on 15/1/15.
//  Copyright (c) 2015年 com.peroit. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#include "pdf_colorspace.h"

using namespace chelib;

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
    CPDF_ColorSpacePtr cs = CPDF_ColorSpace::CreateDeviceGray();
    
    XCTAssertTrue( cs, @"CPDF_ColorSpace::CreateDeviceCMYK return null" );
    
    PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
    
    XCTAssertTrue( type == COLORSPACE_DEVICE_GRAY, @"CPDF_ColorSpace::GetColorSpaceType return not right" );
    
    uint32 c = cs->GetComponentCount();
    
    XCTAssertTrue( c == 1, @"CPDF_ColorSpace::GetComponentCount return not right" );
}

- (void)testCreateDeviceRGB
{
    CPDF_ColorSpacePtr cs = CPDF_ColorSpace::CreateDeviceRGB();
   
    XCTAssertTrue( cs, @"CPDF_ColorSpace::CreateDeviceRGB return null" );
    
    PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
    
    XCTAssertTrue( type == COLORSPACE_DEVICE_RGB, @"CPDF_ColorSpace::GetColorSpaceType return not right" );
    
    uint32 c = cs->GetComponentCount();
    
    XCTAssertTrue( c == 3, @"CPDF_ColorSpace::GetComponentCount return not right" );
}

- (void)testCreateDeviceCMYK
{
    CPDF_ColorSpacePtr cs = CPDF_ColorSpace::CreateDeviceCMYK();
    
    XCTAssertTrue( cs, @"CPDF_ColorSpace::CreateDeviceCMYK return null" );
    
    PDF_COLORSPACE_TYPE type = cs->GetColorSpaceType();
    
    XCTAssertTrue( type == COLORSPACE_DEVICE_CMYK, @"CPDF_ColorSpace::GetColorSpaceType return not right" );
    
    uint32 c = cs->GetComponentCount();
    
    XCTAssertTrue( c == 4, @"CPDF_ColorSpace::GetComponentCount return not right" );
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
