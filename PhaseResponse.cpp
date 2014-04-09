#include "StdAfx.h"
#include "PhaseResponse.h"
#include "fstream"


PhaseResponse::PhaseResponse(Response &res):res(res)
{
	this ->pRe = res.GetpRe();
	this ->pIm = res.GetpIm();
	this ->qRe = res.GetqRe();
	this ->qIm = res.GetqIm();
	this ->SetInit();
}
void PhaseResponse::SetInit()
{
	this ->phase = new float[res.GetSize()];
	for(int i = 0; i < this ->res.GetSize(); ++i)
		*(this ->phase + i) = 0.0;
	wcscpy(this ->XAxisLabel[0], L"0");
	wcscpy(this ->XAxisLabel[1], L"0.25Pi");
	wcscpy(this ->XAxisLabel[2], L"0.5Pi");
	wcscpy(this ->XAxisLabel[3], L"0.75Pi");
	wcscpy(this ->XAxisLabel[4], L"  Pi");
	wcscpy(this ->YAxisLabel[0], L"\0");
	wcscpy(this ->YAxisLabel[1], L"\0");
	wcscpy(this ->YAxisLabel[2], L"\0");
	wcscpy(this ->YAxisLabel[3], L"\0");
	wcscpy(this ->YAxisLabel[4], L"\0");
	this ->SetPhase();
	this ->SetYAxisLabel();
}
void PhaseResponse::SetPhase()
{
	float y; 
	float x;
	for(int i = 0; i < this ->res.GetSize(); ++i)
	{
		y = -*(this ->pIm + i) * (*(this ->qRe + i)) - (*(this ->pRe + i)) * (*(this ->qIm + i));
		x = *(this ->pRe + i) * (*(this ->qRe +i)) + (*(this ->pIm + i)) * (*(this ->qIm + i));
		*(this ->phase + i) = atan2(y,x);
	}
	Unwrap();
}
void PhaseResponse::Unwrap()
{
	float difference;
	float Pi;
	int j = 1;
	Pi = 4*atan(1.0);
	for(int i = 1; i < this ->res.GetSize(); ++i)
	{
		difference = *(this ->phase + i) - *(this ->phase + i-1);
		if(difference > Pi)
		{
			for(j = i; j < this ->res.GetSize(); ++j)
				*(this ->phase + j) -= 2.0 * Pi;
		}
		if(difference < -1.0*Pi)
		{
			for(j = i; j < this ->res.GetSize(); ++j)
				*(this ->phase + j) += 2.0 * Pi;
		}
	}
}
void PhaseResponse::SetYAxisLabel()
{
	/*int XAxisx;
	int XAxisy;
	int YAxisx;
	int YAxisy;*/
	int maxofphase;
	int minofphase;
	//wchar_t t[5] = L"joke";
	char phaseunite[8] = {"\0"};
	size_t convertedChars = 0;
	maxofphase = *(this ->phase);
	minofphase = maxofphase;
	for(int i = 0; i < res.GetSize()/2; ++i)
	{
		if(maxofphase <= *(this ->phase + i))
			maxofphase = *(this ->phase +i);
		if(minofphase >= *(this ->phase + i))
			minofphase = *(this ->phase +i);
	}
	for(int i = 0; i < 5; ++i)
	{
		sprintf(phaseunite,"%.2f",minofphase + (maxofphase-minofphase)/4.0*i);
		mbstowcs_s(&convertedChars,this ->YAxisLabel[i],phaseunite,5);
	}
	wcscpy(this ->YAxisLabel[5],L"rad");
}
void PhaseResponse::DrawCoordinate(CDC* dc ,CRect Rect)
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
		AxisLabelRect.right = XAxisx + wcslen(this ->XAxisLabel[i])*fontwidth;
		AxisLabelRect.top = XAxisy +4;
		AxisLabelRect.bottom = XAxisy +4 +15;
		if(i > 0)
		{
			dc ->MoveTo(XAxisx,XAxisy);
			dc ->LineTo(XAxisx,XAxisy-4);
		}
		dc ->DrawTextW(this ->XAxisLabel[i],wcslen(this ->XAxisLabel[i]),AxisLabelRect,DT_LEFT|DT_TOP);
	}
	for(int i = 0; i < 5; ++i)
	{
		YAxisy = Rect.top + 30 + static_cast<int>((Rect.bottom - Rect.top - 60)*(i+1)/5.0);
		AxisLabelRect.left = YAxisx - wcslen(this ->YAxisLabel[4-i])*fontwidth -2;
		AxisLabelRect.right = YAxisx -2;
		AxisLabelRect.top = YAxisy - 6;
		AxisLabelRect.bottom = YAxisy + currentfont.lfHeight;
		dc ->DrawTextW(this ->YAxisLabel[4-i],wcslen(this ->YAxisLabel[4-i]),AxisLabelRect,DT_LEFT|DT_TOP);
		dc ->MoveTo(YAxisx,YAxisy);
		dc ->LineTo(YAxisx+4,YAxisy);
	}
	YAxisy = Rect.top + 30;
	AxisLabelRect.left = YAxisx - wcslen(this ->YAxisLabel[5])*(fontwidth+2) -5;
	AxisLabelRect.right = YAxisx -5;
	AxisLabelRect.top = YAxisy - 6;
	AxisLabelRect.bottom = YAxisy + currentfont.lfHeight;
	dc ->DrawTextW(this ->YAxisLabel[5],wcslen(this ->YAxisLabel[5]),AxisLabelRect,DT_LEFT|DT_TOP);
}
float* PhaseResponse::GetPhase()
{
	return this ->phase;
}
int PhaseResponse::GetSize()
{
	return this ->res.GetSize();
}
void PhaseResponse::Output()
{
	std::ofstream f1;
	f1.open("phase.txt",32);
	for(int i = 0; i < (this ->res.GetSize()); ++i)
		f1 << *(i + this ->phase) <<"\r\n";
	f1.close();
}
PhaseResponse::~PhaseResponse(void)
{
	//delete this ->phase;
	delete phase;
}
