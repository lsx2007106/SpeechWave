
// SpeechWaveView.h : CSpeechWaveView 类的接口
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
protected: // 仅从序列化创建
	CSpeechWaveView();
	DECLARE_DYNCREATE(CSpeechWaveView)

public:
	enum{ IDD = IDD_SPEECHWAVE_FORM };

// 特性
public:
	CSpeechWaveDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CSpeechWaveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
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

#ifndef _DEBUG  // SpeechWaveView.cpp 中的调试版本
inline CSpeechWaveDoc* CSpeechWaveView::GetDocument() const
   { return reinterpret_cast<CSpeechWaveDoc*>(m_pDocument); }
#endif

