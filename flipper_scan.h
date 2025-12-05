#ifndef BTMAGUS_FLIPPER_SCAN_H
#define BTMAGUS_FLIPPER_SCAN_H

#include <iostream>
#include <map>
#include <sdbus-c++/sdbus-c++.h>

extern const std::vector<std::string> FlipperMACPrefixes;
extern const std::string FlipperSpamUUID;
extern const std::unordered_map<std::string, std::string> FlipperSpoofedUUID;
extern uint32_t spam_device_count;

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

void scanStart();

#endif //BTMAGUS_FLIPPER_SCAN_H