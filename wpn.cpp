/**
 * @file wpn.cpp
 */

#include <thread>
#include <string>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <argtable2.h>

#include "wpn.h"
#include "wp-encryption.h"

#ifdef _WIN32

void setSignalHandler(int signal)
{
}

#else
void signalHandler(int signal)
{
	switch(signal)
	{
	case SIGINT:
		std::cerr << MSG_INTERRUPTED << std::endl;
		break;
	default:
		break;
	}
}

void setSignalHandler(int signal)
{
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = &signalHandler;
	sigaction(signal, &action, NULL);
}
#endif

int main(int argc, char** argv)
{
	// Signal handler
	setSignalHandler(SIGINT);

	WpnConfig config(argc, argv);
	if (config.error())
		exit(config.error());

	WpnKeys wpnKeys(config.file_name);
	
	std::cout << wpnKeys.getPrivateKey() << " "
		<< wpnKeys.getPublicKey() << " "
		<< wpnKeys.getAuthSecret() << std::endl;
		
	wpnKeys.save(config.file_name);
	
	return 0;
}
