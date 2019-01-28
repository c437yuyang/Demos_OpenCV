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

//所有调用windows API的函数返回的都是\\
//全部使用std::string,不使用CString
//所有操作使用使用Ansi版本
namespace yxp_utility
{
	class YXPFileIO
	{
		//TODO:GetDirectoryFiles增加排序选择
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

		static std::string BrowseFolder(const std::string & title = "请选择要打开的目录", const HWND owner = NULL);
		static void DeleteDirectory(const std::string path, bool deleteFolder = false);
		static std::string GetFileNameNoPath(const std::string &filename); 

		static std::string GetFileNameNoExt(const std::string &filename); //只判断.的位置，不会去除路径
		static std::string GetFileNameExt(const std::string &filename);
		static	bool CheckFileExt(const std::string& Path, const std::string &ext);
		static bool Rename(const std::string &path, const std::string &dst, bool IsExistReplace = false);
		
		static bool CopyDirFiles(const std::string&src, const std::string &dst, bool IsExistReplace = false);

		static CString GetAppCStringPath(); //公共函数用于取得程序运行路径，返回值带'\\'
		static std::string GetAppPath(); //返回值没有'\\'

		static bool ReadAllText(const std::string &filename, std::string &txt);
		static bool WriteToText(const std::string &filename, const std::string &txt);
		static long long GetFileSize(const std::string &filename);
	};
}
