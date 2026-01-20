#ifndef BTMAGUS_MESSAGE_H
#define BTMAGUS_MESSAGE_H
#include <string>
#include <utility>

class Message {
    public:
        enum Source {
            FlipperScan,
            BleSpam
        };
        Source source;
        std::string message;
        Message(const Source src, std::string msg) : source(src), message(std::move(msg)) {}

        template<typename T>
        Message& operator<<(const T& value) {
            std::ostringstream ss;
            ss << value;
            message += ss.str();
            return *this;
        }
};

#endif //BTMAGUS_MESSAGE_H