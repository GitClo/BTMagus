#include "globals.h"
#include <atomic>

std::atomic<bool> isFlipperScanRunning = std::atomic<bool>(false);
std::atomic<bool> isBleSpamRunning = std::atomic<bool>(false);
std::atomic<bool> isMessageDispatcherRunning = std::atomic<bool>(false);

std::string adapterObjectPath{"/org/bluez/hci0"};
std::mutex adapterObjectPathMutex;

std::string getAdapterObjectPath() {
    std::lock_guard<std::mutex> lock(adapterObjectPathMutex);
    return adapterObjectPath;
}

void setAdapterObjectPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(adapterObjectPathMutex);
    adapterObjectPath = path;
}

std::mutex BTDevicesMutex;