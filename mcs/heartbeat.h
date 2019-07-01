#ifndef HEARTBEAT_MANAGER_H
#define HEARTBEAT_MANAGER_H

#include <thread>
#include "mcs.pb.h"

class HeartbeatManager {
public:
    typedef std::function<void()> OnSendHeartBeat;
    typedef std::function<void()> OnReconnectRequest;
private:
    time_t startTime;
    time_t responseTime;
    int32_t intervalMs;
    OnSendHeartBeat onSendHeartbeat;
    OnReconnectRequest onReconnect;
    std::thread *thread;
    int requestStop;
protected:
    bool isStarted();
    bool isStopped();
    void runner();
    bool hasResponse();
public:
    HeartbeatManager(
        const OnSendHeartBeat& cbSendHeartbeat,
        const OnReconnectRequest& cbReconnect
    );
    ~HeartbeatManager();
    void start();
    void restart();
    void stop();

    // Reset heartbeat timer
    void reset();
    // Update heartbeat interval
    void applyHeartbeatConfig(
        const mcs_proto::HeartbeatConfig& config
    );
};

#endif  // HEARTBEAT_MANAGER_H