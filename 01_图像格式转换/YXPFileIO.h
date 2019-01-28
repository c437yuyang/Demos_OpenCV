#pragma once
#include <io.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <afxwin.h>
#include <windows.h>
#include <Shlobj.h>

//���е���windows API�ĺ������صĶ���\\
//ȫ��ʹ��std::string,��ʹ��CString
//���в���ʹ��ʹ��Ansi�汾
namespace yxp_utility
{
	class YXPFileIO
	{
		//TODO:GetDirectoryFiles��������ѡ��
	public:
		static bool FindOrMkDir(const std::string& pszPath);
		static bool FolderExists(const std::string& s);

		static bool FileExists(const std::string& s);
		static bool RecurMkDir(const std::string& path);
		static void GetDirectoryFiles(const std::string &strFolder,
			std::vector<std::string> &strVecFileNames,
			bool do_sort = true,
			bool OnlyFiles = false,
			bool OnlyDirectories = false,
			const std::string &extInclude = "",
			const std::string &extExclude = "");

		static std::vector<std::string> GetDirectoryFiles(const std::string &strFolder,
			bool do_sort = true,
			bool OnlyFiles = false,
			bool OnlyDirectories = false,
			const std::string &extInclude = "",
			const std::string &extExclude = "");

		static std::string BrowseFile(const std::string strFilter = "Images (*.jpg;*.png)\0*.jpg;*.png\0All (*.*)\0*.*\0\0", bool isOpen = true);

		static std::string BrowseFolder(const std::string & title = "��ѡ��Ҫ�򿪵�Ŀ¼", const HWND owner = NULL);
		static void DeleteDirectory(const std::string path, bool deleteFolder = false);
		static std::string GetFileNameNoPath(const std::string &filename); 

		static std::string GetFileNameNoExt(const std::string &filename); //ֻ�ж�.��λ�ã�����ȥ��·��
		static std::string GetFileNameExt(const std::string &filename);
		static	bool CheckFileExt(const std::string& Path, const std::string &ext);
		static bool Rename(const std::string &path, const std::string &dst, bool IsExistReplace = false);
		
		static bool CopyDirFiles(const std::string&src, const std::string &dst, bool IsExistReplace = false);

		static CString GetAppCStringPath(); //������������ȡ�ó�������·��������ֵ��'\\'
		static std::string GetAppPath(); //����ֵû��'\\'

		static bool ReadAllText(const std::string &filename, std::string &txt);
		static bool WriteToText(const std::string &filename, const std::string &txt);
		static long long GetFileSize(const std::string &filename);
	};
}
