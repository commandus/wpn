#ifndef COMMANDOUTPUT_H
#define COMMANDOUTPUT_H 1

#include "errlist.h"

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
