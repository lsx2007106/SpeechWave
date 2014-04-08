#pragma once
#include<cmath>
#include<sstream>
#include<string>
#include<vector>
class Response
{
public:
	Response(void);
	Response(char* buffer,int PixelX);
	void FrequencyResponse();
	void PhaseResponse();
	void DrawResponseCurve(CDC* dc, CRect rect);
	void DrawCoordinate(CDC* dc, CRect rect);
	void SetInit(float* p,int size);
	void CopyInit(float* p, float* q,int size);
	~Response(void);
private:
	float* freqz;
	float* phase;
	float* pRe;
	float* pIm;
	float* qRe;
	float* qIm;
	float* unite;
	int freqzpixel[1024];
	int phasepixel[1024];
	wchar_t YAxisLabelFreq[6][8];
	wchar_t YAxisLabelPhase[6][8];
	wchar_t XAxisLabel[5][7];
	void GetPixel(CRect Rect, float* response, int responsepixel[1024]);
	void DrawCurve(CDC* dc, CRect rect, int* data);
	void DrawCoordinateWithLabel(CDC* dc,CRect Rect,wchar_t YAxisLabel[6][8]);
	int pReSize;
	int qReSize;
	void SetComplexPQ(char* buffer, int PixelX);
	void SetYAxisLabelFreq();
	void SetYAxisLabelPhase();
	void SetYAxisLabel();
	void Unwrap();
};

