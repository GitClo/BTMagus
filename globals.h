#ifndef BTMAGUS_GLOBALS_H
#define BTMAGUS_GLOBALS_H

#include "memory"
#include <queue>

extern std::shared_ptr<std::atomic<bool>> isFlipperScanRunning;

extern std::string btAdapterPath;
extern std::mutex btAdapterPathMutex;
extern std::string getBtAdapterPath();
void setBtAdapterPath(const std::string& path);

extern std::queue<std::string> outputQueue;
extern std::mutex queueMutex;

extern std::mutex BTDevicesMutex;

extern std::mutex cliOutputMutex;
extern std::ostream * cliOutput;

#endif //BTMAGUS_GLOBALS_H