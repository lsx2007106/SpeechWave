#if !defined __plotwave_h
#define  __plotwave_h

//
// FileName: PlotWave.h			By YANG Jian		2000.3.28
//

#include  "stdafx.h"


#include "PCMWave.h"
#define COLOR_RGB(r, g, b)  (COLORREF)((b<<16) + (g<<8) + r)

enum {
	EDGE = 3
};
class  CPlotWave {
  public:
	CPlotWave(CWave* pwave);
	~CPlotWave();

	BOOL operator ! ();
	void  SetDC(CDC *pdc) { pDC = pdc; };
	void  SetRect(CRect &rect) { ClientRect.CopyRect(&rect); };
	void  Plot(CDC* pdc, CRect &rect);
	void  Plot(CDC* pdc, CRect &rect, int lleft, int lright);
	// void  PlotCursorLine(int xpos, CDC* pdc=0);
	// void  PlotLabelLine(int xpos, CDC* pdc=0);
	int   SamplePointToXPos(int lpoint);
	int   XPosToSamplePoint(int xpos);
	float SamplePointToTime(int lpoint);
	inline int GetMaxAbsAmp() { return  maxAmp > -minAmp ? maxAmp : -minAmp; };

  private:
	CWave  *pWave;
	CDC    *pDC;
	BOOL   Is8Bit;
	int	   minAmp, maxAmp;
	int    lLeft, lRight;
	CRect  ClientRect;

	void  SetInitValue();
	void  PlotWaveform_8Bit(int lStartPoint, int lEndPoint);
	void  PlotWaveform_16Bit(int lStartPoint, int lEndPoint);
	void  ErrorMessage(const char *text, const char *title = "CPlotWave");
};
#endif // __plotwave_h