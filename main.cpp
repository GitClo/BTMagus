#include <thread>
#include <iostream>
#include "flipper_scan/flipper_scan.h"
#include "ble_spam/ble_spam.h"
#include "core/MessageDispatcher.h"
#include "external/cli/include/cli/cli.h"
#include "external/cli/include/cli/clifilesession.h"
#include "core/globals.h"

void checkSystemEnvironment(MessageDispatcher& msgDispatcher) {
    std::cout << "\033[1;34m[=] Initializing BTMagus Environment...\033[0m\n";
    
    try {
        auto con = sdbus::createSystemBusConnection();
        
        // 1. Check BlueZ Daemon
        auto dbusProxy = sdbus::createProxy(*con, 
            sdbus::ServiceName("org.freedesktop.DBus"), 
            sdbus::ObjectPath("/org/freedesktop/DBus"));

        bool hasOwner = false;
        dbusProxy->callMethod("NameHasOwner")
            .onInterface(sdbus::InterfaceName("org.freedesktop.DBus"))
            .withArguments("org.bluez")
            .storeResultsTo(hasOwner);

        std::cout << (hasOwner ? "  \033[32m●\033[0m" : "  \033[31m○\033[0m") 
                  << " BlueZ daemon:    " << (hasOwner ? "Running" : "Not Running") << "\n";

        // 2. Check Adapter State
        auto adapterProxy = sdbus::createProxy(*con, 
            sdbus::ServiceName{"org.bluez"}, 
            sdbus::ObjectPath{getAdapterObjectPath()});

        auto adapterPowerState = adapterProxy->getProperty("Powered")
            .onInterface(sdbus::InterfaceName{"org.bluez.Adapter1"})
            .get<bool>();

        std::cout << (adapterPowerState ? "  \033[32m●\033[0m" : "  \033[31m○\033[0m") 
                  << " BT Adapter:     " << (adapterPowerState ? "Powered ON" : "Powered OFF") << "\n";

    } catch (const sdbus::Error& e) {
        std::cerr << "  \033[31m!\033[0m D-Bus Error: " << e.what() << "\n";
    }
    
    std::cout << "------------------------------------------\n" << std::endl;
}

/**
 * Creates and configures the Flipper Scan sub-menu.
 */
std::unique_ptr<cli::Menu> createFlipperScanMenu(MessageDispatcher& dispatcher) {
    auto menu = std::make_unique<cli::Menu>("flipper_scan", "Scan for nearby flippers", "flipper_scan");

    menu->Insert("run", [&dispatcher](std::ostream& out) {
        if (isFlipperScanRunning.load()) {
            out << "Scan is already running!\n";
            return;
        }
        out << "Scan is starting...\n";
        isFlipperScanRunning.store(true);
        std::thread(scanStart, std::ref(dispatcher)).detach();
    }, "Scan for nearby flippers");

    menu->Insert("stop", [](std::ostream& out) {
        if (!isFlipperScanRunning.load()) {
            out << "No scan is currently running.\n";
            return;
        }
        out << "Stopping scan...\n";
        isFlipperScanRunning.store(false);
    }, "Stop the current scan");

    menu->Insert("status", [](std::ostream& out) {
        out << (isFlipperScanRunning.load() ? "Scan is currently RUNNING\n" : "Scan is NOT running\n");
    }, "Check scan status");

    menu->Insert("show", [](std::ostream& out) {
        std::lock_guard lock(BTDevicesMutex);
        if (BTDevices.empty()) {
            out << "You haven't scanned for bluetooth devices yet!\n";
            return;
        }

        bool has_flipper = false;
        for (auto& device : BTDevices) {
            if (device.amIFlipper() || device.amISpoofedFlipper()) {
                has_flipper = true;
                out << "------------------------------\n"
                    << (device.amIFlipper() ? "🐬 | " : "🎭 | ")
                    << device.Name << " | " << device.Address << "\nUUIDS:\n";
                for (const auto& uuid : device.Uuids) out << uuid << "\n";
                out << "------------------------------\n";
            }
        }
        if (!has_flipper) out << "No flippers have been detected yet.\n";
    }, "Show a list of detected flippers");

    return menu;
}

/**
 * Creates and configures the BLE Spam sub-menu.
 */
std::unique_ptr<cli::Menu> createBleSpamMenu(MessageDispatcher& dispatcher) {
    auto menu = std::make_unique<cli::Menu>("ble_spam", "Spam BLE advertisements", "ble_spam");

    menu->Insert("run", [&dispatcher](std::ostream& out) {
        if (isBleSpamRunning.load()) {
            out << "BLE spam already running.\n";
            return;
        }
        out << "Running BLE spam...\n";
        isBleSpamRunning.store(true);
        std::thread(startBleSpam, std::ref(dispatcher)).detach();
    }, "Run BLE Spam");

    menu->Insert("stop", [](std::ostream& out) {
        if (!isBleSpamRunning.load()) {
            out << "BLE spam is not currently running.\n";
            return;
        }
        out << "Stopping BLE spam...\n";
        isBleSpamRunning.store(false);
    }, "Stop BLE spam");

    return menu;
}

// --- Main Execution ---

int main() {
    // 1. Set Initial Global States
    isFlipperScanRunning.store(false);
    isBleSpamRunning.store(false);
    isMessageDispatcherRunning.store(true);

    // 2. Initialize Dispatcher
    MessageDispatcher msgDispatcher(std::cout);

    // 3. Display System Status BEFORE starting CLI
    checkSystemEnvironment(msgDispatcher);

    // 4. Build Menu Structure
    auto rootMenu = std::make_unique<cli::Menu>("BTMagus");
    rootMenu->Insert(createFlipperScanMenu(msgDispatcher));
    rootMenu->Insert(createBleSpamMenu(msgDispatcher));

    cli::Cli cli(std::move(rootMenu));

    // 5. Configure Exit Behavior
    cli.ExitAction([](auto& out) {
        isFlipperScanRunning.store(false);
        isBleSpamRunning.store(false);
        isMessageDispatcherRunning.store(false);
        out << "Bye.\n";
    });

    // 6. Launch CLI Session
    cli::CliFileSession input(cli);
    input.Start();

    return 0;
}
