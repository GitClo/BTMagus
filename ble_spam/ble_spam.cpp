#include "ble_spam.h"

#include <chrono>
#include <thread>

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
    advertisementGenerator ag;
    while (isBleSpamRunning->load()) {
        msgDispatcher.dispatchMessage(Message(Message::BleSpam, ag.getGenuineBudsAdvertisement()));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}