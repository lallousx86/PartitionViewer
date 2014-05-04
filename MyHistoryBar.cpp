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
#include "MyHistoryBar.h"


// CMyHistoryBar

//---------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMyHistoryBar, CStatic)
CMyHistoryBar::CMyHistoryBar(MYHISTORYBARPARAMETERS *params)
{
  m_params = params;
  m_params->_bInitialized = false;
}

//---------------------------------------------------------------------------
CMyHistoryBar::~CMyHistoryBar()
{
}


BEGIN_MESSAGE_MAP(CMyHistoryBar, CStatic)
  ON_WM_PAINT()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
void CMyHistoryBar::OnPaint()
{
  CPaintDC dc(this); // device context for painting

  if (!m_params->_bInitialized)
  {
    CStatic::OnPaint();
    return;
  }

  RECT r;

  CBrush brushFill(m_params->bgHistory);

  this->GetClientRect(&r);

  dc.FillRect(&r, &brushFill);

  int boxHeight = m_params->BoxHeight, 
    boxWidth = m_params->BoxWidth,
    baseX = m_params->BoxBaseX, baseY = m_params->BoxBaseY,
    lineHeight = boxHeight + m_params->BoxLineSpacing,
    colWidth = m_params->ColWidth, 
    colCount = m_params->ColCount;

  CFont txtFont;

  txtFont.CreatePointFont(m_params->FontPointSize, m_params->FontName);

  dc.SelectObject(&txtFont);

  r.left = baseX;
  r.top = baseY;

  int items = 0;

  CString s;
  for (MYHISTORYBARTYPECOLORLIST::iterator it = m_colorlist.begin(); it != m_colorlist.end(); ++it)
  {
    MYHISTORYBARTYPECOLOR &clr = *it;

    CBrush brush(clr.color);
    r.right = r.left + boxWidth;
    r.bottom = r.top + boxHeight;
    dc.FillRect(&r, &brush);
    r.left += boxWidth + 5;

    s.Format("(%02X) %s", clr.type, clr.name);
    dc.TextOut(r.left, r.top - 2, s);

    r.left += colWidth;

    // New line
    if (++items % colCount == 0)
    {
      r.left = baseX;
      r.top += lineHeight;
    }
  }
}

//---------------------------------------------------------------------------
void CMyHistoryBar::SetColors(MYHISTORYBARTYPECOLOR *cm, size_t sz)
{
  m_colorlist.clear();

  for (size_t i=0;i<sz;i++)
    m_colorlist.push_back(cm[i]);
}

//---------------------------------------------------------------------------
bool CMyHistoryBar::Init()
{
  if (m_params->FontName == 0)
    m_params->FontName = "Arial";

  if (m_params->FontPointSize == 0)
    m_params->FontPointSize = 80;

  if (m_params->bgHistory == RGB(0, 0, 0))
    m_params->bgHistory = RGB(255,255,255);

  if (m_params->ColCount == 0)
    m_params->ColCount = 3;

  if (m_params->ColWidth == 0)
    m_params->ColWidth = 120;

  if (m_params->BoxHeight == 0)
    m_params->BoxHeight = 10;

  if (m_params->BoxWidth == 0)
    m_params->BoxWidth = 10;

  if (m_params->BoxBaseX == 0)
    m_params->BoxBaseX = 3;

  if (m_params->BoxBaseY == 0)
    m_params->BoxBaseY = 3;

  if (m_params->BoxLineSpacing == 0)
    m_params->BoxLineSpacing = 5;

  m_params->_bInitialized = true;
  return true;
}