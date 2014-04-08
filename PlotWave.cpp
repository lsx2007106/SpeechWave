//
// FileName: PlotWave.cpp		By Yang Jian	2000.2.29
//

#include "stdafx.h"


#include "PlotWave.h"

CPlotWave::CPlotWave(CWave* pwave)
{
	SetInitValue();
	pWave = pwave;
	Is8Bit = (pWave->GetBitsPerSample() == 8);
}

BOOL CPlotWave::operator ! ()
{
	if(!pWave) return FALSE;
	return !(*pWave);
}

void  CPlotWave::SetInitValue()
{
	pWave = 0;
	lLeft = 0L;
	lRight = 0L;
	minAmp = 1;
	maxAmp = -1;
}

// CPlotWave::~CPlotWave()
CPlotWave::~CPlotWave()
{
}

// void CPlotWave::ErrorMessage() uses to print error message.
void CPlotWave::ErrorMessage(const char *text, const char *title)
{
	wchar_t textw[256];
	wchar_t titlew[256];
	size_t newsize = strlen(text) +1;
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, textw, newsize, text, _TRUNCATE);
	newsize = strlen(title) +1;
	mbstowcs_s(&convertedChars, titlew, newsize, title, _TRUNCATE);
	::MessageBox(NULL, textw, titlew, MB_OK);
}

// float CPlotWave::SamplePointToTime()
float CPlotWave::SamplePointToTime(int lPoint)
{
	return (float)lPoint / (float)pWave->GetSamplingRate();
}

// void CPlotWave::PlotWave()    (form 1):
void CPlotWave::Plot(CDC *pdc, CRect &rect, int lleft, int lright)
{
	SetDC(pdc);
	SetRect(rect);

	lLeft = max(lleft, 0);
	lRight = min(lright, pWave->GetSampleNumber() - 1);

	if(Is8Bit) PlotWaveform_8Bit(lLeft, lRight);
	  else PlotWaveform_16Bit(lLeft, lRight);
}

// void CPlotWave::PlotWave()  	(form 2):
void CPlotWave::Plot(CDC *pdc, CRect &rect)
{
	SetDC(pdc);
	SetRect(rect);

	lLeft = 0;
	lRight = pWave->GetSampleNumber() - 1;

	if(Is8Bit) PlotWaveform_8Bit(lLeft, lRight);
	  else PlotWaveform_16Bit(lLeft, lRight);
}

// CPlotWave::PlotWaveform_8Bit() use to plot waveform stored with 8 bit.
void CPlotWave::PlotWaveform_8Bit(int lStartPoint, int lEndPoint)
{
	unsigned char *pwave, *p;
	int  ymax, xmax, xmax0, s_max, s_min, i, ijk;
	float  rxstep, ytran, one_unit, x;
	int  l;
	wchar_t zoomw[5] = {L"ZOOM"};
	// lock global memory:
	pwave = (unsigned char *)pWave->GetDataPtr();

	// Fill Rect according to Zooming status.
	if(lStartPoint != 0L || lEndPoint != pWave->GetSampleNumber() - 1L) {
		pDC->FillRect(ClientRect, &CBrush(COLOR_RGB(0, 255, 255)));
		//pDC->DrawText("ZOOM", -1, CRect(ClientRect.left+5, ClientRect.top+5, ClientRect.left+60, ClientRect.top+30), DT_LEFT);
		pDC->DrawText(zoomw, -1, CRect(ClientRect.left+5, ClientRect.top+5, ClientRect.left+60, ClientRect.top+30), DT_LEFT);
	}

	// get the width and length of screen:
	ymax = (ClientRect.bottom - ClientRect.top);
	xmax0 = (ClientRect.right - ClientRect.left);
	xmax = xmax0 - 2*EDGE;

	// define the pens and set mode and origin:
	CPen  pen(PS_SOLID, 1, COLOR_RGB(0, 0, 127));
	pDC->SelectObject(pen);
	pDC->SetViewportOrg(CPoint(EDGE, ymax));
	pDC->SetMapMode(MM_TEXT);

	// find the Max. Amplitude:
	s_min = SHRT_MAX;
	s_max = SHRT_MIN;
	p = pwave;
	for(l=lStartPoint; l<=lEndPoint; l++) {
		ijk = (int) p[l];
		s_min = min(s_min, ijk);
		s_max = max(s_max, ijk);
	}
	minAmp = s_min - 128;
	maxAmp = s_max - 128;

	// deside the ralation between sampling point and screen pixel:
	rxstep=(float)(lEndPoint-lStartPoint)/(float)(xmax-1);
	ytran=(float)ymax / 255.0;

	// main loop:
	p = pwave;
	if(rxstep > 1.0) {
		int  w_max, w_min;
		int  y_max, y_min;
		int  time1, time2;
		float  rtime1 = 0.0, rtime2;

		for(i=0; i<xmax; i++) {
			rtime2 = rtime1+rxstep;
			time1 = lStartPoint + (int)rtime1;
			time2 = lStartPoint + (int)rtime2;
			if(i == xmax-1) time2 = lEndPoint+1;
			rtime1 = rtime2;

			// find out the local minimun point and maximun point:
			w_max = 0;
			w_min = 255;
			for(l=time1; l<time2; l++) {
				w_max=max((int)pwave[l], w_max);
				w_min=min((int)pwave[l], w_min);
			}
			y_max = -(int)((float)w_max*ytran);
			y_min = -(int)((float)w_min*ytran);
			if(i==0) {
				pDC->MoveTo(i, y_max);
				pDC->LineTo(i, y_min);
			} else {
				pDC->LineTo(i, y_max);
				pDC->LineTo(i, y_min);
			}
		}
	} else {
		float	rtime=0.0;
		int	time, yy;

		rxstep = 1.0/rxstep;
		for(l=lStartPoint; l<=lEndPoint; l++) {
			time = (int)(rtime+0.5);
			if(time > xmax-1) time=xmax-1;
			rtime += rxstep;
			yy = -(int)((float)pwave[l]*ytran+0.5);
			if(l==lStartPoint) {
				pDC->MoveTo(time, yy);
			} else {
				pDC->LineTo(time, yy);
			}
		}
	}

	// plot the X-coordinate:
	CPen  pen0(PS_SOLID, 1, COLOR_RGB(255, 0, 0));
	pDC->SelectObject(pen0);
	ijk=ymax-(int)(128.0*ytran+0.5);
	pDC->SetViewportOrg(CPoint(EDGE, ijk));
	pDC->MoveTo( 0, 0);
	pDC->LineTo( xmax, 0);

	// plot the scale:
	one_unit = pWave->GetSamplingRate() / rxstep;
	one_unit *= 10000.0;
	while( one_unit > 100.0) one_unit /= 10.0;
	x=0.0;
	ijk=0;
	do {    						// print '1.0':
		ijk = (int)(x+0.5);
		pDC->MoveTo(ijk, -4);
		pDC->LineTo(ijk, 4);
		x += one_unit;
		ijk = (int)(x+0.5);
	} while(ijk<xmax);
	x = 0.0;
	ijk = 0;
	do {							// print '10.0':
		pDC->MoveTo( ijk, -6);
		pDC->LineTo( ijk, 6);
		x += 10.0*one_unit;
		ijk = (int)(x+0.5);
	} while(ijk<xmax);
	if(one_unit > 40.0) {
		x=0.0;
		ijk=0;                     	// print '0.5':
		do {
			ijk = (int)(x+0.5);
			pDC->MoveTo( ijk, -2);
			pDC->LineTo( ijk, 3);
			x += one_unit/2.0;
			ijk = (int)(x+0.5);
		} while(ijk<xmax);
	}
}

// CPlotWave::PlotWaveform_16Bit() use to plot waveform stored with 16 bit.
void CPlotWave::PlotWaveform_16Bit(int lStartPoint, int lEndPoint)
{
	__int16  *pwave, *p;
	int  ymax, xmax0, xmax, s_max, s_min, i, ijk;
	float  rxstep, ytran, one_unit, x;
	int  l;
	wchar_t zoomsw[5] = L"Zoom";
	// lock global memory:
	pwave = (__int16*)pWave->GetDataPtr();

	// Fill Rect according to Zooming status.
	if(lStartPoint != 0L || lEndPoint != pWave->GetSampleNumber() - 1L) {
		pDC->FillRect(ClientRect, &CBrush(COLOR_RGB(0, 255, 255)));
		//pDC->DrawText("ZOOM", -1, CRect(ClientRect.left+5, ClientRect.top+5, ClientRect.left+60, ClientRect.top+30), DT_LEFT);
		pDC->DrawText(zoomsw, -1, CRect(ClientRect.left+5, ClientRect.top+5, ClientRect.left+60, ClientRect.top+30), DT_LEFT);
		
	}

	// get the width and length of screen:
	ymax = ClientRect.bottom - ClientRect.top;
	xmax0 = ClientRect.right - ClientRect.left;
	xmax = xmax0 - 2*EDGE;

	// define the pens and set mode and origin:
	CPen  pen(PS_SOLID, 1, COLOR_RGB(0, 0, 127));
	pDC->SelectObject(pen);
	pDC->SetViewportOrg(CPoint(EDGE+ClientRect.left, ymax/2+ClientRect.top));
	pDC->SetMapMode(MM_TEXT);

	// find the Max. Amplitude:
	s_min = SHRT_MAX;
	s_max = SHRT_MIN;
	p = pwave;
	for(l=lStartPoint; l<=lEndPoint; l++) {
		s_min = min(s_min, p[l]);
		s_max = max(s_max, p[l]);
	}
	minAmp = s_min;
	maxAmp = s_max;
	if((abs(s_min) + abs(s_max)) == 0) s_max = 1;

	// deside the ralation between sampling point and screen pixel:
	rxstep = (float)(lEndPoint-lStartPoint)/(float)(xmax-1);
	ytran = 0.49*(float)ymax / (float)max(abs(s_max), abs(s_min));

	// main loop:
	p = (__int16 *)pwave;
	if(rxstep > 1.0) {
		int  w_max, w_min;
		int  y_max, y_min;
		int  time1, time2;
		float  rtime1=0.0, rtime2;

		for(i=0;i<xmax;i++) {
			rtime2 = rtime1+rxstep;
			time1 = lStartPoint+(int)(rtime1+0.5);
			time2 = lStartPoint+(int)(rtime2+0.5);
			if(i == xmax-1) time2 = lEndPoint+1;
			rtime1 = rtime2;

			// find out the local minimun point and maximun point:
			w_max=SHRT_MIN;
			w_min=SHRT_MAX;
			for(l=time1;l<time2;l++) {
				w_max=max(p[l],w_max);
				w_min=min(p[l],w_min);
			}
			y_max = -(int)((float)w_max*ytran + 0.5);
			y_min = -(int)((float)w_min*ytran + 0.5);
			if(i==0) {
				pDC->MoveTo(i, y_max);
				pDC->LineTo(i, y_min);
			} else {
				pDC->LineTo(i, y_max);
				pDC->LineTo(i, y_min);
			}
		}
	} else {
		float	rtime=0.0;
		int	time, yy;

		rxstep = 1.0/rxstep;
		for(l=lStartPoint; l<=lEndPoint; l++) {
			time=(int)(rtime+0.5);
			if(time > xmax-1) time = xmax-1;
			rtime += rxstep;
			yy = -(int)((float)p[l]*ytran+0.5);
			if(l==lStartPoint) {
				pDC->MoveTo(time, yy);
			} else {
				pDC->LineTo(time, yy);
			}
		}
	}

	// plot the X-coordinate:
	CPen  pen0(PS_SOLID, 1, COLOR_RGB(255, 0, 0));
	pDC->SelectObject(pen0);
	pDC->MoveTo(0, 0);
	pDC->LineTo(xmax, 0);

	// plot the scale:
	one_unit = pWave->GetSamplingRate() / rxstep;
	one_unit *= 10000.0;
	while(one_unit > 100.0) one_unit /= 10.0;
	x = 0.0;
	ijk = 0;
	do {    						// print '1.0':
		ijk = (int)(x+0.5);
		pDC->MoveTo(ijk, -4);
		pDC->LineTo(ijk, 4);
		x += one_unit;
		ijk = (int)(x+0.5);
	} while(ijk<xmax);
	x = 0.0;
	ijk = 0;
	do {							// print '10.0':
		pDC->MoveTo(ijk, -6);
		pDC->LineTo( ijk, 6);
		x += 10.0*one_unit;
		ijk = (int)(x+0.5);
	} while(ijk<xmax);
	if(one_unit > 40.0) {
		x = 0.0;
		ijk = 0;                     	// print '0.5':
		do {
			ijk = (int)(x+0.5);
			pDC->MoveTo(ijk, -2);
			pDC->LineTo(ijk, 3);
			x += one_unit/2.0;
			ijk = (int)(x+0.5);
		} while(ijk<xmax);
	}
}

// CPlotWave::PlotCursorLine() use to plot the cursor line.
/*void CPlotWave::PlotCursorLine(int XPos, CDC *pdc)
{
	if(pdc) pDC = pdc;

	int  oldrop2 = dc->GetROP2();
	int  ymax = (Rect.bottom - Rect.top);

	// select color and mode:
	CPen  pen = CPen(TColor(255, 0, 0)^pdc->GetBkColor() );
	pdc->SetViewportOrg(TPoint(0, 0));
	pdc->SelectObject(pen);
	pdc->SetROP2(R2_XORPEN);

	// plot:
	pdc->MoveTo(XPos, 0);
	pdc->LineTo(XPos, ymax);
	pdc->SetROP2(oldrop2);
}*/

// CPlotWave::PlotLabelLine() use to plot the label line.
/*void CPlotWave::PlotLabelLine(int  XPos, CDC *pdc)
{
	if(XPos < Rect.left - EDGE || XPos > Rect.right + EDGE)  return;

	if(!pdc) pDC = pdc;

	int  oldrop2 = dc->GetROP2();
	int  ymax = (Rect.bottom - Rect.top);

	// select color and mode:
	CPen  pen = CPen(TColor(255, 0, 0), 1, PS_DASH);
	pdc->SetViewportOrg(TPoint(0, 0));
	pdc->SelectObject(pen);
	pdc->SetROP2(R2_COPYPEN);

	// plot:
	pdc->MoveTo(XPos, 0);
	pdc->LineTo(XPos, ymax);
	pdc->SetROP2(oldrop2);
}*/

// CPlotWave::SamplePointToXPos() uses to transfer sample point to
//  x_position in pixel.
int  CPlotWave::SamplePointToXPos(int  lSamplePoint)
{
	float  temp;
	int  xpos;

	// convert SamplePoint to x_postion:
	temp = (float)(lSamplePoint - lLeft)/(float)(lRight - lLeft);
	xpos=(int)((float)(ClientRect.right - ClientRect.left - 2*EDGE - 1)*temp+0.5) + EDGE;
//	if(xpos < EDGE) xpos = EDGE;
//	if(xpos > Rect.right - EDGE - 1) xpos = Rect.right - EDGE - 1;

	return  xpos;
}

// CPlotWave::XPosToSamplePoint() uses to transfer x_position in pixel to
//   SamplePoint.
int  CPlotWave::XPosToSamplePoint(int XPos)
{
	float  temp;
	int  l;
	int  xpos;

	// convert x_postion to sample point:
	xpos = XPos - ClientRect.left - EDGE;
	temp = (float)xpos / (float)(ClientRect.right - ClientRect.left - 2*EDGE - 1);
	l = lLeft + (int)(temp * (lRight - lLeft) + 0.5);
	if( l < lLeft ) l = lLeft;
	if( l > lRight) l = lRight;

	return  l;
}