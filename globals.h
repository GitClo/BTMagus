#ifndef BTMAGUS_GLOBALS_H
#define BTMAGUS_GLOBALS_H

#include "memory"

extern std::shared_ptr<std::atomic<bool>> isFlipperScanRunning;
extern std::shared_ptr<std::atomic<bool>> isBleSpamRunning;
extern std::shared_ptr<std::atomic<bool>> isMessageDispatcherRunning;

extern std::string btAdapterPath;
extern std::mutex btAdapterPathMutex;
extern std::string getBtAdapterPath();
void setBtAdapterPath(const std::string& path);

extern std::mutex BTDevicesMutex;

#endif //BTMAGUS_GLOBALS_H