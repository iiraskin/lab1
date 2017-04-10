/*Tab = 4 space*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

/*Check if the path exist*/
char CheckPass( char* path )
{
	int i = 0;

	for(; path[i] != 0; i++ ){	
		/*Check each directory in the path*/
		if( path[i] == '/' ){
			path[i] = 0;
			DIR* dir = opendir( path );
			if ( !dir && strcmp( path, "\0" ) )
			{
				/*If the path doesn't exist - it's an error*/
				printf( "wc: %s: No such file or directory\n", path );
				closedir( dir );
				return 1;
			}
			closedir( dir );
			path[i] = '/';
		}
	}
	struct stat buf;
	if( stat( path, &buf ) ){
		/*If the path doesn't exist - it's an error*/
		printf( "wc: %s: No such file or directory\n", path );
		return 1;
	}
	return 0;
}

void Information( char* path, char* options, char* total, char whitespace )
{	
	if( !CheckPass( path ) ){
		int	l = 0, w = 0, c = 0, word = 1;	
		char byte;
		int fd = open( path, O_RDONLY );
		/*Read from file and find nesessery characteristics*/		
		while( read( fd, &byte, 1 ) ){		
			c++;
			if( isspace( byte ) ){
				word = 1;
			}else if( word ){
				w++;
				word = 0;
			}
			if( byte == '\n' ){
				l++;
			}
		}
		close( fd );
	/*Output depends on the options*/
	if( options[0] ){
		printf( "%c%d", whitespace, l );
		total[0] += l;
	}
	if( options[1] ){
		printf( "%c%d", whitespace, w );
		total[1] += w;
	}
	if( options[2] ){
		printf( "%c%d", whitespace, c );
		total[2] += c;
	}
		printf( " %s\n", path );
	}
}

void IformationFromInput( char* options, char path, char* total )
{
	int	l = 0, w = 0, c = 0, word = 1;	
	int byte;
	/*Read from console and find nesessery characteristics*/
	while( ( byte = fgetc( stdin ) ) != EOF){		
		c++;
		if( isspace( byte ) ){
			word = 1;
		}else if( word ){
			w++;
			word = 0;
		}
		if( byte == '\n' ){
			l++;
		}
	}
	/*Output depends on the options*/
	if( options[0] ){
		printf( "	%d", l );
		total[0] += l;
	}
	if( options[1] ){
		printf( "	%d", w );
		total[1] += w;
	}
	if( options[2] ){
		printf( "	%d", c );
		total[2] += c;
	}
	if( path ){
		printf( " -\n" );
	}else{
		printf( " \n" );
	}
}

int main( int argc, char* argv[] )
{
	char date;
	char options[3] = {0,0,0};/*Options which should be used: 0-l, 1-w, 2-c*/
	int counter = 0;
	char total[3] = {0,0,0};/*To find total characteristics*/
	char whitespace = ' ';/*This useless thing is made for imitation of the original ps*/

	/*Determine which options should be used*/
	while( ( date = getopt( argc, argv, "-lwc" ) ) != -1 ){
		if( date == 'l' ){
			options[0] = 1;
			counter++;
		}else if( date == 'w' ){
			options[1] = 1;
			counter++;
		}else if( date == 'c' ){
			options[2] = 1;
			counter++;
		}else if( date == '-' ){
			whitespace = '\t';
		}
	}
	/*If there are no options to use, we use all options*/	
	if( !counter ){
		options[0] = options[1] = options[2] = 1;
	}
		
	int i = 1;
	counter = 0;
	/*Find necessary information for each argument*/
	for(; i < argc; ++i ){
		if( strcmp( argv[i], "-l" ) && strcmp( argv[i], "-w" ) && strcmp( argv[i], "-c" ) ){
			if( !strcmp( argv[i], "-" ) ){
				IformationFromInput( options, 1, total );
			}else{		   
				Information( argv[i], options, total, whitespace );
			}			
			counter++;
		}
	}
	/*If there are no arguments use input from console*/
	if( !counter ){
		IformationFromInput( options, 0, total );
		return 0;
	}
	/*If there are more than one arguments we should output total result*/
	if( counter > 1 ){
		printf( "%c%d%c%d%c%d total\n", whitespace, total[0], whitespace, total[1], whitespace, total[2] );
	}
	return 0;
}
