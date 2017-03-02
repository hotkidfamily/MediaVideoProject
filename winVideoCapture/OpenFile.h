#pragma once
#include <Windows.h>
#include <Commdlg.h>
#include <tchar.h>

// copy https://msdn.microsoft.com/en-us/library/windows/desktop/ms646927(v=vs.85).aspx

class OpenFileDialog
{
public:
	OpenFileDialog(HWND owner);

	TCHAR*DefaultExtension;
	TCHAR*FileName;
	TCHAR*Filter;
	int FilterIndex;
	int Flags;
	TCHAR*InitialDir;
	HWND Owner;
	TCHAR*Title;

	bool ShowDialog();
};
// 
// 
// ------------------------ - How To Use It------------------------ -
// 
// #include "OpenFileDialog.h"
// ...
// OpenFileDialog* openFileDialog1 = new OpenFileDialog();
// 
// if (openFileDialog1->ShowDialog())
// {
// 	MessageBox(0, openFileDialog1->FileName, _T("§£§í§Ò§â§Ñ§ß §æ§Ñ§Û§Ý"),
// 		MB_OK | MB_ICONINFORMATION);
// }
// ...
// openFileDialog1->FilterIndex = 1;
// openFileDialog1->Flags |= OFN_SHOWHELP;
// openFileDialog1->InitialDir = _T("C:\\Windows\\");
// openFileDialog1->Title = _T("Open Text File");
// 
// if (openFileDialog1->ShowDialog())
// {
// 	MessageBox(0, openFileDialog1->FileName, _T("§£§í§Ò§â§Ñ§ß §æ§Ñ§Û§Ý"),
// 		MB_OK | MB_ICONINFORMATION);
// }
// 
