#ifndef COMMANDOUTPUT_H
#define COMMANDOUTPUT_H

#define ERR_FAILED_RUN		-1	// Failed to run command
#define ERR_FAILED_PIPE		-2	// Failed create pipe. Windows only
#define ERR_PIPE_SET_INFO	-3	// Failed set up info. Windows only

#include <string>
#include <ostream>

class CommandOutput
{
public:
	CommandOutput();
	~CommandOutput();
	int exec
	(
		std::ostream *output,
		const std::string& cmd
	);
};

#endif // COMMANDOUTPUT_H
