#ifndef  __pcm_wave_h
#define  __pcm_wave_h

//
// FileName: Wave2000.h	 By YANG Jian 1995.10.04, 1999.11.20, 2000.3.22, 2000.10.30, 2014.4.1
// 
/*-------------------------------------------------------------------------
Wave�ļ����¾����ָ�ʽ˵����
	�¸�ʽ��(a)��ͷ�ļ��а���һ����ʶ��Ϊ"fact"�Ŀ�(Chunk)��
			(b)fmt���У���PCMWAVEFORMATEX�ṹ��
			(c)����WinNT,Win98
	�ɸ�ʽ��(a)ͷ�ļ���û�а���fact��(Chunk);
			(b)fmt���У���PCMWAVEFORMAT�ṹ��
			(c)����win31/win32

    ��������������һ���ļ�ʱ��CWaveͬʱ�����¾����ָ�ʽ������¼���ķ�����
������ʱ��ֱ����Ϊ�¸�ʽ�������ֲ���������������������win98�£����¸�ʽ
�����ݣ�����¼����ʵ����
--------------------------------------------------------------------------*/

#include  <mmsystem.h>   // ��ý��ͷ�ļ��������������
#pragma hdrstop
#pragma comment(lib, "winmm.lib")  // ��ý����ļ����������ӣ���

#define  WAVE_HDR_LENGTH_MAX	128

/*---------------------Wave�ļ���ʽ------------------------------
riff-Chunk: (12Bytes)
	FOURCC  RIFF_ID;	����"RIFF"
	DWORD   size;		�������ݵ��ֽ���
	FOURCC  wave;		����"WAVE"

fmt-Chunk: (26Byte)
	FOURCC  fmt_ID;		����"fmt "
	DWORD   fmt_size;   �����к������ݵ��ֽ���,�� sizeof(PCMWAVEFORMAT) + sizeof(WORD)
	WORD  wFormatTag;
	WORD  nChannels;
	DWORD nSamplesPerSec; 
    DWORD nAvgBytesPerSec;
	WORD  nBlockAlign;
	WORD  wBitsPerSample; 
	WORD  cbSize;		ָ�����ڷ�PCM��ʽ�ĸ����ֽ�����PCM���Ը������ɸ�ʽ�޴���

fact-Chunk: (12Byte)    �����Win NT, Win98�п�ʼ����, ��Win3.1/Win3.2��û�иÿ�
	FOURCC  fact_id;	����"fact"
	DWORD   fact_size;	�� sizeof(DWORD)
	DWORD   sample_number;  ��������

data-Chunk: (8Byte)
	FOURCC	data_id;	����"data"
	DWORD	data_size;	�������ݵ��ֽ������� sizeof(���ݿ�)
	���ݿ�;				��������

	��������(��ѡ);		�����ֲ�����data_size�У�������size
--------------------��Wave�ļ���ʽ�� ����--------------------*/

// ����Wave�ļ���ʽ�����·ֱ���fmt�飬fact�飬riff��ͷ��data��ͷ��
// �����ж�����ȷ��ÿ��struct��sizeofֵΪ8Byte, ������ʱ������顣
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

//------"����fmt�飬fact�飬riff��ͷ��data��ͷ" ����---------

// declare CWave class
class CWave {

public:
	// ȱʡ���캯��
	CWave();

	// ����һ�������ļ��Ĺ��캯�����ò����ļ������򿪡�����
	CWave(const char *filename);
	
	// ������һ��CWave����Ĺ��캯����������������
	CWave(CWave &xwave);
	
	// ������һ��CWave����Ĺ��캯����������lbegin(������)��lend(������)��һ�β���
	CWave(CWave &xwave, int lbegin, int lend);

	~CWave();

	// public operator
	inline int operator ! () { return  ! (IsOk && pWave); };

	// �õȺ�(=)ʵ�ֿ�������
	CWave& operator = (CWave &xwave) {
		Copy(xwave);
		return *this;
	};

	// ����һ���ѵ����ڴ�Ĳ������ݣ�ͷ�����ݣ�������CWave����
	void Construct(char *pw, int wavesize);

	// ���ز���ָ�룬��ָ��ָ��Wave�ļ����ڴ��е��׵�ַ
	inline char* GetWavePtr() const { return pWave; };  
	
	// ���ز�������ָ��
	inline char* GetDataPtr() const { return pData; };  

	// ���ز����ֽ������������θ�ʽ(Wave�ļ�ͷ)�Ͳ�������
	inline int GetWaveSize() const { return nWaveSize; };
	
	 // ���ز��θ�ʽ(Wave�ļ�ͷ)���ֽ���
	inline int GetHdrSize() const { return (int)(pData - pWave); };

	// ���ز������ݵ��ֽ���
	inline int GetDataSize() const { return pDataHdrChunk->size; };
	
	// ���ò������ݵ��ֽ���������Ч���ݵ�����Ԥ�ڵ���ʱ���øú����������ݵ�����
	// �ú���û�д��·����Ų��ε��ڴ�顣
	// �������Ҫ�� datasizeinbytes < GetDataSize(), ������Ч��
	void SetDataSize(int datasizeinbytes);

	// ���ز�������
	inline int GetSampleNumber() const {
		return GetDataSize() / (GetBitsPerSample() == 8 ? 1 : 2);
	};

	// ����ÿ������ı�������8 �� 16
	inline int GetBitsPerSample() const {
		return pFmtChunk ? pFmtChunk->wBitsPerSample : 0;
	};

	// ���ز���Ƶ��
	inline int GetSamplingRate() const {
		return pFmtChunk ? pFmtChunk->nSamplesPerSec : 0 ;
	};
	
	// ������������������Ϊ1, ������Ϊ 2, ���������������������
	inline int GetChannelNumber() const {
		return pFmtChunk ? pFmtChunk->nChannels : 0;
	};

	// ���� PCMWAVEFORMAT ���ֵ�ָ��
	inline char* GetWaveFmtPtr() {
		return (char*)pFmtChunk + 8;
	};

	// ��ȡ�����ļ���
	void GetWaveFileName(char *wavefilename) {
		strcpy(wavefilename, WaveFileName);
	};

	// ���ò����ļ���
	void SetWaveFileName(const char *wavefilename) {
		strcpy(WaveFileName, wavefilename);
	};

	// �������ñ������������ʡ�����������������洢�ռ�
	void ResetWave(int bits, int  sampling_rate, int sample_number);

	//����һ��CWave������lbegin(������)��lend(������)(���������˵�)��һ�β���
	void  Copy(CWave &xwave, int lbegin, int lend);

	//����һ��CWave��������������
	void  Copy(CWave &xwave);

	// ɾ����lbegin(������)��lend(������) (���������˵�)��һ�β���
	// ����Ҫ��0 <= lbegin, lend < GetSampleNumber()
	void Delete(int lbegin, int lend);
	
	// ��lpoint(������)ǰ��������һ��CWave����
	// ����Ҫ��0 <= lpoint < GetSampleNumber()
	void Insert(CWave *pw_a, int lpoint);

	// �򿪲�����һ�������ļ���Ȼ����¹���CWave����
	void Open(const char *wavefilename);

	//ָ��һ���µ��ļ������沨���ļ������ɹ������µ�ǰ�ļ���
	void  Write(const char *wavefilename); 

	// �õ�ǰ�ļ���(����WaveFileName��)�沨�Σ����ļ����գ���Ϊ��noname.wav��
	void Save();

	// ����ָ���ļ����沨�Ρ����洢��Ч����wavefilename��ֵ���µ�ǰ�ļ���(��WaveFileName)
	void SaveAs(const char *wavefilename);
	
	// �����ڶ˵���
	BOOL DetectEnds_S(int &ls, int &le, char *text);

	// �����ڶ˵���
	BOOL DetectEnds_M(int &ls, int &le, char *text);

private:
	char  *WaveFileName;	//ָ��ǰ�ļ���
	TRiffHdr  *pRiffHdr;	//ָ��RIFF��ͷ���׵�ַ
	TFmtChunk  *pFmtChunk;	//ָ��Fmt��ͷ���׵�ַ
	TFactChunk  *pFactChunk;		//ָ��RIFF��ͷ���׵�ַ
	TDataHdrChunk  *pDataHdrChunk;  //ָ��Data��ͷ���׵�ַ
	int  nWaveSize;			//�����ֽ������������θ�ʽ(Wave�ļ�ͷ)�Ͳ�������
	char  *pWave, *pData;	//�ֱ�ָ����(��ʽ������)���׵�ַ���������ݵ��׵�ַ
	int   IsOk;				//��IsOk==0����ʾ������Ч�����

	//��ʼ���������ɹ��캯������
	void  SetInitValue();	
	
	//���洢���εĵ�ַ(��pWave��ֵ)�ı�ʱ�����µ�����֮������ָ��ֵ
	BOOL UpdateMapping();  
	BOOL CWave::FindFOURCC(char *keyword, char* &p);
	
	//�öԻ��������Ϣ
	void  ErrorMessage(const char *text, const char *title = 0);
};

//class CRecordWaveParam {
//public:
//	CRecordWaveParam () {
//		nRate = 16000;	// 16kHz
//		nBit = 16;		// 16bit
//		nDuration = 2;  // 10 ��
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