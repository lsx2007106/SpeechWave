#pragma once
#include"Response.h"
#include<cmath>
class FreqResponse
{
private:
		Response res;
		wchar_t YAxisLabel[6][8];
		wchar_t XAxisLabel[5][7];
		float* pRe;
		float* pIm;
		float* qRe;
		float* qIm;
		float* freqz;
		void SetInit();
		void SetYAxisLabel();
		void SetFreqz();
	
public:
	void DrawCoordinate(CDC* pDC, CRect Rect);
	//FreqResponse(void);
	FreqResponse(Response &res);	
	void Output();
	float* GetFreqz();
	int GetSize();
	~FreqResponse(void);
};

