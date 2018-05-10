#ifndef COMMANDOUTPUT_H
#define COMMANDOUTPUT_H

#define ERR_FAILED_RUN	-1	// Failed to run command

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
