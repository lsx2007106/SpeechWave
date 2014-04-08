#pragma once
class Response
{
private:
	float* pRe;
	float* pIm;
	float* qRe;
	float* qIm;
	int size;
	CRect Rect;
	char YAxisLabel[5][8];
public:
	Response(void); //Add some comment
	~Response(void);
};

