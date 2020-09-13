#ifndef ARDUINOSERIALINTERFACE_H
#define ARDUINOSERIALINTERFACE_H

#include <memory>

class SerialPort;

const unsigned char COMMAND_SHOW_LEDS = 145;
const unsigned char COMMAND_CLEAR_LEDS = 146;

const unsigned long int BAUDRATE = 115200;
const unsigned char KEYMAP_LENGTH = 73;

class ArduinoSerialInterface
{
    public:
        ArduinoSerialInterface(const char* comPort, unsigned char offset);
        virtual ~ArduinoSerialInterface();

        const unsigned char getOffset();
        void setOffset(const unsigned char offset);
        void processMidiCommand(unsigned char* data, unsigned long int length);
        void close();
        bool clearLeds();
        bool setLed(unsigned char idx, unsigned char col);
        bool showLeds();


    protected:

    private:
        unsigned char               _offset;
        bool                        _isBlackMap[12];
        unsigned char               _keyMap[KEYMAP_LENGTH];
        std::unique_ptr<SerialPort> _serialPort;

        bool isBlack (unsigned char key);
        void activateLed(unsigned char key, unsigned char col);
        void deactivateLed(unsigned char key);
};

#endif // ARDUINOSERIALINTERFACE_H
