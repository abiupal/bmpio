/*
//  bmpio.h
//
//  Bitmap ファイル定義
//  
//  Created by 武村　健二 on Fri 27th Jan 2003.
//  Copyright (c) 1994-2002 J.T.S.TAKEMURA CO.,LTD. All rights reserved.
//  Copyright (c) 2002,2005 ORIYA. All rights reserved.
//
*/
#ifndef _BMPIO_H
#define _BMPIO_H

#pragma pack(2)

typedef struct {
	unsigned short  bfType;
	unsigned int   bfSize;
	short           bfReserved1;
	short           bfReserved2;
	unsigned int   bfOffBits;
} BITMAPFILEHEADER;
	
typedef BITMAPFILEHEADER*  bmpFileHPtr;

typedef struct {
    unsigned int    biSize;
    unsigned int    biWidth;
    unsigned int    biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int   biCompression;
    unsigned int   biSizeImage;
    unsigned int   biXPelsPerMeter;
    unsigned int    biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
} BITMAPINFOHEADER;

typedef BITMAPINFOHEADER*  bmpInfoHPtr;

typedef struct {
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
} BITMAPHEADERS;

typedef struct {
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;
} BITMAP_RGBQUAD;

typedef struct {
	BITMAP_RGBQUAD pal[256];
} BITMAP_PAL256;

#pragma pack()

#endif

