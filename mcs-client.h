#ifndef MCS_CLIENT_H_
#define MCS_CLIENT_H_

#include <stdint.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "mcs-client.h"
#include "mcs.pb.h"

class MCSClient {
public:
	MCSClient();
	virtual ~MCSClient();
	static int checkIn(
		uint64_t androidId, 
		uint64_t securityToken
	);
private:
};

#endif  // MCS_CLIENT_H_
