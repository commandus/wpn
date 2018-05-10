#include <stdio.h>
#include <linux/limits.h>
#include "commandoutput.h"

CommandOutput::CommandOutput()
{

}

CommandOutput::~CommandOutput()
{

}

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

	char buffer[4096];
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
