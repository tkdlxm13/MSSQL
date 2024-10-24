#include "stdafx.h"
#import "C:\\Program Files\\Common Files\\System\\ado\\msado15.dll" \
    no_namespace rename("EOF", "EndOfFile")
#include "MSSQLDlg.h"  // MSSQLDlg.h를 포함합니다.
#include "AboutDlg.h"  // CAboutDlg 클래스 포함
#include "afxdialogex.h"
#include <afxdb.h>
#include <comdef.h>  // _com_error 정의
#include <afxwin.h> // MFC 기본 헤더 파일
#include "resource.h" // 리소스 ID 정의 포함

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMSSQLDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CMSSQLDlg::OnBnClickedAddButton)
	ON_BN_CLICKED(IDC_SEARCH_BUTTON, &CMSSQLDlg::OnBnClickedSearchButton)
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMSSQLDlg::OnBnClickedDeleteButton)
	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CMSSQLDlg::OnBnClickedEditButton) // 수정 버튼 핸들러 추가
	ON_BN_CLICKED(IDC_REFRESH_BUTTON, &CMSSQLDlg::OnBnClickedRefreshButton) // 목록 갱신 버튼 핸들러 추가
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULT, &CMSSQLDlg::OnLvnItemchangedListResult)
	ON_WM_CLOSE()
	ON_STN_CLICKED(IDC_STATIC_TEXT, &CMSSQLDlg::OnStnClickedStaticText)
END_MESSAGE_MAP()

// CMSSQLDlg 대화 상자
CMSSQLDlg::CMSSQLDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MSSQL_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMSSQLDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PARTNO, m_editPartNo);
	DDX_Control(pDX, IDC_EDIT_QUANTITY, m_editQuantity);
	DDX_Control(pDX, IDC_EDIT_DATE, m_editDate);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult); // 리스트 컨트롤 바인딩
	DDX_Control(pDX, IDC_STATIC_TEXT, m_staticText);
}

// CMSSQLDlg 메시지 처리기
BOOL CMSSQLDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	// 텍스트 컨트롤에 폰트 적용
	m_staticText.SetFont(&m_font); // m_staticText는 CStatic 변수

								   // COM 라이브러리 초기화
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		AfxMessageBox(_T("COM 라이브러리 초기화 실패"));
		return FALSE;
	}

	// 리스트 컨트롤 초기화
	m_listResult.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listResult.ModifyStyle(0, LVS_REPORT);
	m_listResult.InsertColumn(0, _T("품번"), LVCFMT_LEFT, 100);
	m_listResult.InsertColumn(1, _T("수량"), LVCFMT_LEFT, 100);
	m_listResult.InsertColumn(2, _T("최근 수정 날짜"), LVCFMT_LEFT, 150);

	// DB에서 데이터를 가져와서 리스트 컨트롤에 추가
	try {
		_ConnectionPtr conn;
		hr = conn.CreateInstance(__uuidof(Connection));
		if (FAILED(hr)) {
			AfxMessageBox(_T("Connection 객체 생성 실패"));
			return FALSE;
		}

		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		_CommandPtr cmd;
		hr = cmd.CreateInstance(__uuidof(Command));
		if (FAILED(hr)) {
			AfxMessageBox(_T("Command 객체 생성 실패"));
			return FALSE;
		}

		cmd->ActiveConnection = conn;
		cmd->CommandText = _bstr_t("SELECT partNo, quantity, date FROM material_stock");

		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);
		if (rs == NULL) {
			AfxMessageBox(_T("Recordset 가져오기 실패"));
			return FALSE;
		}

		int nItem = 0;
		CString partNo, quantity, date;

		while (!rs->EndOfFile) {
			partNo = (LPCTSTR)(_bstr_t)rs->Fields->Item["partNo"]->Value;
			quantity = (LPCTSTR)(_bstr_t)rs->Fields->Item["quantity"]->Value;

			_variant_t dateVariant = rs->Fields->Item["date"]->Value;
			if (dateVariant.vt == VT_DATE) {
				COleDateTime oleDate = (COleDateTime)dateVariant;
				date = oleDate.Format(_T("%Y-%m-%d %H:%M:%S"));
			}

			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), partNo);
			m_listResult.SetItemText(nItem, 1, quantity);
			m_listResult.SetItemText(nItem, 2, date);

			rs->MoveNext();
		}

		rs->Close();
		conn->Close();
	}
	catch (_com_error& e) {
		CString errorMsg(e.ErrorMessage());
		AfxMessageBox(_T("데이터 로드 중 오류 발생: ") + errorMsg);
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.


}

// COM 라이브러리 해제
void CMSSQLDlg::OnClose() {
	CoUninitialize(); // COM 라이브러리 해제
	CDialogEx::OnClose();
}

void CMSSQLDlg::OnBnClickedAddButton() {
	CString partNo;
	CString quantity;
	CString date;

	// 사용자 입력값 가져오기
	m_editPartNo.GetWindowText(partNo);
	m_editQuantity.GetWindowText(quantity);
	m_editDate.GetWindowText(date);

	if (partNo.IsEmpty() || quantity.IsEmpty() || date.IsEmpty()) {
		AfxMessageBox(_T("모든 필드를 입력해야 합니다."));
		return;
	}

	// 날짜와 시간을 현재로 설정
	COleDateTime currentTime = COleDateTime::GetCurrentTime();
	CString dateTimeString = date + _T(" ") + currentTime.Format(_T("%H:%M:%S"));

	try {
		// 데이터베이스 연결
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL 쿼리 작성
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		cmd->ActiveConnection = conn;

		cmd->CommandText = _bstr_t("INSERT INTO material_stock (partNo, quantity, date) VALUES (?, ?, ?)");
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("partNo"), adVarChar, adParamInput, 20, _bstr_t(partNo)));
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("quantity"), adDouble, adParamInput, 0, _variant_t(_ttof(quantity))));
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("date"), adDate, adParamInput, 0, _variant_t(dateTimeString)));

		cmd->Execute(NULL, NULL, adCmdText);

		AfxMessageBox(_T("데이터가 추가되었습니다."));
		conn->Close();
	}
	catch (_com_error &e) {
		CString errorMsg(e.ErrorMessage());
		AfxMessageBox(_T("데이터 추가 중 오류 발생: ") + errorMsg);
	}
}

// 검색 버튼 핸들러 추가
void CMSSQLDlg::OnBnClickedSearchButton() {
	CString partNo, quantity, date;
	m_editPartNo.GetWindowText(partNo);
	m_editQuantity.GetWindowText(quantity);
	m_editDate.GetWindowText(date); // 날짜 입력 필드에서 텍스트 가져오기

	if (partNo.IsEmpty() && quantity.IsEmpty() && date.IsEmpty()) {
		AfxMessageBox(_T("품번, 수량 또는 날짜를 입력하세요."));
		return;
	}

	try {
		// 데이터베이스 연결
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL 쿼리 작성
		CString sqlQuery = _T("SELECT partNo, quantity, date FROM material_stock WHERE ");
		bool firstCondition = true;

		if (!partNo.IsEmpty()) {
			sqlQuery += _T("partNo = ?");
			firstCondition = false;
		}

		if (!quantity.IsEmpty()) {
			if (!firstCondition) sqlQuery += _T(" OR ");
			sqlQuery += _T("quantity = ?");
			firstCondition = false;
		}

		if (!date.IsEmpty()) {
			if (!firstCondition) sqlQuery += _T(" OR ");
			sqlQuery += _T("CONVERT(VARCHAR(10), date, 120) = ?");
		}

		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		cmd->ActiveConnection = conn;
		cmd->CommandText = _bstr_t(sqlQuery);

		// 파라미터 추가
		if (!partNo.IsEmpty()) {
			cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("partNo"), adVarChar, adParamInput, 20, _bstr_t(partNo)));
		}
		if (!quantity.IsEmpty()) {
			cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("quantity"), adDouble, adParamInput, 0, _variant_t(_ttof(quantity))));
		}
		if (!date.IsEmpty()) {
			cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("date"), adVarChar, adParamInput, 10, _bstr_t(date)));
		}

		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);

		// 리스트 컨트롤 초기화
		m_listResult.DeleteAllItems(); // 이전 검색 결과 삭제

		int nItem = 0;
		CString resultPartNo, resultQuantity, resultDate;

		while (!rs->EndOfFile) {
			resultPartNo = (LPCTSTR)(_bstr_t)rs->Fields->Item["partNo"]->Value;
			resultQuantity = (LPCTSTR)(_bstr_t)rs->Fields->Item["quantity"]->Value;

			_variant_t dateVariant = rs->Fields->Item["date"]->Value;
			if (dateVariant.vt == VT_DATE) {
				COleDateTime oleDate = (COleDateTime)dateVariant;
				resultDate = oleDate.Format(_T("%Y-%m-%d %H:%M:%S"));
			}

			// 리스트 컨트롤에 결과 추가
			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), resultPartNo); // 첫 번째 열에 품번 추가
			m_listResult.SetItemText(nItem, 1, resultQuantity); // 두 번째 열에 수량 추가
			m_listResult.SetItemText(nItem, 2, resultDate);     // 세 번째 열에 날짜 추가

			rs->MoveNext();
		}

		if (m_listResult.GetItemCount() == 0) {
			AfxMessageBox(_T("해당 검색 조건에 맞는 데이터가 없습니다."));
		}

		rs->Close();
		conn->Close();
	}
	catch (_com_error &e) {
		CString errorMsg(e.ErrorMessage());
		AfxMessageBox(errorMsg);
	}
}

void CMSSQLDlg::OnBnClickedDeleteButton() {
	// 리스트 컨트롤에서 선택된 아이템의 인덱스 가져오기
	int nSelectedIndex = m_listResult.GetSelectionMark();
	if (nSelectedIndex == -1) {
		AfxMessageBox(_T("삭제할 항목을 선택하세요."));
		return;
	}

	// 선택된 항목의 Part No와 Quantity 가져오기
	CString partNo = m_listResult.GetItemText(nSelectedIndex, 0); // 첫 번째 열에서 Part No 가져오기
	CString quantity = m_listResult.GetItemText(nSelectedIndex, 1); // 두 번째 열에서 Quantity 가져오기

																	// 사용자 확인 메시지
	CString message;
	message.Format(_T("Part No: %s, Quantity: %s를 삭제하시겠습니까?"), partNo, quantity);
	if (AfxMessageBox(message, MB_YESNO | MB_ICONQUESTION) == IDNO) {
		return; // 사용자가 '아니오'를 선택하면 삭제하지 않음
	}

	try {
		// 데이터베이스 연결
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL 쿼리 작성
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		cmd->ActiveConnection = conn;

		cmd->CommandText = _bstr_t("DELETE FROM material_stock WHERE partNo = ? AND quantity = ?");
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("partNo"), adVarChar, adParamInput, 20, _bstr_t(partNo)));
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("quantity"), adDouble, adParamInput, 0, _variant_t(_ttof(quantity))));

		cmd->Execute(NULL, NULL, adCmdText);

		AfxMessageBox(_T("데이터가 삭제되었습니다."));
		conn->Close();

		// 삭제된 아이템 제거
		m_listResult.DeleteItem(nSelectedIndex); // 리스트에서 삭제된 아이템 제거
	}
	catch (_com_error &e) {
		CString errorMsg(e.ErrorMessage());
		AfxMessageBox(_T("데이터 삭제 중 오류 발생: ") + errorMsg);
	}
}

void CMSSQLDlg::OnBnClickedEditButton() {
	// 리스트 컨트롤에서 선택된 아이템의 인덱스 가져오기
	int nSelectedIndex = m_listResult.GetSelectionMark();
	if (nSelectedIndex == -1) {
		AfxMessageBox(_T("수정할 항목을 선택하세요."));
		return;
	}

	// 선택된 항목의 Part No 가져오기
	CString partNo = m_listResult.GetItemText(nSelectedIndex, 0);

	// 사용자에게 수정할 값을 입력받음
	CString newPartNo, newQuantity;
	m_editPartNo.GetWindowText(newPartNo);
	m_editQuantity.GetWindowText(newQuantity);

	// 사용자에게 수정 확인 메시지
	CString message;
	message.Format(_T("Part No: %s, Quantity: %s로 수정하시겠습니까?"), newPartNo, newQuantity);
	if (AfxMessageBox(message, MB_YESNO | MB_ICONQUESTION) == IDNO) {
		return; // 사용자가 '아니오'를 선택하면 수정하지 않음
	}

	// 현재 날짜와 시간을 가져옵니다.
	COleDateTime currentTime = COleDateTime::GetCurrentTime();
	CString newDate = currentTime.Format(_T("%Y-%m-%d %H:%M:%S")); // 원하는 형식으로 포맷

																   // 수정 기능 호출 (DB 업데이트)
	try {
		// 데이터베이스 연결
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL 쿼리 작성
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		cmd->ActiveConnection = conn;

		// 수정할 내용의 동적 SQL 생성
		CString sqlQuery = _T("UPDATE material_stock SET ");
		bool first = true;

		if (!newPartNo.IsEmpty()) {
			sqlQuery += _T("partNo = ? ");
			first = false;
		}
		if (!newQuantity.IsEmpty()) {
			if (!first) sqlQuery += _T(", ");
			sqlQuery += _T("quantity = ? ");
			first = false;
		}

		// 무조건 날짜를 업데이트하도록 쿼리에 추가
		if (!first) sqlQuery += _T(", ");
		sqlQuery += _T("date = ? ");

		sqlQuery += _T("WHERE partNo = ?");

		cmd->CommandText = _bstr_t(sqlQuery);

		// 파라미터 추가
		if (!newPartNo.IsEmpty()) {
			cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("newPartNo"), adVarChar, adParamInput, 20, _bstr_t(newPartNo)));
		}
		if (!newQuantity.IsEmpty()) {
			cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("newQuantity"), adDouble, adParamInput, 0, _variant_t(_ttof(newQuantity))));
		}

		// 현재 날짜와 시간을 파라미터에 추가
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("newDate"), adDate, adParamInput, 0, _variant_t(newDate)));

		// 기존 Part No 파라미터 추가
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("oldPartNo"), adVarChar, adParamInput, 20, _bstr_t(partNo)));

		cmd->Execute(NULL, NULL, adCmdText);

		AfxMessageBox(_T("데이터가 수정되었습니다."));
		conn->Close();

		// 리스트 컨트롤에서 수정된 아이템의 정보 업데이트
		if (!newPartNo.IsEmpty()) m_listResult.SetItemText(nSelectedIndex, 0, newPartNo); // 품번 업데이트
		if (!newQuantity.IsEmpty()) m_listResult.SetItemText(nSelectedIndex, 1, newQuantity); // 수량 업데이트
		m_listResult.SetItemText(nSelectedIndex, 2, newDate);     // 현재 시간으로 날짜 업데이트
	}
	catch (_com_error &e) {
		CString errorMsg;
		errorMsg.Format(_T("오류 발생: %s\n코드: %ld"), e.ErrorMessage(), e.Error());
		AfxMessageBox(errorMsg);
	}
}

void CMSSQLDlg::OnBnClickedRefreshButton() {
	// 데이터베이스에서 새로운 데이터를 가져와서 리스트를 갱신
	try {
		// 데이터베이스 연결
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL 쿼리 작성
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		cmd->ActiveConnection = conn;

		cmd->CommandText = _bstr_t("SELECT partNo, quantity, date FROM material_stock");

		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);

		// 리스트 컨트롤 초기화
		m_listResult.DeleteAllItems(); // 이전 데이터 삭제

									   // 새로운 데이터 추가
		int nItem = 0;
		CString partNo, quantity, date;

		while (!rs->EndOfFile) {
			partNo = (LPCTSTR)(_bstr_t)rs->Fields->Item["partNo"]->Value;
			quantity = (LPCTSTR)(_bstr_t)rs->Fields->Item["quantity"]->Value;

			_variant_t dateVariant = rs->Fields->Item["date"]->Value;
			if (dateVariant.vt == VT_DATE) {
				COleDateTime oleDate = (COleDateTime)dateVariant;
				date = oleDate.Format(_T("%Y-%m-%d %H:%M:%S"));
			}

			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), partNo);
			m_listResult.SetItemText(nItem, 1, quantity);
			m_listResult.SetItemText(nItem, 2, date);

			rs->MoveNext();
		}

		rs->Close();
		conn->Close();
	}
	catch (_com_error& e) {
		CString errorMsg(e.ErrorMessage());
		AfxMessageBox(_T("데이터 갱신 중 오류 발생: ") + errorMsg);
	}
}


void CMSSQLDlg::OnPaint() {
	if (IsIconic()) {
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialogEx::OnPaint();
	}
}

void CMSSQLDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	CDialogEx::OnSysCommand(nID, lParam);
}

HCURSOR CMSSQLDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}


void CMSSQLDlg::OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CMSSQLDlg::OnStnClickedStaticText()
{
	// TODO: Add your control notification handler code here
}
