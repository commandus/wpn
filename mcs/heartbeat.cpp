#include "heartbeat.h"

#define MIN_INTERVAL    30 * 1000
#define DEF_INTERVAL    60 * 1000

HeartbeatManager::HeartbeatManager(
    const OnSendHeartBeat &cbSendHeartbeat,
    const OnReconnectRequest &cbReconnect
)
    : startTime(0), responseTime(0), intervalMs(DEF_INTERVAL), 
    onSendHeartbeat(cbSendHeartbeat), onReconnect(cbReconnect),
    thread(NULL), requestStop(0)
{
    start();
}

HeartbeatManager::~HeartbeatManager()
{
    stop();
}

bool HeartbeatManager::isStarted()
{
    return thread && (startTime != 0);
}

bool HeartbeatManager::isStopped()
{
    return !isStarted() && (requestStop != 0);
}

void HeartbeatManager::start()
{
    if (isStarted())
        return;
    thread = new std::thread(&HeartbeatManager::runner, this);
    thread->detach();
}

void HeartbeatManager::restart()
{
    stop();
    start();
}

void HeartbeatManager::stop()
{
    if (requestStop)
        return;
    if (!isStarted())
        return;
    requestStop++;
    delete thread;
    thread = NULL;
    startTime = 0;
}

void HeartbeatManager::runner()
{
    startTime = time(NULL);
    while (!requestStop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
        startTime = time(NULL);
        if (!hasResponse()) {
            onReconnect();
            reset();
        } else {
            onSendHeartbeat();
        }
    }
    requestStop--;
}

void HeartbeatManager::applyHeartbeatConfig(
    const mcs_proto::HeartbeatConfig& config
) {
    intervalMs = config.interval_ms();
    if (intervalMs < MIN_INTERVAL)
        intervalMs = MIN_INTERVAL;
    restart();
}

// Reset heartbeat timer
void HeartbeatManager::reset()
{
    startTime = 0;
    responseTime = 0;
}

bool HeartbeatManager::hasResponse()
{
    return (responseTime - startTime) * 1000 < intervalMs;
}
