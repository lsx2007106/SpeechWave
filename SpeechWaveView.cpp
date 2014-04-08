
// SpeechWaveView.cpp : CSpeechWaveView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "SpeechWave.h"
#endif

#include "SpeechWaveDoc.h"
#include "SpeechWaveView.h"
#pragma comment(lib,"winmm.lib")
#include  <mmsystem.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpeechWaveView

IMPLEMENT_DYNCREATE(CSpeechWaveView, CFormView)

BEGIN_MESSAGE_MAP(CSpeechWaveView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_INPUT_RECORD, &CSpeechWaveView::OnInputRecord)
	ON_COMMAND(ID_INPUT_PLAY, &CSpeechWaveView::OnInputPlay)
//	ON_MESSAGE(MM_WOM_DONE, EvPlayDone)
//	ON_MESSAGE(MM_WIM_DATA, EvRecordDone)
ON_MESSAGE(MM_WIM_DATA, &CSpeechWaveView::EvRecordDone)
ON_COMMAND(ID_WAVEFORM, &CSpeechWaveView::OnWaveform)
ON_COMMAND(ID_SPECTROGRAM, &CSpeechWaveView::OnSpectrogram)
ON_COMMAND(ID_Menu, &CSpeechWaveView::OnMenu)
END_MESSAGE_MAP()

// CSpeechWaveView 构造/析构

CSpeechWaveView::CSpeechWaveView()
	: CFormView(CSpeechWaveView::IDD)
{
	// TODO: 在此处添加构造代码
	m_pWave = 0;
	hWaveOut = 0;
	hWaveIn = 0;
	IsDeviceUsing = false;
	m_RecordIsReady = false;
	m_pResponse = 0;
}

CSpeechWaveView::~CSpeechWaveView()
{
}

void CSpeechWaveView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CSpeechWaveView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);
}

void CSpeechWaveView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}

void CSpeechWaveView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSpeechWaveView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSpeechWaveView 诊断

#ifdef _DEBUG
void CSpeechWaveView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSpeechWaveView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSpeechWaveDoc* CSpeechWaveView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSpeechWaveDoc)));
	return (CSpeechWaveDoc*)m_pDocument;
}
#endif //_DEBUG


// CSpeechWaveView 消息处理程序


void CSpeechWaveView::OnInputRecord()
{
	// TODO: 在此添加命令处理程序代码
	if(IsDeviceUsing)
	{
		PostMessage(WM_COMMAND, ID_INPUT_RECORD);
		return;
	}
	int returnvalue;
	try
	{
		if(m_pWave) delete m_pWave;
		m_pWave = 0;
		if(waveOutGetNumDevs() == 0)
			throw "Doesn't find sound device.";

		int n_sample_number = 
			g_RecordWaveParam.GetnDuration()*g_RecordWaveParam.GetnRate();
		
		m_pWave = new CWave;
		m_pWave->ResetWave(g_RecordWaveParam.GetnBit(), 
			g_RecordWaveParam.GetnRate(), n_sample_number);

		char *pw = m_pWave->GetWavePtr();

		// open sound device
		returnvalue = waveInOpen(&hWaveIn, WAVE_MAPPER, 
				(WAVEFORMATEX *)m_pWave->GetWaveFmtPtr(), 
					(unsigned long)m_hWnd,	0, CALLBACK_WINDOW); 
		if (returnvalue != MMSYSERR_NOERROR) 
		  if (returnvalue == WAVERR_BADFORMAT)
		    throw "Attempted to open with an unsupported waveform-audio format.";
		       else throw "Could not open sound device.";

		// prepare header and start input sound.
		waveHdr.lpData = m_pWave->GetDataPtr();
		waveHdr.dwBufferLength = n_sample_number * (g_RecordWaveParam.GetnBit() <= 8 ? 1 : 2);
		waveHdr.dwBytesRecorded = 0;
		waveHdr.dwUser = 0;
		waveHdr.dwFlags = 0;
		waveHdr.dwLoops = 0;
		waveHdr.lpNext = 0;

		if (waveInPrepareHeader(hWaveIn, &waveHdr, 
						sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			throw  "Could not run waveOutPrepareHeader for record.";
			
		if (waveInAddBuffer(hWaveIn, &waveHdr, 
						sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			throw  "Could not run waveInAddBuffer.";
		
		if (waveInStart(hWaveIn) != MMSYSERR_NOERROR) 
			throw  "Could not waveInStart.";
	
		IsDeviceUsing = 1;

		// 启动定时器
		//int n_ms = 1000 * g_RecordWaveParam.GetnDuration(); // nTimeProgressStep;
		//if(!SetTimer(1, n_ms, NULL))
		//	throw  "Could not start Timer.";
		m_RecordIsReady = true;
		//// 使提前终止录音按钮获取输入焦点
		//::SetFocus(m_buttonStopRecord.m_hWnd);
	}
	catch (char *text) {
		if (hWaveIn) {
			waveInStop(hWaveIn);
			waveInUnprepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
			waveInClose(hWaveIn);
		}
		hWaveIn = 0;
		delete m_pWave;
		m_pWave = 0;
		//MyMessageBox(text);
		wchar_t textw[256];
		size_t newsize;
		newsize = strlen(text) +1;
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, textw, newsize, text, _TRUNCATE);
		MessageBox(textw);
	}
}



void CSpeechWaveView::OnInputPlay()
{
	// TODO: 在此添加命令处理程序代码
	//if(!m_pWaveIn) return;
	if(!m_RecordIsReady) return;
	if(!(*m_pWave)) return;
	
	if(IsDeviceUsing) {
		PostMessage(WM_COMMAND, ID_INPUT_PLAY);
		return;
	}

	// 回放全部波形 
	try {
		if(waveOutGetNumDevs() == 0)
			throw "Doesn't find sound device.";

		char *p_data = m_pWave->GetDataPtr();
		int  len_data = m_pWave->GetDataSize();

		 //open sound device.
		if(waveOutOpen(&hWaveOut, WAVE_MAPPER, 
						(WAVEFORMATEX *)m_pWave->GetWaveFmtPtr(), 
						(unsigned long)m_hWnd,	0, 
						CALLBACK_WINDOW | WAVE_ALLOWSYNC ) 
						!= MMSYSERR_NOERROR)
			throw "Could not open sound device.";
	
		waveHdr.lpData = p_data;
		waveHdr.dwBufferLength = len_data;
		waveHdr.dwBytesRecorded = 0;
		waveHdr.dwUser = 0;
		waveHdr.dwFlags = 0;
		waveHdr.dwLoops = 0;
		waveHdr.lpNext = 0;
		waveHdr.reserved = 0;
		
		if(waveOutPrepareHeader(hWaveOut, &waveHdr, 
					sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			throw "Could not do waveOutPrepareHeader for play.";

		if(waveOutWrite(hWaveOut, &waveHdr, 
					sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			throw "Could not do waveOutwrite.";

		//IsDeviceUsing = 1;
		IsDeviceUsing = false;
	}

	catch(char *text) {
		if(hWaveOut) {
			waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(WAVEHDR));
			waveOutClose(hWaveOut);
		}
		//MyMessageBox(text);
		wchar_t textw[256];
		size_t newsize = strlen(text) +1;
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, textw, newsize, text, _TRUNCATE);
		MessageBox(textw);
	}

}


afx_msg LRESULT CSpeechWaveView::EvRecordDone(WPARAM wParam, LPARAM lParam)
{
	if(IsDeviceUsing && hWaveIn)
	{
		waveInStop(hWaveIn);
		waveInUnprepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
		waveInClose(hWaveIn);
		if(waveHdr.dwBytesRecorded != waveHdr.dwBufferLength)
			m_pWave->SetDataSize(waveHdr.dwBytesRecorded);
	}
	else 
		return -1;
	m_pWave ->Write("1.wav");
	IsDeviceUsing = false;
	CRect inputRect;
	this ->GetDlgItem(IDC_STATIC_INPUT) ->GetWindowRect(inputRect);
	ScreenToClient(inputRect);
	this ->InvalidateRect(inputRect,true);
	UpdateWindow();
	
	//Invalidate(true);
	return 0;
}


void CSpeechWaveView::OnDraw(CDC* pDC)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(m_pWave)
	{
		CRect inputrect;
		this ->GetDlgItem(IDC_STATIC_INPUT) ->GetWindowRect(&inputrect);
		ScreenToClient(inputrect);
		if(m_IsWaveForm)
		{
			CPlotWave pplotwave(m_pWave);
			pplotwave.Plot(pDC,inputrect);
		}
		else
		{
			CSpectrogram spectro(m_pWave); 
			spectro.Display(pDC,inputrect);
		}
	}
	if(m_pResponse)
	{
		CRect FreqRect;
		CRect PhaseRect;
		this ->GetDlgItem(IDC_STATIC_PHASE) ->GetWindowRect(PhaseRect);
		this ->GetDlgItem(IDC_STATIC_MAGNITUDE) ->GetWindowRect(FreqRect);
		ScreenToClient(FreqRect);
		ScreenToClient(PhaseRect);
		m_pResponse ->DrawCoordinate(pDC,FreqRect);
		m_pResponse ->DrawResponseCurve(pDC,FreqRect);
		m_pResponse ->DrawCoordinate(pDC,FreqRect);
		m_pResponse ->DrawResponseCurve(pDC,FreqRect);
	}
}


void CSpeechWaveView::OnWaveform()
{
	// TODO: 在此添加命令处理程序代码
	m_IsWaveForm = true;
	CRect inputRect;
	this ->GetDlgItem(IDC_STATIC_INPUT) ->GetWindowRect(inputRect);
	ScreenToClient(inputRect);
	this ->InvalidateRect(inputRect,true);
	UpdateWindow();
}


void CSpeechWaveView::OnSpectrogram()
{
	// TODO: 在此添加命令处理程序代码
	m_IsWaveForm = false;
	CRect inputRect;
	this ->GetDlgItem(IDC_STATIC_INPUT) ->GetWindowRect(inputRect);
	ScreenToClient(inputRect);
	this ->InvalidateRect(inputRect,true);
	UpdateWindow();
}


void CSpeechWaveView::OnMenu()
{
	// TODO: 在此添加命令处理程序代码
	if(m_pResponse) delete m_pResponse;
		m_pResponse = 0;
	CFile f;
	f.Open(L"parameter.txt",CFile::modeRead);
	unsigned filelength;
	filelength = f.GetLength();
	char* buffer = new char[filelength +1];
	*(buffer + filelength) = 0;
	f.Read(buffer,filelength);
	f.Close();
	CRect FreqRect;
	CRect PhaseRect;
	this ->GetDlgItem(IDC_STATIC_MAGNITUDE) ->GetWindowRect(FreqRect);
	ScreenToClient(FreqRect);
	m_pResponse = new Response(buffer,FreqRect.right - FreqRect.left - 75); //Xaxis width 75px
	CClientDC dc(this);
	this ->GetDlgItem(IDC_STATIC_MAGNITUDE) ->GetWindowRect(FreqRect);
	this ->GetDlgItem(IDC_STATIC_PHASE) ->GetWindowRect(PhaseRect);
	ScreenToClient(FreqRect);
	ScreenToClient(PhaseRect);
	m_pResponse ->FrequencyResponse();
	m_pResponse ->PhaseResponse();
	this ->InvalidateRect(FreqRect,true);
	this ->InvalidateRect(PhaseRect,true);
	UpdateWindow();
	//UpdateWindow();
	delete buffer;
}
