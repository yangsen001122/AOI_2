#pragma once
#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include <PshPack2.h>  //memory forced to times of 2 Bytes

class CCreateBMP
{
 public:
	static bool RawData2BMP(BYTE *ptrData, int nWidth, int nHeight, const char * cFileName, bool bColor, int nBitDeep = 8)
	{
		FILE * fp;
		BITMAPFILEHEADER bmpFileHeader;
		BITMAPINFOHEADER bmpInfoHeader;
		unsigned long nSizeOrg, nSizeEx;
		int nExtend;
		bool bCreateOK = true;
		BYTE* byteEx=nullptr;
		nExtend = (nWidth + 3) / 4 * 4 - nWidth;//expended part out of times of 4
		if (bColor)
		{
			nSizeOrg = nWidth*nHeight * 3;
			nSizeEx=(nWidth+ nExtend)*nHeight * 3;
		}
		else
		{
			nSizeOrg = nWidth*nHeight;
			nSizeEx = (nWidth + nExtend)*nHeight ;
		}
		fopen_s(&fp, cFileName, "wb");  //open or create mode
		if (!fp)
		{ 
			bCreateOK = false;
			return bCreateOK;
		}
		bmpFileHeader.bfType = 0x4d42;
		if (bColor)
		{
			bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			bmpInfoHeader.biBitCount = nBitDeep*3;
		}
		else
		{
			bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4;//append palette after 2 headfiles
			bmpInfoHeader.biBitCount = nBitDeep;
		}
		bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + nSizeEx;
		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;

		bmpInfoHeader.biSize = 40;
		bmpInfoHeader.biWidth = nWidth;
		bmpInfoHeader.biHeight = -nHeight;
		bmpInfoHeader.biPlanes = 1;
		
		bmpInfoHeader.biCompression = 0;
		bmpInfoHeader.biSizeImage = 0;
		bmpInfoHeader.biXPelsPerMeter = 0;
		bmpInfoHeader.biYPelsPerMeter = 0;
		bmpInfoHeader.biClrUsed = 0;
		bmpInfoHeader.biClrImportant = 0;

		fwrite((void *)&bmpFileHeader, 1, sizeof(bmpFileHeader), fp);//write file head
		fwrite((void *)&bmpInfoHeader, 1, sizeof(bmpInfoHeader), fp);//write information head
		if (!bColor)
		{
			uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
			BITMAPINFO* bmi = (BITMAPINFO*)buffer;
			RGBQUAD* palette = bmi->bmiColors;
			for (int i = 0; i < 256; i++)
			{
				palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
				palette[i].rgbReserved = 0;
			}
			fwrite((void *)palette, 1, 1024, fp); //write palette
		}
		if (nExtend == 0)
		{
			fwrite(ptrData, 1, nSizeOrg, fp); //write pixel data
		}
		else
		{
			byteEx = new BYTE[nExtend];
			memset(byteEx, 0, nExtend);  //the expended part (if exists) fill up with black ,
			BYTE *pTempData;
			for (pTempData = ptrData + (nHeight - 1)*nWidth; pTempData >= ptrData; pTempData -= nWidth)
			{
				if (fwrite((void *)pTempData, 1, nWidth, fp) != (unsigned int)nWidth) // real pixel data
				{
					bCreateOK = false;
				}
				if (fwrite((void *)byteEx, 1, nExtend, fp) != 1) //the expended data
				{
					bCreateOK = false;
				}
			}
		}
		fclose(fp);
		if (nExtend != 0)
		{
			delete[] byteEx;
		}
		return bCreateOK;
	}
};

