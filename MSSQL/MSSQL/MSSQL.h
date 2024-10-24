// MSSQL.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.

#pragma once

#include "afxwin.h" // CDialogEx를 사용하기 위해 필요함
#include "resource.h"
#include "AboutDlg.h" // CAboutDlg 정의 포함
#include "MSSQLDlg.h" // CMSSQLDlg 정의 포함

class CMSSQLApp : public CWinApp {
public:
	CMSSQLApp();
	virtual BOOL InitInstance();
};

