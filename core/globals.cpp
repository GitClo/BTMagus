#include "globals.h"
#include <atomic>

auto isFlipperScanRunning = std::make_shared<std::atomic<bool>>(false);
auto isBleSpamRunning = std::make_shared<std::atomic<bool>>(false);
auto isMessageDispatcherRunning = std::make_shared<std::atomic<bool>>(false);

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