/*
//  bmpio.c
// 
//  BITMAP の処理
//  
//  Created by 武村　健二 on Wed 11th Dec 2002.
//  Copyright (c) 2002 J.T.S.TAKEMURA CO.,LTD. All rights reserved.
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "bmpio.h"
#include "../common/tmp_rgbdata.h"

int CPU;


void chk_cpu()
{
	int i = 1;
	
	CPU = 0;
	
	CPU = *((char *)&i);
	/*
	int inf, i = 1;
	
	if((inf = creat("tmp.tmp",0x1b6)) >= 0)
	{
		write(inf,&i,sizeof(int)) ;
		close(inf) ;
		if((inf = open("tmp.tmp",O_RDONLY)) >= 0)
		{ 	
			read(inf,&CPU,1) ;
		}
		unlink("tmp.tmp") ;
	}
	*/
}

long intel_ltol(long src)
{
	long	dst ;

	if(CPU) { return(src) ; }
	dst = ((src<<24)&0xff000000)+((src<<8)&0xff0000)+((src>>8)&0xff00)+((src>>24)&0xff) ;
	return(dst) ;
 }

short	intel_stos(short src)
{
	short	dst ;

	if(CPU) { return(src) ; }
	dst = ((src<<8)&0xff00)+((src>>8)&0xff) ;
	return(dst) ;
 }

int main( int argc, char *argv[] )
{
	int mode, i, j, cbit;
	char path[512], str[512], ext[8], *p;
	unsigned char *b24;
	TMP_RGBDATA	tmp;
	FILE	*fp;
	BITMAPFILEHEADER	bfh;
	BITMAPINFOHEADER	bih;
	BITMAP_RGBQUAD		rgb[256];
	unsigned long		offset, data, *p24, p24Size, *p2;
	
	mode = 0;
	memset( ext, 0, sizeof(ext) );
	for ( i = 1; i < argc; i++ )
	{
		if( argv[i][0] == '-' )
		{
			switch( argv[i][1] )
			{
			  case 'e':
				strncpy( ext, &argv[i][2], 8 ); break;
				
			  case 'p':
				strcpy( path, &argv[i][2] ); break;
			  case 'w':
				mode = 2; break;
			  case 'r':
				mode = 3; break;
			}
		}
	}
	if( strlen(ext) < 3 ) strcat( ext, ".bmp" );
	
	chk_cpu();
	
	if( mode == 2 )
	{
		for( i  = 0 ; i < 5; i++ )
		{
			p = fgets( str, sizeof(str), stdin ) ;
			switch( *p )
			{
			  case 'x':
				tmp.x = atoi( p + 2 ); break;
			  case 'y':
				tmp.y = atoi( p + 2 ); break;
			  case 'w':
				tmp.w = atoi( p + 2 ); break;
			  case 'h':
				tmp.h = atoi( p + 2 ); break;
			  case 'p':
				tmp.pal = atoi( p + 2 ); break;
			}
		}
		if( !strlen( path ) ) return 1;
		
		if( 0 < tmp.pal && tmp.pal <= 256 )
			cbit = 8;
		else if( tmp.pal < 0 )
			cbit = 24;
		else
			return 1;
		
		path[ strlen(path) -4] = '\0';
		strcat( path, ext );
		fp = fopen( path, "wb" );
		if( fp == NULL ) return 1;
		
		memset( &bfh, 0, sizeof(bfh) );
		memset( &bih, 0, sizeof(bih) );
		memset( &rgb, 0, sizeof(rgb) );
		
		/* インデックス作成
		// FileHeader */
		bfh.bfType = intel_stos( 0x4d42 );
		i = sizeof(bfh) + sizeof(bih);
		if( cbit == 8 ) i += sizeof(rgb);
		fprintf( stderr, "i:0x%4x\n",i);
		i = intel_ltol( i );
		fprintf( stderr, "bfOffBits:0x%04x, >>16:0x%04x, >>00x%04x\n",i,(i >>16),i &0xffff );
		if(!CPU)
		{
			bfh.bfOffBits[0] = (i >> 16) & 0xffff ;
			bfh.bfOffBits[1] = i & 0xffff ;
		}
		else
		{
			bfh.bfOffBits[0] = i & 0xffff ;
			bfh.bfOffBits[1] = (i >> 16) &0xffff ;
		}
		fprintf( stderr, "[0]:0x%04x, [1]:0x%04x\n",bfh.bfOffBits[0],bfh.bfOffBits[1]);

		
		if( 0 < tmp.pal && tmp.pal <= 256 )
		{
			i += ((tmp.w +3) & ~3) * tmp.h;
			cbit = 8;
		}
		else if( tmp.pal < 0 )
		{
			i += (((tmp.w * 3) +3) & ~3) * tmp.h;
			cbit = 24;
		}
		i = intel_ltol( i );
		if(!CPU)
		{
			bfh.bfSize[0] = (i >> 16) ;
			bfh.bfSize[1] = i & 0xffff ;
		}
		else
		{
			bfh.bfSize[0] = i & 0xffff ;
			bfh.bfSize[1] = (i >> 16);
		}

		/* InfoHeader */
		bih.biSize = intel_ltol( sizeof(bih) );
		bih.biWidth = intel_ltol( tmp.w );
		bih.biHeight = intel_ltol( tmp.h );
		bih.biPlanes = intel_stos( 1 );
		bih.biBitCount = intel_stos( cbit );
		i = sizeof(bfh);
		if( fwrite( &bfh, 1, i, fp ) != i )
		{
			perror( "fwrite(bfh):" );
			return 1;
		}
		i = sizeof(bih);
		if( fwrite( &bih, 1, i, fp ) != i )
		{
			perror( "fwrite(bih):" );
			return 1;
		}
		/* パレット作成 */
		if( cbit == 8 )
		{
			tmp.max = tmp.pal * 3;
			for( tmp.cnt = 0; tmp.cnt < tmp.max ; tmp.cnt++ )
			{
				i = fgetc( stdin );
				switch( tmp.cnt % 3 )
				{
					case 0: rgb[ (tmp.cnt / 3) ].rgbRed = (unsigned char)i; break;
					case 1: rgb[ (tmp.cnt / 3) ].rgbGreen = (unsigned char)i; break;
					case 2: rgb[ (tmp.cnt / 3) ].rgbBlue = (unsigned char)i; break;
				}
			}
			i = sizeof(rgb);
			if( fwrite( &rgb, 1, i, fp ) != i )
			{
				perror( "fwrite(rgb):" );
				return 1;
			}
		}
		else
		{
			fprintf( stderr, "Palette is 256 colors over\n" );
			return 1;
		}
		
		/* データ書き出し */
		tmp.cnt = tmp.cntw = 0;
		tmp.max = tmp.w - tmp.x;
		for( ; (i = fgetc( stdin )) != EOF;  )
		{
			if( tmp.x <= tmp.cnt && tmp.cnt < tmp.max )
			{
				fputc( i, fp );
			}
			tmp.cnt++;
			if( tmp.cnt == tmp.w )
			{
				if( tmp.w % 4 )
				{
					tmp.cnt = 4 - (tmp.w % 4);
					for( i = 0; i < tmp.cnt; i++ )
					{
						fputc( i, fp );
					}
				}
				tmp.cnt = 0;
			}
		}
	}
	else if( mode == 3 )
	{   /* 読込み */
		fp = fopen( path, "rb" );
		if( fp == NULL ) return 1;
		i = sizeof(bfh);
		if( fread( &bfh, 1, i, fp ) != i )
		{
			perror( "fread(bfh):" );
			goto BITMAPERR;
		}
		i = sizeof(bih);
		if( fread( &bih, 1, i, fp ) != i )
		{
			perror( "fread(bih):" );
			goto BITMAPERR;
		}
		
		i = intel_stos( bfh.bfType );
		if( i != 0x4d42 )
		{
			perror( "Not Windows Bitmap file!" );
		  BITMAPERR:
			fclose( fp );
			fprintf( stderr, "path:%s\n", path );
			return 1;
		}
		if(!CPU)
		{
			bfh.bfOffBits[0] = intel_stos( bfh.bfOffBits[0] );
			bfh.bfOffBits[1] = intel_stos( bfh.bfOffBits[1] );
			i   = bfh.bfOffBits[0] ;
			i  += bfh.bfOffBits[1] << 16 ;
		}
		else
		{
			i  = bfh.bfOffBits[1] ;
			i += bfh.bfOffBits[0] << 16 ;
		}
		offset = i;
		
		/* InfoHeader */
		i = intel_stos( bih.biPlanes );
		if( i != 1 )
		{
			perror( "Not supported 1 over planes" );
			goto BITMAPERR;
		}
		i = intel_stos( bih.biBitCount );
		if( !((i == 8) || (i == 24)) )
		{
			perror( "Not supported any bits without 8 or 24" );
			goto BITMAPERR;
		}
		/* Bitmapヘッダ部、読込処理終了 */
		
		/*
		// ヘッダ部 */
		memset( &tmp, 0, sizeof(tmp) );
		tmp.w = intel_ltol( bih.biWidth );
		tmp.h = intel_ltol( bih.biHeight );
		if( i == 8 )
		{
			tmp.pal = intel_ltol( bih.biClrUsed );
			if( !tmp.pal ) tmp.pal = 256;
			i = sizeof(BITMAP_RGBQUAD) * tmp.pal;
			if( fread( &rgb, 1, i, fp ) != i )
			{
				perror( "fread(rgb):" );
				goto BITMAPERR;
			}
			fprintf( stdout, "x:0\ny:0\nw:%d\nh:%d\np:%d\n",
					tmp.w,tmp.h,tmp.pal );
			/*
			// パレット部 */
			tmp.max = tmp.pal * 3;
			for( tmp.cnt = 0; tmp.cnt < tmp.max ; tmp.cnt++ )
			{
				switch( tmp.cnt % 3 )
				{
					case 0: i = rgb[ (tmp.cnt / 3) ].rgbRed;  break;
					case 1: i = rgb[ (tmp.cnt / 3) ].rgbGreen ; break;
					case 2: i = rgb[ (tmp.cnt / 3) ].rgbBlue ; break;
				}
				fputc( i,stdout );
			}
			
			/*
			// データ部 */
			fseek( fp, offset, SEEK_SET );
			tmp.cntw = 0;
			for( tmp.cnt = 0 ; (i = fgetc( fp )) != EOF;  )
			{
				fputc( i, stdout );
				tmp.cnt++;
				if( tmp.cnt == tmp.w )
				{
					if( tmp.w % 4 )
					{
						tmp.cnt = 4 - (tmp.w % 4);
						for( i = 0; i < tmp.cnt; i++ )
							fgetc( fp );
					}
					tmp.cnt = 0;
					tmp.cntw++;
				}
				if( tmp.cntw == tmp.h ) break;
			}
		}
		else /* 24 bits */
		{
			b24 = p24 = NULL;
			b24 = malloc( tmp.w * 3 );
			p24Size = 2048;
			p24 = malloc( p24Size );
			/* mode is count of Palette */
			mode = 0;
			
			/*
			// バレット部 */
			fseek( fp, offset, SEEK_SET );
			for( tmp.cnt = 0; tmp.cnt < tmp.h; ++tmp.cnt )
			{
				i = fread( b24, tmp.w * 3, 1, fp );
				if( i != 1 )
				{
					perror( "b24:fread1" );
					goto END_PAL24;
				}
				for( i = 0; i < tmp.w; ++i )
				{
					data = *(b24 + i * 3);
					data <<= 8;
					data += *(b24 + i * 3 +1);
					data <<= 8;
					data += *(b24 + i * 3 +2);
					for( j = 0; j < mode; ++j )
					{
						if( data == *(p24 +j) )
								break;
					}
					if( data == *(p24 +j) )
							continue;
					*(p24 +mode) = data;
					mode++;
					if( p24Size <= mode )
					{
						p24Size *= 2;
						p2 = realloc( p24, p24Size );
						if( p2 == NULL )
								goto END_PAL24;
						p24 = p2;
					}
				}
			}
			tmp.pal = mode;
			fprintf( stdout, "x:0\ny:0\nw:%d\nh:%d\np:%d\n",
					tmp.w,tmp.h,tmp.pal );
			/*
			// パレット部 */
			tmp.max = tmp.pal;
			for( tmp.cnt = 0; tmp.cnt < tmp.max ; tmp.cnt++ )
			{
				for( j = 0; j < 3; ++j )
				{
					switch( j )
					{
						case 2: i = *(p24 + tmp.cnt) >> 16;  break;
						case 1: i = *(p24 + tmp.cnt) >> 8 ; break;
						case 0: i = *(p24 + tmp.cnt); break;
					}
					i &= 0xff;
					fputc( i,stdout );
				}
			}
			
			/*
			// データ部 */
			i = fseek( fp, offset, SEEK_SET );
			if( i )
			{
				perror( "b24:fseek2" );
				goto END_PAL24;
			}
			for( tmp.cnt = 0; tmp.cnt < tmp.h; ++tmp.cnt )
			{
				i = fread( b24, tmp.w * 3, 1, fp );
				if( i != 1 )
				{
					perror( "b24:fread2" );
					goto END_PAL24;
				}
				for( i = 0; i < tmp.w; ++i )
				{
					data = *(b24 + i * 3);
					data <<= 8;
					data += *(b24 + i * 3 +1);
					data <<= 8;
					data += *(b24 + i * 3 +2);
					for( j = 0; j < mode; ++j )
					{
						if( data == *(p24 +j) )
								break;
					}
					fputc( j, stdout );
				}
			}
		  END_PAL24:
			if( b24 != NULL ) free( b24 );
			if( p24 != NULL ) free( p24 );
		}
	}
	else return 1;
	
	fclose( fp );
	
	return 0;
}

