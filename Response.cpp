#include "StdAfx.h"
#include "Response.h"


Response::Response(void)
{
}
inline void Response::SetInit(float* p,int size)
{
	for(int i = 0; i < size; ++i)
		*(p+i) = 0.0;
}
inline void Response::CopyInit(float* p, float* q, int size)
{
	for(int i = 0; i < size; ++i)
		*(p + i) = * (q + i);
}
float* Response::GetpIm()
{
	return this ->pIm;
}
float* Response::GetpRe()
{
	return this ->pRe;
}
float* Response::GetqRe()
{
	return  this ->qRe;
}
float* Response::GetqIm()
{
	return this ->qIm;
}
int Response::GetSize()
{
	return this ->size;
}
void Response::SetComplexPQ(char* buffer,int PixelX)
{
	char* tmp;
	char* header;
	float* p;
	float* q;
	int psize = 0;
	int qsize = 0;
	int offset = 0;
	int i = 0;
	int power = 0;
	int sizeoffft = 1;
	int PixelXBackup = PixelX;
	tmp = buffer;
	tmp++;
	while(*tmp != 0x0A)
		tmp++;
	*tmp = 0;
	psize = atoi(buffer+1);
	p = new float[psize];
	tmp++;
	for(i = 0; i < psize; ++i)
	{
		header = tmp;
		while(*tmp != 0x0A)
			tmp++;
		*tmp = 0;
		*(p + i) = atof(header);
		tmp++;
	}
	tmp++;
	while(*tmp != 0x0A)
	{
		offset++;
		tmp++;
	}
	*tmp = 0;
	qsize = atoi(tmp - offset);
	q = new float[qsize];
	tmp++;
	for(i = 0; i < qsize; ++i)
	{
		header = tmp;
		while(*tmp != 0x0A)
			tmp++;
		*tmp = 0;
		*(q + i) = atof(header);
		tmp++;
	}
	while(PixelX > 1)
	{
		PixelX >>= 1;
		power++;
	}
	while(power != 0)
	{
		sizeoffft <<= 1;
		power--;
	}
	if(sizeoffft == PixelXBackup)
		sizeoffft <<= 1; //判断是否是的整数次幂。
	else
	{
		sizeoffft <<= 1;
		sizeoffft <<= 1;  //fft 的点数要多一倍。
	}
	this ->size = sizeoffft;
	this ->pRe = new float[sizeoffft];
	this ->pIm = new float[sizeoffft];
	this ->qRe = new float[sizeoffft];
	this ->qIm = new float[sizeoffft];
	//this ->responsedata = new float[sizeoffft];
	SetInit(this ->pRe,sizeoffft);
	SetInit(this ->pIm,sizeoffft);
	SetInit(this ->qRe,sizeoffft);
	SetInit(this ->qIm,sizeoffft);
	//SetInit(this ->responsedata,sizeoffft);
	CopyInit(this ->pRe,p,psize);
	CopyInit(this ->qRe,q,qsize);
	delete p;
	delete q;
	/*wcscpy(this ->XAxisLabel[0], L"0");
	wcscpy(this ->XAxisLabel[1], L"0.25Pi");
	wcscpy(this ->XAxisLabel[2], L"0.5Pi");
	wcscpy(this ->XAxisLabel[3], L"0.75Pi");
	wcscpy(this ->XAxisLabel[4], L"  Pi");
	wcscpy(this ->YAxisLabel[0], L"\0");
	wcscpy(this ->YAxisLabel[1], L"\0");
	wcscpy(this ->YAxisLabel[2], L"\0");
	wcscpy(this ->YAxisLabel[3], L"\0");
	wcscpy(this ->YAxisLabel[4], L"\0");*/
	FFTTransform();
}
Response::Response(char* buffer, int PixelX)
{
	SetComplexPQ(buffer,PixelX);
}
void Response::FFTTransform()
{
	TFft transp(this ->size);
	TFft transq(this ->size);
	transp.Transform(pRe,pIm);
	transq.Transform(qRe,qIm);
}
Response::~Response(void)
{
	delete pRe;
	delete pIm;
	delete qRe;
	delete qIm;
}
