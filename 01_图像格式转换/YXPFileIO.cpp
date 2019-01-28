
#include "YXPFileIO.h"
#include <fstream>
using namespace yxp_utility;
//创建指定目录，其父目录必须存在
bool YXPFileIO::FindOrMkDir(const std::string& path)
{
	USES_CONVERSION;
	WIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA(path.c_str(), &fd);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}

	if (!::CreateDirectoryA(path.c_str(), NULL))
	{
		::AfxMessageBox(_T("创建目录失败"));
		return false;
	}
	else
		return true;
}



// 判断目录是否存在(/或\\都可以，带后不带后\\都可以)  
bool YXPFileIO::FolderExists(const std::string& path)
{
	DWORD attr;
	attr = GetFileAttributesA(path.c_str());
	return (attr != (DWORD)(-1)) &&
		(attr & FILE_ATTRIBUTE_DIRECTORY);
}


bool YXPFileIO::FileExists(const std::string& filename)
{
	DWORD attr;
	attr = GetFileAttributesA(filename.c_str());
	return (attr != (DWORD)(-1)) &&
		(attr & FILE_ATTRIBUTE_ARCHIVE); //这样，隐藏的或者只读的都能检测到
}

// 递归创建目录（不能使用/，只能\\，带后不带后\\都可以)
// 如果目录已经存在或者创建成功返回TRUE  
bool YXPFileIO::RecurMkDir(const std::string& path)
{
	std::string p(path);
	int len = p.length();
	if (len < 2) return false;

	if ('\\' == p[len - 1] || '/' == p[len - 1])
	{
		p = p.substr(0, len - 1);
		len = p.length();
	}
	if (len <= 0) return false;

	if (len <= 3)
	{
		if (FolderExists(p)) return true;
		else return false;
	}

	if (FolderExists(p))return true;

	std::string parDir = p.substr(0, p.find_last_of('\\/'));

	if (parDir.length() <= 0) return false;

	bool Ret = RecurMkDir(parDir);

	if (Ret)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = 0;
		Ret = (CreateDirectoryA(p.c_str(), &sa) == TRUE);
		return Ret;
	}
	else
		return false;
}

//TODO:换成用CString，mfc里面就用CString，不要想那么多
//取得指定文件夹下的文件及文件夹名称（不递归，只取一级目录），全部用\\，不能/，后面可以带\\(修复了),
// 返回的是文件的全路径
//可以用/的目录了
void YXPFileIO::GetDirectoryFiles(const std::string &src_folder,
	std::vector<std::string> &file_names,
	bool do_sort,
	bool only_files,
	bool only_dirs,
	const std::string &ext_include, //指定只包含的后缀名，带.
	const std::string &ext_exclude) //指定要排除的后缀名，带.
{
	file_names = GetDirectoryFiles(src_folder, do_sort, only_files, only_dirs, ext_include, ext_exclude);
}


std::vector<std::string> YXPFileIO::GetDirectoryFiles(const std::string &src_folder,
	bool do_sort,
	bool only_files,
	bool only_dirs,
	const std::string &ext_include, //指定只包含的后缀名，带.
	const std::string &ext_exclude) //指定要排除的后缀名，带.
{
	USES_CONVERSION;
	if (only_files&&only_dirs)  //如果两个都选中，那么就是所有文件，直接默认不选了
	{
		only_files = false;
		only_dirs = false;
	}
	std::vector<std::string> file_names;
	struct _finddata_t filefind;
	std::string  curr = src_folder + "\\*.*";
	int  done = 0;
	int  handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1)
		return file_names;

	std::string tmp_folder = src_folder;
	if (src_folder[src_folder.size() - 1] != '\\')
	{
		tmp_folder += "\\";
	}

	while (!(done = _findnext(handle, &filefind)))
	{
		if (!strcmp(filefind.name, ".."))  //用此方法第一个找到的文件名永远是".."，所以需要单独判断
			continue;
		if (only_files)
		{
			if (FileExists(tmp_folder + filefind.name))
				file_names.push_back(tmp_folder + filefind.name);
		}
		else if (only_dirs)
		{
			CString temp = A2W((tmp_folder + filefind.name).c_str());
			if (FileExists(tmp_folder + filefind.name))
				file_names.push_back(tmp_folder + filefind.name);
		}
		else
			file_names.push_back(tmp_folder + filefind.name);
	}
	_findclose(handle);
	if (do_sort) //简单的排序版本，直接调用的string.operator<
		std::sort(file_names.begin(), file_names.end());

	if (only_dirs)
		return file_names; //只检索目录的话就返回了

	//去掉不需要的后缀名文件
	if (!ext_include.empty())
	{
		for (auto path = file_names.begin(); path != file_names.end();)
		{
			if (!FileExists(path->c_str()))
			{
				++path;
				continue; //不考虑目录
			}

			if (!CheckFileExt(*path, ext_include))
				path = file_names.erase(path);
			else
				++path;
		}
	}

	//去掉不需要的后缀名文件
	if (!ext_exclude.empty())
	{
		for (auto path = file_names.begin(); path != file_names.end();)
		{
			if (!FileExists(path->c_str()))
			{
				++path;
				continue; //不考虑目录
			}

			if (CheckFileExt(*path, ext_exclude))
				path = file_names.erase(path);
			else
				++path;
		}
	}

	return file_names;
}


//后面一定要再检查返回值是否为空!,操作还是会继续的
std::string YXPFileIO::BrowseFolder(const std::string & title, const HWND owner)
{
	//TCHAR wchPath[MAX_PATH];     //存放选择的目录路径 
	char path[MAX_PATH];
	ZeroMemory(path, sizeof(path));
	BROWSEINFOA bi;
	bi.hwndOwner = owner;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path;
	bi.lpszTitle = title.c_str();
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	//弹出选择目录对话框
	LPITEMIDLIST lp = SHBrowseForFolderA(&bi);
	CString cStrPath;
	if (lp && SHGetPathFromIDListA(lp, path))
	{
		return path;
	}
	return "";

}

//删除当前层级目录的所有文件，不能删除子目录
void YXPFileIO::DeleteDirectory(const std::string path, bool delete_folder)
{
	if (path.empty() || !FolderExists(path.c_str())) return;
	std::vector<std::string> files;
	GetDirectoryFiles(path, files);
	if (!files.empty())
		for (int i = 0; i != files.size(); ++i)
			DeleteFileA(files[i].c_str());
	if (delete_folder)
		RemoveDirectoryA(path.c_str());
}

std::string YXPFileIO::GetFileNameNoPath(const std::string &filename) //支持\\或/的目录
{
	int pos = filename.find_last_of("\\/");
	return filename.substr(pos + 1, filename.length() - pos);
}
std::string YXPFileIO::GetFileNameNoExt(const std::string &filename) //只判断.的位置，不会去除路径
{
	int pos = filename.rfind(".");
	return filename.substr(0, pos);
}
std::string YXPFileIO::GetFileNameExt(const std::string &filename) //返回带.的
{
	int pos = filename.rfind(".");
	return filename.substr(pos, filename.length() - pos);
}

bool YXPFileIO::CheckFileExt(const std::string& path, const std::string &ext)
{
	if (path.empty() || ext.empty()) return false;
	std::string ext_src = GetFileNameExt(path);

	std::string ext_dst(ext);

	std::transform(ext_src.begin(), ext_src.end(), ext_src.begin(), ::tolower); //转换成小写
	std::transform(ext_dst.begin(), ext_dst.end(), ext_dst.begin(), ::tolower); //转换成小写

	if (ext_dst.compare(ext_src)) return false;
	return true;

}


//重命名目录或文件，exist_replace指定存在时是否替换
bool YXPFileIO::Rename(const std::string &src, const std::string &dst, bool exist_replace)
{
	//文件不存在直接返回false
	if (!FileExists(src) && !FolderExists(src)) return false;
	if ((FileExists(dst) || FolderExists(dst)) && !exist_replace)
		return false;

	if (FileExists(dst) && exist_replace)
	{
		DeleteFileA(dst.c_str());
	}

	if (FolderExists(dst) && exist_replace)
	{
		DeleteDirectory(dst, true); //不能递归删除目录，只能删除含有文件的目录
	}
	//win sdk的rename函数说明:
	//1.目标文件名已经存在会返回-1失败
	//2.理所应当的可以跨目录
	//3.可以直接重命名目录，里面有文件也可以,当然同理目标目录已存在(无论有文件否)会报错
	return	rename(src.c_str(), dst.c_str()) < 0 ? false : true;
}

std::string YXPFileIO::GetAppPath()
{
	char exeFullPath[MAX_PATH];

	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
	std::string pathName(exeFullPath);

	//返回值最后不带'\\'
	int index = pathName.rfind('\\');
	return pathName.substr(0, index);
}

//其实这些操作可以返回值返回为enum,更直观，光是返回false，太不清晰了
//要求dst目录必须已经存在,只拷贝文件
bool YXPFileIO::CopyDirFiles(const std::string & src, const std::string & dst, bool exist_replace)
{
	std::string dst_path(dst);
	if (!FolderExists(src) || dst_path.empty()) return false;

	if (dst_path[dst_path.length() - 1] == '\\')
		dst_path = dst_path.substr(0, dst_path.length() - 1);

	std::vector<std::string> src_files;

	GetDirectoryFiles(src, src_files, true, true);
	if (!exist_replace) //不进行替换的情况下，先检查有重复文件否,防止出现部分拷贝
	{
		std::vector<std::string> dst_files;
		GetDirectoryFiles(dst_path, dst_files, true, true);
		for (auto src_file : src_files)
		{
			for (auto dst_file : dst_files)
			{
				if (_stricmp(GetFileNameNoPath(src_file).c_str(),
					GetFileNameNoPath(dst_file).c_str()) == 0)
					return false;
			}
		}
	}
	//CopyFile:
	//If the function succeeds, the return value is nonzero.
	//If the function fails, the return value is zero.To get extended error information, call GetLastError.
	for (auto it = src_files.begin(); it != src_files.end(); ++it)
	{
		if (!CopyFileA(it->c_str(), (dst + "\\" + GetFileNameNoPath(*it)).c_str(), !exist_replace))
			return false;
	}
	return true;
}


std::string YXPFileIO::BrowseFile(const std::string filter, bool open)
{
	char Buffer[MAX_PATH];
	OPENFILENAMEA   ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = Buffer;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filter.c_str();
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST;

	if (open)
	{
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		GetOpenFileNameA(&ofn);
		return Buffer;
	}

	GetSaveFileNameA(&ofn);
	return std::string(Buffer);

}

CString YXPFileIO::GetAppCStringPath() //公共函数用于取得程序运行路径，返回值带'\\'
{
	//取得应用程序路径	
	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	CString pathName(exeFullPath);

	//返回值最后自带'\\'
	int index = pathName.ReverseFind('\\');
	return pathName.Left(index + 1);
}

//从文件读入到string里  
bool YXPFileIO::ReadAllText(const std::string & filename, std::string & txt)
{
	std::ifstream ifs(filename);
	if (!ifs.is_open()) return false;

	//将文件读入到ostringstream对象buf中  
	std::ostringstream buf;
	char ch;
	while (buf&&ifs.get(ch))
		buf.put(ch);
	//返回与流对象buf关联的字符串  
	txt = buf.str();
	return true;
}

bool YXPFileIO::WriteToText(const std::string & filename, const std::string & txt)
{
	std::ofstream ofs(filename);
	if (!ofs.is_open()) return false;
	ofs.write(txt.c_str(), txt.length());
	ofs.close();
	return true;
}

long long YXPFileIO::GetFileSize(const std::string &filename)
{
	std::ifstream ifs(filename);
	if (!ifs.is_open())
		return -1;
	ifs.seekg(std::ios::end);
	return static_cast<long long>(ifs.tellg());
}
