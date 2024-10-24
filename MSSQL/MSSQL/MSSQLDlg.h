#pragma once

// CMSSQLDlg 대화 상자
class CMSSQLDlg : public CDialogEx
{
public:
	CMSSQLDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MSSQL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:
	HICON m_hIcon;

private:
	CFont m_font; // 폰트 객체

protected:
	virtual void OnClose(); // OnClose 함수 선언

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedSearchButton();
	afx_msg void OnBnClickedAddButton();
	afx_msg void OnBnClickedDeleteButton();
	afx_msg void OnBnClickedEditButton();
	afx_msg void OnBnClickedRefreshButton();
	DECLARE_MESSAGE_MAP()

	// Edit Control 변수 선언
	CEdit m_editPartNo;     // partNo 입력 컨트롤
	CEdit m_editQuantity;   // quantity 입력 컨트롤
	CEdit m_editDate;       // date 입력 컨트롤
	CListCtrl m_listResult; // 결과를 출력할 리스트 컨트롤 추가
	CEdit m_editNewPartNo;     // 새로운 partNo 입력 컨트롤
	CEdit m_editNewQuantity;   // 새로운 quantity 입력 컨트롤
	CEdit m_editNewDate;       // 새로운 date 입력 컨트롤
	CEdit m_editSearchValue;         // 검색 입력 필드
	CComboBox m_comboSearchCriteria;  // 검색 기준 콤보 박스
	CStatic m_staticText; // CStatic 변수 추가

public:
	afx_msg void OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStnClickedStaticText();
};
