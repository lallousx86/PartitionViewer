#pragma once

#include <map>
#include <vector>

//---------------------------------------------------------------------------
// CMyDrawBar
struct MYDRAWBARSTRUCT
{
  LONG type; // <-- associated w/ a color

  // absolute start / absolute end
  double a_start, a_end;
  
  // ID to link to another structure
  LONG id;

  // scaled start / scaled end
  double s_start, s_end;

  COLORREF color;
};


typedef std::vector<MYDRAWBARSTRUCT> MYDRAWBARSTRUCT_LIST;

//---------------------------------------------------------------------------
struct MYDRAWBARPARAMETERS
{
  // The window that will receive the events
  HWND hwndNotifyWindow;

  // The message to send when the user clicks an area
  UINT msgClick;

  // The message to send when the user moves the mouse
  UINT msgMouseMove;

  // Debug flags
  bool bDebug;

  // How to draw?
  bool bShowOverlapping;

  // If two items are consecutive and with same color, should we shade them?
  bool bShadeConsecutive;

  // Absolute sizes
  ULONGLONG AbsWidth;
  ULONGLONG AbsHeight;

  // Colors
  COLORREF bgBar;

  // Private fields
  bool _bInitialized;
};

//---------------------------------------------------------------------------
class CMyDrawBar : public CStatic
{
	DECLARE_DYNAMIC(CMyDrawBar)

private:
  RECT m_rRect;
  DWORD m_nRelativeWidth, m_nRelativeHeight;

  // The parameters
  MYDRAWBARPARAMETERS *m_params, m_dummyparams;

  MYDRAWBARSTRUCT_LIST m_list;

  int m_nSelectedId;

public:

	CMyDrawBar(MYDRAWBARPARAMETERS *params);
	virtual ~CMyDrawBar();

  bool Init();

  void GetRelativeSizes(DWORD &Width, DWORD &Height);

  void SetDrawStruct(MYDRAWBARSTRUCT *ds, size_t sz);

protected:

  void drawBar();
	DECLARE_MESSAGE_MAP()
public:
  // For messages to be passed, make sure the static control has its SS_NOTIFY attribute toggled
  afx_msg void OnPaint();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnStnClicked();
};