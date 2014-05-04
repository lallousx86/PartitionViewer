/* ----------------------------------------------------------------------------- 
 * Copyright (c) Elias Bachaalany <lallousz-x86@yahoo.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ----------------------------------------------------------------------------- */

#include "stdafx.h"
#include "PartitionViewer.h"
#include ".\Aboutdlg.h"
#include "PartitionViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------
static const char PV_VERSION [] = "1.0.0";

//---------------------------------------------------------------------------
CPartitionViewerDlg::CPartitionViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartitionViewerDlg::IDD, pParent), m_drawBar(&m_barparams), m_historyBar(&m_historyparams)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

//---------------------------------------------------------------------------
void CPartitionViewerDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_DRAWBAR, m_drawBar);
  DDX_Control(pDX, IDC_CB_PDRIVELIST, m_cbDrivesList);
  DDX_Control(pDX, IDC_HISTORYBAR, m_historyBar);
}

//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CPartitionViewerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_MESSAGE(MY_WM_BARCLICK, OnBarClick)
  ON_MESSAGE(MY_WM_BARMOUSEMOVE, OnBarMouseMove)
  ON_BN_CLICKED(IDC_BTN_REPRESENT, OnBnClickedBtnRepresent)
  ON_BN_CLICKED(IDC_BTN_CANCEL, OnBnClickedBtnCancel)
  //}}AFX_MSG_MAP
  ON_CBN_SELCHANGE(IDC_CB_PDRIVELIST, &CPartitionViewerDlg::OnCbnSelchangeCbPdrivelist)
END_MESSAGE_MAP()


//-------------------------------------------------------------------------
// CPartitionViewerDlg message handlers
void CPartitionViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
  {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else
  {
    CDialog::OnSysCommand(nID, lParam);
  }
}

//---------------------------------------------------------------------------
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CPartitionViewerDlg::OnPaint() 
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialog::OnPaint();
  }
}

//---------------------------------------------------------------------------
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPartitionViewerDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

//-------------------------------------------------------------------------
BOOL CPartitionViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitDrivesList();

  InitDrawing();
	
  if (::GetVersion() >= 0x80000000)
  {
    MessageBox("This program can only run on Windows NT", "Error", MB_ICONERROR | MB_OK);
    m_cbDrivesList.EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_REPRESENT)->EnableWindow(FALSE);
  }
  else
  {
    if (m_cbDrivesList.GetCount())
      OnBnClickedBtnRepresent();
  }
	return TRUE;  // return TRUE  unless you set the focus to a control
}

//-------------------------------------------------------------------------
/*!
\brief Uses a primitive method to detect all physical drives on the system
*/
void CPartitionViewerDlg::InitDrivesList()
{
  m_cbDrivesList.ResetContent();

  CHAR name[MAX_PATH];

  for (size_t i=0;i<32;i++)
  {
    sprintf(name, "\\\\.\\PhysicalDrive%d", i);

    HANDLE h = ::CreateFile(name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);

    if (h == INVALID_HANDLE_VALUE)
      continue;
    ::CloseHandle(h);

    CString s;
    s.Format("Physical Disk %d", i);

    int idx = m_cbDrivesList.AddString(s);
    m_cbDrivesList.SetItemData(idx, i);
  }

  if (m_cbDrivesList.GetCount())
  {
    m_cbDrivesList.SetCurSel(0);
  }
}

//---------------------------------------------------------------------------
void CPartitionViewerDlg::OnBnClickedOk()
{
  CDialog::OnOK();
}

//---------------------------------------------------------------------------
/*!
\brief This is the HISTORYBAR information used by the "MyHistoryBar" control
We use this table to display partition info and color (in "legend" form)
*/
static const MYHISTORYBARTYPECOLOR part_colors[] = 
{
  {0x0,  RGB(255,255,255), "Free space"},
  {0x6,  RGB(100,10,0), "FAT16B (>= 32 MB)"},
  {0xC,  RGB(255,0,128), "Win95 FAT32 (LBA)"},
  {0xE,  RGB(0,110,110), "Win95 FAT16 (LBA)"},
  {0x7,  RGB(200,0,20), "Installable File System (NTFS, HPFS)"},
  {0x0B, RGB(10,100,0), "Win95 FAT32"},
  {0x83, RGB(20,200,0), "Linux native file system (Ext2/3)"},
  {0x17, RGB(10,0,100), "Hidden IFS (HPFS/NTFS)"},
  {0x82, RGB(101, 12, 130), "Linux swap / Solaris"},
  {0x1C, RGB(90, 127, 30), "Hidden Win95 FAT32 (LBA)"},
  {0x1E, RGB(0, 0, 255), "Hidden Win95 FAT16 (LBA)"},
  {0x04, RGB(101, 12, 130), "FAT16 <32MB"}
};

//---------------------------------------------------------------------------
/*!
\brief Given a partition type, it returns the color.
It searches in the "part_colors" table
*/
bool GetColorFromType(
  const MYHISTORYBARTYPECOLOR *cm, 
  size_t cnt, 
  int type, 
  COLORREF &ret)
{
  for (size_t i=0;i<cnt;i++)
  {
    if (cm[i].type == type)
    {
      ret = cm[i].color;
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------------------
/*!
  Handles the draw bar click message
*/
LRESULT CPartitionViewerDlg::OnBarClick(WPARAM wParam, LPARAM lParam)
{
  CString s;

  if (wParam < 0 || wParam >= m_pm.partlist.size())
    return 0;

  partition_info &pi = m_pm.partlist[wParam];

  s.Format("%d - %s - %d", wParam, pi.sys_name, pi.sys_id);
  SetWindowText(s);
  return 0;
}

//---------------------------------------------------------------------------
/*!
\brief Handles the mouse position movements over the drawing bar
*/
LRESULT CPartitionViewerDlg::OnBarMouseMove(WPARAM wParam, LPARAM lParam)
{
    if (wParam < 0 || wParam >= m_pm.partlist.size())
    {
      GetDlgItem(IDC_EDT_PARTITION_INFO)->SetWindowText("Free space");
      return 0;
    }

    CString s;

    partition_info &pi = m_pm.partlist[wParam];

    s.Format("CHS_START(%d, %d, %d) / CHS_END(%d, %d, %d)\r\n"
      "LBA_START: %I64d / LBA_END: %I64d\r\n"
      "System Name: %s [%02X]\r\n"
      "Sectors Before: %d\r\n"
      "Number of Sectors: %d\r\n"
      "Size: %d MB\r\n"
      "Partition is: %s / Is Active: %s",
      pi.chs_start.c, pi.chs_start.h, pi.chs_start.s, pi.chs_end.c, pi.chs_end.h, pi.chs_end.s,
      pi.lba_start, pi.lba_end,
      pi.sys_name, pi.sys_id,
      pi.sectors_before, 
      pi.number_of_sectors,
      pi.number_of_sectors / 2048,
      pi.parent_id == 0 ? "Primary" : "Extended",
      pi.is_active ? "Yes" : "No");

    GetDlgItem(IDC_EDT_PARTITION_INFO)->SetWindowText(s);

    return 0;
}

//---------------------------------------------------------------------------
/*!
\brief Initializes the drawing structures
*/
void CPartitionViewerDlg::InitDrawing()
{
  memset(&m_barparams, 0, sizeof(m_barparams));
  memset(&m_historyparams, 0, sizeof(m_historyparams));

  // Draw bar parameters
  m_barparams.hwndNotifyWindow = this->m_hWnd;
  m_barparams.msgMouseMove = MY_WM_BARMOUSEMOVE;

  m_barparams.AbsHeight = 1;
  m_barparams.bShowOverlapping = true;
  
  m_barparams.bShadeConsecutive = true;
  m_barparams.bDebug = false;
  m_drawBar.Init();

  // History bar parameters
  m_historyparams.ColCount = 2;
  m_historyparams.ColWidth = 200;
  m_historyparams.BoxLineSpacing = 10;
  m_historyparams.BoxBaseX = 10;
  m_historyparams.BoxBaseY = 10;
  m_historyparams.DefaultColor = RGB(255, 255, 255);

  m_historyBar.SetColors(
    (MYHISTORYBARTYPECOLOR *)&part_colors[0], 
    sizeof(part_colors) / sizeof(part_colors[0]));

  m_historyBar.Init();
}

//---------------------------------------------------------------------------
void CPartitionViewerDlg::OnBnClickedBtnRepresent()
{
#ifdef _MY_TEST_
  static const MYDRAWBARSTRUCT ds[] = 
  {
    {6, 63, 192780, 192717}, // #1
    {6, 2666853, 2875635, 208782}, // #3
    {11, 2956023, 3373650, 417627}, // #5
    {7, 3855663, 21318255, 17462592}, // #7
    {6, 21736008, 25655805, 3919797}, // #9
    {130, 25912908, 26346600, 433692}, // #11
    {11, 26346663, 29415015, 3068352}, // #13
    {23, 208845, 2490075, 2281230}, // #14
    {28, 29415015, 33543720, 4128705} // #15
  };

  m_barparams.AbsWidth = 17179803648 / 512;
  m_drawBar.SetDrawStruct((MYDRAWBARSTRUCT *)&ds, sizeof(ds)/sizeof(ds[0]));
#else
  int idx = m_cbDrivesList.GetCurSel();
  if (idx == -1)
    return;

  DWORD volNumber = (DWORD)m_cbDrivesList.GetItemData(idx);

  DiskSector d;
  if (!d.OpenPhysicalDrive(volNumber))
  {
    MessageBox("Could not open physical disk for IO!", "Error", MB_ICONERROR | MB_OK);
    return;
  }

  ULONGLONG diskLength;
  if (!d.GetDiskLength(diskLength))
  {
    MessageBox("Cannot determine the disk's capacity", "Error", MB_ICONERROR | MB_OK);
    return;
  }

  d.Close();

  // Convert from bytes to sector count
  m_barparams.AbsWidth = diskLength / 512;

  m_pm.m_bIncludeExtendedPartitionDefinitions = false;

  if (!m_pm.ReadPartitionTable(volNumber))
  {
    MessageBox(
      "Could not read partition table of this physical disk!", 
      "Error",
      MB_ICONERROR | MB_OK);

    return;
  };

  size_t nbPartsTotal = 0, nbParts = m_pm.partlist.size();

  m_partitions.clear();
  
  CString DriveName, s2;
  m_cbDrivesList.GetLBText(idx, DriveName);
  s2.Format(
    "PartitionView %s: \"%s\" [%I64d MB/%I64d GB]", 
    PV_VERSION, 
    DriveName, 
    (ULONGLONG)(diskLength / (1024L*1024L)), 
    (ULONGLONG)(diskLength / (1024L*1024L*1024L)));

  SetWindowText(s2);

  for (size_t i=0; i<nbParts; i++)
  {
    MYDRAWBARSTRUCT dsi;
    partition_info &pi = m_pm.partlist[i];
    DWORD size = pi.number_of_sectors / 2048; // size in MegaBytes

    dsi.type    = pi.sys_id;
    dsi.a_start = (double)pi.lba_start;
    dsi.a_end   = (double) pi.lba_end;
    dsi.id      = (LONG) i;

    if (!GetColorFromType(part_colors, _countof(part_colors), dsi.type, dsi.color))
      dsi.color = m_historyparams.DefaultColor;

    m_partitions.push_back(dsi);
  }

  // No partitions?
  if (m_partitions.empty())
  {
    MYDRAWBARSTRUCT dsi;
    dsi.type = 0; // Unpartitioned
    dsi.a_start = 0;
    dsi.a_end = (double)m_barparams.AbsWidth;
    dsi.id = 0;
    GetColorFromType(part_colors, _countof(part_colors), dsi.type, dsi.color);
    m_partitions.push_back(dsi);
  }

  m_drawBar.SetDrawStruct(
    &m_partitions[0], 
    m_partitions.size());

#endif
  m_drawBar.Init();
  m_drawBar.RedrawWindow();
}

//---------------------------------------------------------------------------
void CPartitionViewerDlg::OnBnClickedBtnCancel()
{
  CAboutDlg dlg;
  dlg.DoModal();
}

//---------------------------------------------------------------------------
void CPartitionViewerDlg::OnCbnSelchangeCbPdrivelist()
{
  OnBnClickedBtnRepresent();
}