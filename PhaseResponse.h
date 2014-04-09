#pragma once
#include "Response.h"
class PhaseResponse
{
	private:
		Response res;
		wchar_t YAxisLabel[6][8];
		wchar_t XAxisLabel[5][7];
		float* pRe;
		float* pIm;
		float* qRe;
		float* qIm;
		float* phase;
		void SetInit();
		void SetYAxisLabel();
		void SetPhase();
		void Unwrap();
public:
	void DrawCoordinate(CDC* pDC, CRect Rect);
	//FreqResponse(void);
	PhaseResponse(Response &res);	
	void Output();
	float* GetPhase();
	int GetSize();
	~PhaseResponse(void);
};

