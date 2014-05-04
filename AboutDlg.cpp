#include "stdafx.h"
#include "AboutDlg.h"

//-------------------------------------------------------------------------
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

//-------------------------------------------------------------------------
BOOL CAboutDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  CString s;

  s.Format("PartitionViewer Version %s", PV_VERSION);
  GetDlgItem(IDC_STATIC_ABOUT1)->SetWindowText(s);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------------
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
