#pragma once

// CMSSQLDlg ��ȭ ����
class CMSSQLDlg : public CDialogEx
{
public:
	CMSSQLDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MSSQL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

protected:
	HICON m_hIcon;

private:
	CFont m_font; // ��Ʈ ��ü

protected:
	virtual void OnClose(); // OnClose �Լ� ����

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

	// Edit Control ���� ����
	CEdit m_editPartNo;     // partNo �Է� ��Ʈ��
	CEdit m_editQuantity;   // quantity �Է� ��Ʈ��
	CEdit m_editDate;       // date �Է� ��Ʈ��
	CListCtrl m_listResult; // ����� ����� ����Ʈ ��Ʈ�� �߰�
	CEdit m_editNewPartNo;     // ���ο� partNo �Է� ��Ʈ��
	CEdit m_editNewQuantity;   // ���ο� quantity �Է� ��Ʈ��
	CEdit m_editNewDate;       // ���ο� date �Է� ��Ʈ��
	CEdit m_editSearchValue;         // �˻� �Է� �ʵ�
	CComboBox m_comboSearchCriteria;  // �˻� ���� �޺� �ڽ�
	CStatic m_staticText; // CStatic ���� �߰�

public:
	afx_msg void OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStnClickedStaticText();
};
