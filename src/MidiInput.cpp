#include "MidiInput.h"
#include <memory>
#include <iostream>
#include "teVirtualMIDI.h"

class MidiInputImpl
{
private:
    LPVM_MIDI_PORT port;

public:
    MidiInputImpl(const wchar_t* portName)
    {
        std::cout << "MidiInputImpl erstellt" << std::endl;

        port = virtualMIDICreatePortEx2(portName, NULL, 0, FMAX_SYSEX_LENGTH, TE_VM_FLAGS_PARSE_RX);

        if (port == NULL)
        {
            std::cerr << "Error: Creation of virtual midi port failed!" << port << std::endl;
        } else
        {
            std::wcout << "Virtual midi port \"" << portName << "\" created (" << port << ")" << std::endl;
        }
    }

    virtual ~MidiInputImpl()
    {
        std::cout << "trying to shut down port...";
        if ( port != NULL && virtualMIDIShutdown(port) )
        {
            std::cout << "success" << std::endl;
        }
        else
        {
            std::cout << "failed (was it active actually?)" << std::endl;
        }


        std::cout << "MidiInputImpl zerstoert" << std::endl;
    }

    long unsigned int getMidiCommand(unsigned char* buffer)
    {
        long unsigned int length(FMAX_SYSEX_LENGTH);
        bool success = virtualMIDIGetData(port, buffer, &length);
        if (success)
          return length;
        return 0;
    }

    bool sendMidiCommand(unsigned char* buffer, unsigned long int length)
    {
        return virtualMIDISendData(port, buffer, length);
    }
};

MidiInput::MidiInput(const wchar_t* portName)
{
    std::cout << "MidiInput erstellt" << std::endl;

    _impl = std::make_unique<MidiInputImpl>(portName);
}

MidiInput::~MidiInput()
{
    _impl.reset();

    std::cout << "MidiInput zerstoert" << std::endl;
}

long unsigned int  MidiInput::getMidiCommand(unsigned char* buffer)
{
    return _impl->getMidiCommand(buffer);
}

bool MidiInput::sendMidiCommand(unsigned char* buffer, unsigned long int length)
{
    return _impl->sendMidiCommand(buffer, length);
}
