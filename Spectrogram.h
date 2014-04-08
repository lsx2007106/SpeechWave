#if !defined __Spectrogram_h
#define  __Spectrogram_h

#include  <math.h>

//
// FileName: Spectrogram.h			By YANG Jian	1995.11.29 2000.3.31
//
#include "PCMWave.h"
#include "PlotWave.h"
//#define COLOR_RGB(r, g, b)  (COLORREF)((b<<16) + (g<<8) + r)
//
//enum {
//	EDGE = 3
//};
class CSpectrogram
{
public:
	CSpectrogram();
	CSpectrogram(CWave *p_wave);
	~CSpectrogram();

	// ��ʾ��������ͼ
	void  Display(CDC* pdc, RECT rect);

	// ֱ��������ͼ����(BMP)�Ŵ�/��С���������¼�����������
	void  Display(CDC* pdc, RECT rect, int lStartPoint, int lEndPoint);

	inline BOOL operator !() { return !w; };

private:
	char	*strBmpFileName;
	CWave	*w;
	int		nSample, nBits, nSamplingRate;
	int     lLeft, lRight;
	BITMAPINFOHEADER	bmpInfoHdr;
	RECT  m_Rect;
	BYTE  *pBmp, *pBmpData;
	int   Width, Height;
	int   nFft, nWindow, dbRange, nGrey;
	int   *pSPData;
	int   Max_DB, nFreq;
	CDC   *pDC;
 
	void  SetInitValue();
	void  ErrorMessage(const char *text, const char *title = "CSpectrogram");
	bool  ComputeSpectrum();
	void  ConstructBMP();
	inline BYTE* pX(int h, int w) { return  pBmpData + h*Width + w; };
	inline BYTE  X(int h, int w) { return  *(pBmpData + h*Width + w); };
	void  DisplayBmp(int lStartPoint, int lEndPoint);
};
#endif  // end of __Spectrogram_h