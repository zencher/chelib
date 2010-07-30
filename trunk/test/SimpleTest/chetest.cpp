#include "../../include/che_base.h"
#include "../../include/che_string.h"
#include <stdio.h>
#include <math.h>

int main()
{
	CHE_ByteString str1 = "asdf";
	CHE_ByteString str2 = 'a';
	CHE_ByteString str3(str1);

	//str1 = CHE_ByteString("1asdfasd") + "2sdfasdfasdfasdfasd" + '3' + "4gasdfda" ;
	//str1+="3542342";
	//str1+=str1;

// 	if ( operator==( "asdf", str1 ) )
// 	{
// 		int x = 3;
// 	}
	
	CHE_ByteString temp = str1 + 'a';

// 	if ( "asdf" == str1 )
// 	{
// 		int x =0;
// 	}

	return 0;
}