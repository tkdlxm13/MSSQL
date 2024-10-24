#pragma once

// CMSSQLDlg ��ȭ ���� Ŭ����
class CMSSQLDlg : public CDialogEx
{
public:
	CMSSQLDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MSSQL_DIALOG }; // ��ȭ ���� ���ҽ� ID
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

protected:
	HICON m_hIcon;  // ��ȭ ������ ������ �ڵ�

private:
	CFont m_font; // ����� ���� ��Ʈ ��ü

protected:
	virtual void OnClose(); // ��ȭ ���ڰ� ���� �� ȣ��Ǵ� �Լ�

	virtual BOOL OnInitDialog(); // ��ȭ ���� �ʱ�ȭ �Լ�
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam); // �ý��� ��� ó�� �Լ�
	afx_msg void OnPaint(); // ������ ó�� �Լ�
	afx_msg HCURSOR OnQueryDragIcon(); // ������ �巡�� �̺�Ʈ ó�� �Լ�
	afx_msg void OnBnClickedSearchButton(); // �˻� ��ư Ŭ�� �̺�Ʈ ó�� �Լ�
	afx_msg void OnBnClickedAddButton(); // �߰� ��ư Ŭ�� �̺�Ʈ ó�� �Լ�
	afx_msg void OnBnClickedDeleteButton(); // ���� ��ư Ŭ�� �̺�Ʈ ó�� �Լ�
	afx_msg void OnBnClickedEditButton(); // ���� ��ư Ŭ�� �̺�Ʈ ó�� �Լ�
	afx_msg void OnBnClickedRefreshButton(); // ���� ��ħ ��ư Ŭ�� �̺�Ʈ ó�� �Լ�
	DECLARE_MESSAGE_MAP() // �޽��� �� ��ũ��

						  // Edit Control ���� ����
	CEdit m_editPartNo;     // partNo �Է� ��Ʈ��
	CEdit m_editQuantity;   // quantity �Է� ��Ʈ��
	CEdit m_editDate;       // date �Է� ��Ʈ��
	CListCtrl m_listResult; // ����� ����� ����Ʈ ��Ʈ��
	CEdit m_editNewPartNo;     // ���ο� partNo �Է� ��Ʈ��
	CEdit m_editNewQuantity;   // ���ο� quantity �Է� ��Ʈ��
	CEdit m_editNewDate;       // ���ο� date �Է� ��Ʈ��
	CEdit m_editSearchValue;   // �˻� �Է� �ʵ�
	CComboBox m_comboSearchCriteria; // �˻� ���� �޺� �ڽ�
	CStatic m_staticText; // ���� �ؽ�Ʈ ��Ʈ��

public:
	afx_msg void OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult); // ����Ʈ �׸� ���� �̺�Ʈ ó�� �Լ�
	afx_msg void OnStnClickedStaticText(); // ���� �ؽ�Ʈ Ŭ�� �̺�Ʈ ó�� �Լ�
};