// EditDialog.h
#pragma once

class CEditDialog : public CDialogEx
{
public:
	CEditDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.

	enum { IDD = IDD_EDIT_DIALOG };          // 다이얼로그 ID

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CString m_partNo;     // 수정할 Part No
	CString m_quantity;   // 수정할 Quantity
	CString m_date;       // 수정할 Date

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
