// KDlgNewProject.cpp: 구현 파일
//

#include "stdafx.h"
#include "resource.h"
#include "KDlgNewProject.h"
#include "afxdialogex.h"


// KDlgNewProject 대화 상자

IMPLEMENT_DYNAMIC(KDlgNewProject, CDialogEx)

KDlgNewProject::KDlgNewProject(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_NEW_PROJECT, pParent)
{

}

KDlgNewProject::~KDlgNewProject()
{
}

void KDlgNewProject::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KDlgNewProject, CDialogEx)
END_MESSAGE_MAP()


// KDlgNewProject 메시지 처리기
