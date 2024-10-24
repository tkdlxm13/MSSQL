#pragma once

// CMSSQLDlg 대화 상자 클래스
class CMSSQLDlg : public CDialogEx
{
public:
	CMSSQLDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MSSQL_DIALOG }; // 대화 상자 리소스 ID
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:
	HICON m_hIcon;  // 대화 상자의 아이콘 핸들

private:
	CFont m_font; // 사용자 정의 폰트 객체

protected:
	virtual void OnClose(); // 대화 상자가 닫힐 때 호출되는 함수

	virtual BOOL OnInitDialog(); // 대화 상자 초기화 함수
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam); // 시스템 명령 처리 함수
	afx_msg void OnPaint(); // 페인팅 처리 함수
	afx_msg HCURSOR OnQueryDragIcon(); // 아이콘 드래그 이벤트 처리 함수
	afx_msg void OnBnClickedSearchButton(); // 검색 버튼 클릭 이벤트 처리 함수
	afx_msg void OnBnClickedAddButton(); // 추가 버튼 클릭 이벤트 처리 함수
	afx_msg void OnBnClickedDeleteButton(); // 삭제 버튼 클릭 이벤트 처리 함수
	afx_msg void OnBnClickedEditButton(); // 수정 버튼 클릭 이벤트 처리 함수
	afx_msg void OnBnClickedRefreshButton(); // 새로 고침 버튼 클릭 이벤트 처리 함수
	DECLARE_MESSAGE_MAP() // 메시지 맵 매크로

						  // Edit Control 변수 선언
	CEdit m_editPartNo;     // partNo 입력 컨트롤
	CEdit m_editQuantity;   // quantity 입력 컨트롤
	CEdit m_editDate;       // date 입력 컨트롤
	CListCtrl m_listResult; // 결과를 출력할 리스트 컨트롤
	CEdit m_editNewPartNo;     // 새로운 partNo 입력 컨트롤
	CEdit m_editNewQuantity;   // 새로운 quantity 입력 컨트롤
	CEdit m_editNewDate;       // 새로운 date 입력 컨트롤
	CEdit m_editSearchValue;   // 검색 입력 필드
	CComboBox m_comboSearchCriteria; // 검색 기준 콤보 박스
	CStatic m_staticText; // 정적 텍스트 컨트롤

public:
	afx_msg void OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult); // 리스트 항목 변경 이벤트 처리 함수
	afx_msg void OnStnClickedStaticText(); // 정적 텍스트 클릭 이벤트 처리 함수
};