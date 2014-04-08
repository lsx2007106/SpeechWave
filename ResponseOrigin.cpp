#include "StdAfx.h"
#include "Response.h"
#include "Fft.h"
//const wchar_t XAxisLabel[5][7]= {L"0",L"0.25Pi",L"0.5Pi",L"0.75Pi",L"Pi"};
Response::Response(void)
{
}
inline void Response::SetInit(float* p,int size)
{
	for(int i = 0; i < size; ++i)
		*(p+i) = 0.0;
}
inline void Response::CopyInit(float* p, float* q, int size)
{
	for(int i = 0; i < size; ++i)
		*(p + i) = * (q + i);
}
void Response::SetComplexPQ(char* buffer,int PixelX)
{
	char* tmp;
	char* header;
	float* p;
	float* q;
	int psize = 0;
	int qsize = 0;
	int offset = 0;
	int i = 0;
	int power = 0;
	int sizeoffft = 1;
	int PixelXBackup = PixelX;
	tmp = buffer;
	tmp++;
	while(*tmp != 0x0A)
		tmp++;
	*tmp = 0;
	psize = atoi(buffer+1);
	p = new float[psize];
	tmp++;
	for(i = 0; i < psize; ++i)
	{
		header = tmp;
		while(*tmp != 0x0A)
			tmp++;
		*tmp = 0;
		*(p + i) = atof(header);
		tmp++;
	}
	tmp++;
	while(*tmp != 0x0A)
	{
		offset++;
		tmp++;
	}
	*tmp = 0;
	qsize = atoi(tmp - offset);
	q = new float[qsize];
	tmp++;
	for(i = 0; i < qsize; ++i)
	{
		header = tmp;
		while(*tmp != 0x0A)
			tmp++;
		*tmp = 0;
		*(q + i) = atof(header);
		tmp++;
	}
	while(PixelX > 1)
	{
		PixelX >>= 1;
		power++;
	}
	while(power != 0)
	{
		sizeoffft <<= 1;
		power--;
	}
	if(sizeoffft != PixelXBackup)
		sizeoffft <<= 1; //判断是否是的整数次幂。
	else
	{
		sizeoffft <<= 1;
		sizeoffft <<= 1;  //fft 的点数要多一倍。
	}
	this ->pReSize = sizeoffft;
	this ->qReSize = sizeoffft;
	this ->pRe = new float[sizeoffft];
	this ->pIm = new float[sizeoffft];
	this ->qRe = new float[sizeoffft];
	this ->qIm = new float[sizeoffft];
	this ->unite = new float[sizeoffft];
	this ->freqz = new float[sizeoffft];
	this ->phase = new float[sizeoffft];
	SetInit(this ->pRe,sizeoffft);
	SetInit(this ->pIm,sizeoffft);
	SetInit(this ->qRe,sizeoffft);
	SetInit(this ->qIm,sizeoffft);
	SetInit(this ->freqz,sizeoffft);
	SetInit(this ->phase,sizeoffft);
	CopyInit(this ->pRe,p,psize);
	CopyInit(this ->qRe,q,qsize);
	delete p;
	delete q;
	wcscpy(this ->XAxisLabel[0], L"0");
	wcscpy(this ->XAxisLabel[1], L"0.25Pi");
	wcscpy(this ->XAxisLabel[2], L"0.5Pi");
	wcscpy(this ->XAxisLabel[3], L"0.75Pi");
	wcscpy(this ->XAxisLabel[4], L"  Pi");
	wcscpy(this ->YAxisLabelFreq[0], L"\0");
	wcscpy(this ->YAxisLabelFreq[1], L"\0");
	wcscpy(this ->YAxisLabelFreq[2], L"\0");
	wcscpy(this ->YAxisLabelFreq[3], L"\0");
	wcscpy(this ->YAxisLabelFreq[4], L"\0");
	wcscpy(this ->YAxisLabelPhase[0], L"\0");
	wcscpy(this ->YAxisLabelPhase[1], L"\0");
	wcscpy(this ->YAxisLabelPhase[2], L"\0");
	wcscpy(this ->YAxisLabelPhase[3], L"\0");
	wcscpy(this ->YAxisLabelPhase[4], L"\0");
}
Response::Response(char* buffer,int PixelX)
{
	SetComplexPQ(buffer,PixelX);
}
void Response::FrequencyResponse()
{
	TFft transp(this ->pReSize);
	TFft transq(this ->qReSize);
	transq.Transform(qRe,qIm);
	transp.Transform(pRe,pIm);
	for(int i = 0; i < this ->pReSize; ++i)
		*(this ->freqz +i) = log10(((*(this ->pRe +i)) * (*(this ->pRe +i)) + (*(this ->pIm +i)) * (*(this ->pIm +i))) / ((*(this ->qRe +i)) * (*(this ->qRe +i)) + (*(this ->qIm +i)) * (*(this ->qIm +i))));
}
void Response::SetYAxisLabelFreq()
{
	int XAxisx;
	int XAxisy;
	int YAxisx;
	int YAxisy;
	int maxofamplitude;
	int minofamplitude;
	//wchar_t t[5] = L"joke";
	char amplitudeunite[8] = {"\0"};
	size_t convertedChars = 0;
	maxofamplitude = *(this ->freqz);
	minofamplitude = maxofamplitude;
	for(int i = 0; i < this ->pReSize; ++i)
	{
		if(maxofamplitude <= *(this ->freqz + i))
			maxofamplitude = *(this ->freqz +i);
		if(minofamplitude >= *(this ->freqz + i))
			minofamplitude = *(this ->freqz +i);
	}
	for(int i = 0; i < 5; ++i)
	{
		sprintf(amplitudeunite,"%.2f",minofamplitude + (maxofamplitude-minofamplitude)/4.0*i);
		mbstowcs_s(&convertedChars,this ->YAxisLabelFreq[i],amplitudeunite,5);
	}
	wcscpy(YAxisLabelFreq[5],L"dB");
}
void Response::SetYAxisLabelPhase()
{
	int XAxisx;
	int XAxisy;
	int YAxisx;
	int YAxisy;
	int maxofphase;
	int minofphase;
	//wchar_t t[5] = L"joke";
	char phaseunite[8] = {"\0"};
	size_t convertedChars = 0;
	maxofphase = *(this ->phase);
	minofphase = maxofphase;
	for(int i = 0; i < this ->pReSize; ++i)
	{
		if(maxofphase <= *(this ->phase + i))
			maxofphase = *(this ->phase +i);
		if(minofphase >= *(this ->phase + i))
			minofphase = *(this ->phase +i);
	}
	for(int i = 0; i < 5; ++i)
	{
		sprintf(phaseunite,"%.2f",minofphase + (maxofphase-minofphase)/4.0*i);
		mbstowcs_s(&convertedChars,this ->YAxisLabelPhase[i],phaseunite,5);
	}
	wcscpy(YAxisLabelFreq[5],L"dB");
}
//           -------------------------------------
//          |    width30                         |
//          |     ---------------------------    |
//          |w45 |                          |    |
//          |    |                          |w30 |
//          |    |                          |    |
//          |    |                          |    |
//          |    |                          |    |
//          |    |                          |    |
//          |    |--------------------------|    |
//          |    width30                         |
//          --------------------------------------
void Response::DrawCoordinateWithLabel(CDC* dc,CRect Rect,wchar_t YAxisLabel[6][8])
{
	CFont f;
	CPen p;
	p.CreatePen(PS_SOLID,2,RGB(0,0,0));
	f.CreateFontW(15,6,0,0,FW_NORMAL,false,false,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,_T("Arial"));
	dc ->SelectObject(f);
	dc ->SelectObject(p);
	int XAxisx;
	int XAxisy;
	int YAxisx;
	int YAxisy;
	long fontwidth;
	LOGFONT currentfont;
	f.GetLogFont(&currentfont);
	fontwidth = currentfont.lfWidth;
	YAxisx = Rect.left + 45;
	XAxisy = Rect.bottom - 30;
	dc ->MoveTo(YAxisx,XAxisy);
	dc ->LineTo(YAxisx,Rect.top + 30);
	dc ->MoveTo(YAxisx,XAxisy);
	dc ->LineTo(Rect.right -30,XAxisy);
	bool flag;
	CRect AxisLabelRect;
	for(int i = 0; i < 5; ++i)
	{
		XAxisx = Rect.left + 45 + static_cast<int>((Rect.right - Rect.left -75)*i/4.0);
		if(i == 0)
			AxisLabelRect.left = XAxisx;
		else
			AxisLabelRect.left = XAxisx -10;
		AxisLabelRect.right = XAxisx + wcslen(XAxisLabel[i])*fontwidth;
		AxisLabelRect.top = XAxisy +4;
		AxisLabelRect.bottom = XAxisy +4 +15;
		if(i > 0)
		{
			dc ->MoveTo(XAxisx,XAxisy);
			dc ->LineTo(XAxisx,XAxisy-4);
		}
		dc ->DrawTextW(XAxisLabel[i],wcslen(XAxisLabel[i]),AxisLabelRect,DT_LEFT|DT_TOP);
	}
	for(int i = 0; i < 5; ++i)
	{
		YAxisy = Rect.top + 30 + static_cast<int>((Rect.bottom - Rect.top - 60)*(i+1)/5.0);
		AxisLabelRect.left = YAxisx - wcslen(YAxisLabel[4-i])*fontwidth -2;
		AxisLabelRect.right = YAxisx -2;
		AxisLabelRect.top = YAxisy - 6;
		AxisLabelRect.bottom = YAxisy + currentfont.lfHeight;
		dc ->DrawTextW(YAxisLabel[4-i],wcslen(YAxisLabel[4-i]),AxisLabelRect,DT_LEFT|DT_TOP);
		dc ->MoveTo(YAxisx,YAxisy);
		dc ->LineTo(YAxisx+4,YAxisy);
	}
	YAxisy = Rect.top + 30;
	AxisLabelRect.left = YAxisx - wcslen(YAxisLabel[5])*(fontwidth+2) -5;
	AxisLabelRect.right = YAxisx -5;
	AxisLabelRect.top = YAxisy - 6;
	AxisLabelRect.bottom = YAxisy + currentfont.lfHeight;
	dc ->DrawTextW(YAxisLabel[5],wcslen(YAxisLabel[5]),AxisLabelRect,DT_LEFT|DT_TOP);
}
void Response::SetYAxisLabel()
{
	SetYAxisLabelFreq();
	SetYAxisLabelPhase();
}
void Response::PhaseResponse()
{
	float y; 
	float x;
	for(int i = 0; i < this ->pReSize; ++i)
	{
		y = -*(this ->pIm + i) * (*(this ->qRe + i)) - (*(this ->pRe + i)) * (*(this ->qIm + i));
		x = *(this ->pRe + i) * (*(this ->qRe +i)) + (*(this ->pIm + i)) * (*(this ->qIm + i));
		*(this ->phase + i) = atan2(y,x);
	}
	Unwrap();
}
void Response::Unwrap()
{
	float difference;
	float Pi;
	int j = 1;
	Pi = 4*atan(1.0);
	for(int i = 1; i < this ->pReSize; ++i)
	{
		difference = *(this ->phase + i) - *(this ->phase + i-1);
		if(difference > Pi)
		{
			for(j = i; j < this ->pReSize; ++j)
				*(this ->phase + j) -= 2.0 * Pi;
		}
		if(difference < -1.0*Pi)
		{
			for(j = i; j < this ->pReSize; ++j)
				*(this ->phase + j) += 2.0 * Pi;
		}
	}
}
void Response::DrawCoordinate(CDC* dc,CRect Rect)
{
	DrawCoordinateWithLabel(dc,Rect,this ->YAxisLabelFreq);
	DrawCoordinateWithLabel(dc,Rect,this ->YAxisLabelPhase);
}
void Response::GetPixel(CRect Rect, float* response, int responsepixel[1024])
{
	float max;
	float min;
	CRect subrect;
	subrect.top = Rect.top + 30;
	subrect.bottom = Rect.bottom - 30;
	subrect.left = Rect.left + 45;
	subrect.right = Rect.right - 30;
	max = *(this ->freqz);
	min = max;
	for(int i = 1; i < this ->pReSize; ++i)
	{
		if(*(this ->freqz + i) > max)
			max = *(this ->freqz + i);
		if(*(this ->freqz + i) < min)
			min = *(this ->freqz + i);
	}
	for(int i = 0; i < this ->pReSize; ++i)
		responsepixel[i] = (subrect.top - subrect.bottom + 0.0)/(max - min + 0.0) * (*(response + i) - min) + subrect.top;
}
void Response::DrawResponseCurve(CDC* dc, CRect Rect)
{
	GetPixel(Rect,this ->freqz,this ->freqzpixel);
	GetPixel(Rect,this ->phase,this ->phasepixel);
	DrawCurve(dc,Rect,this ->freqzpixel);
	DrawCurve(dc,Rect,this ->phasepixel);
}
void Response::DrawCurve(CDC* dc,CRect Rect,int* data)
{
	CRect subrect;
	CPen p;
	p.CreatePen(PS_SOLID,2,RGB(0,0,255));
	subrect.top = Rect.top + 30;
	subrect.bottom = Rect.bottom - 30;
	subrect.left = Rect.left + 45;
	subrect.right = Rect.right - 30;
	float step;
	int stepint;
	step = (this ->pReSize + 0.0) / (subrect.right - subrect.left +0.0) + 0.5;
	int pixelx;
	for(int i = 0; i < this ->pReSize; ++i)
	{
		stepint = static_cast<int>(step*i);
		pixelx = subrect.top + stepint;
		dc ->LineTo(pixelx,data[pixelx]);
	}
}
Response::~Response(void)
{
	delete pRe;
	delete qRe;
	delete pIm;
	delete qIm;
	delete unite;
	delete freqz;
	delete phase;
}
