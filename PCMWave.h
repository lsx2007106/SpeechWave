#ifndef  __pcm_wave_h
#define  __pcm_wave_h

//
// FileName: Wave2000.h	 By YANG Jian 1995.10.04, 1999.11.20, 2000.3.22, 2000.10.30, 2014.4.1
// 
/*-------------------------------------------------------------------------
Wave文件分新旧两种格式说明：
	新格式：(a)在头文件中包含一个标识字为"fact"的块(Chunk)；
			(b)fmt块中，用PCMWAVEFORMATEX结构；
			(c)用于WinNT,Win98
	旧格式：(a)头文件中没有包含fact块(Chunk);
			(b)fmt块中，用PCMWAVEFORMAT结构；
			(c)用于win31/win32

    当波形数据来自一个文件时，CWave同时兼容新旧两种格式；当用录音的方法产
生波形时，直接设为新格式。曾发现部分老声卡、驱动程序，在win98下，对新格式
不兼容，不能录音的实例。
--------------------------------------------------------------------------*/

#include  <mmsystem.h>   // 多媒体头文件，必须包含！！
#pragma hdrstop
#pragma comment(lib, "winmm.lib")  // 多媒体库文件，必须链接！！

#define  WAVE_HDR_LENGTH_MAX	128

/*---------------------Wave文件格式------------------------------
riff-Chunk: (12Bytes)
	FOURCC  RIFF_ID;	等于"RIFF"
	DWORD   size;		后续内容的字节数
	FOURCC  wave;		等于"WAVE"

fmt-Chunk: (26Byte)
	FOURCC  fmt_ID;		等于"fmt "
	DWORD   fmt_size;   本块中后续内容的字节数,即 sizeof(PCMWAVEFORMAT) + sizeof(WORD)
	WORD  wFormatTag;
	WORD  nChannels;
	DWORD nSamplesPerSec; 
    DWORD nAvgBytesPerSec;
	WORD  nBlockAlign;
	WORD  wBitsPerSample; 
	WORD  cbSize;		指明用于非PCM格式的附加字节数，PCM忽略该量，旧格式无此量

fact-Chunk: (12Byte)    本块从Win NT, Win98中开始引入, 在Win3.1/Win3.2中没有该块
	FOURCC  fact_id;	等于"fact"
	DWORD   fact_size;	即 sizeof(DWORD)
	DWORD   sample_number;  采样点数

data-Chunk: (8Byte)
	FOURCC	data_id;	等于"data"
	DWORD	data_size;	波形数据的字节数，即 sizeof(数据块)
	数据块;				波形数据

	附加数据(可选);		本部分不计入data_size中，但计入size
--------------------“Wave文件格式” 结束--------------------*/

// 根据Wave文件格式，以下分别定义fmt块，fact块，riff块头和data块头。
// 在下列定义中确认每个struct的sizeof值为8Byte, 不满足时填充数组。
typedef struct riff_hdr_tag {
	FOURCC		id;
	DWORD		size;
	FOURCC		wave_id;
	char		null[4];
} TRiffHdr;  // 12 + 4 Byte
#define			SIZEOF_riff_hdr		12

typedef struct fmt_chunk_tag {
	FOURCC		id;
	DWORD		size;
	WORD		wFormatTag;
	WORD		nChannels;
	DWORD		nSamplesPerSec; 
    DWORD		nAvgBytesPerSec;
	WORD		nBlockAlign;
	WORD		wBitsPerSample; 
	WORD		cbSize;
	char		null[6];
} TFmtChunk;  // 26 + 6 Byte
#define			SIZEOF_fmt		26

typedef struct fact_chunk_tag {
	FOURCC		id;
	DWORD		size;
	DWORD		sample_number;
	char		null[4];
} TFactChunk; // 12 + 4 Byte
#define			SIZEOF_fact		12

typedef struct data_hdr_chunk_tag {
	FOURCC		id;
	DWORD		size;
} TDataHdrChunk; // 8 Byte
#define			SIZEOF_data_hdr		8

//------"定义fmt块，fact块，riff块头和data块头" 结束---------

// declare CWave class
class CWave {

public:
	// 缺省构造函数
	CWave();

	// 基于一个波形文件的构造函数，该波形文件将被打开、读入
	CWave(const char *filename);
	
	// 基于另一个CWave对象的构造函数，复制整个波形
	CWave(CWave &xwave);
	
	// 基于另一个CWave对象的构造函数，仅复制lbegin(采样点)到lend(采样点)的一段波形
	CWave(CWave &xwave, int lbegin, int lend);

	~CWave();

	// public operator
	inline int operator ! () { return  ! (IsOk && pWave); };

	// 用等号(=)实现拷贝对象
	CWave& operator = (CWave &xwave) {
		Copy(xwave);
		return *this;
	};

	// 基于一个已调入内存的波形数据（头和数据），构造CWave对象
	void Construct(char *pw, int wavesize);

	// 返回波形指针，该指针指向Wave文件在内存中的首地址
	inline char* GetWavePtr() const { return pWave; };  
	
	// 返回波形数据指针
	inline char* GetDataPtr() const { return pData; };  

	// 返回波形字节数，包括波形格式(Wave文件头)和波形数据
	inline int GetWaveSize() const { return nWaveSize; };
	
	 // 返回波形格式(Wave文件头)的字节数
	inline int GetHdrSize() const { return (int)(pData - pWave); };

	// 返回波形数据的字节数
	inline int GetDataSize() const { return pDataHdrChunk->size; };
	
	// 设置波形数据的字节数。当有效数据点数比预期的少时，用该函数调整数据点数。
	// 该函数没有从新分配存放波形的内存块。
	// 输入参数要求： datasizeinbytes < GetDataSize(), 否则无效。
	void SetDataSize(int datasizeinbytes);

	// 返回采样点数
	inline int GetSampleNumber() const {
		return GetDataSize() / (GetBitsPerSample() == 8 ? 1 : 2);
	};

	// 返回每采样点的比特数，8 或 16
	inline int GetBitsPerSample() const {
		return pFmtChunk ? pFmtChunk->wBitsPerSample : 0;
	};

	// 返回采样频率
	inline int GetSamplingRate() const {
		return pFmtChunk ? pFmtChunk->nSamplesPerSec : 0 ;
	};
	
	// 返回声道数，单声道为1, 立体声为 2, 本类仅处理单声道的声音。
	inline int GetChannelNumber() const {
		return pFmtChunk ? pFmtChunk->nChannels : 0;
	};

	// 返回 PCMWAVEFORMAT 部分的指针
	inline char* GetWaveFmtPtr() {
		return (char*)pFmtChunk + 8;
	};

	// 读取波形文件名
	void GetWaveFileName(char *wavefilename) {
		strcpy(wavefilename, WaveFileName);
	};

	// 设置波形文件名
	void SetWaveFileName(const char *wavefilename) {
		strcpy(WaveFileName, wavefilename);
	};

	// 重新设置比特数、采样率、采样点数，并分配存储空间
	void ResetWave(int bits, int  sampling_rate, int sample_number);

	//从另一个CWave对象复制lbegin(采样点)到lend(采样点)(包括两个端点)的一段波形
	void  Copy(CWave &xwave, int lbegin, int lend);

	//从另一个CWave对象复制整个波形
	void  Copy(CWave &xwave);

	// 删除从lbegin(采样点)到lend(采样点) (包括两个端点)的一段波形
	// 参数要求：0 <= lbegin, lend < GetSampleNumber()
	void Delete(int lbegin, int lend);
	
	// 在lpoint(采样点)前，插入另一个CWave对象
	// 参数要求：0 <= lpoint < GetSampleNumber()
	void Insert(CWave *pw_a, int lpoint);

	// 打开并读入一个波形文件，然后从新构建CWave对象
	void Open(const char *wavefilename);

	//指定一个新的文件名，存波形文件。若成功，更新当前文件名
	void  Write(const char *wavefilename); 

	// 用当前文件名(存于WaveFileName中)存波形，若文件名空，存为“noname.wav”
	void Save();

	// 重新指定文件名存波形。若存储有效，用wavefilename的值更新当前文件名(即WaveFileName)
	void SaveAs(const char *wavefilename);
	
	// 单音节端点检测
	BOOL DetectEnds_S(int &ls, int &le, char *text);

	// 多音节端点检测
	BOOL DetectEnds_M(int &ls, int &le, char *text);

private:
	char  *WaveFileName;	//指向当前文件名
	TRiffHdr  *pRiffHdr;	//指向RIFF块头的首地址
	TFmtChunk  *pFmtChunk;	//指向Fmt块头的首地址
	TFactChunk  *pFactChunk;		//指向RIFF块头的首地址
	TDataHdrChunk  *pDataHdrChunk;  //指向Data块头的首地址
	int  nWaveSize;			//波形字节数，包括波形格式(Wave文件头)和波形数据
	char  *pWave, *pData;	//分别指向波形(格式和数据)的首地址，波形数据的首地址
	int   IsOk;				//若IsOk==0，表示对象无效或出错

	//初始化变量，由构造函数调用
	void  SetInitValue();	
	
	//当存储波形的地址(即pWave的值)改变时，重新调整与之关联的指针值
	BOOL UpdateMapping();  
	BOOL CWave::FindFOURCC(char *keyword, char* &p);
	
	//用对话框输出信息
	void  ErrorMessage(const char *text, const char *title = 0);
};

//class CRecordWaveParam {
//public:
//	CRecordWaveParam () {
//		nRate = 16000;	// 16kHz
//		nBit = 16;		// 16bit
//		nDuration = 2;  // 10 秒
//		nDCvalue = 0;
//	};
//
//	void Set(int rate, int bit, int duration, int dcvalue) {
//		nRate = rate;
//		nBit = bit;
//		nDuration = duration;
//		nDCvalue = dcvalue;
//	};
//	void Get(int &rate, int &bit, int &duration, int &dcvalue) {
//		rate = nRate;
//		bit = nBit;
//		duration = nDuration;
//		dcvalue = nDCvalue;
//	};
//	int GetnRate() { return nRate; };
//	int GetnBit() { return nBit; };
//	int GetnDuration() { return nDuration; };
//	int GetnDCvalue() { return nDCvalue; };
//
//private:
//	int		nRate;			// sampling rate in Hz.
//	int		nBit;			// number bits per sample.
//	int		nDuration;   	// duration in second.
//	int		nDCvalue;		// Direct Current value.
//};

#endif // __pcm_wave_h