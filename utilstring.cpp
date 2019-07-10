#include "utilstring.h"

#include <iostream>
#include <stdlib.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <ctime>
#include <limits>
#include <locale>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <iomanip>

#ifdef _MSC_VER
#else
#include <arpa/inet.h>
#endif
#include <curl/curl.h>

#ifdef _MSC_VER
#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#define PATH_DELIMITER "\\"
#else
#include <sys/param.h>
#include <fcntl.h>
#include <ftw.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <string.h>
#include <errno.h>
#include <cstdio>

#define PATH_DELIMITER "/"

#ifndef F_GETPATH
#define F_GETPATH	(1024 + 7)
#endif

#endif


/// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start
std::string &ltrim(std::string &s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
std::string &rtrim(std::string &s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
std::string &trim(std::string &s) 
{
	return ltrim(rtrim(s));
}

std::string replace(const std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
    	return str;
	std::string ret(str);
	ret.replace(start_pos, from.length(), to);
	return ret;
}

std::string file2string(std::istream &strm)
{
	if (!strm)
		return "";
	return std::string((std::istreambuf_iterator<char>(strm)), std::istreambuf_iterator<char>());
}

std::string file2string(const char *filename)
{
	if (!filename)
		return "";
	std::ifstream t(filename);
	return file2string(t);
}

std::string file2string(const std::string &filename)
{
	return file2string(filename.c_str());
}

/**
  * @brief CURL write callback
  */
static size_t write_string(void *contents, size_t size, size_t nmemb, void *userp)
{
	if (userp)
		((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

// read file from URL
std::string url2string(const std::string &url)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return ""; 
	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	std::string r;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
	{
		r = "";
	}
	curl_easy_cleanup(curl);
	return r;
}

bool string2file(const std::string &filename, const std::string &value)
{
	return string2file(filename.c_str(), value);
}

bool string2file(const char *filename, const std::string &value)
{
	if (!filename)
		return "";
	FILE* f = fopen(filename, "w");
	if (!f)
		return false;
	fwrite(value.c_str(), value.size(), 1, f);
	fclose(f);
	return true;

}

/**
 * Split
 * http://stackoverflow.com/questions/236129/split-a-string-in-c
 */
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

inline bool predicate_num(char c)
{
	return !isdigit(c);
};

std::string doubleToString(const double value)
{
	std::stringstream idss;
	idss.precision(std::numeric_limits<double>::digits10 + 1);
	idss << std::fixed << value;
	return idss.str();
}

void *get_in_addr
(
    struct sockaddr *sa
)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in *)sa)->sin_addr); 
	return &(((struct sockaddr_in6 *)sa)->sin6_addr); 
}

std::string sockaddrToString
(
	struct sockaddr_storage *value
)
{
	char s[INET6_ADDRSTRLEN]; // an empty string 
	inet_ntop(value->ss_family, get_in_addr((struct sockaddr *) value), s, sizeof(s)); 
	return s;
}

std::string timeToString(time_t value)
{
	if (!value)
		value = std::time(NULL);
	std::tm *ptm = std::localtime(&value);
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%c", ptm);
	return std::string(buffer);
}

std::string spaces(char ch, int count)
{
    std::stringstream ss;
    for (int i = 0; i < count; i++) {
        ss << ch;
    }
    return ss.str();
}

// http://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
struct HexCharStruct
{
        unsigned char c;
        HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
        return (o << std::setfill('0') << std::setw(2) << std::hex << (int) hs.c);
}

inline HexCharStruct hex(unsigned char c)
{
        return HexCharStruct(c);
}

void bufferPrintHex(std::ostream &sout, void* value, size_t size)
{
        if (value == NULL)
                return;
        unsigned char *p = (unsigned char*) value;
        for (size_t i = 0; i < size; i++)
        {
                sout << hex(*p);
                p++;
        }
}

std::string hexString(void *buffer, size_t size)
{
        std::stringstream r;
        bufferPrintHex(r, buffer, size);
        return r.str();
}

/**
 * Return hex string
 * @param data
 * @return
 */
std::string hexString(const std::string &data)
{
	return hexString((void *) data.c_str(), data.size());
}

std::string readHex(std::istream &s)
{
	std::stringstream r;
	s >> std::noskipws;
	char c[3] = {0, 0, 0};
	while (s >> c[0])
	{
		if (!(s >> c[1]))
			break;
		unsigned char x = (unsigned char) strtol(c, NULL, 16);
		r << x;
	}
	return r.str();
}

std::string hex2string(const std::string &hex)
{
	std::stringstream ss(hex);
    return readHex(ss);
}

std::string arg2String(
	int argc, 
	char *argv[]
)
{
	std::stringstream ss;
	for (int i = 0; i < argc; i ++)
	{
		ss << argv[i] << " ";
	}
	return ss.str();
}

uint64_t string2id(
	const std::string &v
)
{
	return std::atoll(v.c_str());	// contains trailing "\n"
}

/**
 * Escape JSON string
 * @see https://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
 */
std::string escapeJsonString
(
	const std::string& input
)
{
	std::ostringstream ss;
	for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) 
	{
		switch (*iter) 
		{
			case '\\': ss << "\\\\"; break;
			case '"': ss << "\\\""; break;
			case '/': ss << "\\/"; break;
			case '\b': ss << "\\b"; break;
			case '\f': ss << "\\f"; break;
			case '\n': ss << "\\n"; break;
			case '\r': ss << "\\r"; break;
			case '\t': ss << "\\t"; break;
			default: ss << *iter; break;
		}
	}
	return ss.str();
}

/**
 * Escape URL
 * @see https://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
 */
std::string escapeURLString
(
	const std::string& input
)
{
	std::ostringstream ss;
	for (std::string::const_iterator c = input.begin(); c != input.end(); c++) 
	{
		// Keep alphanumeric and other accepted characters intact
        if (isalnum(*c) || *c == '-' || *c == '_' || *c == '.' || *c == '~') 
		{
            ss << *c;
            continue;
        }
        ss << std::uppercase << std::hex << '%' << std::setw(2) << int((unsigned char) *c) << std::nouppercase;
	}
	return ss.str();
}

// -------------------------- from utilfile.cpp ------------------------------

#ifdef _MSC_VER
/**
 * Return list of files in specified path
 * @param path path
 * @param suffix file extension
 * @param flags 0- as is, 1- full path, 2- relative (remove parent path)
 * @param retval can be NULL
 * @return count files
 * FreeBSD fts.h fts_*()
 */
size_t filesInPath
(
	const std::string &path,
	const std::string &suffix,
	int flags,
	std::vector<std::string> *retval
)
{
	HANDLE hFind;
	WIN32_FIND_DATAA data;
	std::string mask = path + "\\*" + suffix;
	hFind = FindFirstFileA(mask.c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE)
		return 0;
	size_t c = 0;
	do 
	{
		std::string s(data.cFileName);
		if (s.find(suffix) != std::string::npos)
		{
			retval->push_back(path + "\\" + s);
			c++;
		}
	} while (FindNextFileA(hFind, &data));
	FindClose(hFind);
	return c;
}

bool isDir
(
	const std::string &path
)
{
	DWORD ftyp = GetFileAttributesA(path.c_str());
	return ((ftyp != INVALID_FILE_ATTRIBUTES) && (ftyp & FILE_ATTRIBUTE_DIRECTORY));
}

#else

int compareFile
(
		const FTSENT **a,
		const FTSENT **b
)
{
	return strcmp((*a)->fts_name, (*b)->fts_name);
}

/**
 * Return list of files in specified path
 * @param path path
 * @param suffix file extension
 * @param flags 0- as is, 1- full path, 2- relative (remove parent path)
 * @param retval can be NULL
 * @return count files
 * FreeBSD fts.h fts_*()
 */
size_t filesInPath
(
	const std::string &path,
	const std::string &suffix,
	int flags,
	std::vector<std::string> *retval
)
{
	char *pathlist[2];
	pathlist[1] = NULL;
	if (flags & 1)
	{
		char realtapth[PATH_MAX+1];
		pathlist[0] = realpath((char *) path.c_str(), realtapth);
	}
	else
	{
		pathlist[0] = (char *) path.c_str();
	}
	int parent_len = strlen(pathlist[0]) + 1;	///< Arggh. Remove '/' path delimiter(I mean it 'always' present). Not sure is it works fine. It's bad, I know.

	FTS* file_system = fts_open(pathlist, FTS_LOGICAL | FTS_NOSTAT, NULL);

    if (!file_system)
    	return 0;
    size_t count = 0;
    FTSENT* parent;
	while ((parent = fts_read(file_system)))
	{
		FTSENT* child = fts_children(file_system, 0);
		if (errno != 0)
		{
			// ignore, perhaps permission error
		}
		while (child)
		{
			switch (child->fts_info) {
				case FTS_F:
					{
						std::string s(child->fts_name);
						if (s.find(suffix) != std::string::npos)
						{
							count++;
							if (retval)
							{
								if (flags & 2)
								{
									// extract parent path
									std::string p(&child->fts_path[parent_len]);
									retval->push_back(p + s);
								}
								else
									retval->push_back(std::string(child->fts_path) + s);
							}
						}
					}
					break;
				default:
					break;
			}
			child = child->fts_link;
		}
	}
	fts_close(file_system);
	return count;
}

bool isDir
(
	const std::string &path
)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		return (s.st_mode & S_IFDIR);
	}
	return false;
}

#endif
