#include "ArduinoSerialInterface.h"
#include <iostream>
#include "SerialPort.h"

ArduinoSerialInterface::ArduinoSerialInterface(const char* comPort, unsigned char offset, unsigned char isBlackOffset)
: _offset(offset),
  _isBlackOffset(isBlackOffset),
  _isBlackMap{false, true, false, true, false, false, true, false, true, false, true, false},
  _keyMap{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26,
          27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51,
          53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77,
          79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103,
          105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125,
          127, 129, 131, 133, 135, 137, 139, 141, 143
        },
  _serialPort(new SerialPort(comPort, BAUDRATE))
{
    _serialPort->open();
}

ArduinoSerialInterface::~ArduinoSerialInterface()
{}

const unsigned char ArduinoSerialInterface::getOffset()
{
    return _offset;
}

void ArduinoSerialInterface::setOffset(const unsigned char offset)
{
    _offset = offset;
}

void ArduinoSerialInterface::processMidiCommand(unsigned char* data, unsigned long int length)
{
    if (length < 3)
    {
        return;
    }

    unsigned char first = data[0];
    unsigned char key = data[1];
    unsigned char vel = data[2];
    unsigned char upper = (unsigned char )(first / 16);
    unsigned char channelNumber = (unsigned char )(first % 16);

    if (upper == 8 || (upper == 9 && vel == 0x0))
    {
        // NoteOff
        deactivateLed(key);
    }
    else if (upper == 9)
    {
        // NoteOn
        unsigned char color = (unsigned char)((isBlack(key) ? (2 + 2 * channelNumber) : (1 + 2 * channelNumber)));
        char bok = isBlack(key) ? 'k' : 'w';
        std::cout << "Setze fuer Kanal " << (unsigned int) (channelNumber + 1) << " Note " << (unsigned int) key << "(" << bok << ") auf Farbcode " << (unsigned int) color << std::endl;
        activateLed(key, color);
    }
}

bool ArduinoSerialInterface::isBlack (unsigned char key)
{
    return _isBlackMap[(key + _isBlackOffset) % 12];
}

bool ArduinoSerialInterface::isOpen ()
{
    return _serialPort->isOpen();
}

void ArduinoSerialInterface::close()
{
    if (_serialPort->isOpen())
    {
        clearLeds();
        _serialPort->close();
    }
}

bool ArduinoSerialInterface::clearLeds()
{
    unsigned char buf[]{ COMMAND_CLEAR_LEDS };
   _serialPort->write(buf, 0, 1);
   _serialPort->read(buf, 0, 1);
   if (buf[0] == COMMAND_CLEAR_LEDS)
   {
       return true;
   }
   return false;
}

bool ArduinoSerialInterface::setLed(unsigned char idx, unsigned char col)
{
    if ( idx >= _offset && idx < KEYMAP_LENGTH + _offset)
    {
        unsigned char key = _keyMap[idx - _offset];
        unsigned char buf[]{ key, col };
        _serialPort->write(buf, 0, 2);
        _serialPort->read(buf, 0, 2);
        if (buf[0] == key && buf[1] == col)
        {
            return true;
        }
    }
    return false;
}

void ArduinoSerialInterface::activateLed(unsigned char key, unsigned char col)
{
    if (setLed(key, col))
    {
        std::cout << "Set led " << key << " to ""on""" << std::endl;
    }
    else
    {
        std::cout << "Error setting led " << key << " to ""on""" << std::endl;
    }
}

void ArduinoSerialInterface::deactivateLed(unsigned char key)
{
    if (setLed(key, 0x0))
    {
        std::cout << "Set led " << key << " to ""off""" << std::endl;
    }
    else
    {
        std::cout << "Error setting led " << key << " to ""off""" << std::endl;
    }
}

bool ArduinoSerialInterface::showLeds()
{
    unsigned char buf[]{ COMMAND_SHOW_LEDS };
    _serialPort->write(buf, 0 ,1);
    _serialPort->read(buf, 0, 1);
    if (buf[0] == COMMAND_SHOW_LEDS)
    {
        return true;
    }
    return false;
}



