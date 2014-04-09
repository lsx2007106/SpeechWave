#pragma once
#include "Fft.h"
class Response
{
private:
	float* pRe;
	float* pIm;
	float* qRe;
	float* qIm;
//	float* responsedata;
	int size;
	//CRect Rect;
	//wchar_t YAxisLabel[6][8];
	//wchar_t XAxisLabel[5][7];
	/*virtual void CalcResponseData();
	virtual void GetYAxisLabel();*/
	void FFTTransform();
public:
	Response(void); //Add some comment
	Response(char* buffer,int PixelX);
	/*void DrawYAxisLabel(CDC* pDC);
	void DrawCurve(CDC* pDC);*/
	void SetInit(float*p,int size);
	void CopyInit(float* p,float* q, int size);
	void SetComplexPQ(char* buffer, int PixelX);
	float* GetpRe();
	float* GetpIm();
	float* GetqRe();
	float* GetqIm();
	int GetSize();
	~Response(void);
};

