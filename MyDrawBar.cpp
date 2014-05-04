// MyDrawBar.cpp : implementation file
//

#include "stdafx.h"
#include "mydrawbar.h"

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

// CMyDrawBar

//---------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CMyDrawBar, CStatic)
CMyDrawBar::CMyDrawBar(MYDRAWBARPARAMETERS *params)
{
  m_nSelectedId = -1;
  m_params = params;
  m_params->_bInitialized = false;
}

//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMyDrawBar, CStatic)
  ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_CONTROL_REFLECT(STN_CLICKED, OnStnClicked)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
// Initializes the structures and variables
bool CMyDrawBar::Init()
{
  DWORD w, h;
  GetRelativeSizes(w, h);

  m_nSelectedId = -1;

  if (m_params->bgBar == RGB(0, 0, 0))
    m_params->bgBar = RGB(255,255,255);


  if (m_params->AbsHeight == 0)
    m_params->AbsHeight = m_nRelativeHeight;

  if (m_params->AbsWidth == 0)
    m_params->AbsWidth = m_nRelativeWidth;

  m_params->_bInitialized = true;

  return true;
}

//---------------------------------------------------------------------------
void CMyDrawBar::SetDrawStruct(
  MYDRAWBARSTRUCT *ds, 
  size_t sz)
{
  double aw = (double)m_params->AbsWidth, rw = (double)m_nRelativeWidth;

  MYDRAWBARSTRUCT *vds = new MYDRAWBARSTRUCT[sz];
  MYDRAWBARSTRUCT d;

  CString s, s1;

  // Scale the sizes
  for (size_t i=0;i<sz;i++)
  {
    d = ds[i];

    d.s_start = (rw * d.a_start) / aw;
    d.s_end   = (rw * d.a_end) / aw;

    vds[i] = d;
  }

  // Sort by start locations
  for (size_t i=0;i<sz;i++)
  {
    for (size_t j=0;j<sz;j++)
    {
      if (vds[j].a_start > vds[i].a_start)
      {
        d = vds[i];
        vds[i] = vds[j];
        vds[j] = d;
      }
    }
  }

  // Save the struct
  m_list.clear();

  COLORREF lastTypeColor = COLORREF(-1);

  for (size_t i=0;i<sz;i++)
  {
    if (m_params != NULL && m_params->bShadeConsecutive)
    {
      if (vds[i].color == lastTypeColor)
        vds[i].color *= 2;
    }

    m_list.push_back(vds[i]);

    lastTypeColor = vds[i].color;

    if (m_params->bDebug)
    {
      s1.Format(
        "%d)id=%d s_start=%d s_end=%d / a_start=%f a_end=%f / color: %06x\n", 
        i, vds[i].id, 
        (long)vds[i].s_start, (long)vds[i].s_end, 
        vds[i].a_start, vds[i].a_end,
        vds[i].color);
      s += s1;
    }

  }

  if (m_params->bDebug)
  {
    s1.Format("rw=%d rh=%d\n", m_nRelativeWidth, m_nRelativeHeight);
    s += s1;
    MessageBox(s);
  }

  delete [] vds;
}

//---------------------------------------------------------------------------
// This call adjusts the internal variables and also gives the users the relative dimensions
void CMyDrawBar::GetRelativeSizes(DWORD &Width, DWORD &Height)
{
  this->GetClientRect(&m_rRect);

  m_nRelativeHeight = m_rRect.bottom - m_rRect.top;
  m_nRelativeWidth = m_rRect.right - m_rRect.left;

  Width = m_nRelativeWidth;
  Height = m_nRelativeHeight;
}

//---------------------------------------------------------------------------
CMyDrawBar::~CMyDrawBar()
{
}

//---------------------------------------------------------------------------
void CMyDrawBar::OnPaint()
{
  size_t nElements = m_list.size();

  CPaintDC dc(this); // device context for painting

  LONG rel = 0, box_height;

  if (nElements == 0)
    box_height = 0;
  else
    box_height = m_nRelativeHeight / nElements;

  CBrush brushFill(m_params->bgBar);

  CPen penBorder(PS_SOLID, 2, RGB(0, 0, 0)), penNormal(PS_SOLID, 1, RGB(0, 0, 0));
  dc.FillRect(&m_rRect, &brushFill);

  RECT r;

  for (MYDRAWBARSTRUCT_LIST::iterator it = m_list.begin(); it != m_list.end();++it)
  {
    MYDRAWBARSTRUCT &d = *it;

    CBrush brush(d.color);
    if (m_params->bShowOverlapping)
    {
      r.top = 0;
      r.bottom = m_nRelativeHeight;
    }
    else
    {
      r.top = rel;
      r.bottom = r.top + box_height;
      rel = r.bottom + 1;
    }

    r.left  = (LONG) d.s_start;
    r.right = (LONG) d.s_end;

    if (m_params->bShowOverlapping)
    {
      //r.bottom -= 1;
      //r.top    += 1;
    }
    dc.FillRect(&r, &brush);
  }
}

//---------------------------------------------------------------------------
void CMyDrawBar::OnMouseMove(UINT nFlags, CPoint point)
{
  CString s, s1;

  if (m_params->bDebug)
  {
    s1.Format("bar: x=%d y=%d | ", point.x, point.y);
    s += s1;
  }

  bool bFound = false;

  for (int i=0;i<(int)m_list.size();i++)
  {
    MYDRAWBARSTRUCT &ds = m_list[i];

    if ((point.x >= (LONG) ds.s_start) && (point.x <= (LONG) ds.s_end))
    {
      m_nSelectedId = ds.id;
      bFound   = true;
      if (m_params->bDebug)
      {
        s1.Format("sx=%d Id: %d MB | ", (LONG) ds.s_start, ds.id);
        s += s1;
      }
      break;
    }
  }

  if (m_params->bDebug)
  {
    GetParent()->SetWindowText(s);
  }

  if (!bFound)
    m_nSelectedId = -1;

  if (m_params->hwndNotifyWindow != NULL && m_params->msgMouseMove != 0)
    ::SendMessage(m_params->hwndNotifyWindow, m_params->msgMouseMove, m_nSelectedId, MAKELONG(point.x, point.y));

  CStatic::OnMouseMove(nFlags, point);
}

//---------------------------------------------------------------------------
void CMyDrawBar::OnStnClicked()
{
  if (m_params->hwndNotifyWindow != NULL && m_params->msgClick != 0)
    ::SendMessage(m_params->hwndNotifyWindow, m_params->msgClick, m_nSelectedId, 0);
}
