#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <memory>

class SerialPortImpl;

class SerialPort
{
    public:
        SerialPort(const char*, unsigned long int);
        virtual ~SerialPort();

        void open();
        bool isOpen();
        void close();
        int read(unsigned char* buffer, int offset, int length);
        void write(unsigned char* buffer, int offset, int length);

    protected:

    private:
        std::unique_ptr<SerialPortImpl> _impl;
};

#endif // SERIALPORT_H
