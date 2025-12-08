#ifndef BTMAGUS_BLE_SPAM_H
#define BTMAGUS_BLE_SPAM_H
#include <map>
#include <string>
#include <random>

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


public:
    explicit advertisementGenerator(const std::map<std::string, std::string> &genuineBudsIds,
        const std::map<std::string, std::string> &genuineWatchIds)
        : genuineBudsIds(genuineBudsIds), genuineWatchIds(genuineWatchIds) {};

    std::string getGenuineBudsAdvertisement() const {
        return pickRandomKey(genuineBudsIds);
    }

    std::string getGenuineWatchAdvertisement() const {
        return pickRandomKey(genuineWatchIds);
    }
};
#endif //BTMAGUS_BLE_SPAM_H