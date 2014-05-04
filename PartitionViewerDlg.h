// PartitionViewerDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "partitionmanager.h"
#include "myhistorybar.h"

#define MY_WM_BARCLICK (WM_USER+1)
#define MY_WM_BARMOUSEMOVE (WM_USER+2)

// CPartitionViewerDlg dialog
class CPartitionViewerDlg : public CDialog
{
// Construction
public:
	CPartitionViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PARTITIONVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnBnClickedOk();
  afx_msg LRESULT OnBarClick(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnBarMouseMove(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
  void InitDrawing();
  void InitDrivesList();

private:
  CComboBox m_cbDrivesList;
  CMyDrawBar m_drawBar;
  CMyHistoryBar m_historyBar;

  MYDRAWBARPARAMETERS m_barparams;
  MYHISTORYBARPARAMETERS m_historyparams;

  std::vector<MYDRAWBARSTRUCT> m_partitions;
  CPartitionManager m_pm;
public:
  afx_msg void OnBnClickedBtnRepresent();
  afx_msg void OnBnClickedBtnCancel();
public:
  afx_msg void OnCbnSelchangeCbPdrivelist();
};
