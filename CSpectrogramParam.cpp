#include "CSpectrogramParam.h"
#include <math.h>
CSpectrogramParam::CSpectrogramParam()
{

	this ->nAnalysisWindow = (float)6.66;		// 对应于 300Hz宽带语谱图
	this ->nFft = 128;
	this ->dbRange = 48;
	this ->nGrey = 256;
	this ->nWidthInPixel = 760;
}
void CSpectrogramParam::Set(float nanalysiswindow, int nfft, int nwidthinpixel, int dbrange, int ngrey)
{
	this ->nAnalysisWindow = nanalysiswindow;
	int  p = (int)(log(nfft+0.0)/log(2.0) + 0.5);
	this ->nFft = (int)(pow(2.0, (double)p) + 0.5);
	this ->dbRange = dbrange;
	this ->nGrey = ngrey;
	this ->nWidthInPixel = nwidthinpixel;
}
int CSpectrogramParam::GetnFft()
{ 
	return this ->nFft;
}
float CSpectrogramParam::GetnAnalysisWindow()
{ 
	return this ->nAnalysisWindow ;
}
int CSpectrogramParam::GetdbRange()
{ 
	return this ->dbRange ;
}
int CSpectrogramParam::GetnGrey()
{ 
	return this ->nGrey;
}
int CSpectrogramParam::GetnWidthInPixel()
{ 
	return this ->nWidthInPixel;
}