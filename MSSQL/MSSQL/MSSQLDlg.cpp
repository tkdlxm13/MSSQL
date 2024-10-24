#include "stdafx.h"
#import "C:\\Program Files\\Common Files\\System\\ado\\msado15.dll" \
    no_namespace rename("EOF", "EndOfFile")
#include "MSSQLDlg.h"  // MSSQLDlg.h�� �����մϴ�.
#include "AboutDlg.h"  // CAboutDlg Ŭ���� ����
#include "afxdialogex.h"
#include <afxdb.h>
#include <comdef.h>  // _com_error ����
#include <afxwin.h> // MFC �⺻ ��� ����
#include "resource.h" // ���ҽ� ID ���� ����

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
	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CMSSQLDlg::OnBnClickedEditButton) // ���� ��ư �ڵ鷯 �߰�
	ON_BN_CLICKED(IDC_REFRESH_BUTTON, &CMSSQLDlg::OnBnClickedRefreshButton) // ��� ���� ��ư �ڵ鷯 �߰�
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULT, &CMSSQLDlg::OnLvnItemchangedListResult)
	ON_WM_CLOSE()
	ON_STN_CLICKED(IDC_STATIC_TEXT, &CMSSQLDlg::OnStnClickedStaticText)
END_MESSAGE_MAP()

// CMSSQLDlg ��ȭ ����
CMSSQLDlg::CMSSQLDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MSSQL_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMSSQLDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PARTNO, m_editPartNo);
	DDX_Control(pDX, IDC_EDIT_QUANTITY, m_editQuantity);
	DDX_Control(pDX, IDC_EDIT_DATE, m_editDate);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult); // ����Ʈ ��Ʈ�� ���ε�
	DDX_Control(pDX, IDC_STATIC_TEXT, m_staticText);
}

// CMSSQLDlg �޽��� ó����
BOOL CMSSQLDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	// �ؽ�Ʈ ��Ʈ�ѿ� ��Ʈ ����
	m_staticText.SetFont(&m_font); // m_staticText�� CStatic ����

								   // COM ���̺귯�� �ʱ�ȭ
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		AfxMessageBox(_T("COM ���̺귯�� �ʱ�ȭ ����"));
		return FALSE;
	}

	// ����Ʈ ��Ʈ�� �ʱ�ȭ
	m_listResult.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listResult.ModifyStyle(0, LVS_REPORT);
	m_listResult.InsertColumn(0, _T("ǰ��"), LVCFMT_LEFT, 100);
	m_listResult.InsertColumn(1, _T("����"), LVCFMT_LEFT, 100);
	m_listResult.InsertColumn(2, _T("�ֱ� ���� ��¥"), LVCFMT_LEFT, 150);

	// DB���� �����͸� �����ͼ� ����Ʈ ��Ʈ�ѿ� �߰�
	try {
		_ConnectionPtr conn;
		hr = conn.CreateInstance(__uuidof(Connection));
		if (FAILED(hr)) {
			AfxMessageBox(_T("Connection ��ü ���� ����"));
			return FALSE;
		}

		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		_CommandPtr cmd;
		hr = cmd.CreateInstance(__uuidof(Command));
		if (FAILED(hr)) {
			AfxMessageBox(_T("Command ��ü ���� ����"));
			return FALSE;
		}

		cmd->ActiveConnection = conn;
		cmd->CommandText = _bstr_t("SELECT partNo, quantity, date FROM material_stock");

		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);
		if (rs == NULL) {
			AfxMessageBox(_T("Recordset �������� ����"));
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
		AfxMessageBox(_T("������ �ε� �� ���� �߻�: ") + errorMsg);
	}

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.


}

// COM ���̺귯�� ����
void CMSSQLDlg::OnClose() {
	CoUninitialize(); // COM ���̺귯�� ����
	CDialogEx::OnClose();
}

void CMSSQLDlg::OnBnClickedAddButton() {
	CString partNo;
	CString quantity;
	CString date;

	// ����� �Է°� ��������
	m_editPartNo.GetWindowText(partNo);
	m_editQuantity.GetWindowText(quantity);
	m_editDate.GetWindowText(date);

	if (partNo.IsEmpty() || quantity.IsEmpty() || date.IsEmpty()) {
		AfxMessageBox(_T("��� �ʵ带 �Է��ؾ� �մϴ�."));
		return;
	}

	// ��¥�� �ð��� ����� ����
	COleDateTime currentTime = COleDateTime::GetCurrentTime();
	CString dateTimeString = date + _T(" ") + currentTime.Format(_T("%H:%M:%S"));

	try {
		// �����ͺ��̽� ����
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL ���� �ۼ�
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		cmd->ActiveConnection = conn;

		cmd->CommandText = _bstr_t("INSERT INTO material_stock (partNo, quantity, date) VALUES (?, ?, ?)");
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("partNo"), adVarChar, adParamInput, 20, _bstr_t(partNo)));
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("quantity"), adDouble, adParamInput, 0, _variant_t(_ttof(quantity))));
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("date"), adDate, adParamInput, 0, _variant_t(dateTimeString)));

		cmd->Execute(NULL, NULL, adCmdText);

		AfxMessageBox(_T("�����Ͱ� �߰��Ǿ����ϴ�."));
		conn->Close();
	}
	catch (_com_error &e) {
		CString errorMsg(e.ErrorMessage());
		AfxMessageBox(_T("������ �߰� �� ���� �߻�: ") + errorMsg);
	}
}

// �˻� ��ư �ڵ鷯 �߰�
void CMSSQLDlg::OnBnClickedSearchButton() {
	CString partNo, quantity, date;
	m_editPartNo.GetWindowText(partNo);
	m_editQuantity.GetWindowText(quantity);
	m_editDate.GetWindowText(date); // ��¥ �Է� �ʵ忡�� �ؽ�Ʈ ��������

	if (partNo.IsEmpty() && quantity.IsEmpty() && date.IsEmpty()) {
		AfxMessageBox(_T("ǰ��, ���� �Ǵ� ��¥�� �Է��ϼ���."));
		return;
	}

	try {
		// �����ͺ��̽� ����
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL ���� �ۼ�
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

		// �Ķ���� �߰�
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

		// ����Ʈ ��Ʈ�� �ʱ�ȭ
		m_listResult.DeleteAllItems(); // ���� �˻� ��� ����

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

			// ����Ʈ ��Ʈ�ѿ� ��� �߰�
			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), resultPartNo); // ù ��° ���� ǰ�� �߰�
			m_listResult.SetItemText(nItem, 1, resultQuantity); // �� ��° ���� ���� �߰�
			m_listResult.SetItemText(nItem, 2, resultDate);     // �� ��° ���� ��¥ �߰�

			rs->MoveNext();
		}

		if (m_listResult.GetItemCount() == 0) {
			AfxMessageBox(_T("�ش� �˻� ���ǿ� �´� �����Ͱ� �����ϴ�."));
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
	// ����Ʈ ��Ʈ�ѿ��� ���õ� �������� �ε��� ��������
	int nSelectedIndex = m_listResult.GetSelectionMark();
	if (nSelectedIndex == -1) {
		AfxMessageBox(_T("������ �׸��� �����ϼ���."));
		return;
	}

	// ���õ� �׸��� Part No�� Quantity ��������
	CString partNo = m_listResult.GetItemText(nSelectedIndex, 0); // ù ��° ������ Part No ��������
	CString quantity = m_listResult.GetItemText(nSelectedIndex, 1); // �� ��° ������ Quantity ��������

																	// ����� Ȯ�� �޽���
	CString message;
	message.Format(_T("Part No: %s, Quantity: %s�� �����Ͻðڽ��ϱ�?"), partNo, quantity);
	if (AfxMessageBox(message, MB_YESNO | MB_ICONQUESTION) == IDNO) {
		return; // ����ڰ� '�ƴϿ�'�� �����ϸ� �������� ����
	}

	try {
		// �����ͺ��̽� ����
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL ���� �ۼ�
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		cmd->ActiveConnection = conn;

		cmd->CommandText = _bstr_t("DELETE FROM material_stock WHERE partNo = ? AND quantity = ?");
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("partNo"), adVarChar, adParamInput, 20, _bstr_t(partNo)));
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("quantity"), adDouble, adParamInput, 0, _variant_t(_ttof(quantity))));

		cmd->Execute(NULL, NULL, adCmdText);

		AfxMessageBox(_T("�����Ͱ� �����Ǿ����ϴ�."));
		conn->Close();

		// ������ ������ ����
		m_listResult.DeleteItem(nSelectedIndex); // ����Ʈ���� ������ ������ ����
	}
	catch (_com_error &e) {
		CString errorMsg(e.ErrorMessage());
		AfxMessageBox(_T("������ ���� �� ���� �߻�: ") + errorMsg);
	}
}

void CMSSQLDlg::OnBnClickedEditButton() {
	// ����Ʈ ��Ʈ�ѿ��� ���õ� �������� �ε��� ��������
	int nSelectedIndex = m_listResult.GetSelectionMark();
	if (nSelectedIndex == -1) {
		AfxMessageBox(_T("������ �׸��� �����ϼ���."));
		return;
	}

	// ���õ� �׸��� Part No ��������
	CString partNo = m_listResult.GetItemText(nSelectedIndex, 0);

	// ����ڿ��� ������ ���� �Է¹���
	CString newPartNo, newQuantity;
	m_editPartNo.GetWindowText(newPartNo);
	m_editQuantity.GetWindowText(newQuantity);

	// ����ڿ��� ���� Ȯ�� �޽���
	CString message;
	message.Format(_T("Part No: %s, Quantity: %s�� �����Ͻðڽ��ϱ�?"), newPartNo, newQuantity);
	if (AfxMessageBox(message, MB_YESNO | MB_ICONQUESTION) == IDNO) {
		return; // ����ڰ� '�ƴϿ�'�� �����ϸ� �������� ����
	}

	// ���� ��¥�� �ð��� �����ɴϴ�.
	COleDateTime currentTime = COleDateTime::GetCurrentTime();
	CString newDate = currentTime.Format(_T("%Y-%m-%d %H:%M:%S")); // ���ϴ� �������� ����

																   // ���� ��� ȣ�� (DB ������Ʈ)
	try {
		// �����ͺ��̽� ����
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL ���� �ۼ�
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		cmd->ActiveConnection = conn;

		// ������ ������ ���� SQL ����
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

		// ������ ��¥�� ������Ʈ�ϵ��� ������ �߰�
		if (!first) sqlQuery += _T(", ");
		sqlQuery += _T("date = ? ");

		sqlQuery += _T("WHERE partNo = ?");

		cmd->CommandText = _bstr_t(sqlQuery);

		// �Ķ���� �߰�
		if (!newPartNo.IsEmpty()) {
			cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("newPartNo"), adVarChar, adParamInput, 20, _bstr_t(newPartNo)));
		}
		if (!newQuantity.IsEmpty()) {
			cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("newQuantity"), adDouble, adParamInput, 0, _variant_t(_ttof(newQuantity))));
		}

		// ���� ��¥�� �ð��� �Ķ���Ϳ� �߰�
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("newDate"), adDate, adParamInput, 0, _variant_t(newDate)));

		// ���� Part No �Ķ���� �߰�
		cmd->Parameters->Append(cmd->CreateParameter(_bstr_t("oldPartNo"), adVarChar, adParamInput, 20, _bstr_t(partNo)));

		cmd->Execute(NULL, NULL, adCmdText);

		AfxMessageBox(_T("�����Ͱ� �����Ǿ����ϴ�."));
		conn->Close();

		// ����Ʈ ��Ʈ�ѿ��� ������ �������� ���� ������Ʈ
		if (!newPartNo.IsEmpty()) m_listResult.SetItemText(nSelectedIndex, 0, newPartNo); // ǰ�� ������Ʈ
		if (!newQuantity.IsEmpty()) m_listResult.SetItemText(nSelectedIndex, 1, newQuantity); // ���� ������Ʈ
		m_listResult.SetItemText(nSelectedIndex, 2, newDate);     // ���� �ð����� ��¥ ������Ʈ
	}
	catch (_com_error &e) {
		CString errorMsg;
		errorMsg.Format(_T("���� �߻�: %s\n�ڵ�: %ld"), e.ErrorMessage(), e.Error());
		AfxMessageBox(errorMsg);
	}
}

void CMSSQLDlg::OnBnClickedRefreshButton() {
	// �����ͺ��̽����� ���ο� �����͸� �����ͼ� ����Ʈ�� ����
	try {
		// �����ͺ��̽� ����
		_ConnectionPtr conn;
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		// SQL ���� �ۼ�
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		cmd->ActiveConnection = conn;

		cmd->CommandText = _bstr_t("SELECT partNo, quantity, date FROM material_stock");

		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);

		// ����Ʈ ��Ʈ�� �ʱ�ȭ
		m_listResult.DeleteAllItems(); // ���� ������ ����

									   // ���ο� ������ �߰�
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
		AfxMessageBox(_T("������ ���� �� ���� �߻�: ") + errorMsg);
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
