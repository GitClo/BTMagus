#include <thread>
#include "flipper_scan/flipper_scan.h"
#include "ble_spam/ble_spam.h"
#include "core/MessageDispatcher.h"

#include <cli/cli.h>
#include <cli/clifilesession.h>
#include "core/globals.h"

int main() {
    isFlipperScanRunning.store(false);
    isBleSpamRunning.store(false);
    isMessageDispatcherRunning.store(true);

    // ---- Dispatcher (ONE output authority)
    MessageDispatcher msgDispatcher(std::cout);

    auto rootMenu = std::make_unique<cli::Menu>("BTMagus");

    // ================== FLIPPER SCAN ==================
    auto flipperScanMenu = std::make_unique<cli::Menu>(
        "flipper_scan",
        "Scan for nearby flippers",
        "flipper_scan"
    );

    flipperScanMenu->Insert(
        "run",
        [&msgDispatcher](std::ostream& out) {
            if (isFlipperScanRunning.load()) {
                out << "Scan is already running!\n";
                return;
            }

            out << "Scan is starting...\n";
            isFlipperScanRunning.store(true);
            std::thread(scanStart, std::ref(msgDispatcher)).detach();
        },
        "Scan for nearby flippers"
    );

    flipperScanMenu->Insert(
        "stop",
        [](std::ostream& out) {
            if (!isFlipperScanRunning.load()) {
                out << "No scan is currently running.\n";
                return;
            }

            out << "Stopping scan...\n";
            isFlipperScanRunning.store(false);
        },
        "Stop the current scan"
    );

    flipperScanMenu->Insert(
        "status",
        [](std::ostream& out) {
            out << (isFlipperScanRunning.load()
                ? "Scan is currently RUNNING\n"
                : "Scan is NOT running\n");
        },
        "Check scan status"
    );

    flipperScanMenu->Insert(
        "show",
        [](std::ostream& out) {
            std::lock_guard lock(BTDevicesMutex);

            if (BTDevices.empty()) {
                out << "You haven't scanned for bluetooth devices yet!\n";
                return;
            }

            bool has_flipper = false;
            for (auto& device : BTDevices) {
                if (device.amIFlipper() || device.amISpoofedFlipper()) {
                    has_flipper = true;
                    out << "------------------------------\n";
                    out << (device.amIFlipper() ? "🐬 | " : "🎭 | ");
                    out << device.Name << " | " << device.Address << '\n';
                    out << "UUIDS:\n";
                    for (const auto& uuid : device.Uuids)
                        out << uuid << '\n';
                    out << "------------------------------\n";
                }
            }

            if (!has_flipper)
                out << "No flippers have been detected yet.\n";
        },
        "Show a list of detected flippers"
    );

    // ================== BLE SPAM ==================
    auto bleSpamMenu = std::make_unique<cli::Menu>(
        "ble_spam",
        "Spam BLE devices with false advertisements.",
        "ble_spam"
    );

    bleSpamMenu->Insert(
        "run",
        [&msgDispatcher](std::ostream& out) {
            if (isBleSpamRunning.load()) {
                out << "BLE spam already running.\n";
                return;
            }

            out << "Running BLE spam...\n";
            isBleSpamRunning.store(true);
            std::thread(startBleSpam, std::ref(msgDispatcher)).detach();
        },
        "Run BLE Spam"
    );

    bleSpamMenu->Insert(
        "stop",
        [](std::ostream& out) {
            if (!isBleSpamRunning.load()) {
                out << "BLE spam is not currently running.\n";
                return;
            }

            out << "Stopping BLE spam...\n";
            isBleSpamRunning.store(false);
        },
        "Stop BLE spam"
    );

    // ================== MENU SETUP ==================
    rootMenu->Insert(std::move(flipperScanMenu));
    rootMenu->Insert(std::move(bleSpamMenu));

    cli::Cli cli(std::move(rootMenu));

    // ================== EXIT ==================
    cli.ExitAction([](auto& out) {
        isFlipperScanRunning.store(false);
        isBleSpamRunning.store(false);
        isMessageDispatcherRunning.store(false);
        out << "Bye.\n";
    });

    cli::CliFileSession input(cli);
    input.Start();

    return 0;
}