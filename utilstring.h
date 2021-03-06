#ifndef UTILSTRING_H_
#define UTILSTRING_H_	1

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdint.h>

#ifdef _MSC_VER
#else
#include <sys/socket.h>
#endif

// trim from start
std::string &ltrim(std::string &s);

// trim from end
std::string &rtrim(std::string &s);

// trim from both ends
std::string &trim(std::string &s);

// replace string
std::string replace(const std::string &str, const std::string &from, const std::string &to);

// read file
std::string file2string(std::istream &strm);

// read file
std::string file2string(const std::string &filename);

// read file
std::string file2string(const char *filename);

// read file from URL
std::string url2string(const std::string &url);

// write file
bool string2file(const std::string &filename, const std::string &value);

// write file
bool string2file(const char *filename, const std::string &value);

/**
 * Split string
 * See http://stackoverflow.com/questions/236129/split-a-string-in-c
 */
std::vector<std::string> split(const std::string &s, char delim);

template <typename T> std::string toString(const T value)
{
        std::stringstream idss;
        idss << value;
        return idss.str();
}

std::string doubleToString(const double value);

void *get_in_addr(struct sockaddr *sa);

std::string sockaddrToString(struct sockaddr_storage *value);

std::string timeToString(time_t value);

std::string spaces(char ch, int count);

/**
 * @brief Return hex string
 * @param buffer buffer
 * @param size buffer size
 * @return hex string
 */
std::string hexString(void *buffer, size_t size);

/**
 * @brief Return hex string
 * @param data binary data
 * @return string hex
 */
std::string hexString(const std::string &data);

std::string hex2string(const std::string &hex);

std::string arg2String(int argc, char *argv[]);

uint64_t string2id(const std::string &v);
/**
 * Escape JSON string
 * @see https://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
 */
std::string escapeJsonString
(
	const std::string& input
);

/**
 * Escape URL
 * @see https://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
 */
std::string escapeURLString
(
	const std::string& input
);

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
);

bool isDir
(
	const std::string &path
);

#endif
