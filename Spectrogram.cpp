//
// FileName:  Spectrogram.cpp   BY YANG Jian	2000.3.31

#include "stdafx.h"
#include <math.h>
//#include "VS.h"

#include  "Spectrogram.h"
#include  "fft.h"
#include "CSpectrogramParam.h"

CSpectrogram::CSpectrogram()
{
	SetInitValue();
}

// 构造函数，p_wave所指的CWave对象被引用，但不另外复制
CSpectrogram::CSpectrogram(CWave *p_wave)
{
	SetInitValue();

	if(p_wave) if(!(!(*p_wave))) {
		w = p_wave;
		nBits = w->GetBitsPerSample();
		nSamplingRate = w->GetSamplingRate();
		nSample = w->GetSampleNumber();
	}
}

// Function SetInitValue() uses to set init value to members of CSpectrogram.
void CSpectrogram::SetInitValue()
{
	strBmpFileName = new char [128];
	*strBmpFileName = '\0';
	pBmp = 0;
	w = 0;
	pDC = 0;
	nFft = 0;
	nFreq = 0;
	nGrey = 0;
	dbRange = 0;
	Width = 0;
	Height = 0;
	pSPData = 0;
	nWindow = 0;
	lLeft = 0;
	lRight = 0;
}

//  Deconstruct function of CSpectrogram.
CSpectrogram::~CSpectrogram()
{
	if(pBmp) delete[] pBmp;
	if(pSPData) delete[] pSPData;
	delete[] strBmpFileName;
}

// Function ComputeSpectrum() uses to compute spectrum.
bool CSpectrogram::ComputeSpectrum()
{
	int	  i;
	int  lpoint, l, l1, l3;
	float  move;
	float  *xr, *xi, *hamming;
	double pi2;
	short  *sample;
	TFft   *f;
	CSpectrogramParam g_SpectrogramParam;
	// set computing parameters.
	nGrey = g_SpectrogramParam.GetnGrey();
	dbRange = g_SpectrogramParam.GetdbRange();
	Width = g_SpectrogramParam.GetnWidthInPixel();
	nFft = g_SpectrogramParam.GetnFft();
	nWindow = (g_SpectrogramParam.GetnAnalysisWindow() * w->GetSamplingRate() / (float)1000.0 + 0.5);
	while(nWindow > nFft) nFft *= 2;	
	nFreq = nFft / 2;
	Height = nFreq;

	lpoint = lRight - lLeft + 1;
	if(lpoint < 10*nWindow) {
		ErrorMessage("Data length too short.");
		return false;
	}
	sample = new short [lpoint+100];
	if(!sample) {
		ErrorMessage("Error allocate memory.");
		return false;
	}

	// 确任Width为8的整数倍 !!!
	Width = ((Width + 7)/8) * 8; 
	move = (float)(lpoint-1)/(float)(Width-1);

	// 将数据存入sample[]，然后预加重
	if(nBits == 8) {
		unsigned char *p_byte = (unsigned char*)w->GetDataPtr() + lLeft;
		short *s = sample;
		for(l=0; l<lpoint; l++) *s++ = (short)(*p_byte++) - 128;
	} else {
		short *p_int16 = (short *)w->GetDataPtr() + lLeft;
		short *s = sample;
		for(l=0; l<lpoint; l++) *s++ = *p_int16++;
	}
	for(l=1; l<lpoint; l++) sample[l] -= (short)(0.95*sample[l-1] + 0.5);
	
	// declare and allocate a data array.
	if(pSPData) delete[] pSPData;
	pSPData = 0;
	pSPData = new int [Width*Height];
	int *pdata = pSPData; 

	// compute spectrum and store to pdata[].
	xr = new float [nFft];
	xi = new float [nFft];
	hamming = new float [nWindow];
	f = new TFft(nFft);
	if(!xr || !xi || !hamming || !f ) {
		ErrorMessage("Couldn't construct TFft and arrays.");
		return false;
	}

	// compute hamming window:
	pi2 = 8.0*atan(1.0) / (double)(nWindow - 1);
	for(i=0; i<nWindow; i++) hamming[i] = 0.54 - 0.46*cos(i*pi2);
	Max_DB = -32000;
	float temp; 
	for(l=0; l<Width; l++) {
		l1 = (int)(l*move + 0.5) - nWindow/2;
		l1 = max(l1, 0L);
		l1 = min(l1, lpoint-nWindow);
		for(i=0; i<nWindow; i++) {
			temp = (float)sample[l1+i]*hamming[i];
			xr[i] = temp;
			xi[i] = 0;
		}
		for(i=nWindow; i<nFft; i++) {
			xr[i] = 0.0;
			xi[i] = 0.0;
		}
		f->Transform(xr, xi);
		l3 = l*nFreq;
		for(i=0; i<nFreq; i++) {
			pdata[l3+i] = (int)(100.0*log10(max(0.25*(xr[i]*xr[i] + xi[i]*xi[i]), (float)1.0E-10))+0.5);
			Max_DB = max(Max_DB, pdata[l3+i]);
		}
	}
	delete  f;
	delete[] sample;	
	delete[] hamming;
	delete[] xr;
	delete[] xi;
	return true;
}

// Fuction construct convert struct data to BMP data.
void CSpectrogram::ConstructBMP()
{
	int  i;
	int  size, l, ll;
	float  ttt, qq;

	// Allocate memory for hBmp.
	size = Width * Height + sizeof(BITMAPINFOHEADER) + nGrey*sizeof(RGBQUAD);
	if(pBmp) delete[] pBmp;
	pBmp = 0;
	pBmp = new BYTE [size];

	// get BMP Data pointer (this->pBmpData).
	pBmpData = pBmp + sizeof(BITMAPINFOHEADER) + nGrey*sizeof(RGBQUAD);

	// construct DIB Bit Map Info Header.
	bmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHdr.biWidth = Width;
	bmpInfoHdr.biHeight = Height;
	bmpInfoHdr.biPlanes = 1;
	bmpInfoHdr.biBitCount = 8;
	bmpInfoHdr.biCompression = 0L;
	bmpInfoHdr.biSizeImage = 0L;
	bmpInfoHdr.biXPelsPerMeter = 0L;
	bmpInfoHdr.biYPelsPerMeter = 0L;
	bmpInfoHdr.biClrUsed = 0L;
	bmpInfoHdr.biClrImportant = 0L;

	BYTE   *pp = (BYTE  *)(&bmpInfoHdr);
	for(i=0; i < sizeof(BITMAPINFOHEADER); i++) pBmp[i] = pp[i];
	RGBQUAD *ppp = (RGBQUAD *) (pBmp + sizeof(BITMAPINFOHEADER));
	for(i=0; i<nGrey; i++) {
		(ppp+i)->rgbBlue = i;
		(ppp+i)->rgbGreen = i;
		(ppp+i)->rgbRed = i;
		(ppp+i)->rgbReserved = 0;
	}

	// transform BMP data from temp[] to pBmp[] (hBmp). Here, DBHigh <0, DBLow <0.
	int  *spdata = pSPData;
	
	qq = -255.0 / (float)(dbRange);
	for(l=0L; l<Width; l++) {
		ll = l*nFreq;
		for(i=0; i<nFreq; i++) {
			ttt = 0.1*( spdata[ll + i] - Max_DB );
			if( ttt > -(float)dbRange )  ttt *= qq;
			  else ttt = 255.0;
			*pX((int)i, l) = (BYTE)ttt;
		}
	}
}

// 显示整个语谱图
void CSpectrogram::Display(CDC* pdc, RECT rect)
{
	pDC = pdc;
	m_Rect = rect;

	DisplayBmp(0, nSample - 1);
}

// 直接用语谱图数据(BMP)放大/缩小，而不重新计算语谱数据
void CSpectrogram::Display(CDC* pdc, RECT rect, int lStartPoint, int lEndPoint)
{
	pDC = pdc;
	m_Rect = rect;

	DisplayBmp(lStartPoint, lEndPoint);
}

// Function DisplayBmp() uses to display SpectroGram. (form 3)
void CSpectrogram::DisplayBmp(int lStartPoint, int lEndPoint)
{
	if(min(lStartPoint, lEndPoint) != lLeft || max(lStartPoint, lEndPoint) != lRight) {
		lLeft = min(lStartPoint, lEndPoint);
		lRight = max(lStartPoint, lEndPoint);
		if(!ComputeSpectrum()) return;
		ConstructBMP();
	}

	CDC		*dc = pDC;
	BYTE	*pbmp;
	BYTE	*pbmpdata;
	int		nc;

	// to deside the number of color table:
	switch(bmpInfoHdr.biBitCount ) {
		case  1:
			nc = 2;
			break;
		case  4:
			nc = 16;
			break;
		case  8:
			nc = 256;
			break;
		case  24:
			nc = 0;
			break;
	}
	if(nc == 0) {
		ErrorMessage("Could not view the true color image.");
		return;
	}

	// get BMP pointer and BMP data pointer :
	pbmp = pBmp;
	pbmpdata = pbmp + sizeof(BITMAPINFOHEADER) + nc*sizeof(RGBQUAD);

	// create TPaint and DClogical palette:
	float  	rs = (float)0.0,	//(float)lStartPoint / (float)(nSample - 1),
			re = (float)1.0;	//(float)lEndPoint / (float)(nSample - 1);
	CRect  dstrect(m_Rect.left + EDGE, m_Rect.top + EDGE,
								m_Rect.right - EDGE, m_Rect.bottom - EDGE);
	CRect  srcrect(0, 0, bmpInfoHdr.biWidth, bmpInfoHdr.biHeight);

	// select and realize palette:
	dc->SetMapMode(MM_TEXT);
	dc->SetStretchBltMode(STRETCH_DELETESCANS);
	dc->SetViewportOrg(CPoint(0, 0));

	// copy image to window from memory:
	int  isok;
	isok = ::StretchDIBits(pDC->m_hDC, dstrect.left, dstrect.top, 
		dstrect.right - dstrect.left, dstrect.bottom - dstrect.top,
		srcrect.left, srcrect.top, 
		srcrect.right - srcrect.left, srcrect.bottom - srcrect.top,
		pbmpdata, (BITMAPINFO *)pbmp, DIB_RGB_COLORS, SRCCOPY);
	if(isok == GDI_ERROR) {
		ErrorMessage("GDI错误！可能数据太长。");
	}
	
	// 画方框和kHz线
	CPen  pen1(PS_SOLID, 1, COLOR_RGB(255,0,0));
	dc->SelectObject(&pen1);
	dc->MoveTo(m_Rect.left+EDGE-1, m_Rect.top+EDGE-1);
	dc->LineTo(m_Rect.right-EDGE+1, m_Rect.top+EDGE-1);
	dc->LineTo(m_Rect.right-EDGE+1, m_Rect.bottom-EDGE+1);
	dc->LineTo(m_Rect.left+EDGE-1, m_Rect.bottom-EDGE+1);
	dc->LineTo(m_Rect.left+EDGE-1, m_Rect.top+EDGE-1);

	float  step = (float)(m_Rect.bottom - m_Rect.top - 2*EDGE) / (0.0005 * nSamplingRate);
	float  x = step;
	int i = m_Rect.bottom - EDGE - (int)(x+0.5);
	CPen  pen2(PS_SOLID, 1, COLOR_RGB(127, 127, 127));
	dc->SelectObject(&pen2);
	while(i > m_Rect.top + EDGE) {
		dc->MoveTo(m_Rect.left+EDGE+1, i);
		dc->LineTo(m_Rect.right-EDGE-1, i);
		x += step;
		i = m_Rect.bottom - EDGE - (int)(x+0.5);
	}
}

// Function ErrorMessage() uses to print out error message.
void CSpectrogram::ErrorMessage(const char  *text, const char  *title)
{
	wchar_t textw[256];
	size_t convertedChars = 0;
	size_t newsize = strlen(text) +1;
    mbstowcs_s(&convertedChars, textw, newsize, text, _TRUNCATE);
	wchar_t titlew[256];
	mbstowcs_s(&convertedChars,titlew,newsize,text,_TRUNCATE);
	::MessageBox(NULL, textw, titlew, MB_OK);
}