#include "watchdog_manager.h"

#include "../clock/clock.h"

void WatchdogManager::init()
{
	lastHeartbeatMs_ = Clock::millis();
	missedHeartbeats_ = 0;
	healthy_ = false;
}

void WatchdogManager::heartbeat(unsigned /*taskIndex*/)
{
	lastHeartbeatMs_ = Clock::millis();
	healthy_ = true;
}

void WatchdogManager::update()
{
	if (Clock::millis() - lastHeartbeatMs_ > timeoutMs_)
	{
		healthy_ = false;
		++missedHeartbeats_;
	}
}

