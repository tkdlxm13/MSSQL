// AboutDlg.h
#pragma once

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <afxwin.h>

class CAboutDlg : public CDialogEx {
public:
	CAboutDlg(CWnd* pParent = nullptr); // ǥ�� �������Դϴ�.

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX }; // ���ҽ� ID
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

#endif // ABOUTDLG_H
