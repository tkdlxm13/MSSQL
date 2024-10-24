// AboutDlg.h
#pragma once

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <afxwin.h>

class CAboutDlg : public CDialogEx {
public:
	CAboutDlg(CWnd* pParent = nullptr); // 표준 생성자입니다.

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX }; // 리소스 ID
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};

#endif // ABOUTDLG_H
