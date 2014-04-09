#include "StdAfx.h"
#include "DrawCurve.h"
#include <fstream>

DrawCurve::DrawCurve(void)
{
}
DrawCurve::DrawCurve(CRect Rect, float* data, int size)
{
	SetInit(Rect,data,size);
}
void DrawCurve::SetInit(CRect Rect,float* data, int size)
{
	GetPixelY(Rect,data,size);
	this ->size = size;
}
void DrawCurve::GetPixelY(CRect Rect,float* data, int size)
{
	float max;
	float min;
	CRect subrect;
	//subrect.top = Rect.top + 30;
	subrect.top = Rect.top + 30 + static_cast<int>((Rect.bottom - Rect.top - 60)/5.0);
	subrect.bottom = Rect.bottom - 30;
	subrect.left = Rect.left + 45;
	subrect.right = Rect.right - 30;
	max = *(data);
	min = max;
	for(int i = 1; i < size; ++i)
	{
		if(*(data + i) > max)
			max = *(data + i);
		if(*(data + i) < min)
			min = *(data + i);
	}
	this ->pixely = new int[size];
	for(int i = 0; i < size; ++i)
	{
		*(pixely + i) = (subrect.bottom - subrect.top + 0.0)/(max - min + 0.0) * (*(data + i) - min) + subrect.top;
		*(pixely +i) = subrect.bottom  + subrect.top - *(pixely +i);
		/*if(*(pixely + i) > (subrect.bottom - subrect.top + 0.0) / 2.0)
			*(pixely + i) -= (subrect.bottom - subrect.top + 0.0) / 2.0;
		else
			*(pixely + i) += (subrect.bottom - subrect.top + 0.0) / 2.0;*/
	}
}
void DrawCurve::Draw(CDC* pDC,CRect Rect)
{
	CRect subrect;
	CPen p;
	p.CreatePen(PS_SOLID,2,RGB(255,0,0));
	pDC ->SelectObject(p);
	//subrect.top = Rect.top + 30 + static_cast<int>((Rect.bottom - Rect.top - 60)/5.0);
	subrect.bottom = Rect.bottom - 30;
	subrect.left = Rect.left + 45;
	subrect.right = Rect.right - 30;
	float step;
	int stepint;
	step = (this ->size/2 + 0.0) / (subrect.right - subrect.left +0.0) + 0.5;
	int pixelx = 0;
	std::ofstream f1;
	f1.open("pixelx.txt",32);
	//for(int i = 0; i < (this ->size); ++i)
	//for(int i = 0; i < this ->size/2; ++i)
	stepint = static_cast<int>(step);
	pDC ->MoveTo(subrect.left,*(this ->pixely));
	for(int i = 1; i < subrect.right - subrect.left; ++i)
	{
		//stepint = static_cast<int>(step*i);
		//pixelx = subrect.left + stepint;
		//f1 << *(this ->pixely + stepint)<<"\r\n";
		f1 << subrect.left + i;
		f1 << " ";
		f1 << *(this ->pixely + stepint) <<"\r\n";
		//pDC ->LineTo(pixelx,*(this ->pixely + i));
		pDC ->LineTo(subrect.left + i,*(this ->pixely + i*stepint));
	}
	f1.close();
}
void DrawCurve::Output()
{
	std::ofstream f1;
	f1.open("pixely.txt",32);
	for(int i = 0; i < (this ->size); ++i)
		f1 << *(i + pixely) <<"\r\n";
	f1.close();
}
DrawCurve::~DrawCurve(void)
{
}
