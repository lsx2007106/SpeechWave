//---------------------------------------------------------------------
//
// Filename: Wave2000.cpp		By Yang Jian	1995.10.05
//
// The all member function of class CWave are defined in this file.
//

#include "stdafx.h"

#include  "PCMWave.h"

// Construct function form 1, 缺省构造函数
CWave::CWave()
{
	SetInitValue();
}
	
// Construct function form 2
// 基于一个波形文件的构造函数，该波形文件将被打开、读入
CWave::CWave(const char *wavefilename)
{
	SetInitValue();
	Open(wavefilename);
}

// Construct function form 3
// 基于另一个CWave对象的构造函数，复制整个波形
CWave::CWave(CWave &xwave)
{
	SetInitValue();
	Copy(xwave);
}

// Construct function form 4
// 基于另一个CWave对象的构造函数，仅复制lbegin(采样点)到lend(采样点)的一段波形
CWave::CWave(CWave &xwave, int lbegin, int lend)
{
	SetInitValue();
	Copy(xwave, lbegin, lend);
}

// 基于一个已调入内存的波形数据（头和数据），构造CWave对象
void CWave::Construct(char *pw, int wavesize)
{
	delete pWave;
	pWave = pw;
	nWaveSize = wavesize;
	UpdateMapping();
}

// void CWave::SetInitValue(), 初始化变量，由构造函数调用
void CWave::SetInitValue()
{
	WaveFileName = new char[256];
	*WaveFileName = '\0';
	pRiffHdr = 0;
	pFmtChunk = 0;
	pFactChunk = 0;
	pDataHdrChunk = 0;
	nWaveSize = 0;
	pWave = 0;
	pData = 0;
	IsOk = 1;
}

// CWave::~CWave()	Deconstruct function.
CWave::~CWave()
{
	if(pWave) delete[] pWave;
	if(WaveFileName) delete[] WaveFileName;
}

// 从另一个CWave对象复制lbegin(采样点)到lend(采样点)的一段波形,
// 复制时，包括前(lbegin)后(lend)端点
void CWave::Copy(CWave &xwave, int lbegin, int lend)
{
	if(!(&xwave)) {
		ErrorMessage("Copy a Null Wave !!");
		return;
	}
	if(!xwave) {
		ErrorMessage("Copy a Null Wave !!");
		return;
	}
	
	int lb = min(lbegin, lend);
	int le = max(lbegin, lend);

	if(lb<0 || le >= xwave.GetSampleNumber()) {
		ErrorMessage("The copying segment out of range.");
		IsOk = 0;
		return;
	}
	
	Copy(xwave);
	if(le != GetSampleNumber()-1) Delete(le+1, GetSampleNumber()-1);
	if(lb != 0) Delete(0, lb-1);
}

//从另一个CWave对象复制整个波形，为this->pWave重新分配空间，
// 将复制的结果存入this->pWave，调整所有参数，并复制波形文件名。
void CWave::Copy(CWave &xwave) 
{
	if(!(&xwave)) {
		ErrorMessage("Copy a Null Wave !!");
		return;
	}
	if(!xwave) {
		ErrorMessage("Copy a Null Wave !!");
		return;
	}

	if(pWave) delete[] pWave;
	nWaveSize = xwave.GetWaveSize();
	pWave = new char [nWaveSize];
	
	int  l;
	char  *p = xwave.GetWavePtr();
	for(l=0; l < nWaveSize; l++) pWave[l] = p[l];
	IsOk = 1;
	UpdateMapping();

	char  wavefilename[256];
	xwave.GetWaveFileName(wavefilename);
	SetWaveFileName(wavefilename);
}

// void CWave::ErrorMessage() uses to print error message.
void  CWave::ErrorMessage(const char *text, const char *title)
{
	wchar_t textw[1024];
	wchar_t titlew[1024];
	size_t newsize = strlen(text) +1;
	size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, textw, newsize, text, _TRUNCATE);
	newsize = strlen(title) +1;
    mbstowcs_s(&convertedChars, titlew, newsize, title, _TRUNCATE);
	if(title)
		::MessageBox(NULL, textw, titlew, MB_OK);
	else
		::MessageBox(NULL, textw, L"CWave", MB_OK);
}

// CWave::Open() use to load wave format and data from a file.
// 打开一个文件，判断是否为波形文件。若是，则读入，然后从新构
// 建CWave对象。该函数同时支持新旧Wave格式。
void  CWave::Open(const char* wavefilename)
{
	// handle to open input WAVE file
	HMMIO	hmmio_in = NULL;	
	
	// chunk info. for input RIFF, fmt, fact and data chunk 
	MMCKINFO	ckin_riff, ckin_fmt;
	
	// Open the input file
	try
	{
		char temp_name[256];
		wchar_t temp1_name[256];
		strcpy(temp_name, wavefilename);
		size_t convertedChars = 0;
		size_t newsize = strlen(temp_name) +1;
		mbstowcs_s(&convertedChars, temp1_name, newsize, temp_name, _TRUNCATE);
		hmmio_in = mmioOpen(temp1_name, NULL, MMIO_READ);
		if (hmmio_in == NULL) 
			throw "Cannot open WAVE file.";

		// Descend the input file into the 'RIFF' chunk.
		if (mmioDescend(hmmio_in, &ckin_riff, NULL, 0) != MMSYSERR_NOERROR)
			throw "Error, when reading MMCKINFO.";

		// Make sure the input file is a WAVE file.
		if ((ckin_riff.ckid != FOURCC_RIFF) ||
			(ckin_riff.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
				throw "It's not a Wave file.";
	
		// Search the input file for the 'fmt ' chunk.
		ckin_fmt.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if (mmioDescend(hmmio_in, &ckin_fmt, &ckin_riff, 
							MMIO_FINDCHUNK) != MMSYSERR_NOERROR )
			throw "It's not a PCM waveform";
		
		// Expect the 'fmt' chunk to be as large as a PCMWAVEFORMAT;
		if (ckin_fmt.cksize < sizeof(PCMWAVEFORMAT))
			throw "The size of 'fmt ' chunk too small.";

		// Read the 'fmt ' chunk into pcm_wave_fmt.
		PCMWAVEFORMAT  pcm_wave_fmt;
		if (mmioRead(hmmio_in, (char *)&pcm_wave_fmt, 
					sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
			throw "Truncated file, probably.";

		// Make sure the input file is a PCM WAVE file.
		if (pcm_wave_fmt.wf.wFormatTag != WAVE_FORMAT_PCM)
			throw "Bad PCM Wave file";

		// Make sure the input file is a mono PCM WAVE file.
		if (pcm_wave_fmt.wf.nChannels != 1)
			throw "Not MONO channel wave.";

		// 确保文件为单声道的PCM波形文件后，将文件的所有数据读入pWave所指的内存块中
		if(pWave) delete[] pWave;
		pWave = 0;
		nWaveSize = ckin_riff.cksize + WAVE_HDR_LENGTH_MAX;
		pWave = new char [nWaveSize];

		mmioSeek(hmmio_in, 0, SEEK_SET);
		nWaveSize = mmioRead(hmmio_in, pWave, nWaveSize);
		
		// mapping chunk point to wave memory (pWave[]) 
		if(!UpdateMapping()) 
			throw "Bad Wave format.";

		mmioClose(hmmio_in, 0);
		IsOk = 1;
		SetWaveFileName(wavefilename);
	}

	catch(char *exception_text)
	{
		if (hmmio_in != NULL) mmioClose(hmmio_in, 0);
		if (pWave) delete pWave;
		pWave = 0;
		IsOk = 0;
		ErrorMessage(exception_text, "Open Wave Error");
	}
}

// 用当前文件名(存于WaveFileName中)存波形，若文件名空，存为“noname.wav”
void CWave::Save()
{
	char new_name[256];
	new_name[0] = '\0';

	if(strlen(WaveFileName) > 0) {
		strcpy(new_name, WaveFileName);
	} else {
		if (::MessageBox(NULL, L"Wave File Name is NULL. Save as noname.wav?", 
										L"CWave", MB_YESNO) == IDYES )
			strcpy(new_name, "noname.wav");
	}

	SaveAs(new_name);
}

// 重新指定文件名存波形。若存储有效，用wavefilename的值更新当前文件名(即WaveFileName)
void CWave::SaveAs(const char *wavefilename)
{
	if(wavefilename) if(wavefilename[0]) {
		HMMIO	hmmio_test = NULL;
		char  temp_name[256];
		wchar_t temp1_name[256];
		size_t convertedChars = 0;
		strcpy(temp_name, wavefilename); 
		size_t newsize = strlen(temp_name) +1;
		mbstowcs_s(&convertedChars, temp1_name, newsize, temp_name, _TRUNCATE);
		hmmio_test = mmioOpen(temp1_name, NULL, MMIO_EXIST);
		if (hmmio_test != NULL) {
			mmioClose(hmmio_test, 0);
			if (::MessageBox(NULL, L"Wave file is existed. Overwrite it?", 
											L"CWave", MB_YESNO) == IDYES) {
				Write(wavefilename);
			}
		} else {
			Write(wavefilename);
		}
	} else ErrorMessage("File name is NULL.");
	else ErrorMessage("File name is NULL.");
}

//指定一个新的文件名，存波形文件。若成功，更新当前文件名
void CWave::Write(const char *wavefilename)
{
	if(!wavefilename) return;
	if(!(*wavefilename)) return;

	// handle to open output WAVE file
	HMMIO	hmmio_out = NULL;	
	
	// write wave file
	try
	{
		char  temp_name[256];
		strcpy(temp_name, wavefilename);
		wchar_t temp1_name[256];
		size_t convertedChars = 0;
		size_t newsize = strlen(temp_name)+1;
		mbstowcs_s(&convertedChars, temp1_name, newsize, temp_name, _TRUNCATE);
		hmmio_out = mmioOpen(temp1_name, NULL, MMIO_WRITE | MMIO_CREATE);
		if (hmmio_out == NULL) 
			throw "Cannot create a new WAVE file.";

		if (mmioWrite(hmmio_out, pWave, nWaveSize) != nWaveSize)
			throw "Error, when write wave data.";

		mmioFlush(hmmio_out, MMIO_EMPTYBUF);
		mmioClose(hmmio_out, 0);
		SetWaveFileName(wavefilename);
	}

	catch(char *exception_text)
	{
		if (hmmio_out != NULL) {
			mmioFlush(hmmio_out, MMIO_EMPTYBUF);
			mmioClose(hmmio_out, 0);
		}
		ErrorMessage(exception_text, "Write Wave File Error");
	}
}

// delete the segment within [lbegin, lend] from this wave.
// 删除从lbegin(采样点)到lend(采样点) (包括两个端点)的一段波形
// 参数要求：0 <= lbegin, lend < GetSampleNumber()
void CWave::Delete(int lbegin, int lend)
{
	int  lb = min(lbegin, lend);
	int  le = max(lbegin, lend);
	if (lb < 0 || le >= GetSampleNumber()) return;

	int  delpoint = le - lb + 1; 
	int  delbyte = delpoint * (GetBitsPerSample() == 8 ? 1 : 2);
	int  newdatasize = GetDataSize() - delbyte;
	int  newwavesize = nWaveSize - delbyte;
	char  *pbuffer = new char [newwavesize]; 
	
	// copy the wave header
	int  l;
	for(l=0; l<GetHdrSize(); l++) pbuffer[l] = pWave[l];

	// copy the segment which from 0 to lb-1 (sample)
	if(GetBitsPerSample() == 8) {
		char  *ps = pData;
		char  *pt = pbuffer + GetHdrSize();

		for(l=0; l<lb; l++) pt[l] = ps[l];
	} else {
		short  *ps = (short *)pData;
		short  *pt = (short *)(pbuffer + GetHdrSize());

		for(l=0; l<lb; l++) pt[l] = ps[l];
	}

	// copy the segment which from le (sample) to the-end-byte
	int lss = (le + 1) * (GetBitsPerSample()==8 ? 1 : 2);
	int lee = nWaveSize - GetHdrSize();
	char  *ps = pData;
	char  *pt = pbuffer + GetHdrSize();
	
	for(l=lss; l<lee; l++) pt[l-delbyte] = ps[l];

	// update pointers value
	delete[] pWave;
	pWave = pbuffer;
	nWaveSize = newwavesize;
	UpdateMapping();

	// adjust wave-size, data-size, sample-number
	pRiffHdr->size -= delbyte;
	pDataHdrChunk->size -= delbyte;
	if (pFactChunk) {
		pFactChunk->sample_number -= delpoint;
	}
}

// inserts a another wave to this wave at insert point (lpoint).
// 在lpoint(采样点)前，插入另一个CWave对象
// 参数要求：0 <= lpoint < GetSampleNumber()
void CWave::Insert(CWave *pw_a, int lpoint)
{
	if(!(&pw_a)) {
		ErrorMessage("Insert a Null Wave !!");
		return;
	}
	if(!pw_a) {
		ErrorMessage("Insert a Null Wave !!");
		return;
	}

	// check if these two waves have same format
	if (GetBitsPerSample() != pw_a->GetBitsPerSample()
			|| GetSamplingRate() != pw_a->GetSamplingRate()
			|| GetHdrSize() != pw_a->GetHdrSize()
			|| GetChannelNumber() != pw_a->GetChannelNumber() ) {
		::MessageBox(NULL, L"Wave format mismatch", L"CWave", MB_OK);
		return;
	}
	if (lpoint < 0 || lpoint > GetSampleNumber()) return;

	int lbyte, newdatasize, newwavesize, addbyte; 
	
	addbyte = pw_a->GetDataSize();
	lbyte = lpoint * (GetBitsPerSample() == 8 ? 1 : 2);
	newdatasize = GetDataSize() + addbyte;
	newwavesize = nWaveSize + addbyte;
	
	char  *pbuffer = new char [newwavesize]; 
	
	// copy the wave header from pWave[] to pbuffer[] 
	int  l;
	for(l=0; l<GetHdrSize(); l++) pbuffer[l] = pWave[l];

	// copy the segment which from 0 to lbyte-1
	char  *ps = pData;
	char  *pt = pbuffer + GetHdrSize();

	for(l=0; l<lbyte; l++) pt[l] = ps[l];

	// append whole data of another wave to pbuffer[] 
	ps = pw_a->GetDataPtr();
	pt = pbuffer + GetHdrSize() + lbyte;

	for(l=0; l < pw_a->GetDataSize(); l++) pt[l] = ps[l];

	// move the segment which lbyte to the-end 
	ps = pData;
	pt = pbuffer + GetHdrSize() + pw_a->GetDataSize();

	for(l=lbyte; l < GetDataSize(); l++) pt[l] = ps[l];

	// update pointers value
	delete[] pWave;
	pWave = pbuffer;
	nWaveSize = newwavesize;
	UpdateMapping();

	// adjust wave-size, data-size, sample-number
	pRiffHdr->size += pw_a->GetDataSize();
	pDataHdrChunk->size += pw_a->GetDataSize();
	if(pFactChunk) {
		pFactChunk->sample_number += pw_a->GetSampleNumber();
	}
}

// 设置波形数据的字节数。当有效数据点数比预期的少时，用该函数调整数据点数。
// 该函数没有从新分配存放波形的内存块。
// 输入参数要求： datasizeinbytes < GetDataSize(), 否则无效。
void CWave::SetDataSize(int datasizeinbytes)
{
	if(datasizeinbytes < GetDataSize() && datasizeinbytes > 0) {
		// 首先计算改变量，然后调整文件头中与波形数据字节数相关位的值
		int decreasedbytes = GetDataSize() - datasizeinbytes;
		pRiffHdr->size -= decreasedbytes;
		pDataHdrChunk->size -= decreasedbytes;
		if (pFactChunk) {
			int decreasedsamples = decreasedbytes / (GetBitsPerSample()==8 ? 1 : 2);
			pFactChunk->sample_number -= decreasedsamples;
		}
		nWaveSize -= decreasedbytes;
	}
}

// 重新设置比特数、采样率、采样点数，并分配存储空间
void CWave::ResetWave(int bits, int sampling_rate, int sample_number)
{
	int  hdr_size = SIZEOF_riff_hdr + SIZEOF_fmt + SIZEOF_fact + SIZEOF_data_hdr;
	int  data_size = sample_number * (bits <= 8 ? 1 : 2);

	// 重新分配存储空间
	if (pWave) delete[] pWave; 
	pWave = new char [data_size + hdr_size];

	int  riff_offset = 0;
	int  fmt_offset = riff_offset + SIZEOF_riff_hdr;
	int  fact_offset = fmt_offset + SIZEOF_fmt;
	int  data_offset = fact_offset + SIZEOF_fact;

	TRiffHdr  *p_riff = (TRiffHdr *)(pWave + riff_offset);
	p_riff->id = mmioFOURCC('R', 'I', 'F', 'F');
	p_riff->size = data_size + hdr_size - sizeof(FOURCC) - sizeof(DWORD);	
	p_riff->wave_id = mmioFOURCC('W', 'A', 'V', 'E');

	TFmtChunk *p_fmt = (TFmtChunk *)(pWave + fmt_offset);
	p_fmt->id = mmioFOURCC('f', 'm', 't', ' ');
	p_fmt->size = SIZEOF_fmt - sizeof(FOURCC) - sizeof(DWORD);
	p_fmt->wFormatTag = WAVE_FORMAT_PCM;
	p_fmt->nChannels = 1;
	p_fmt->nSamplesPerSec = sampling_rate;
	p_fmt->nAvgBytesPerSec  = sampling_rate * (bits <= 8 ? 1:2);
	p_fmt->nBlockAlign = (bits <= 8 ? 1 : 2);
	p_fmt->wBitsPerSample = bits;
	p_fmt->cbSize = 0;   // PCM 格式忽略该量
	
	TFactChunk *p_fact = (TFactChunk *)(pWave + fact_offset);  
	p_fact->id = mmioFOURCC('f', 'a', 'c', 't');
	p_fact->size = sizeof(DWORD);
	p_fact->sample_number = sample_number;

	TDataHdrChunk *p_data = (TDataHdrChunk *)(pWave + data_offset); 
	p_data->id = mmioFOURCC('d', 'a', 't', 'a');
	p_data->size = data_size;

	nWaveSize = data_size + hdr_size;

	UpdateMapping();
}

BOOL CWave::FindFOURCC(char *keyword, char *&p)
{
	int i;

	for(i=0; i<WAVE_HDR_LENGTH_MAX; i++) {
		if(p[i] == keyword[0] && p[i+1] == keyword[1] 
		   && p[i+2] == keyword[2] && p[i+3] == keyword[3]) {
			p += i;
			return TRUE;
		}
		if(p[i] == 'd' && p[i+1] == 'a' && p[i+2] == 't' && p[i+3] == 'a') {
			return FALSE;
		}
	}
	return  FALSE;
}

// when, pWave changed, update the mapping relation between pRiffHdr, 
// pFmtChunk, pFactChunk, pDataHdrChunk and pWave.
// Wave文件由文件头和波形数据组成，所有波形参数均可在文件头中找到。
// 把波形参数定义为指针，然后指向相应的位置，是个好方法。问题是，
// 当存储波形的地址(即pWave的值)改变时，必须重新调整与之关联的指针值。
// 该函数的目的在于此。
// 该函数不调整波形(文件)指针：pWave，不调整波形(文件)长度： nWaveSize。
BOOL CWave::UpdateMapping()
{
	// 按块顺序，将块指针分别指向相应的位置
	char  *p;
	
	// 定位Riff块
	p = pWave;
	if(FindFOURCC("RIFF", p)) {
		pRiffHdr = (TRiffHdr *) p;
	} else {
		return FALSE;
	}

	// 定位Fmt块
	p = pWave;
	if(FindFOURCC("fmt ", p)) {
		pFmtChunk = (TFmtChunk *) p;
	} else {
		return FALSE;
	}

	// 定位data块
	p = pWave;
	if(FindFOURCC("data", p)) {
		pDataHdrChunk = (TDataHdrChunk *) p;
		pData = (char *)pDataHdrChunk + sizeof(TDataHdrChunk);
	} else {
		return FALSE;
	}

	// 定位fact块
	p = pWave;
	if(FindFOURCC("fact", p)) {
		pFactChunk = (TFactChunk *)p;
	} else {
		pFactChunk = 0;
	}

	return TRUE;
}