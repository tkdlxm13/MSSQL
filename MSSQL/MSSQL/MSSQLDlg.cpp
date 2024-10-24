#include "stdafx.h"  // 표준 헤더 파일
#import "C:\\Program Files\\Common Files\\System\\ado\\msado15.dll" \
    no_namespace rename("EOF", "EndOfFile")
#include "MSSQLDlg.h"  // MSSQLDlg.h를 포함합니다.
#include "AboutDlg.h"  // CAboutDlg 클래스 포함
#include "afxdialogex.h"  // MFC 대화상자 클래스 확장 포함
#include <afxdb.h>  // MFC 데이터베이스 관련 헤더
#include <comdef.h>  // COM 관련 정의 포함
#include <afxwin.h> // MFC 기본 헤더 파일
#include "resource.h" // 리소스 ID 정의 포함

#ifdef _DEBUG
#define new DEBUG_NEW  // 디버그 모드에서 새 메모리 할당 추적
#endif

// 메시지 맵 정의
BEGIN_MESSAGE_MAP(CMSSQLDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CMSSQLDlg::OnBnClickedAddButton)  // 추가 버튼 클릭 이벤트
	ON_BN_CLICKED(IDC_SEARCH_BUTTON, &CMSSQLDlg::OnBnClickedSearchButton)  // 검색 버튼 클릭 이벤트
	ON_WM_PAINT()  // 페인트 이벤트 처리
	ON_WM_SYSCOMMAND()  // 시스템 명령 이벤트 처리
	ON_WM_QUERYDRAGICON()  // 드래그 아이콘 쿼리 처리
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMSSQLDlg::OnBnClickedDeleteButton)  // 삭제 버튼 클릭 이벤트
	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CMSSQLDlg::OnBnClickedEditButton) // 수정 버튼 클릭 이벤트
	ON_BN_CLICKED(IDC_REFRESH_BUTTON, &CMSSQLDlg::OnBnClickedRefreshButton) // 목록 갱신 버튼 클릭 이벤트
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULT, &CMSSQLDlg::OnLvnItemchangedListResult) // 리스트 아이템 변경 이벤트
	ON_WM_CLOSE()  // 대화상자 닫기 이벤트 처리
	ON_STN_CLICKED(IDC_STATIC_TEXT, &CMSSQLDlg::OnStnClickedStaticText)  // 정적 텍스트 클릭 이벤트
END_MESSAGE_MAP()

// CMSSQLDlg 대화 상자 생성자
CMSSQLDlg::CMSSQLDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MSSQL_DIALOG, pParent) {  // 부모 창을 인자로 받음
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);  // 아이콘 로드
}

// 데이터 바인딩 함수
void CMSSQLDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);  // 기본 데이터 교환 호출
	DDX_Control(pDX, IDC_EDIT_PARTNO, m_editPartNo);  // 품번 편집 컨트롤 바인딩
	DDX_Control(pDX, IDC_EDIT_QUANTITY, m_editQuantity);  // 수량 편집 컨트롤 바인딩
	DDX_Control(pDX, IDC_EDIT_DATE, m_editDate);  // 날짜 편집 컨트롤 바인딩
	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult); // 리스트 컨트롤 바인딩
	DDX_Control(pDX, IDC_STATIC_TEXT, m_staticText);  // 정적 텍스트 바인딩
}

// CMSSQLDlg 메시지 처리기
BOOL CMSSQLDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();  // 기본 대화상자 초기화 호출

	// 텍스트 컨트롤에 폰트 적용
	m_staticText.SetFont(&m_font); // m_staticText는 CStatic 변수

	// COM 라이브러리 초기화
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {  // 초기화 실패 시 메시지 박스 표시
		AfxMessageBox(_T("COM 라이브러리 초기화 실패"));
		return FALSE;  // 초기화 실패
	}

	// 리스트 컨트롤 초기화
	m_listResult.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  // 전체 행 선택 및 그리드라인 스타일 적용
	m_listResult.ModifyStyle(0, LVS_REPORT);  // 리포트 스타일로 변경
	m_listResult.InsertColumn(0, _T("품번"), LVCFMT_LEFT, 100);  // 첫 번째 열에 품번 추가
	m_listResult.InsertColumn(1, _T("수량"), LVCFMT_LEFT, 100);  // 두 번째 열에 수량 추가
	m_listResult.InsertColumn(2, _T("최근 수정 날짜"), LVCFMT_LEFT, 150);  // 세 번째 열에 최근 수정 날짜 추가

	// DB에서 데이터를 가져와서 리스트 컨트롤에 추가
	try {
		_ConnectionPtr conn;  // ADO 연결 포인터
		hr = conn.CreateInstance(__uuidof(Connection));  // 연결 객체 생성
		if (FAILED(hr)) {  // 생성 실패 시 메시지 박스 표시
			AfxMessageBox(_T("Connection 객체 생성 실패"));
			return FALSE;  // 실패
		}

		// 데이터베이스 연결
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		_CommandPtr cmd;  // ADO 커맨드 포인터
		hr = cmd.CreateInstance(__uuidof(Command));  // 커맨드 객체 생성
		if (FAILED(hr)) {  // 생성 실패 시 메시지 박스 표시
			AfxMessageBox(_T("Command 객체 생성 실패"));
			return FALSE;  // 실패
		}

		cmd->ActiveConnection = conn;  // 현재 연결 설정
		cmd->CommandText = _bstr_t("SELECT partNo, quantity, date FROM material_stock");  // SQL 쿼리 설정

		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);  // SQL 쿼리 실행
		if (rs == NULL) {  // 결과가 없으면 메시지 박스 표시
			AfxMessageBox(_T("Recordset 가져오기 실패"));
			return FALSE;  // 실패
		}

		int nItem = 0;  // 리스트 아이템 인덱스 초기화
		CString partNo, quantity, date;  // 품번, 수량, 날짜를 저장할 변수

		while (!rs->EndOfFile) {  // 결과의 끝에 도달할 때까지 반복
			partNo = (LPCTSTR)(_bstr_t)rs->Fields->Item["partNo"]->Value;  // 품번 가져오기
			quantity = (LPCTSTR)(_bstr_t)rs->Fields->Item["quantity"]->Value;  // 수량 가져오기

			_variant_t dateVariant = rs->Fields->Item["date"]->Value;  // 날짜 필드 가져오기
			if (dateVariant.vt == VT_DATE) {  // 날짜 형식이 맞으면
				COleDateTime oleDate = (COleDateTime)dateVariant;  // COleDateTime으로 변환
				date = oleDate.Format(_T("%Y-%m-%d %H:%M:%S"));  // 원하는 형식으로 포맷
			}

			// 리스트 컨트롤에 아이템 추가
			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), partNo);  // 품번 추가
			m_listResult.SetItemText(nItem, 1, quantity);  // 수량 추가
			m_listResult.SetItemText(nItem, 2, date);  // 날짜 추가

			rs->MoveNext();  // 다음 레코드로 이동
		}

		rs->Close();  // 레코드셋 닫기
		conn->Close();  // 연결 닫기
	}
	catch (_com_error& e) {  // COM 오류 처리
		CString errorMsg(e.ErrorMessage());  // 오류 메시지 가져오기
		AfxMessageBox(_T("데이터 로드 중 오류 발생: ") + errorMsg);  // 오류 메시지 표시
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// COM 라이브러리 해제
void CMSSQLDlg::OnClose() {
	CoUninitialize(); // COM 라이브러리 해제
	CDialogEx::OnClose();  // 기본 닫기 호출
}

// 추가 버튼 클릭 처리
void CMSSQLDlg::OnBnClickedAddButton() {
	CString partNo;  // 품번
	CString quantity;  // 수량
	CString date;  // 날짜

	// 사용자 입력값 가져오기
	m_editPartNo.GetWindowText(partNo);  // 품번 입력값 가져오기
	m_editQuantity.GetWindowText(quantity);  // 수량 입력값 가져오기
	m_editDate.GetWindowText(date);  // 날짜 입력값 가져오기

	// 입력 필드가 비어 있는지 확인
	if (partNo.IsEmpty() || quantity.IsEmpty() || date.IsEmpty()) {
		AfxMessageBox(_T("모든 필드를 입력해주세요."));  // 경고 메시지
		return;  // 함수 종료
	}

	// 데이터베이스에 추가
	try {
		_ConnectionPtr conn;  // ADO 연결 포인터
		conn.CreateInstance(__uuidof(Connection));  // 연결 객체 생성
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // 데이터베이스 연결

		_CommandPtr cmd;  // ADO 커맨드 포인터
		cmd.CreateInstance(__uuidof(Command));  // 커맨드 객체 생성
		cmd->ActiveConnection = conn;  // 현재 연결 설정

		// SQL 쿼리 작성
		CString sqlQuery;
		sqlQuery.Format(_T("INSERT INTO material_stock (partNo, quantity, date) VALUES ('%s', %s, '%s')"), partNo, quantity, date);  // INSERT 쿼리 생성
		cmd->CommandText = _bstr_t(sqlQuery);  // SQL 쿼리 설정

		cmd->Execute(NULL, NULL, adCmdText);  // SQL 쿼리 실행
		AfxMessageBox(_T("데이터 추가 성공!"));  // 성공 메시지

		// 추가 후 리스트 갱신
		OnBnClickedRefreshButton();  // 리스트 갱신 함수 호출

		conn->Close();  // 연결 닫기
	}
	catch (_com_error& e) {  // COM 오류 처리
		CString errorMsg(e.ErrorMessage());  // 오류 메시지 가져오기
		AfxMessageBox(_T("데이터 추가 중 오류 발생: ") + errorMsg);  // 오류 메시지 표시
	}
}

// 검색 버튼 클릭 처리
void CMSSQLDlg::OnBnClickedSearchButton() {
	CString partNo;  // 품번
	m_editPartNo.GetWindowText(partNo);  // 품번 입력값 가져오기

	// 품번 입력값이 비어 있는지 확인
	if (partNo.IsEmpty()) {
		AfxMessageBox(_T("품번을 입력해주세요."));  // 경고 메시지
		return;  // 함수 종료
	}

	// 데이터베이스에서 검색
	try {
		_ConnectionPtr conn;  // ADO 연결 포인터
		conn.CreateInstance(__uuidof(Connection));  // 연결 객체 생성
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // 데이터베이스 연결

		_CommandPtr cmd;  // ADO 커맨드 포인터
		cmd.CreateInstance(__uuidof(Command));  // 커맨드 객체 생성
		cmd->ActiveConnection = conn;  // 현재 연결 설정

		// SQL 쿼리 작성
		CString sqlQuery;
		sqlQuery.Format(_T("SELECT partNo, quantity, date FROM material_stock WHERE partNo = '%s'"), partNo);  // 품번으로 검색하는 쿼리
		cmd->CommandText = _bstr_t(sqlQuery);  // SQL 쿼리 설정

		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);  // SQL 쿼리 실행
		if (rs == NULL) {  // 결과가 없으면 메시지 박스 표시
			AfxMessageBox(_T("검색 결과가 없습니다."));  // 결과 없음 메시지
			return;  // 함수 종료
		}

		// 리스트 클리어
		m_listResult.DeleteAllItems();  // 리스트 아이템 삭제

										// 검색 결과를 리스트에 추가
		int nItem = 0;  // 리스트 아이템 인덱스 초기화
		CString quantity, date;  // 수량, 날짜를 저장할 변수

		while (!rs->EndOfFile) {  // 결과의 끝에 도달할 때까지 반복
								  // 데이터 가져오기
			CString partNo = (LPCTSTR)(_bstr_t)rs->Fields->Item["partNo"]->Value;  // 품번 가져오기
			quantity = (LPCTSTR)(_bstr_t)rs->Fields->Item["quantity"]->Value;  // 수량 가져오기

			_variant_t dateVariant = rs->Fields->Item["date"]->Value;  // 날짜 필드 가져오기
			if (dateVariant.vt == VT_DATE) {  // 날짜 형식이 맞으면
				COleDateTime oleDate = (COleDateTime)dateVariant;  // COleDateTime으로 변환
				date = oleDate.Format(_T("%Y-%m-%d %H:%M:%S"));  // 원하는 형식으로 포맷
			}

			// 리스트 컨트롤에 아이템 추가
			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), partNo);  // 품번 추가
			m_listResult.SetItemText(nItem, 1, quantity);  // 수량 추가
			m_listResult.SetItemText(nItem, 2, date);  // 날짜 추가

			rs->MoveNext();  // 다음 레코드로 이동
		}

		rs->Close();  // 레코드셋 닫기
		conn->Close();  // 연결 닫기
	}
	catch (_com_error& e) {  // COM 오류 처리
		CString errorMsg(e.ErrorMessage());  // 오류 메시지 가져오기
		AfxMessageBox(_T("데이터 검색 중 오류 발생: ") + errorMsg);  // 오류 메시지 표시
	}
}

// 수정 버튼 클릭 처리
void CMSSQLDlg::OnBnClickedEditButton() {
	CString partNo;  // 품번
	CString quantity;  // 수량
	CString date;  // 날짜

	// 입력값 가져오기
	m_editPartNo.GetWindowText(partNo);  // 품번 입력값 가져오기
	m_editQuantity.GetWindowText(quantity);  // 수량 입력값 가져오기
	m_editDate.GetWindowText(date);  // 날짜 입력값 가져오기

	// 입력 필드가 비어 있는지 확인
	if (partNo.IsEmpty() || quantity.IsEmpty() || date.IsEmpty()) {
		AfxMessageBox(_T("모든 필드를 입력해주세요."));  // 경고 메시지
		return;  // 함수 종료
	}

	// 데이터베이스에서 수정
	try {
		_ConnectionPtr conn;  // ADO 연결 포인터
		conn.CreateInstance(__uuidof(Connection));  // 연결 객체 생성
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // 데이터베이스 연결

		_CommandPtr cmd;  // ADO 커맨드 포인터
		cmd.CreateInstance(__uuidof(Command));  // 커맨드 객체 생성
		cmd->ActiveConnection = conn;  // 현재 연결 설정

		// SQL 쿼리 작성
		CString sqlQuery;
		sqlQuery.Format(_T("UPDATE material_stock SET quantity = %s, date = '%s' WHERE partNo = '%s'"), quantity, date, partNo);  // UPDATE 쿼리 생성
		cmd->CommandText = _bstr_t(sqlQuery);  // SQL 쿼리 설정

		cmd->Execute(NULL, NULL, adCmdText);  // SQL 쿼리 실행
		AfxMessageBox(_T("데이터 수정 성공!"));  // 성공 메시지

		// 수정 후 리스트 갱신
		OnBnClickedRefreshButton();  // 리스트 갱신 함수 호출

		conn->Close();  // 연결 닫기
	}
	catch (_com_error& e) {  // COM 오류 처리
		CString errorMsg(e.ErrorMessage());  // 오류 메시지 가져오기
		AfxMessageBox(_T("데이터 수정 중 오류 발생: ") + errorMsg);  // 오류 메시지 표시
	}
}

// 삭제 버튼 클릭 처리
void CMSSQLDlg::OnBnClickedDeleteButton() {
	CString partNo;  // 품번
	m_editPartNo.GetWindowText(partNo);  // 품번 입력값 가져오기

	// 품번 입력값이 비어 있는지 확인
	if (partNo.IsEmpty()) {
		AfxMessageBox(_T("삭제할 품번을 입력해주세요."));  // 경고 메시지
		return;  // 함수 종료
	}

	// 데이터베이스에서 삭제
	try {
		_ConnectionPtr conn;  // ADO 연결 포인터
		conn.CreateInstance(__uuidof(Connection));  // 연결 객체 생성
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // 데이터베이스 연결

		_CommandPtr cmd;  // ADO 커맨드 포인터
		cmd.CreateInstance(__uuidof(Command));  // 커맨드 객체 생성
		cmd->ActiveConnection = conn;  // 현재 연결 설정

		// SQL 쿼리 작성
		CString sqlQuery;
		sqlQuery.Format(_T("DELETE FROM material_stock WHERE partNo = '%s'"), partNo);  // DELETE 쿼리 생성
		cmd->CommandText = _bstr_t(sqlQuery);  // SQL 쿼리 설정

		cmd->Execute(NULL, NULL, adCmdText);  // SQL 쿼리 실행
		AfxMessageBox(_T("데이터 삭제 성공!"));  // 성공 메시지

		// 삭제 후 리스트 갱신
		OnBnClickedRefreshButton();  // 리스트 갱신 함수 호출

		conn->Close();  // 연결 닫기
	}
	catch (_com_error& e) {  // COM 오류 처리
		CString errorMsg(e.ErrorMessage());  // 오류 메시지 가져오기
		AfxMessageBox(_T("데이터 삭제 중 오류 발생: ") + errorMsg);  // 오류 메시지 표시
	}
}

void CMSSQLDlg::OnBnClickedRefreshButton() {
	// 데이터베이스에서 새로운 데이터를 가져와서 리스트를 갱신하는 함수
	try {
		// 데이터베이스 연결
		_ConnectionPtr conn;  // ADO 연결 포인터 생성
		conn.CreateInstance(__uuidof(Connection));  // 연결 객체 생성
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // 데이터베이스 연결

																																										// SQL 쿼리 작성
		_CommandPtr cmd;  // ADO 커맨드 포인터 생성
		cmd.CreateInstance(__uuidof(Command));  // 커맨드 객체 생성
		cmd->ActiveConnection = conn;  // 현재 연결 설정

		// material_stock 테이블에서 데이터 선택
		cmd->CommandText = _bstr_t("SELECT partNo, quantity, date FROM material_stock");

		// 쿼리 실행 및 결과 집합 가져오기
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);

		// 리스트 컨트롤 초기화
		m_listResult.DeleteAllItems();  // 이전 데이터 삭제

		// 새로운 데이터 추가
		int nItem = 0;  // 리스트의 아이템 인덱스 초기화
		CString partNo, quantity, date;  // 품번, 수량, 날짜를 저장할 변수 선언

		// 결과 집합에서 데이터를 읽어 리스트에 추가
		while (!rs->EndOfFile) {  // 결과 집합의 끝까지 반복
								  // 각 필드의 값 가져오기
			partNo = (LPCTSTR)(_bstr_t)rs->Fields->Item["partNo"]->Value;  // 품번
			quantity = (LPCTSTR)(_bstr_t)rs->Fields->Item["quantity"]->Value;  // 수량

			// 날짜 형식 변환
			_variant_t dateVariant = rs->Fields->Item["date"]->Value;  // 날짜 필드 값 가져오기
			if (dateVariant.vt == VT_DATE) {  // 날짜 형식 확인
				COleDateTime oleDate = (COleDateTime)dateVariant;  // COleDateTime 객체로 변환
				date = oleDate.Format(_T("%Y-%m-%d %H:%M:%S"));  // 문자열로 포맷
			}

			// 리스트에 새로운 아이템 추가
			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), partNo);  // 품번을 첫 번째 열에 추가
			m_listResult.SetItemText(nItem, 1, quantity);  // 수량을 두 번째 열에 추가
			m_listResult.SetItemText(nItem, 2, date);  // 날짜를 세 번째 열에 추가

			rs->MoveNext();  // 다음 레코드로 이동
		}

		rs->Close();  // 결과 집합 닫기
		conn->Close();  // 데이터베이스 연결 닫기
	}
	catch (_com_error& e) {  // 예외 처리
		CString errorMsg(e.ErrorMessage());  // 오류 메시지 가져오기
		AfxMessageBox(_T("데이터 갱신 중 오류 발생: ") + errorMsg);  // 오류 메시지 표시
	}
}

void CMSSQLDlg::OnPaint() {
	// 윈도우의 페인팅 처리
	if (IsIconic()) {  // 아이콘 상태인지 확인
		CPaintDC dc(this);  // 그리기 DC 생성
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);  // 아이콘 배경 지우기
		int cxIcon = GetSystemMetrics(SM_CXICON);  // 아이콘의 너비
		int cyIcon = GetSystemMetrics(SM_CYICON);  // 아이콘의 높이
		CRect rect;
		GetClientRect(&rect);  // 클라이언트 영역의 크기 가져오기
		int x = (rect.Width() - cxIcon + 1) / 2;  // 아이콘을 중앙에 배치하기 위한 X 좌표 계산
		int y = (rect.Height() - cyIcon + 1) / 2;  // 아이콘을 중앙에 배치하기 위한 Y 좌표 계산
		dc.DrawIcon(x, y, m_hIcon);  // 아이콘 그리기
	}
	else {
		CDialogEx::OnPaint();  // 기본 페인팅 처리
	}
}

void CMSSQLDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	// 시스템 명령 처리
	CDialogEx::OnSysCommand(nID, lParam);  // 기본 시스템 명령 처리
}

HCURSOR CMSSQLDlg::OnQueryDragIcon() {
	// 아이콘 드래그 이벤트 처리
	return static_cast<HCURSOR>(m_hIcon);  // 아이콘 커서 반환
}

void CMSSQLDlg::OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);  // 리스트 뷰의 알림 메시지 구조체를 캐스팅
	 // TODO: Add your control notification handler code here
	*pResult = 0;  // 결과 값 설정
}

void CMSSQLDlg::OnStnClickedStaticText() {
	// 정적 텍스트 클릭 이벤트 처리
	// TODO: Add your control notification handler code here
}
