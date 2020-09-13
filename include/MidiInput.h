#ifndef MIDIINPUT_H
#define MIDIINPUT_H

#include <memory>

class MidiInputImpl;

const unsigned int  FMAX_SYSEX_LENGTH    = 65535;
const unsigned long TE_VM_FLAGS_PARSE_RX = 1;

class MidiInput
{
    public:
        MidiInput(const wchar_t* portName);
        virtual ~MidiInput();

        unsigned long int getMidiCommand(unsigned char*);
        bool sendMidiCommand(unsigned char*, unsigned long int);

    protected:

    private:
        std::unique_ptr<MidiInputImpl> _impl;
};

#endif // MIDIINPUT_H
