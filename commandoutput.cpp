#include <stdio.h>
#ifdef _MSC_VER
#include <windows.h> 
#else
#include <linux/limits.h>
#endif
#include "commandoutput.h"

CommandOutput::CommandOutput()
{

}

CommandOutput::~CommandOutput()
{

}

#define BUFSIZE 4096

#ifndef _MSC_VER
int CommandOutput::exec
(
	std::ostream *output,
	const std::string& cmd
)
{
	// Open the command for reading
	FILE *fp = popen(cmd.c_str(), "r");
	if (fp == NULL) 
	{
		return ERR_FAILED_RUN;
	}

	char buffer[BUFSIZE];
	size_t r;
	while ((r = fread(buffer, 1, sizeof(buffer), fp)) != 0) 
	{
		if (output)
		{
			*output << std::string((char *) buffer, r);
		}
	}
	pclose(fp);
	return 0;
}
#else

int CommandOutput::exec
(
	std::ostream *output,
	const std::string& cmd
)
{

	HANDLE g_hInputFile = NULL;

	// Set the bInheritHandle flag so pipe handles are inherited. 
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe for the child process's STDOUT. 
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
	{
		return ERR_FAILED_PIPE;
	}

	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
	{
		CloseHandle(g_hChildStd_OUT_Rd);
		CloseHandle(g_hChildStd_OUT_Wr);
		return ERR_PIPE_SET_INFO;
	}

	// Create a pipe for the child process's STDIN. 
	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
	{
		CloseHandle(g_hChildStd_OUT_Rd);
		CloseHandle(g_hChildStd_OUT_Wr);
		return ERR_FAILED_PIPE;
	}

	// Ensure the write handle to the pipe for STDIN is not inherited. 

	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
	{
		CloseHandle(g_hChildStd_OUT_Rd);
		CloseHandle(g_hChildStd_OUT_Wr);
		CloseHandle(g_hChildStd_IN_Rd);
		CloseHandle(g_hChildStd_IN_Wr);
		return ERR_PIPE_SET_INFO;
	}

	// Create the child process. 
	// Set up members of the PROCESS_INFORMATION structure. 
	PROCESS_INFORMATION piProcInfo;
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.
	STARTUPINFO siStartInfo;
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 
	bool r = CreateProcessA(NULL,
		(LPSTR) cmd.c_str(),   // command line 
		NULL,			// process security attributes 
		NULL,			// primary thread security attributes 
		TRUE,			// handles are inherited 
		0,				// creation flags 
		NULL,			// use parent's environment 
		NULL,			// use parent's current directory 
		&siStartInfo,	// STARTUPINFO pointer 
		&piProcInfo		// receives PROCESS_INFORMATION 
	);
	if (!r)
	{
		CloseHandle(g_hChildStd_OUT_Rd);
		CloseHandle(g_hChildStd_OUT_Wr);
		CloseHandle(g_hChildStd_IN_Rd);
		CloseHandle(g_hChildStd_IN_Wr);

		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);

		return ERR_FAILED_PIPE;
	}
	// Close handles to the child process and its primary thread.
	// Some applications might keep these handles to monitor the status
	// of the child process, for example. 
	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);

	// Read from pipe that is the standard output for child process. 
	// Stop when there is no more data. 

	char buffer[BUFSIZE];
	for (;;)
	{
		DWORD dwRead, dwWritten;
		bool bSuccess = ReadFile(g_hChildStd_OUT_Rd, buffer, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) 
			break;
		if (output)
		{
			*output << std::string((char *)buffer, dwRead);
		}
	}

	// The remaining open handles are cleaned up when this process terminates. 
	// To avoid resource leaks in a larger application, close handles explicitly. 
	CloseHandle(g_hChildStd_OUT_Rd);
	CloseHandle(g_hChildStd_OUT_Wr);
	CloseHandle(g_hChildStd_IN_Rd);
	CloseHandle(g_hChildStd_IN_Wr);
	return 0;
}

#endif
