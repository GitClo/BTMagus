#ifndef BTMAGUS_MESSAGEDISPATCHER_H
#define BTMAGUS_MESSAGEDISPATCHER_H

#include <queue>
#include <mutex>
#include <thread>
#include <ostream>
#include <chrono>

#include "globals.h"
#include "message.h"

class MessageDispatcher {
public:
    std::mutex cliOutputMutex;
    std::ostream* cliOutput = nullptr;

    std::queue<Message> messageQueue;
    std::mutex messageQueueMutex;

    std::thread dispatcherThread;

    MessageDispatcher(std::ostream& out) {
        {
            std::lock_guard lock(cliOutputMutex);
            cliOutput = &out;
        }

        dispatcherThread = std::thread([this]() {
            while (isMessageDispatcherRunning->load()) {
                bool hasMessage = false;
                Message msg(Message::Source::FlipperScan, "");

                {
                    std::lock_guard lock(messageQueueMutex);
                    if (!messageQueue.empty()) {
                        msg = std::move(messageQueue.front());
                        messageQueue.pop();
                        hasMessage = true;
                    }
                }

                if (hasMessage) {
                    std::lock_guard lock(cliOutputMutex);
                    if (cliOutput) {
                        *cliOutput << '\n';

                        *cliOutput << msg.message << '\n';

                        switch (msg.source) {
                            case Message::Source::FlipperScan:
                                *cliOutput << "flipper_scan> ";
                                break;
                            case Message::Source::BleSpam:
                                *cliOutput << "ble_spam> ";
                                break;
                        }

                        cliOutput->flush();
                    }
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
        });

        dispatcherThread.detach();
    }

    void dispatchMessage(Message msg) {
        std::lock_guard lock(messageQueueMutex);
        messageQueue.push(std::move(msg));
    }
};

#endif // BTMAGUS_MESSAGEDISPATCHER_H
