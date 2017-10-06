#include <windows.h>  
#include <vector>  
#include <stdio.h>  
#include <stdlib.h>
#include <TCHAR.H>  
#include <iostream>  
#include <string>  
#include <atltrace.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

using namespace std;
using namespace cv;

HBITMAP get_window_screen(LPWSTR caption_name, int& window_x, int& window_y);
int SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName);
CvPoint getNextMinLoc(IplImage* result, int templatWidth, int templatHeight, double maxValIn, CvPoint lastLoc);
void SafeResetSizeOfRect(IplImage* src, CvRect& rect);
int get_template_location(const char * window_caption, const char * template_path, int& window_x, int& window_y, int& template_in_window_x, int& template_in_window_y);

/*
@author:		about:blank
@mail:		1043840980@qq.com
@qq:			1043840980
-----------------------------
Usage:

./PictureMatcher.exe <window_caption> <template_path> 

Example:

./PictureMatcher.exe "League of Legends" "C:/User/Public/play_btn.png"

*/
int main(int argc, char* argv[]){
	int window_x, window_y, template_x, template_y;
	//get_template_location("League of Legends", argv[1], window_x, window_y, template_x, template_y);
	get_template_location("League of Legends", "C:/Users/Public/Bullup/auto_program/resources/china/play.bmp", window_x, window_y, template_x, template_y);

	printf("{ \"window_x\": %d,\"window_y\": %d,\"template_x\": %d,\"template_y\": %d }", window_x, window_y, template_x, template_y);
	
	//system("pause");
	return 0;
}

int get_template_location(const char * window_caption, const char * template_path, int& window_x, int& window_y, int& template_in_window_x, int& template_in_window_y) {
	wchar_t wtext[40];
	mbstowcs(wtext, window_caption, strlen(window_caption) + 1);//Plus null
	LPWSTR window_caption_lpwstr = wtext;
	HBITMAP hbitmap = get_window_screen(window_caption_lpwstr, window_x, window_y);
	SaveBitmapToFile(hbitmap, L"C:/Users/Public/Bullup/screen_shot.png");
	IplImage*src, *templat, *result, *show;
	int srcW, templatW, srcH, templatH, resultW, resultH;
	//����Դͼ��
	src = cvLoadImage("C:/Users/Public/Bullup/screen_shot.png", CV_LOAD_IMAGE_GRAYSCALE);
	//������ʾ���

	//����ģ��ͼ��
	templat = cvLoadImage(template_path, CV_LOAD_IMAGE_GRAYSCALE);
	srcW = src->width;
	srcH = src->height;
	templatW = templat->width;
	templatH = templat->height;
	//����������Ĵ�С
	resultW = srcW - templatW + 1;
	resultH = srcH - templatH + 1;
	//������Ž���Ŀռ�
	result = cvCreateImage(cvSize(resultW, resultH), 32, 1);
	double minVal, maxVal;
	CvPoint minLoc, maxLoc;
	//����ģ��ƥ�亯��
	cvMatchTemplate(src, templat, result, CV_TM_SQDIFF);
	//���������Ƶ�ֵ������������
	cvMinMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, NULL);
	template_in_window_x = minLoc.x;
	template_in_window_y = minLoc.y;
	//printf("minVal  %f   maxVal %f\n ", minVal, maxVal);
	////��ʾ���
	//printf("%d %d", minLoc.x, minLoc.y);
	return 0;
}

//��̨ץͼ   2017/9/3
HBITMAP get_window_screen(LPWSTR caption_name, int &window_x, int& window_y) {
	HWND dest_window = FindWindow(NULL, caption_name);
	//const char *abc = "001607EE";
	//HWND dest_window = (HWND)strtoul(abc, NULL, 16);	//16����
	HDC dest_DC = GetDC(dest_window);
	HDC mem_DC = CreateCompatibleDC(dest_DC);
	RECT rect;
	BYTE*   Data;
	HBITMAP h_bitmap;
	GetWindowRect(dest_window, &rect);
	window_x = rect.left;
	window_y = rect.top;
	BITMAPINFO   bi;
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bi.bmiHeader.biWidth = GetSystemMetrics(SM_CXSCREEN);
	bi.bmiHeader.biHeight = GetSystemMetrics(SM_CYSCREEN);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	h_bitmap = CreateDIBSection(mem_DC, &bi, DIB_RGB_COLORS, (void**)&Data, NULL, 0);
	SelectObject(mem_DC, h_bitmap);
	BitBlt(mem_DC, 0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, dest_DC, 0, 0, SRCCOPY);
	ReleaseDC(NULL, dest_DC);
	DeleteDC(mem_DC);
	return h_bitmap;
}

int SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName) {
	HDC            hDC; //�豸������  
	int            iBits;//��ǰ��ʾ�ֱ�����ÿ��������ռ�ֽ���  
	WORD           wBitCount;//λͼ��ÿ��������ռ�ֽ���      
	DWORD          dwPaletteSize = 0;//�����ɫ���С  
	DWORD          dwBmBitsSize;//λͼ�������ֽڴ�С  
	DWORD          dwDIBSize;// λͼ�ļ���С  
	DWORD          dwWritten;//д���ļ��ֽ���  
	BITMAP         Bitmap;//λͼ�ṹ  
	BITMAPFILEHEADER   bmfHdr;   //λͼ���Խṹ     
	BITMAPINFOHEADER   bi;       //λͼ�ļ�ͷ�ṹ  
	LPBITMAPINFOHEADER lpbi;     //λͼ��Ϣͷ�ṹ     ָ��λͼ��Ϣͷ�ṹ  
	HANDLE          fh;//�����ļ����  
	HANDLE            hDib;//�����ڴ���  
	HANDLE            hPal;//�����ڴ���  
	HANDLE          hOldPal = NULL;//��ɫ����    
	//����λͼ�ļ�ÿ��������ռ�ֽ���     
	hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else if (iBits <= 32)
		wBitCount = 24;
	//�����ɫ���С     
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) *sizeof(RGBQUAD);
	//����λͼ��Ϣͷ�ṹ     
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	//Ϊλͼ���ݷ����ڴ�     
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	if (lpbi == NULL) {
		return 0;
	}
	*lpbi = bi;
	// �����ɫ��  
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal) {
		hDC = GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	// ��ȡ�õ�ɫ�����µ�����ֵ     
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
	//�ָ���ɫ��        
	if (hOldPal) {
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}
	//����λͼ�ļ�         
	fh = CreateFile(lpFileName, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;
	// ����λͼ�ļ�ͷ     
	bmfHdr.bfType = 0x4D42;  // "BM"     
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	// д��λͼ�ļ�ͷ     
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// д��λͼ�ļ���������     
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//���        
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return 1;
}

//��ȡ���ο����Ͻ�
CvPoint getNextMinLoc(IplImage* result, int templatWidth, int templatHeight, double maxValIn, CvPoint lastLoc) {
	int y, x;
	int startY, startX, endY, endX;
	//�������ε����Ͻ�����
	startY = lastLoc.y - templatHeight;
	startX = lastLoc.x - templatWidth;
	//�������ε����½ǵ�����  ����εĶ��� ���Կ���Ƶ����ʾ
	endY = lastLoc.y + templatHeight;
	endX = lastLoc.x + templatWidth;
	//���������Խ��
	startY = startY < 0 ? 0 : startY;
	startX = startX < 0 ? 0 : startX;
	endY = endY > result->height - 1 ? result->height - 1 : endY;
	endX = endX > result->width - 1 ? result->width - 1 : endX;
	//��������ڲ� ��ֵΪ���ֵ ʹ�� �Ժ��ҵ���Сֵ ����λ�ڸ�����  �����ҵ��ص���Ŀ��
	for (y = startY; y<endY; y++) {
		for (x = startX; x<endX; x++) {
			cvSetReal2D(result, y, x, maxValIn);
		}
	}
	double minVal, maxVal;
	CvPoint minLoc, maxLoc;
	//����result�е���Сֵ ������������
	cvMinMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, NULL);
	return minLoc;
}

// ��ȫ���þ��δ�С  
void SafeResetSizeOfRect(IplImage* src, CvRect& rect) {
	rect.x = rect.x < 0 ? 0 : rect.x;
	rect.y = rect.y < 0 ? 0 : rect.y;
	rect.width = rect.width < 0 ? 0 : rect.width;
	rect.height = rect.height < 0 ? 0 : rect.height;
	if (rect.x > src->width || rect.y > src->height) {
		rect = cvRect(0, 0, src->width, src->height);
	}
	rect.width = std::min(rect.width, src->width - rect.x);
	rect.height = std::min(rect.height, src->height - rect.y);
}
