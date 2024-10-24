// EditDialog.h
#pragma once

class CEditDialog : public CDialogEx
{
public:
	CEditDialog(CWnd* pParent = nullptr);   // ǥ�� �������Դϴ�.

	enum { IDD = IDD_EDIT_DIALOG };          // ���̾�α� ID

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	CString m_partNo;     // ������ Part No
	CString m_quantity;   // ������ Quantity
	CString m_date;       // ������ Date

protected:
	CEdit m_editPartNo;      // Part No Edit Control
	CEdit m_editQuantity;     // Quantity Edit Control
	CEdit m_editDate;         // Date Edit Control
};

// EditDialog.cpp
#include "stdafx.h"
#include "EditDialog.h"
#include "afxdialogex.h"

BEGIN_MESSAGE_MAP(CEditDialog, CDialogEx)
END_MESSAGE_MAP()

CEditDialog::CEditDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_DIALOG, pParent)
{
}

void CEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NEW_PARTNO, m_partNo);
	DDX_Text(pDX, IDC_EDIT_NEW_QUANTITY, m_quantity);
	DDX_Text(pDX, IDC_EDIT_NEW_DATE, m_date);
}
