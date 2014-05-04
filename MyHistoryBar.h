#pragma once

// CMyHistoryBar
#include <list>

//---------------------------------------------------------------------------
struct MYHISTORYBARPARAMETERS
{
  // Debug flags
  bool bDebug;

  // History panel settings
  char *FontName;
  int  FontPointSize;
  int  ColCount;
  int  ColWidth;
  int  BoxHeight, BoxWidth;
  int  BoxBaseX, BoxBaseY;
  int  BoxLineSpacing;

  // Colors
  COLORREF bgHistory;
  COLORREF DefaultColor;

  bool _bInitialized;
};

struct MYHISTORYBARTYPECOLOR
{
  LONG type;
  COLORREF color;
  char *name;
};

typedef std::list<MYHISTORYBARTYPECOLOR> MYHISTORYBARTYPECOLORLIST;

class CMyHistoryBar : public CStatic
{
	DECLARE_DYNAMIC(CMyHistoryBar)

  MYHISTORYBARPARAMETERS *m_params;
  MYHISTORYBARTYPECOLORLIST m_colorlist;

public:
	CMyHistoryBar(MYHISTORYBARPARAMETERS *params);
	virtual ~CMyHistoryBar();

  void SetColors(MYHISTORYBARTYPECOLOR *cm, size_t sz);
  bool Init();

protected:

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnPaint();
};