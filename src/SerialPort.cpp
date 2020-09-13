#include <memory>
#include <iostream>
#include "Windows.h"
#include "SerialPort.h"

/*
    SEE:
            - https://salilkapur.wordpress.com/2013/03/08/communicating-with-arduino-using-c/
            - https://docs.microsoft.com/de-de/windows/win32/api/fileapi/nf-fileapi-createfilea
            - https://docs.microsoft.com/de-de/windows/win32/devio/communications-functions
            - https://docs.microsoft.com/de-de/windows/win32/devio/configuring-a-communications-resource
            - https://docs.microsoft.com/de-de/windows/win32/api/fileapi/nf-fileapi-readfile
            - https://docs.microsoft.com/de-de/windows/win32/api/fileapi/nf-fileapi-writefile
            - https://docs.microsoft.com/de-de/windows/win32/api/handleapi/nf-handleapi-closehandle

            - https://github.com/manashmandal/SerialPort/blob/master/src/SerialPort.cpp
*/


class SerialPortImpl
{
private:

    bool              _connected;
    HANDLE            _file;
    std::string       _portName;
    unsigned long int _baudrate;

    void abort()
    {
        if ( !PurgeComm(_file, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR) )
        {
            printf ("PurgeComm failed with error %u.\n", GetLastError());
        }
    }

    void reset()
    {
        if (_connected)
        {
            abort();
        }

        /*
            The CreateFile function can create a handle to a communications resource, such as the serial port COM1.
            For communications resources, the dwCreationDisposition parameter must be OPEN_EXISTING, the dwShareMode
            parameter must be zero (exclusive access), and the hTemplateFile parameter must be NULL. Read, write, or
            read/write access can be specified, and the handle can be opened for overlapped I/O.
        */
        _file = CreateFileA(_portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

        if (_file == INVALID_HANDLE_VALUE)
        {
            //  Handle the error.
            DWORD err = GetLastError();
            printf ("CreateFileA failed with error %u.\n", err);
            printf ("COM-PORT \"%s\" could not be opened!\n", _portName.c_str());
            return;
        }

        DCB dcb;
        //  Initialize the DCB structure.
        memset(&dcb, 0, sizeof(DCB));
        dcb.DCBlength = sizeof(DCB);

        //  Build on the current configuration by first retrieving all current settings.
        if ( !GetCommState(_file, &dcb) )
        {
            //  Handle the error.
            DWORD err = GetLastError();
            printf ("GetCommState failed with error %u.\n", err);
            printf ("COM-PORT \"%s\" could not be opened!\n", _portName.c_str());
            return;
        }

        //PrintCommState(dcb);       //  Output to console

        dcb.BaudRate    = _baudrate;     //  baud rate
        dcb.ByteSize    = 8;             //  data size, xmit and rcv
        dcb.Parity      = NOPARITY;      //  parity bit
        dcb.StopBits    = ONESTOPBIT;    //  stop bit
        dcb.fDtrControl = DTR_CONTROL_ENABLE;

        if ( !SetCommState(_file, &dcb) )
        {
            //  Handle the error.
            DWORD err = GetLastError();
            printf ("SetCommState failed with error %u.\n", err);
            printf ("COM-PORT \"%s\" could not be opened!\n", _portName.c_str());
            return;
        }

        //  Get the comm config again.
        if ( !GetCommState(_file, &dcb) )
        {
            //  Handle the error.
            DWORD err = GetLastError();
            printf ("GetCommState failed with error %u.\n", err);
            printf ("COM-PORT \"%s\" could not be opened!\n", _portName.c_str());
            return;
        }

        //PrintCommState(dcb);       //  Output to console

        _connected = true;
    }

public:
    SerialPortImpl(const char* comPort, unsigned long int baudrate):
        _connected(false),
        _file(INVALID_HANDLE_VALUE),
        _portName(comPort),
        _baudrate(baudrate)
    {
    }

    virtual ~SerialPortImpl()
    {
        close();
    }

    void open()
    {
        reset();
    }

    bool isOpen()
    {
        return _connected;
    }

    void close()
    {
        if (_connected)
        {
            abort();
            if ( !CloseHandle(_file) )
            {
                DWORD err = GetLastError();
                printf ("CloseHandle failed with error %u.\n", err);
                printf ("COM-PORT \"%s\" could not be closed!\n", _portName.c_str());
                return;
            }
        }
    }

    long unsigned int read(unsigned char* buffer, int offset, int length)
    {
        long unsigned int ret = 0;
        if ( !ReadFile(_file, (void*)buffer + offset, length, &ret, NULL) )
        {
            printf ("ReadFile failed with error %u.\n", GetLastError());
            return 0;
        }
        return ret;
    }

    long unsigned int write(unsigned char* buffer, int offset, int length)
    {
        long unsigned int ret = 0;
        if ( !WriteFile(_file, (void*)buffer + offset, length, &ret, NULL) )
        {
            printf ("WriteFile failed with error %u.\n", GetLastError());
            return 0;
        }
        return ret;
    }
};


SerialPort::SerialPort(const char* comPort, unsigned long int baudrate)
{
    std::cout << "SerialPort erstellt" << std::endl;

    _impl = std::make_unique<SerialPortImpl>(comPort, baudrate);
}

SerialPort::~SerialPort()
{
    _impl.reset();

    std::cout << "SerialPort zerstoert" << std::endl;
}

void SerialPort::open()
{
    _impl->open();
}

bool SerialPort::isOpen()
{
    return _impl->isOpen();
}

void SerialPort::close()
{
    _impl->close();
}

int SerialPort::read(unsigned char* buffer, int offset, int length)
{
    return _impl->read(buffer, offset, length);
}

void SerialPort::write(unsigned char* buffer, int offset, int length)
{
    _impl->write(buffer, offset, length);
}
