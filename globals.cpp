#include "globals.h"
#include <atomic>

auto isFlipperScanRunning = std::make_shared<std::atomic<bool>>(false);

std::queue<std::string> outputQueue;
std::mutex queueMutex;

std::mutex cliOutputMutex;
std::ostream * cliOutput = nullptr;