// ViewerView.cpp : implementation of the CViewerView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "strutil.h"

#include "ViewerView.h"

BOOL CViewerView::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

void CViewerView::SetData(HEWIO::Data* pData)
{
	m_pData = pData;
	
	char buff[64];
	size_t cnt;
	const HEWIO::ModuleDef* pDef = m_pData->GetModule(cnt);
	for (int i=0; i<cnt; ++i) {
		memcpy(buff, pDef->name.s_, pDef->name.len_);
		buff[pDef->name.len_] = 0;
		m_wndListModules.AddString(ToT(buff).c_str());
		++pDef;
	}
}


LRESULT CViewerView::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	DoDataExchange();
	
	DlgResize_Init(false, true, WS_CLIPCHILDREN);
	return TRUE; // set focus to default control
}

LRESULT CViewerView::OnLbnSelchangeListModules(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_wndListModules.GetCurSel();
	size_t cnt;
	const HEWIO::RegisterDef* pReg = m_pData->GetRegister(idx, cnt);
	m_wndListRegisters.ResetContent();
	m_wndListBitfields.ResetContent();
	char buff[64];
	for (int i=0; i<cnt; ++i) {
		memcpy(buff, pReg->id.s_, pReg->id.len_);
		buff[pReg->id.len_] = 0;
		m_wndListRegisters.AddString(ToT(buff).c_str());
		++pReg;
	}
	return 0;
}

LRESULT CViewerView::OnLbnSelchangeListRegisters(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int moduleIdx = m_wndListModules.GetCurSel();
	int registerIdx = m_wndListRegisters.GetCurSel();
	size_t cnt;
	const HEWIO::BitFieldDef* pDef = m_pData->GetBitField(moduleIdx, registerIdx, cnt);
	
	m_wndListBitfields.ResetContent();
	if (cnt == 0) {
		return 0;
	}
	char buff[64];
	for (size_t i=0; i<cnt; ++i) {
		memcpy(buff, pDef->name.s_, pDef->name.len_);
		buff[pDef->name.len_] = 0;
		m_wndListBitfields.AddString(ToT(buff).c_str());
		++pDef;
	}
	return 0;
}

LRESULT CViewerView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT state = (UINT)wParam;
	CSize Size = CSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	CDialogResize::OnSize(uMsg, wParam, lParam, bHandled);
	return 0;
}
