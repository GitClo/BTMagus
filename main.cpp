#include <iostream>
#include <map>
#include <sdbus-c++/sdbus-c++.h>

// sudo dbus-monitor --system "sender='org.bluez'"

struct BTDevice {
    std::string Name;
    std::string Address;

    bool operator==(const BTDevice& other) const {
        return Address == other.Address;
    }
};

int main() {

    static std::vector<BTDevice> devices;

    try {
        auto connection = sdbus::createSystemBusConnection();

        auto adapterProxy =
            sdbus::createProxy(*connection, sdbus::ServiceName{"org.bluez"},
            sdbus::ObjectPath{"/org/bluez/hci0"});

        auto ObjectManagerProxy =
            sdbus::createProxy(*connection, sdbus::ServiceName{"org.bluez"},
            sdbus::ObjectPath{"/"});

        adapterProxy->callMethod("StartDiscovery").onInterface(sdbus::InterfaceName{"org.bluez.Adapter1"}).dontExpectReply();

        ObjectManagerProxy->uponSignal(sdbus::SignalName{"InterfacesAdded"}).onInterface(
            sdbus::InterfaceName{"org.freedesktop.DBus.ObjectManager"}).call(
            [](sdbus::ObjectPath const & devicePath,
                 std::map<std::string, std::map<std::string, sdbus::Variant>> const & interfaces) {

                if (const auto it = interfaces.find("org.bluez.Device1"); it != interfaces.end()) {

                    const auto properties = it->second; // The second map is the properties map

                    const auto nameIt = properties.find("Name");
                    const auto addressIt = properties.find("Address");

                    BTDevice device;
                        device.Name = nameIt->second.get<std::string>();
                        device.Address = addressIt->second.get<std::string>();

                    if (auto it = std::ranges::find(devices.begin(), devices.end(), device); it != devices.end()) {
                    devices.push_back(device);

                    std::cout << "Name: " << devices.back().Name
                    << " | MAC: " << devices.back().Address << std::endl;
                    }
                }
                });

        connection->enterEventLoop();
    }
    catch (const sdbus::Error& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}