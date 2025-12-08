#include <thread>
#include "flipper_scan/flipper_scan.h"
#include <cli/cli.h>
#include <cli/clifilesession.h>
#include "globals.h"

int main() {
    auto flipperScanThread = std::make_shared<std::unique_ptr<std::thread>>();
    auto outputThread = std::make_shared<std::unique_ptr<std::thread>>();
    isFlipperScanRunning->store(false);

    auto rootMenu = std::make_unique<cli::Menu>("BTMagus");
    auto flipperScanMenu = std::make_unique<cli::Menu>(
        "flipper_scan",
        "Scan for nearby flippers",
        "flipper_scan");

    flipperScanMenu->Insert(
        "run",
        [flipperScanThread, outputThread](std::ostream& out) {

            if (isFlipperScanRunning->load()) {
                out << "Scan is already running!" << std::endl;
                return;
            }

            {
                std::lock_guard lock(cliOutputMutex);
                cliOutput = &out;
            }

            out << "Scan is starting..." << std::endl;
            isFlipperScanRunning->store(true);
            *flipperScanThread = std::make_unique<std::thread>(scanStart);
            (*flipperScanThread)->detach();

            *outputThread = std::make_unique<std::thread>([]() {
            while (isFlipperScanRunning->load()) {
                std::string msg;
                bool hasMessage = false;

                {
                    std::lock_guard qLock(queueMutex);
                    if (!outputQueue.empty()) {
                        msg = outputQueue.front();
                        outputQueue.pop();
                        hasMessage = true;
                    }
                }  // queueMutex released here

                if (hasMessage) {
                    std::lock_guard outLock(cliOutputMutex);
                    if (cliOutput) {
                        *cliOutput << '\n' << msg << "flipper_scan> ";
                        cliOutput->flush();
                    }
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
        });
            (*outputThread)->detach();
        },
        "Scan for nearby flippers");

    flipperScanMenu->Insert(
        "stop",
        [](std::ostream& out) {
            if (!isFlipperScanRunning->load()) {
                out << "No scan is currently running.\n";
                return;
            }
            out << "Stopping scan..." << std::endl;
            isFlipperScanRunning->store(false);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            {
                std::lock_guard lock(cliOutputMutex);
                cliOutput = nullptr;
            }
        },
        "Stop the current scan");

    flipperScanMenu->Insert(
        "status",
        [](std::ostream& out) {
            if (isFlipperScanRunning->load()) {
                out << "Scan is currently RUNNING" << std::endl;
            } else {
                out << "Scan is NOT running" << std::endl;
            }
        },
        "Check scan status");

    flipperScanMenu->Insert(
        "show",
        [](std::ostream& out) {
            {
                std::lock_guard lock_output(cliOutputMutex);
                std::lock_guard lock_btdevices(BTDevicesMutex);

                if (!BTDevices.empty()) {
                    bool has_flipper = false;
                    for (auto& device : BTDevices) {
                        if (device.amIFlipper()) {
                            has_flipper = true;
                            out << "------------------------------" << '\n';
                            out << "🐬 | " << device.Name << " | " << device.Address << " |" << '\n';
                            out << "UUIDS:" << '\n';
                            for (const auto & uuid : device.Uuids) {
                                   out << uuid << " | " << '\n';
                            }
                            out << "------------------------------" << '\n';
                            out.flush();
                        }
                        else if (device.amISpoofedFlipper()) {
                            has_flipper = true;
                            out << "------------------------------" << '\n';
                            out << "🎭 | " << device.Name << " | " << device.Address << " |" << '\n';
                            out << "UUIDS:" << '\n';
                            for (const auto & uuid : device.Uuids) {
                                   out << uuid << " | " << '\n';
                            }
                            out << "------------------------------" << '\n';
                            out.flush();
                        }
                    }
                    if (!has_flipper) {
                            out << "No flippers have been detected yet." << std::endl;
                    }
                }
                else {
                    out << "You haven't scanned for bluetooth devices yet!" << std::endl;
                }
            }
        },
        "Show a list of detected flippers");

    flipperScanMenu->Insert(
        "adapter_path",
        [](std::ostream& out) {
            out << "Adapter path> ";
            out.flush();
            std::string a_path;
            std::getline(std::cin, btAdapterPath);
            setBtAdapterPath(a_path);
        },
        "Set bluetooth adapter path. Default is /org/bluez/hci0");

    rootMenu->Insert(std::move(flipperScanMenu));

    cli::Cli cli(std::move(rootMenu));

    // global exit action
    cli.ExitAction([](auto& out) {
        if (isFlipperScanRunning->load()) {
            out << "Stopping background scan...\n";
            isFlipperScanRunning->store(false);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        out << "Bye.\n";
    });

    cli::CliFileSession input(cli);
    input.Start();

    return 0;
}