class CRecordWaveParam 
{
public:
	CRecordWaveParam();
	~CRecordWaveParam();
	void Set(int rate, int bit, int duration, int dcvalue);
	void Get(int &rate, int &bit, int &duration, int &dcvalue);
	int GetnRate();
	int GetnBit();
	int GetnDuration();
	int GetnDCvalue();
private:
	int		nRate;			// sampling rate in Hz.
	int		nBit;			// number bits per sample.
	int		nDuration;   	// duration in second.
	int		nDCvalue;		// Direct Current value.
};