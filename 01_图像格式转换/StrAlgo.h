#ifndef STRALGO_H
#define STRALGO_H
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
namespace yxp_utility
{
class StrAlgo
{
public:
    /// <summary>
    /// ֧���пո�����
    /// </summary>
    static std::vector<std::string> split(const std::string& src, char delim);

    /// <summary>
    /// ������٣����ǲ��ܴ�����ڿո�����
    /// </summary>
    static std::vector<std::string> split2(const std::string& src, char delim);

    /// <summary>
    /// ƴ���ַ���,'\0'��ʾ���ַ�
    /// </summary>
    static std::string join(const std::vector<std::string>& strs, char delim = '\0');

    /// <summary>
    /// ƴ���ַ���,'\0'��ʾ���ַ�
    /// </summary>
    template<typename Pred>
        static std::string joinIf(const std::vector<std::string>& strs, char delim, Pred pred);

    /// <summary>
    /// �ж���appendix��β
    /// </summary>
    static bool endsWith(const std::string& str, const std::string& appendix);

    /// <summary>
    /// �ж���prefix��ʼ
    /// </summary>
    static bool startsWith(const std::string& str, const std::string& prefix);

    /// <summary>
    /// ɾ���ײ�prefix
    /// </summary>
    static std::string trimStart(const std::string& str, const std::string& prefix=" ");

    /// <summary>
    /// ɾ��β��appendix
    /// </summary>
    static std::string trimEnd(const std::string& str, const std::string& appendix=" ");

    /// <summary>
    /// ɾ���ײ���β����del
    /// </summary>
    static std::string trim(const std::string& str, const std::string& del=" ");

    /// <summary>
    /// �滻ȫ��fromΪto����δ�ҵ���ֱ�ӷ���
    /// </summary>
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

    /// <summary>
    /// �滻��һ��fromΪto����δ�ҵ���ֱ�ӷ���
    /// </summary>
    static std::string replaceFirst(const std::string& str, const std::string& from, const std::string& to);

    /// <summary>
    /// �滻��һ��fromΪ""����δ�ҵ���ֱ�ӷ���
    /// </summary>
    static std::string eraseFirst(const std::string& str, const std::string& from);

    /// <summary>
    /// �滻ȫ��fromΪ""����δ�ҵ���ֱ�ӷ���
    /// </summary>
    static std::string eraseAll(const std::string& str, const std::string& from);

    /// <summary>
    /// �Ƿ����
    /// </summary>
    static bool isPalindrome(const std::string&str);

	/// <summary>
	/// ������Ӵ�
	/// </summary>
	static std::string longestPalindrome(const std::string&str);

};
}

namespace yxp_utility
{
std::vector<std::string> StrAlgo::split(const std::string& src, char delim)
{
    if (src == "")
        return{};

    std::vector<std::string> res;
    std::string tmp = src;

    size_t index = 0;
    size_t last = 0;
    last = tmp.find_first_not_of(delim, last);//�ҵ���һ����Ϊ�ָ������ַ�
    index = tmp.find_first_of(delim, last);//�ҵ���һ���ָ���
    if (index == std::string::npos) //û�ҵ�
        return{};
    while (index != std::string::npos)//npos�����ַ����Ľ�β
    {
        std::string target = tmp.substr(last, index - last);
        res.push_back(target);

        //last = index +1;
        last = tmp.find_first_not_of(delim, index);
        index = tmp.find_first_of(delim, last);
    }
    if (index == std::string::npos && tmp.length() > last)//��ĩβ��,����������ȴ���last����,˵���������һ���ַ�Ҫ�ŵ�vector
        res.push_back(tmp.substr(last));
    //#if 1
    //	std::cout << v.size() << std::endl;
    //	for (size_t i = 0; i < v.size(); i++)
    //	{
    //		std::cout << "i=" << i << "," << v[i].c_str() << std::endl;
    //	}
    //#endif
    return res;
}

std::vector<std::string> StrAlgo::split2(const std::string& src, char delim)
{
    std::stringstream sstr(src);
    std::string tmp;
    std::vector<std::string> v;
    while (std::getline(sstr, tmp, delim))
        v.push_back(tmp);
    return v;
}

std::string StrAlgo::join(const std::vector<std::string>& strs, char delim)
{
/* #ifdef __GNUC__ */
/*     if (strs.empty()) */                         
/*         return ""; */                                
/*     std::string res = ""; */                              	   
/*     for (size_t i = 0; i < strs.size(); ++i) */      	   
/*     { */                                             	   
/*         res += strs[i]; */                           	   
/*         if (i != strs.size() - 1 && delim != '\0') */	   
/*             res += delim; */                         	   
/*     } */                                             	   
/*     return res; */                                 	   
/* #endif */
/* #ifdef __MSC_VER */
    auto pred = [](const std::string&){return true;};
    return joinIf(strs, delim, pred);
/* #endif */
}

    template<typename Pred>
std::string StrAlgo::joinIf(const std::vector<std::string>& strs, char delim, Pred pred)
{
    if (strs.empty())
        return "";
    std::string res = "";
    for (size_t i = 0; i < strs.size(); ++i)
    {
        if (pred(strs[i]))
        {
            res += strs[i];
            if (i != strs.size() - 1 && delim != '\0')
                res += delim;
        }
    }
    if (delim != '\0')
        res = trimEnd(res, std::string(1, delim));
    return res;
}

bool StrAlgo::startsWith(const std::string& str, const std::string& prefix)
{
    if (prefix == "")
        throw std::runtime_error("prefix is empty!!!");

    if (str == "" ||
        str.length() < prefix.length() ||
        str.substr(0, prefix.length()) != prefix)
        return false;

    return true;
}

bool StrAlgo::endsWith(const std::string& str, const std::string& appendix)
{
    if (appendix == "")
        throw std::runtime_error("appendix is empty!!!");

    if (str == "" ||
        str.length() < appendix.length() ||
        str.substr(str.length() - appendix.length()) != appendix)
        return false;

    return true;
}

std::string StrAlgo::trimStart(const std::string& str, const std::string& prefix)
{
    if (!startsWith(str, prefix))
        return str;
    std::string res = str;
    while (startsWith(res, prefix))
        res = res.substr(prefix.length());
    return res;
}

std::string StrAlgo::trimEnd(const std::string& str, const std::string& appendix)
{
    if (!endsWith(str, appendix))
        return str;
    std::string res = str;
    while (endsWith(res, appendix))
        res = res.substr(0, res.length() - appendix.length());
    return res;
}

std::string StrAlgo::trim(const std::string& str, const std::string& del)
{
    std::string res = str;
    if (endsWith(str, del))
        res = trimEnd(res,del);

    if (startsWith(str, del))
        res = trimStart(res, del);
    return res;
}

std::string StrAlgo::replaceAll(const std::string& str, const std::string& from, const std::string& to)
{
    if (str.find(from) == std::string::npos)
        return str;
    std::string res = str;
    std::string::size_type pos = res.find(from);
    do
    {
        res = res.replace(pos, from.length(), to);
        pos = res.find(from, pos);
    }
    while (pos != std::string::npos);
    return res;
}

std::string StrAlgo::replaceFirst(const std::string& str, const std::string& from, const std::string& to)
{
    if (str.find(from) == std::string::npos)
        return str;
    std::string res = str;
    std::string::size_type pos = res.find(from);
    res = res.replace(pos, from.length(), to);
    return res;
}

std::string StrAlgo::eraseFirst(const std::string& str, const std::string& from)
{
    return replaceFirst(str,from,"");
}

std::string StrAlgo::eraseAll(const std::string& str, const std::string& from)
{
    return replaceAll(str, from, "");
}

bool StrAlgo::isPalindrome(const std::string& str)
{
    //for (int i = 0, j = str.length() - 1; i < j; i++, j--)
    //	if (str[i] != str[j])
    //		return false;
    //return true;
    return str == std::string(str.rbegin(), str.rend());
}

std::string StrAlgo::longestPalindrome(const std::string&str)
{
	if (str.length() == 0)
		return "";

	int maxLen = 0, left, right;
	size_t leftBound = -1;
	for (size_t i = 0; i < str.length(); ++i) //ÿ��λ�ÿ�ʼ
	{	//������λ��������
		left = i, right = i;//bab����ʱ��a������
		while (left >= 0 && right < str.length()
			&& str[left] == str[right])
		{
			if (right - left + 1 >= maxLen)
			{
				leftBound = left;
				maxLen = right - left + 1;
			}
			--left;
			++right;
		}

		left = i, right = i + 1; //bb,��b,b������
		while (left >= 0 && right < str.length()
			&& str[left] == str[right])
		{
			if (right - left + 1 >= maxLen)
			{
				leftBound = left;
				maxLen = right - left + 1;
			}
			--left;
			++right;
		}
	}
	return  str.substr(leftBound, maxLen);
}
}
#endif
