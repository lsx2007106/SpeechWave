#pragma once
class DrawCurve
{
private:
	int size;
	int* pixely;
	void SetInit(CRect Rect,float* data, int size);
	void GetPixelY(CRect Rect,float* data, int size);
public:
	void Draw(CDC *pDC,CRect Rect);
	DrawCurve(void);
	DrawCurve(CRect Rect,float* data, int size);
	void Output();
	~DrawCurve(void);
};

