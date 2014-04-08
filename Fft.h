#if  !defined (__fft_h)
#define  __fft_h

//
// FileName: fft.h			By YANG Jian		1995.11.25
//


class  TFft {
  public:
	TFft(int npoint);
	~TFft();
	inline int  operator ! () { return  !IsOk; };
	void  Transform(float *xr, float *xi);
	void  InverseTransform(float *xr, float *xi);
	void  Tran2RealSequence(float *x1, float *x2, float *mag1, float *mag2, int nfreq);
	inline void Tran2RealSequence(float *x1, float *x2, float *mag1, float *mag2)
	{
		Tran2RealSequence(x1, x2, mag1, mag2, nPoint2);
	};

  private:
	float  *fwCos, *fwSin;
	int  *iTable;
	int  nPoint, nPoint2, nM;
	int  IsOk;

  private:
	void  SetInit();
	int  PowOf2(int i);
};
#endif
