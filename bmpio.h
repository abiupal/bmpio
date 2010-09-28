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

typedef struct {
	unsigned short  bfType;
	unsigned long   bfSize;
	short           bfReserved1;
	short           bfReserved2;
	unsigned long   bfOffBits;
} __attribute__ ((packed)) BITMAPFILEHEADER;
	
typedef BITMAPFILEHEADER*  bmpFileHPtr;

typedef struct {
    unsigned long    biSize;
    unsigned long    biWidth;
    unsigned long    biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned long   biCompression;
    unsigned long   biSizeImage;
    unsigned long   biXPelsPerMeter;
    unsigned long    biYPelsPerMeter;
    unsigned long    biClrUsed;
    unsigned long    biClrImportant;
    } __attribute__ ((packed)) BITMAPINFOHEADER;

typedef BITMAPINFOHEADER*  bmpInfoHPtr;

typedef struct {
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
} __attribute__ ((packed)) BITMAPHEADERS;

typedef struct {
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;
} __attribute__ ((packed)) BITMAP_RGBQUAD;

typedef struct {
	BITMAP_RGBQUAD pal[256];
} __attribute__ ((packed)) BITMAP_PAL256;

#endif

