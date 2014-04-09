
// SpeechWaveView.h : CSpeechWaveView ��Ľӿ�
//

#pragma once

#include "resource.h"
#include "CRecordWaveParam.h"
#include "PCMWave.h"
#include "PlotWave.h"
#include "CSpectrogramParam.h"
#include "Spectrogram.h"
#include "Response.h"
#include "FreqResponse.h"
#include "DrawCurve.h"
#include "PhaseResponse.h"

class CSpeechWaveView : public CFormView
{
protected: // �������л�����
	CSpeechWaveView();
	DECLARE_DYNCREATE(CSpeechWaveView)

public:
	enum{ IDD = IDD_SPEECHWAVE_FORM };

// ����
public:
	CSpeechWaveDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CSpeechWaveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnInputRecord();
	afx_msg void OnInputPlay();
protected:
	afx_msg LRESULT EvRecordDone(WPARAM wParam, LPARAM lParam);
public:
	HWAVEOUT hWaveOut;
	HWAVEIN hWaveIn;
	WAVEHDR waveHdr;
	BOOL IsDeviceUsing;
	CWave* m_pWave;
	CRecordWaveParam g_RecordWaveParam;
	virtual void OnDraw(CDC* /*pDC*/);
	afx_msg void OnWaveform();
	bool m_IsWaveForm;
	afx_msg void OnSpectrogram();
	bool m_RecordIsReady;
	afx_msg void OnMenu();
	FreqResponse* m_pFreqResponse;
	Response* m_pResponse;
//	DrawCurve* m_DrawCurve;
	PhaseResponse* m_pPhaseResponse;
//	afx_msg void OnFileOpen();
	afx_msg void OnFileOpen();
};

#ifndef _DEBUG  // SpeechWaveView.cpp �еĵ��԰汾
inline CSpeechWaveDoc* CSpeechWaveView::GetDocument() const
   { return reinterpret_cast<CSpeechWaveDoc*>(m_pDocument); }
#endif

