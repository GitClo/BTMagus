#include "flipper_scan.h"

using namespace flipper_scan;

// sudo dbus-monitor --system "sender='org.bluez'"

namespace flipper_scan {
    std::vector<BTDevice> BTDevices;

    const std::vector<std::string> FlipperMACPrefixes = {"80:e1:26", "80:e1:27", "0c:fa:22"};
    const std::string FlipperSpamUUID = "00001812-0000-1000-8000-00805f9b34fb";
    const std::unordered_map<std::string, std::string> FlipperSpoofedUUID {
                {"startswith", "0000308",},
                {"endswith", "0000-1000-8000-00805f9b34fb"}
    };
    uint32_t spam_device_count = 0;
}

void scanStart(MessageDispatcher &msgDispatcher) {

    try {
        auto connection = sdbus::createSystemBusConnection();

        auto adapterProxy =
            sdbus::createProxy(*connection, sdbus::ServiceName{"org.bluez"},
            sdbus::ObjectPath{getAdapterObjectPath()});

        auto ObjectManagerProxy =
            sdbus::createProxy(*connection, sdbus::ServiceName{"org.bluez"},
            sdbus::ObjectPath{"/"});

        adapterProxy->callMethod("StartDiscovery").onInterface(sdbus::InterfaceName{"org.bluez.Adapter1"}).dontExpectReply();

        ObjectManagerProxy->uponSignal(sdbus::SignalName{"InterfacesAdded"}).onInterface(
            sdbus::InterfaceName{"org.freedesktop.DBus.ObjectManager"}).call(
            [&msgDispatcher](sdbus::ObjectPath const & devicePath,
                             std::map<std::string, std::map<std::string, sdbus::Variant>> const & interfaces) {

                if (const auto it = interfaces.find("org.bluez.Device1"); it != interfaces.end()) {

                    const auto properties = it->second; // The second map is the properties map

                    const auto nameIt = properties.find("Name");
                    const auto addressIt = properties.find("Address");
                    const auto uuidsIt = properties.find("UUIDs");

                    BTDevice device;
                        device.Name = nameIt->second.get<std::string>();

                        // Convert mac address to lowercase
                        auto address = addressIt->second.get<std::string>();
                        std::ranges::transform(address, address.begin(),
                        [](const unsigned char c) {
                            return static_cast<char>(std::tolower(c));
                            });
                        device.Address = std::move(address);

                        device.Uuids = uuidsIt->second.get<std::vector<std::string>>();


                    {
                        std::lock_guard lock_btdevices(BTDevicesMutex);

                        // Message to be sent
                        Message msg(Message::FlipperScan, "");

                        // If device doesn't already exist and is not spam, add it to the list
                       if (auto it = std::ranges::find(BTDevices.begin(),
                           BTDevices.end(), device); it == BTDevices.end() && !device.amISpam()) {
                           BTDevices.push_back(device);

                           // Print found devices
                           if (device.amIFlipper()) {
                               msg << "🐬 |";
                           }
                           else if (device.amISpoofedFlipper()) {
                               msg << " 🎭 |";
                           }
                           if (!device.amIFlipper() && !device.amISpoofedFlipper()) {
                               msg << "ᛒ |";
                           }

                           msg << " Name: " << device.Name
                           << " | MAC: " << device.Address
                           << " | UUIDs: " << '\n';
                           for (const auto & uuid : device.Uuids) {
                               msg << uuid << " | " << '\n';
                           }
                           msgDispatcher.dispatchMessage(msg);
                       }
                       else if (device.amISpam()) {
                           if (++spam_device_count % 20 == 0) {
                              msg << "Bluetooth spam detected! Fake advertisements count: " << spam_device_count
                                  << '\n';
                               msgDispatcher.dispatchMessage(msg);
                          }
                       }
                    }
                }
            });

        // Manual event loop, checking for stop signal
        while (isFlipperScanRunning.load()) {
            connection->processPendingEvent();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Stop bluetooth discovery
        adapterProxy->callMethod("StopDiscovery").onInterface(sdbus::InterfaceName{"org.bluez.Adapter1"}).dontExpectReply();

    }
    catch (const sdbus::Error& e) {
        Message err_msg(Message::FlipperScan, "Error - FlipperScan]: ");
        err_msg << e.what() << '\n';
        msgDispatcher.dispatchMessage(err_msg);
    }
}