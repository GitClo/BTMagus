#ifndef BTMAGUS_GLOBALS_H
#define BTMAGUS_GLOBALS_H

#include "memory"

extern std::atomic<bool> isFlipperScanRunning;
extern std::atomic<bool> isBleSpamRunning;
extern std::atomic<bool> isMessageDispatcherRunning;

extern std::string adapterObjectPath;
extern std::mutex btAdapterPathMutex;
extern std::string getAdapterObjectPath();
void setAdapterObjectPath(const std::string& path);

extern std::mutex BTDevicesMutex;

#endif //BTMAGUS_GLOBALS_H