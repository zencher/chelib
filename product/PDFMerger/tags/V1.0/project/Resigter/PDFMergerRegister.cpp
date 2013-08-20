#include <cstdio>
#include <memory>

int main( int argc, char* argv[] )
{
	if ( argc == 1 )
	{
		printf( "ERROR" );

		return 0;
	}

	unsigned char name[1024];

	memset( name, 0, 1024 );

	for ( size_t i = 0; i < strlen( argv[1] ) && i < 1024; ++i )
	{
		name[i] = (unsigned char)( argv[1][i] );
	}

	for ( size_t i = 0; i < 1024; ++i )
	{
		name[i] = (unsigned char)( name[i] + ~i );
	}

	for ( size_t i = 0; i < 1023; ++i )
	{
		name[i] = (unsigned char)( name[i] * name[i] );
	}
	
	printf( "%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X", name[0], name[1], name[2], name[3], name[4], name[5], name[6], name[7], name[8], name[9], name[10], name[11] );

	return 0;
}

