class  CSpectrogramParam 
{
public:
	CSpectrogramParam ();
	void Set(float nanalysiswindow, int nfft, int nwidthinpixel, int dbrange, int ngrey);
	int GetnFft();
	float GetnAnalysisWindow();
	int GetdbRange();
	int GetnGrey();
	int GetnWidthInPixel();
private:
	int  nFft;				// the number point of FFT.
	float  nAnalysisWindow; // the length analysis window in ms.
	int  dbRange;   		// the dB range.
	int  nGrey;     		// number of grey.
	int  nWidthInPixel;		// number of points in time.
};