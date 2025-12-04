#include <iostream>
#include <map>
#include <sdbus-c++/sdbus-c++.h>

// sudo dbus-monitor --system "sender='org.bluez'"

static const std::vector<std::string> FlipperMACPrefixes = {"80:e1:26", "80:e1:27", "0c:fa:22"};
static const std::string FlipperSpamUUID = "00001812-0000-1000-8000-00805f9b34fb";
static const std::unordered_map<std::string, std::string> FlipperSpoofedUUID {
            {"startswith", "0000308",},
            {"endswith", "0000-1000-8000-00805f9b34fb"}
};
static uint32_t spam_device_count = 0;

struct BTDevice {
    std::string Name;
    std::string Address;
    std::vector<std::string> Uuids;
    bool is_flipper = false;
    bool is_spam = false;
    bool is_flipper_spoofed = false;

    static std::string mac_prefix(const std::string &s) {
        return s.substr(0, 8);
    }

    static std::string uuid_starts_with(const std::string &s) {
        return s.substr(0, 7);
    }

    static std::string uuid_ends_with(const std::string &s) {
        return s.substr(s.length() - 27);
    }

    // MAC needs to be lowercase
    bool amIFlipper() {
        for (const auto &flipper_mac_prefix : FlipperMACPrefixes) {
            if (flipper_mac_prefix == mac_prefix(this->Address)) {
                this->is_flipper = true;
                return true;
            }
        }
        return false;
    }

    bool amISpam() {
        for (const auto &uuid : this->Uuids) {
            if (uuid == FlipperSpamUUID) {
                this->is_spam = true;
                return true;
            }
        }
        return false;
    }

    bool amISpoofedFlipper() {
        for (const auto &uuid : this->Uuids) {
            if (uuid_starts_with(uuid) == FlipperSpoofedUUID.find("startswith")->second
                && uuid_ends_with(uuid) == FlipperSpoofedUUID.find("endswith")->second) {
                this->is_flipper_spoofed = true;
                return true;
            }
        }
        return false;
    }

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

                    // If device doesn't already exist and is not spam, add it to the list
                    if (auto it = std::ranges::find(devices.begin(),
                        devices.end(), device); it == devices.end() && !device.amISpam()) {
                        devices.push_back(device);

                        // Print found devices
                        if (device.amIFlipper()) {
                            std::cout << "🐬 |";
                        }
                        if (device.amISpoofedFlipper()) {
                            std::cout << " 🎭 |";
                        }
                        if (!device.amIFlipper() && !device.amISpoofedFlipper()) {
                            std::cout << "ᛒ |";
                        }

                        std::cout << " Name: " << device.Name
                        << " | MAC: " << device.Address
                        << " | UUIDs: " << std::endl;
                        for (const auto & uuid : device.Uuids)
                            std::cout << uuid << " | " << std::endl;
                    }
                    else if (device.amISpam()) {
                        if (++spam_device_count % 20 == 0) {
                            std::cout << "Bluetooth spam detected! Fake advertisements count: " << spam_device_count
                                << std::endl;
                        }
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