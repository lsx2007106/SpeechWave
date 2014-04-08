//
//	FileName: fft.cpp	By YANG Jian	1995.1.25
//
#include "stdafx.h"
//#include "VS.h"

#include  <math.h>
#include  "fft.h"

// construct function of class TFft.
TFft::TFft(int npoint)
{
	IsOk = 0;
	nM = (int)(log((double)npoint) / log(2.0) + 0.5);

	nPoint = npoint;
	nPoint2 = nPoint / 2;
	fwCos = new float [nPoint2];
	fwSin = new float [nPoint2];
	iTable = new int [nPoint];
	if(fwCos && fwSin && iTable && (PowOf2(nM)==nPoint)) {
		SetInit();
		IsOk = 1;
	}
}

// deconstruct function of class TFft.
TFft::~TFft()
{
	delete[] iTable;
	delete[] fwSin;
	delete[] fwCos;
}

// SetInit() uses to compute iTable[], fwCos[], fwSin[] and fHamming[].
void  TFft::SetInit()
{
	int i, j, k;

	// compute coefficients.
	double  pi = 4.0*atan(1.0);
	double  temp = 2.0 * pi / (float)nPoint;
	for(i=0; i<nPoint2; i++) {
		fwCos[i] = cos(i*temp);
		fwSin[i] = sin(i*temp);
	}

	// construct bit reversal table.
	j = 0;
	iTable[0] = 0;
	iTable[nPoint-1] = nPoint - 1 ;
	for(i=1; i<nPoint-1; i++) {
		k = nPoint / 2;
		while( k<= j) {
			j -= k;
			k /= 2;
		}
		j += k;
		iTable[i] = j;
	}
}

// Powof2() uses to compute 2^i.
int  TFft::PowOf2(int i)
{
	int  j, k;
	k = 1;
	for(j=0; j<i; j++) k *= 2;
	return  k;
}

// Function Transform() compute FFT.
void  TFft::Transform(float *xr, float *xi)
{
	int  l, i, j, m, le, le1, ip;
	int  unit, iwt;
	float  tr, ti;

	// rearragne array xr[], xi[].
	for(i=1; i<nPoint-1; i++) {
		j = iTable[i];
		if(i<j) {
			tr = xr[i];
			ti = xi[i];
			xr[i] = xr[j];
			xi[i] = xi[j];
			xr[j] = tr;
			xi[j] = ti;
		}
	}

	// compute.
	m = nM;
	for(l=1; l<=m; l++) {
		le = PowOf2(l);
		le1 = le / 2;
		unit = nPoint / le;
		iwt = 0;
		for(j=0; j<le1; j++) {
			for(i=j; i<nPoint; i += le) {
				ip = i + le1;
				tr = xr[ip]*fwCos[iwt] + xi[ip]*fwSin[iwt];
				ti = xi[ip]*fwCos[iwt] - xr[ip]*fwSin[iwt];
				xr[ip] = xr[i] - tr;
				xi[ip] = xi[i] - ti;
				xr[i] = xr[i] + tr;
				xi[i] = xi[i] + ti;
			}
			iwt += unit;
		}
	}
}

// Function InverseTransform() compute inverse FFT.
void  TFft::InverseTransform(float *xr, float *xi)
{
	int  l, i, j, m, le, le1, ip;
	int  unit, iwt;
	float  tr, ti;

	// rearragne array xr[], xi[].
	for(i=1; i<nPoint-1; i++) {
		j = iTable[i];
		if(i<j) {
			tr = xr[i];
			ti = xi[i];
			xr[i] = xr[j];
			xi[i] = xi[j];
			xr[j] = tr;
			xi[j] = ti;
		}
	}

	// compute.
	m = nM;
	for(l=1; l<=m; l++) {
		le = PowOf2(l);
		le1 = le / 2;
		unit = nPoint / le;
		iwt = 0;
		for(j=0; j<le1; j++) {
			for(i=j; i<nPoint; i += le) {
				ip = i + le1;
				tr = xr[ip]*fwCos[iwt] - xi[ip]*fwSin[iwt];
				ti = xi[ip]*fwCos[iwt] + xr[ip]*fwSin[iwt];
				xr[ip] = xr[i] - tr;
				xi[ip] = xi[i] - ti;
				xr[i] = xr[i] + tr;
				xi[i] = xi[i] + ti;
			}
			iwt += unit;
		}
	}

	tr = 1.0 / (float)nPoint;
	for(i=0; i<nPoint; i++) {
		xr[i] = tr * xr[i];
		xi[i] = tr * xi[i];
	}
}

// Function Tran2RealSequence() compute two real sequence with one FFT.
// Defaultly, nfreq = nPoint / 2.
void  TFft::Tran2RealSequence(float  *x1,
								float  *x2,
								float  *mag1,
								float  *mag2,
								int  nfreq )
{
	float  x1r, x1i, x2r, x2i;
	int  j, jj, jpoint;

	// compute DFT with FFT.
	Transform(x1, x2);

	// compute power spectrum.
	jpoint = (nfreq <= nPoint) ? nfreq : nPoint;
	mag1[0] = x1[0]*x1[0];
	mag2[0] = x2[0]*x2[0];
	for(j=1; j<jpoint; j++) {
		jj = nPoint - j;
		x1r = x1[j] + x1[jj];
		x1i = x2[j] - x2[jj];
		x2r = x2[j] + x2[jj];
		x2i = x1[jj] - x1[j];
		mag1[j] = 0.25*(x1r*x1r + x1i*x1i);
		mag2[j] = 0.25*(x2r*x2r + x2i*x2i);
	}
}

