#include "globals.h"
#include <atomic>

auto isFlipperScanRunning = std::make_shared<std::atomic<bool>>(false);

std::string btAdapterPath{"/org/bluez/hci0"};
std::mutex btAdapterMutex;

std::string getBtAdapterPath() {
    std::lock_guard<std::mutex> lock(btAdapterMutex);
    return btAdapterPath;
}

void setBtAdapterPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(btAdapterMutex);
    btAdapterPath = path;
}

std::queue<std::string> outputQueue;
std::mutex queueMutex;

std::mutex BTDevicesMutex;

std::mutex cliOutputMutex;
std::ostream * cliOutput = nullptr;