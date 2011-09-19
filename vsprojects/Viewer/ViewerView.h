// ViewerView.h : interface of the CViewerView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlddx.h>
#include "HEWIO.h"
#include <atlcrack.h>
#include <atlmisc.h>

class CViewerView
	:
	public CDialogImpl<CViewerView>,
	public CWinDataExchange<CViewerView>
	,public CDialogResize<CViewerView>
{
public:
	enum { IDD = IDD_VIEWER_FORM };
	
	HEWIO::Data* m_pData;
	CListBox m_wndListModules;CListBox m_wndListRegisters;CListBox m_wndListBitfields;
	
	void SetData(HEWIO::Data* pData);
	
	BEGIN_DLGRESIZE_MAP(CViewerView)
		DLGRESIZE_CONTROL(IDC_LIST_MODULES, DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_LIST_REGISTERS, DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_LIST_BITFIELDS, DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg);
	
	BEGIN_MSG_MAP(CViewerView)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_HANDLER(IDC_LIST_MODULES, LBN_SELCHANGE, OnLbnSelchangeListModules)
		COMMAND_HANDLER(IDC_LIST_REGISTERS, LBN_SELCHANGE, OnLbnSelchangeListRegisters)
	END_MSG_MAP()
	
	BEGIN_DDX_MAP(CViewerView)
		DDX_CONTROL_HANDLE(IDC_LIST_MODULES, m_wndListModules)
		DDX_CONTROL_HANDLE(IDC_LIST_REGISTERS, m_wndListRegisters)
		DDX_CONTROL_HANDLE(IDC_LIST_BITFIELDS, m_wndListBitfields)
	END_DDX_MAP()
	
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnInitDialog(HWND hwndFocus, LPARAM lParam);
	LRESULT OnLbnSelchangeListModules(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLbnSelchangeListRegisters(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
