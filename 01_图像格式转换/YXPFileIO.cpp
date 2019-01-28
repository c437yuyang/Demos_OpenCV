
#include "YXPFileIO.h"
#include <fstream>
using namespace yxp_utility;
//����ָ��Ŀ¼���丸Ŀ¼�������
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
		::AfxMessageBox(_T("����Ŀ¼ʧ��"));
		return false;
	}
	else
		return true;
}



// �ж�Ŀ¼�Ƿ����(/��\\�����ԣ����󲻴���\\������)  
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
		(attr & FILE_ATTRIBUTE_ARCHIVE); //���������صĻ���ֻ���Ķ��ܼ�⵽
}

// �ݹ鴴��Ŀ¼������ʹ��/��ֻ��\\�����󲻴���\\������)
// ���Ŀ¼�Ѿ����ڻ��ߴ����ɹ�����TRUE  
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

//TODO:������CString��mfc�������CString����Ҫ����ô��
//ȡ��ָ���ļ����µ��ļ����ļ������ƣ����ݹ飬ֻȡһ��Ŀ¼����ȫ����\\������/��������Դ�\\(�޸���),
// ���ص����ļ���ȫ·��
//������/��Ŀ¼��
void YXPFileIO::GetDirectoryFiles(const std::string &src_folder,
	std::vector<std::string> &file_names,
	bool do_sort,
	bool only_files,
	bool only_dirs,
	const std::string &ext_include, //ָ��ֻ�����ĺ�׺������.
	const std::string &ext_exclude) //ָ��Ҫ�ų��ĺ�׺������.
{
	file_names = GetDirectoryFiles(src_folder, do_sort, only_files, only_dirs, ext_include, ext_exclude);
}


std::vector<std::string> YXPFileIO::GetDirectoryFiles(const std::string &src_folder,
	bool do_sort,
	bool only_files,
	bool only_dirs,
	const std::string &ext_include, //ָ��ֻ�����ĺ�׺������.
	const std::string &ext_exclude) //ָ��Ҫ�ų��ĺ�׺������.
{
	USES_CONVERSION;
	if (only_files&&only_dirs)  //���������ѡ�У���ô���������ļ���ֱ��Ĭ�ϲ�ѡ��
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
		if (!strcmp(filefind.name, ".."))  //�ô˷�����һ���ҵ����ļ�����Զ��".."��������Ҫ�����ж�
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
	if (do_sort) //�򵥵�����汾��ֱ�ӵ��õ�string.operator<
		std::sort(file_names.begin(), file_names.end());

	if (only_dirs)
		return file_names; //ֻ����Ŀ¼�Ļ��ͷ�����

	//ȥ������Ҫ�ĺ�׺���ļ�
	if (!ext_include.empty())
	{
		for (auto path = file_names.begin(); path != file_names.end();)
		{
			if (!FileExists(path->c_str()))
			{
				++path;
				continue; //������Ŀ¼
			}

			if (!CheckFileExt(*path, ext_include))
				path = file_names.erase(path);
			else
				++path;
		}
	}

	//ȥ������Ҫ�ĺ�׺���ļ�
	if (!ext_exclude.empty())
	{
		for (auto path = file_names.begin(); path != file_names.end();)
		{
			if (!FileExists(path->c_str()))
			{
				++path;
				continue; //������Ŀ¼
			}

			if (CheckFileExt(*path, ext_exclude))
				path = file_names.erase(path);
			else
				++path;
		}
	}

	return file_names;
}


//����һ��Ҫ�ټ�鷵��ֵ�Ƿ�Ϊ��!,�������ǻ������
std::string YXPFileIO::BrowseFolder(const std::string & title, const HWND owner)
{
	//TCHAR wchPath[MAX_PATH];     //���ѡ���Ŀ¼·�� 
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
	//����ѡ��Ŀ¼�Ի���
	LPITEMIDLIST lp = SHBrowseForFolderA(&bi);
	CString cStrPath;
	if (lp && SHGetPathFromIDListA(lp, path))
	{
		return path;
	}
	return "";

}

//ɾ����ǰ�㼶Ŀ¼�������ļ�������ɾ����Ŀ¼
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

std::string YXPFileIO::GetFileNameNoPath(const std::string &filename) //֧��\\��/��Ŀ¼
{
	int pos = filename.find_last_of("\\/");
	return filename.substr(pos + 1, filename.length() - pos);
}
std::string YXPFileIO::GetFileNameNoExt(const std::string &filename) //ֻ�ж�.��λ�ã�����ȥ��·��
{
	int pos = filename.rfind(".");
	return filename.substr(0, pos);
}
std::string YXPFileIO::GetFileNameExt(const std::string &filename) //���ش�.��
{
	int pos = filename.rfind(".");
	return filename.substr(pos, filename.length() - pos);
}

bool YXPFileIO::CheckFileExt(const std::string& path, const std::string &ext)
{
	if (path.empty() || ext.empty()) return false;
	std::string ext_src = GetFileNameExt(path);

	std::string ext_dst(ext);

	std::transform(ext_src.begin(), ext_src.end(), ext_src.begin(), ::tolower); //ת����Сд
	std::transform(ext_dst.begin(), ext_dst.end(), ext_dst.begin(), ::tolower); //ת����Сд

	if (ext_dst.compare(ext_src)) return false;
	return true;

}


//������Ŀ¼���ļ���exist_replaceָ������ʱ�Ƿ��滻
bool YXPFileIO::Rename(const std::string &src, const std::string &dst, bool exist_replace)
{
	//�ļ�������ֱ�ӷ���false
	if (!FileExists(src) && !FolderExists(src)) return false;
	if ((FileExists(dst) || FolderExists(dst)) && !exist_replace)
		return false;

	if (FileExists(dst) && exist_replace)
	{
		DeleteFileA(dst.c_str());
	}

	if (FolderExists(dst) && exist_replace)
	{
		DeleteDirectory(dst, true); //���ܵݹ�ɾ��Ŀ¼��ֻ��ɾ�������ļ���Ŀ¼
	}
	//win sdk��rename����˵��:
	//1.Ŀ���ļ����Ѿ����ڻ᷵��-1ʧ��
	//2.����Ӧ���Ŀ��Կ�Ŀ¼
	//3.����ֱ��������Ŀ¼���������ļ�Ҳ����,��Ȼͬ��Ŀ��Ŀ¼�Ѵ���(�������ļ���)�ᱨ��
	return	rename(src.c_str(), dst.c_str()) < 0 ? false : true;
}

std::string YXPFileIO::GetAppPath()
{
	char exeFullPath[MAX_PATH];

	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
	std::string pathName(exeFullPath);

	//����ֵ��󲻴�'\\'
	int index = pathName.rfind('\\');
	return pathName.substr(0, index);
}

//��ʵ��Щ�������Է���ֵ����Ϊenum,��ֱ�ۣ����Ƿ���false��̫��������
//Ҫ��dstĿ¼�����Ѿ�����,ֻ�����ļ�
bool YXPFileIO::CopyDirFiles(const std::string & src, const std::string & dst, bool exist_replace)
{
	std::string dst_path(dst);
	if (!FolderExists(src) || dst_path.empty()) return false;

	if (dst_path[dst_path.length() - 1] == '\\')
		dst_path = dst_path.substr(0, dst_path.length() - 1);

	std::vector<std::string> src_files;

	GetDirectoryFiles(src, src_files, true, true);
	if (!exist_replace) //�������滻������£��ȼ�����ظ��ļ���,��ֹ���ֲ��ֿ���
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

CString YXPFileIO::GetAppCStringPath() //������������ȡ�ó�������·��������ֵ��'\\'
{
	//ȡ��Ӧ�ó���·��	
	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	CString pathName(exeFullPath);

	//����ֵ����Դ�'\\'
	int index = pathName.ReverseFind('\\');
	return pathName.Left(index + 1);
}

//���ļ����뵽string��  
bool YXPFileIO::ReadAllText(const std::string & filename, std::string & txt)
{
	std::ifstream ifs(filename);
	if (!ifs.is_open()) return false;

	//���ļ����뵽ostringstream����buf��  
	std::ostringstream buf;
	char ch;
	while (buf&&ifs.get(ch))
		buf.put(ch);
	//������������buf�������ַ���  
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
