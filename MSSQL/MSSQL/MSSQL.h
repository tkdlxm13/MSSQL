// MSSQL.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.

#pragma once

#include "afxwin.h" // CDialogEx�� ����ϱ� ���� �ʿ���
#include "resource.h"
#include "AboutDlg.h" // CAboutDlg ���� ����
#include "MSSQLDlg.h" // CMSSQLDlg ���� ����

class CMSSQLApp : public CWinApp {
public:
	CMSSQLApp();
	virtual BOOL InitInstance();
};

