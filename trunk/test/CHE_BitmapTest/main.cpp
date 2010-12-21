#include "../../include/che_bitmap.h"
#include <stdio.h>

#include "windows.h"

__inline LONGLONG GetSecondCount()
{
	static LARGE_INTEGER liCounter = {0};
	if (0 == liCounter.QuadPart)
		QueryPerformanceFrequency(&liCounter);
	
	return liCounter.QuadPart;
}

__inline DWORD highGetTime()
{
	LARGE_INTEGER liCurrent = {0};
	QueryPerformanceCounter(&liCurrent);
	return (DWORD)(liCurrent.QuadPart * 1000 / GetSecondCount());
}

int main()
{
	CHE_Bitmap aBitmap;
	//aBitmap.Create( 200, 200, BITMAP_DEPTH_24BPP, BITMAP_DIRECTION_UP );
	//aBitmap.Fill( 0xFFFFFFFF );

 	//CHE_Bitmap maskBitmap;
 	//maskBitmap.Load( "c:\\15.bmp" );
 	//aBitmap.CompositeMask( 0x00008800, 10, 10, maskBitmap );

	//aBitmap.Save( "OriABitmap.bmp" );

	//LONGLONG iCurrentBegin = highGetTime();
	//LONGLONG iCurrentEnd = 0;

	aBitmap.Load( "c:\\24.bmp" );
	char tempStr[128];

	for ( int i = 1; i <= 20; i++ )
	{
		CHE_Bitmap * pTemp = aBitmap.StretchTo( aBitmap.Width()*0.2*i, aBitmap.Height()*0.2*i, 1, NULL );
		sprintf( tempStr, "c:\\bitmap1\\%d.bmp", i );
		pTemp->Save( tempStr/*"c:\\aadsf.bmp"*/ );
		delete pTemp;
		pTemp = NULL;
	}

	//iCurrentEnd = highGetTime();

	//char tempstr[1024];
	//sprintf( tempstr, "time passed : %d ms", iCurrentEnd - iCurrentBegin );
	//MessageBox( NULL, tempstr, "", 0 );


//	char tempFileName[1024];
//	int iPixelCount = aBitmap.Width() * aBitmap.Height();
//	for ( int i = 0; i < aBitmap.Height() + 10; i++ )	//增加10，检查超过位图范围的情况
//	{
//		for ( int j = 0; j < aBitmap.Width() + 10; j++ )	//增加10，检查超过位图范围的情况
//		{
//			aBitmap.SetPixel( j, i, 0x00FF0000 );	//Set different color
//			sprintf( tempFileName, "SetPixBitmap(%04d-%04d).bmp", i, j );
//			aBitmap.Save( tempFileName );
//		}
//	}
// 	HE_RECT rt;
// 	rt.top = 0;
// 	rt.left = 0;
// 	rt.height = aBitmap.Height();
// 	rt.width = aBitmap.Width();
// 	aBitmap.Fill( 0x00FF0000, &rt );
// 	aBitmap.Save( "FillRectAll.bmp" );

// 	for ( int i = 0; i < aBitmap.Width()+10; i++ )	//增加10，检查超过位图范围的情况
// 	{
// 		rt.width = i;
// 		aBitmap.Fill( 0x000000FF, &rt );	//Set different color
// 		sprintf( tempFileName, "FillRect(width - %04d).bmp", i );
// 		aBitmap.Save( tempFileName );
// 	}

// 	for ( int j = 0; j < aBitmap.Width()+10; j++ )	//增加10，检查超过位图范围的情况
// 	{
// 		rt.width = 10;
// 		rt.left = j;
// 		aBitmap.Fill( 0x000000FF, &rt );	//Set different color
// 		sprintf( tempFileName, "FillRect(left-%04d, width-10).bmp", j );
// 		aBitmap.Save( tempFileName );
// 	}

// 	CHE_Bitmap bBitmap;
// 	bBitmap.Load( "c:\\24.bmp" );
// 	bBitmap.SetChannel( BITMAP_CHANNEL_Blue, 0 );
// 	bBitmap.Save( "c:\\24_NoBlue.bmp" );
// 
// 	bBitmap.Load( "c:\\24.bmp" );
// 	bBitmap.SetChannel( BITMAP_CHANNEL_Green, 0 );
// 	bBitmap.Save( "c:\\24_NoGreen.bmp" );
// 
// 	bBitmap.Load( "c:\\24.bmp" );
// 	bBitmap.SetChannel( BITMAP_CHANNEL_Red, 0 );
// 	bBitmap.Save( "c:\\24_NoRed.bmp" );
// 
// 	bBitmap.Load( "c:\\24.bmp" );
// 	bBitmap.SetChannel( BITMAP_CHANNEL_Blue, 0 );
// 	bBitmap.SetChannel( BITMAP_CHANNEL_Green, 0 );
// 	bBitmap.Save( "c:\\24_OnlyRed.bmp" );
// 
// 	bBitmap.Load( "c:\\24.bmp" );
// 	bBitmap.SetChannel( BITMAP_CHANNEL_Blue, 0 );
// 	bBitmap.SetChannel( BITMAP_CHANNEL_Red, 0 );
// 	bBitmap.Save( "c:\\24_OnlyGreen.bmp" );
// 	
// 	bBitmap.Load( "c:\\24.bmp" );
// 	bBitmap.SetChannel( BITMAP_CHANNEL_Green, 0 );
// 	bBitmap.SetChannel( BITMAP_CHANNEL_Red, 0 );
// 	bBitmap.Save( "c:\\24_OnlyBlue.bmp" );

	return 0;
}