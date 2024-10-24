#include "stdafx.h"  // ǥ�� ��� ����
#import "C:\\Program Files\\Common Files\\System\\ado\\msado15.dll" \
    no_namespace rename("EOF", "EndOfFile")
#include "MSSQLDlg.h"  // MSSQLDlg.h�� �����մϴ�.
#include "AboutDlg.h"  // CAboutDlg Ŭ���� ����
#include "afxdialogex.h"  // MFC ��ȭ���� Ŭ���� Ȯ�� ����
#include <afxdb.h>  // MFC �����ͺ��̽� ���� ���
#include <comdef.h>  // COM ���� ���� ����
#include <afxwin.h> // MFC �⺻ ��� ����
#include "resource.h" // ���ҽ� ID ���� ����

#ifdef _DEBUG
#define new DEBUG_NEW  // ����� ��忡�� �� �޸� �Ҵ� ����
#endif

// �޽��� �� ����
BEGIN_MESSAGE_MAP(CMSSQLDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CMSSQLDlg::OnBnClickedAddButton)  // �߰� ��ư Ŭ�� �̺�Ʈ
	ON_BN_CLICKED(IDC_SEARCH_BUTTON, &CMSSQLDlg::OnBnClickedSearchButton)  // �˻� ��ư Ŭ�� �̺�Ʈ
	ON_WM_PAINT()  // ����Ʈ �̺�Ʈ ó��
	ON_WM_SYSCOMMAND()  // �ý��� ��� �̺�Ʈ ó��
	ON_WM_QUERYDRAGICON()  // �巡�� ������ ���� ó��
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMSSQLDlg::OnBnClickedDeleteButton)  // ���� ��ư Ŭ�� �̺�Ʈ
	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CMSSQLDlg::OnBnClickedEditButton) // ���� ��ư Ŭ�� �̺�Ʈ
	ON_BN_CLICKED(IDC_REFRESH_BUTTON, &CMSSQLDlg::OnBnClickedRefreshButton) // ��� ���� ��ư Ŭ�� �̺�Ʈ
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULT, &CMSSQLDlg::OnLvnItemchangedListResult) // ����Ʈ ������ ���� �̺�Ʈ
	ON_WM_CLOSE()  // ��ȭ���� �ݱ� �̺�Ʈ ó��
	ON_STN_CLICKED(IDC_STATIC_TEXT, &CMSSQLDlg::OnStnClickedStaticText)  // ���� �ؽ�Ʈ Ŭ�� �̺�Ʈ
END_MESSAGE_MAP()

// CMSSQLDlg ��ȭ ���� ������
CMSSQLDlg::CMSSQLDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MSSQL_DIALOG, pParent) {  // �θ� â�� ���ڷ� ����
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);  // ������ �ε�
}

// ������ ���ε� �Լ�
void CMSSQLDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);  // �⺻ ������ ��ȯ ȣ��
	DDX_Control(pDX, IDC_EDIT_PARTNO, m_editPartNo);  // ǰ�� ���� ��Ʈ�� ���ε�
	DDX_Control(pDX, IDC_EDIT_QUANTITY, m_editQuantity);  // ���� ���� ��Ʈ�� ���ε�
	DDX_Control(pDX, IDC_EDIT_DATE, m_editDate);  // ��¥ ���� ��Ʈ�� ���ε�
	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult); // ����Ʈ ��Ʈ�� ���ε�
	DDX_Control(pDX, IDC_STATIC_TEXT, m_staticText);  // ���� �ؽ�Ʈ ���ε�
}

// CMSSQLDlg �޽��� ó����
BOOL CMSSQLDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();  // �⺻ ��ȭ���� �ʱ�ȭ ȣ��

	// �ؽ�Ʈ ��Ʈ�ѿ� ��Ʈ ����
	m_staticText.SetFont(&m_font); // m_staticText�� CStatic ����

	// COM ���̺귯�� �ʱ�ȭ
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {  // �ʱ�ȭ ���� �� �޽��� �ڽ� ǥ��
		AfxMessageBox(_T("COM ���̺귯�� �ʱ�ȭ ����"));
		return FALSE;  // �ʱ�ȭ ����
	}

	// ����Ʈ ��Ʈ�� �ʱ�ȭ
	m_listResult.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  // ��ü �� ���� �� �׸������ ��Ÿ�� ����
	m_listResult.ModifyStyle(0, LVS_REPORT);  // ����Ʈ ��Ÿ�Ϸ� ����
	m_listResult.InsertColumn(0, _T("ǰ��"), LVCFMT_LEFT, 100);  // ù ��° ���� ǰ�� �߰�
	m_listResult.InsertColumn(1, _T("����"), LVCFMT_LEFT, 100);  // �� ��° ���� ���� �߰�
	m_listResult.InsertColumn(2, _T("�ֱ� ���� ��¥"), LVCFMT_LEFT, 150);  // �� ��° ���� �ֱ� ���� ��¥ �߰�

	// DB���� �����͸� �����ͼ� ����Ʈ ��Ʈ�ѿ� �߰�
	try {
		_ConnectionPtr conn;  // ADO ���� ������
		hr = conn.CreateInstance(__uuidof(Connection));  // ���� ��ü ����
		if (FAILED(hr)) {  // ���� ���� �� �޽��� �ڽ� ǥ��
			AfxMessageBox(_T("Connection ��ü ���� ����"));
			return FALSE;  // ����
		}

		// �����ͺ��̽� ����
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);

		_CommandPtr cmd;  // ADO Ŀ�ǵ� ������
		hr = cmd.CreateInstance(__uuidof(Command));  // Ŀ�ǵ� ��ü ����
		if (FAILED(hr)) {  // ���� ���� �� �޽��� �ڽ� ǥ��
			AfxMessageBox(_T("Command ��ü ���� ����"));
			return FALSE;  // ����
		}

		cmd->ActiveConnection = conn;  // ���� ���� ����
		cmd->CommandText = _bstr_t("SELECT partNo, quantity, date FROM material_stock");  // SQL ���� ����

		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);  // SQL ���� ����
		if (rs == NULL) {  // ����� ������ �޽��� �ڽ� ǥ��
			AfxMessageBox(_T("Recordset �������� ����"));
			return FALSE;  // ����
		}

		int nItem = 0;  // ����Ʈ ������ �ε��� �ʱ�ȭ
		CString partNo, quantity, date;  // ǰ��, ����, ��¥�� ������ ����

		while (!rs->EndOfFile) {  // ����� ���� ������ ������ �ݺ�
			partNo = (LPCTSTR)(_bstr_t)rs->Fields->Item["partNo"]->Value;  // ǰ�� ��������
			quantity = (LPCTSTR)(_bstr_t)rs->Fields->Item["quantity"]->Value;  // ���� ��������

			_variant_t dateVariant = rs->Fields->Item["date"]->Value;  // ��¥ �ʵ� ��������
			if (dateVariant.vt == VT_DATE) {  // ��¥ ������ ������
				COleDateTime oleDate = (COleDateTime)dateVariant;  // COleDateTime���� ��ȯ
				date = oleDate.Format(_T("%Y-%m-%d %H:%M:%S"));  // ���ϴ� �������� ����
			}

			// ����Ʈ ��Ʈ�ѿ� ������ �߰�
			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), partNo);  // ǰ�� �߰�
			m_listResult.SetItemText(nItem, 1, quantity);  // ���� �߰�
			m_listResult.SetItemText(nItem, 2, date);  // ��¥ �߰�

			rs->MoveNext();  // ���� ���ڵ�� �̵�
		}

		rs->Close();  // ���ڵ�� �ݱ�
		conn->Close();  // ���� �ݱ�
	}
	catch (_com_error& e) {  // COM ���� ó��
		CString errorMsg(e.ErrorMessage());  // ���� �޽��� ��������
		AfxMessageBox(_T("������ �ε� �� ���� �߻�: ") + errorMsg);  // ���� �޽��� ǥ��
	}

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// COM ���̺귯�� ����
void CMSSQLDlg::OnClose() {
	CoUninitialize(); // COM ���̺귯�� ����
	CDialogEx::OnClose();  // �⺻ �ݱ� ȣ��
}

// �߰� ��ư Ŭ�� ó��
void CMSSQLDlg::OnBnClickedAddButton() {
	CString partNo;  // ǰ��
	CString quantity;  // ����
	CString date;  // ��¥

	// ����� �Է°� ��������
	m_editPartNo.GetWindowText(partNo);  // ǰ�� �Է°� ��������
	m_editQuantity.GetWindowText(quantity);  // ���� �Է°� ��������
	m_editDate.GetWindowText(date);  // ��¥ �Է°� ��������

	// �Է� �ʵ尡 ��� �ִ��� Ȯ��
	if (partNo.IsEmpty() || quantity.IsEmpty() || date.IsEmpty()) {
		AfxMessageBox(_T("��� �ʵ带 �Է����ּ���."));  // ��� �޽���
		return;  // �Լ� ����
	}

	// �����ͺ��̽��� �߰�
	try {
		_ConnectionPtr conn;  // ADO ���� ������
		conn.CreateInstance(__uuidof(Connection));  // ���� ��ü ����
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // �����ͺ��̽� ����

		_CommandPtr cmd;  // ADO Ŀ�ǵ� ������
		cmd.CreateInstance(__uuidof(Command));  // Ŀ�ǵ� ��ü ����
		cmd->ActiveConnection = conn;  // ���� ���� ����

		// SQL ���� �ۼ�
		CString sqlQuery;
		sqlQuery.Format(_T("INSERT INTO material_stock (partNo, quantity, date) VALUES ('%s', %s, '%s')"), partNo, quantity, date);  // INSERT ���� ����
		cmd->CommandText = _bstr_t(sqlQuery);  // SQL ���� ����

		cmd->Execute(NULL, NULL, adCmdText);  // SQL ���� ����
		AfxMessageBox(_T("������ �߰� ����!"));  // ���� �޽���

		// �߰� �� ����Ʈ ����
		OnBnClickedRefreshButton();  // ����Ʈ ���� �Լ� ȣ��

		conn->Close();  // ���� �ݱ�
	}
	catch (_com_error& e) {  // COM ���� ó��
		CString errorMsg(e.ErrorMessage());  // ���� �޽��� ��������
		AfxMessageBox(_T("������ �߰� �� ���� �߻�: ") + errorMsg);  // ���� �޽��� ǥ��
	}
}

// �˻� ��ư Ŭ�� ó��
void CMSSQLDlg::OnBnClickedSearchButton() {
	CString partNo;  // ǰ��
	m_editPartNo.GetWindowText(partNo);  // ǰ�� �Է°� ��������

	// ǰ�� �Է°��� ��� �ִ��� Ȯ��
	if (partNo.IsEmpty()) {
		AfxMessageBox(_T("ǰ���� �Է����ּ���."));  // ��� �޽���
		return;  // �Լ� ����
	}

	// �����ͺ��̽����� �˻�
	try {
		_ConnectionPtr conn;  // ADO ���� ������
		conn.CreateInstance(__uuidof(Connection));  // ���� ��ü ����
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // �����ͺ��̽� ����

		_CommandPtr cmd;  // ADO Ŀ�ǵ� ������
		cmd.CreateInstance(__uuidof(Command));  // Ŀ�ǵ� ��ü ����
		cmd->ActiveConnection = conn;  // ���� ���� ����

		// SQL ���� �ۼ�
		CString sqlQuery;
		sqlQuery.Format(_T("SELECT partNo, quantity, date FROM material_stock WHERE partNo = '%s'"), partNo);  // ǰ������ �˻��ϴ� ����
		cmd->CommandText = _bstr_t(sqlQuery);  // SQL ���� ����

		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);  // SQL ���� ����
		if (rs == NULL) {  // ����� ������ �޽��� �ڽ� ǥ��
			AfxMessageBox(_T("�˻� ����� �����ϴ�."));  // ��� ���� �޽���
			return;  // �Լ� ����
		}

		// ����Ʈ Ŭ����
		m_listResult.DeleteAllItems();  // ����Ʈ ������ ����

										// �˻� ����� ����Ʈ�� �߰�
		int nItem = 0;  // ����Ʈ ������ �ε��� �ʱ�ȭ
		CString quantity, date;  // ����, ��¥�� ������ ����

		while (!rs->EndOfFile) {  // ����� ���� ������ ������ �ݺ�
								  // ������ ��������
			CString partNo = (LPCTSTR)(_bstr_t)rs->Fields->Item["partNo"]->Value;  // ǰ�� ��������
			quantity = (LPCTSTR)(_bstr_t)rs->Fields->Item["quantity"]->Value;  // ���� ��������

			_variant_t dateVariant = rs->Fields->Item["date"]->Value;  // ��¥ �ʵ� ��������
			if (dateVariant.vt == VT_DATE) {  // ��¥ ������ ������
				COleDateTime oleDate = (COleDateTime)dateVariant;  // COleDateTime���� ��ȯ
				date = oleDate.Format(_T("%Y-%m-%d %H:%M:%S"));  // ���ϴ� �������� ����
			}

			// ����Ʈ ��Ʈ�ѿ� ������ �߰�
			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), partNo);  // ǰ�� �߰�
			m_listResult.SetItemText(nItem, 1, quantity);  // ���� �߰�
			m_listResult.SetItemText(nItem, 2, date);  // ��¥ �߰�

			rs->MoveNext();  // ���� ���ڵ�� �̵�
		}

		rs->Close();  // ���ڵ�� �ݱ�
		conn->Close();  // ���� �ݱ�
	}
	catch (_com_error& e) {  // COM ���� ó��
		CString errorMsg(e.ErrorMessage());  // ���� �޽��� ��������
		AfxMessageBox(_T("������ �˻� �� ���� �߻�: ") + errorMsg);  // ���� �޽��� ǥ��
	}
}

// ���� ��ư Ŭ�� ó��
void CMSSQLDlg::OnBnClickedEditButton() {
	CString partNo;  // ǰ��
	CString quantity;  // ����
	CString date;  // ��¥

	// �Է°� ��������
	m_editPartNo.GetWindowText(partNo);  // ǰ�� �Է°� ��������
	m_editQuantity.GetWindowText(quantity);  // ���� �Է°� ��������
	m_editDate.GetWindowText(date);  // ��¥ �Է°� ��������

	// �Է� �ʵ尡 ��� �ִ��� Ȯ��
	if (partNo.IsEmpty() || quantity.IsEmpty() || date.IsEmpty()) {
		AfxMessageBox(_T("��� �ʵ带 �Է����ּ���."));  // ��� �޽���
		return;  // �Լ� ����
	}

	// �����ͺ��̽����� ����
	try {
		_ConnectionPtr conn;  // ADO ���� ������
		conn.CreateInstance(__uuidof(Connection));  // ���� ��ü ����
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // �����ͺ��̽� ����

		_CommandPtr cmd;  // ADO Ŀ�ǵ� ������
		cmd.CreateInstance(__uuidof(Command));  // Ŀ�ǵ� ��ü ����
		cmd->ActiveConnection = conn;  // ���� ���� ����

		// SQL ���� �ۼ�
		CString sqlQuery;
		sqlQuery.Format(_T("UPDATE material_stock SET quantity = %s, date = '%s' WHERE partNo = '%s'"), quantity, date, partNo);  // UPDATE ���� ����
		cmd->CommandText = _bstr_t(sqlQuery);  // SQL ���� ����

		cmd->Execute(NULL, NULL, adCmdText);  // SQL ���� ����
		AfxMessageBox(_T("������ ���� ����!"));  // ���� �޽���

		// ���� �� ����Ʈ ����
		OnBnClickedRefreshButton();  // ����Ʈ ���� �Լ� ȣ��

		conn->Close();  // ���� �ݱ�
	}
	catch (_com_error& e) {  // COM ���� ó��
		CString errorMsg(e.ErrorMessage());  // ���� �޽��� ��������
		AfxMessageBox(_T("������ ���� �� ���� �߻�: ") + errorMsg);  // ���� �޽��� ǥ��
	}
}

// ���� ��ư Ŭ�� ó��
void CMSSQLDlg::OnBnClickedDeleteButton() {
	CString partNo;  // ǰ��
	m_editPartNo.GetWindowText(partNo);  // ǰ�� �Է°� ��������

	// ǰ�� �Է°��� ��� �ִ��� Ȯ��
	if (partNo.IsEmpty()) {
		AfxMessageBox(_T("������ ǰ���� �Է����ּ���."));  // ��� �޽���
		return;  // �Լ� ����
	}

	// �����ͺ��̽����� ����
	try {
		_ConnectionPtr conn;  // ADO ���� ������
		conn.CreateInstance(__uuidof(Connection));  // ���� ��ü ����
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // �����ͺ��̽� ����

		_CommandPtr cmd;  // ADO Ŀ�ǵ� ������
		cmd.CreateInstance(__uuidof(Command));  // Ŀ�ǵ� ��ü ����
		cmd->ActiveConnection = conn;  // ���� ���� ����

		// SQL ���� �ۼ�
		CString sqlQuery;
		sqlQuery.Format(_T("DELETE FROM material_stock WHERE partNo = '%s'"), partNo);  // DELETE ���� ����
		cmd->CommandText = _bstr_t(sqlQuery);  // SQL ���� ����

		cmd->Execute(NULL, NULL, adCmdText);  // SQL ���� ����
		AfxMessageBox(_T("������ ���� ����!"));  // ���� �޽���

		// ���� �� ����Ʈ ����
		OnBnClickedRefreshButton();  // ����Ʈ ���� �Լ� ȣ��

		conn->Close();  // ���� �ݱ�
	}
	catch (_com_error& e) {  // COM ���� ó��
		CString errorMsg(e.ErrorMessage());  // ���� �޽��� ��������
		AfxMessageBox(_T("������ ���� �� ���� �߻�: ") + errorMsg);  // ���� �޽��� ǥ��
	}
}

void CMSSQLDlg::OnBnClickedRefreshButton() {
	// �����ͺ��̽����� ���ο� �����͸� �����ͼ� ����Ʈ�� �����ϴ� �Լ�
	try {
		// �����ͺ��̽� ����
		_ConnectionPtr conn;  // ADO ���� ������ ����
		conn.CreateInstance(__uuidof(Connection));  // ���� ��ü ����
		conn->Open(_bstr_t("Provider=SQLOLEDB;Data Source=DESKTOP-JQ0BOFV;Initial Catalog=Material_StockDB;Integrated Security=SSPI;"), "", "", adConnectUnspecified);  // �����ͺ��̽� ����

																																										// SQL ���� �ۼ�
		_CommandPtr cmd;  // ADO Ŀ�ǵ� ������ ����
		cmd.CreateInstance(__uuidof(Command));  // Ŀ�ǵ� ��ü ����
		cmd->ActiveConnection = conn;  // ���� ���� ����

		// material_stock ���̺��� ������ ����
		cmd->CommandText = _bstr_t("SELECT partNo, quantity, date FROM material_stock");

		// ���� ���� �� ��� ���� ��������
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdText);

		// ����Ʈ ��Ʈ�� �ʱ�ȭ
		m_listResult.DeleteAllItems();  // ���� ������ ����

		// ���ο� ������ �߰�
		int nItem = 0;  // ����Ʈ�� ������ �ε��� �ʱ�ȭ
		CString partNo, quantity, date;  // ǰ��, ����, ��¥�� ������ ���� ����

		// ��� ���տ��� �����͸� �о� ����Ʈ�� �߰�
		while (!rs->EndOfFile) {  // ��� ������ ������ �ݺ�
								  // �� �ʵ��� �� ��������
			partNo = (LPCTSTR)(_bstr_t)rs->Fields->Item["partNo"]->Value;  // ǰ��
			quantity = (LPCTSTR)(_bstr_t)rs->Fields->Item["quantity"]->Value;  // ����

			// ��¥ ���� ��ȯ
			_variant_t dateVariant = rs->Fields->Item["date"]->Value;  // ��¥ �ʵ� �� ��������
			if (dateVariant.vt == VT_DATE) {  // ��¥ ���� Ȯ��
				COleDateTime oleDate = (COleDateTime)dateVariant;  // COleDateTime ��ü�� ��ȯ
				date = oleDate.Format(_T("%Y-%m-%d %H:%M:%S"));  // ���ڿ��� ����
			}

			// ����Ʈ�� ���ο� ������ �߰�
			nItem = m_listResult.InsertItem(m_listResult.GetItemCount(), partNo);  // ǰ���� ù ��° ���� �߰�
			m_listResult.SetItemText(nItem, 1, quantity);  // ������ �� ��° ���� �߰�
			m_listResult.SetItemText(nItem, 2, date);  // ��¥�� �� ��° ���� �߰�

			rs->MoveNext();  // ���� ���ڵ�� �̵�
		}

		rs->Close();  // ��� ���� �ݱ�
		conn->Close();  // �����ͺ��̽� ���� �ݱ�
	}
	catch (_com_error& e) {  // ���� ó��
		CString errorMsg(e.ErrorMessage());  // ���� �޽��� ��������
		AfxMessageBox(_T("������ ���� �� ���� �߻�: ") + errorMsg);  // ���� �޽��� ǥ��
	}
}

void CMSSQLDlg::OnPaint() {
	// �������� ������ ó��
	if (IsIconic()) {  // ������ �������� Ȯ��
		CPaintDC dc(this);  // �׸��� DC ����
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);  // ������ ��� �����
		int cxIcon = GetSystemMetrics(SM_CXICON);  // �������� �ʺ�
		int cyIcon = GetSystemMetrics(SM_CYICON);  // �������� ����
		CRect rect;
		GetClientRect(&rect);  // Ŭ���̾�Ʈ ������ ũ�� ��������
		int x = (rect.Width() - cxIcon + 1) / 2;  // �������� �߾ӿ� ��ġ�ϱ� ���� X ��ǥ ���
		int y = (rect.Height() - cyIcon + 1) / 2;  // �������� �߾ӿ� ��ġ�ϱ� ���� Y ��ǥ ���
		dc.DrawIcon(x, y, m_hIcon);  // ������ �׸���
	}
	else {
		CDialogEx::OnPaint();  // �⺻ ������ ó��
	}
}

void CMSSQLDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	// �ý��� ��� ó��
	CDialogEx::OnSysCommand(nID, lParam);  // �⺻ �ý��� ��� ó��
}

HCURSOR CMSSQLDlg::OnQueryDragIcon() {
	// ������ �巡�� �̺�Ʈ ó��
	return static_cast<HCURSOR>(m_hIcon);  // ������ Ŀ�� ��ȯ
}

void CMSSQLDlg::OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);  // ����Ʈ ���� �˸� �޽��� ����ü�� ĳ����
	 // TODO: Add your control notification handler code here
	*pResult = 0;  // ��� �� ����
}

void CMSSQLDlg::OnStnClickedStaticText() {
	// ���� �ؽ�Ʈ Ŭ�� �̺�Ʈ ó��
	// TODO: Add your control notification handler code here
}
