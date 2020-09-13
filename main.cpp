#include <iostream>
#include <memory>
#include <thread>
#include <sstream>
#include <iomanip>

#include "MidiInput.h"
#include "ArduinoSerialInterface.h"

using namespace std;

class MidiCommandBuffer
{
private:
    unsigned char* buffer;

public:
    MidiCommandBuffer():buffer(new unsigned char[FMAX_SYSEX_LENGTH])
    {
    }

    ~MidiCommandBuffer()
    {
        delete[] buffer;
    }

    unsigned char* const getBuffer()
    {
        return buffer;
    }
};

std::string toHexString(unsigned char* buffer, int l)
{
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < l; i++)
    {
        ss << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
        if (i < l-1)
            ss << ':';
    }
    return ss.str();
}

int main(int argc, char** argv)
{
    // MidiInput-Instanz erstellen
    unique_ptr<MidiInput>         _midiInput = make_unique<MidiInput>(L"MidiVisualizer");
    unique_ptr<MidiCommandBuffer> _buffer(new MidiCommandBuffer());

    // Parse args
    std::string comPort = "COM5";
    int offset = 38;
    if (argc > 1) {
        std::istringstream iss(argv[1]);
        std::string name;
        if (!(iss >> name))
        {
            comPort = "COM5";
        }
        else
        {
            comPort = name;
        }
    }
    if (argc > 2) {
        std::istringstream iss(argv[2]);
        std::string name;
        if (!(iss >> offset))
        {
            offset = 38;
        }
    }

    cout<< "comPort: " << comPort << endl;
    cout<< "offset: " << offset << endl;

    // Arduino-SerialInterface erstellen
    unique_ptr<ArduinoSerialInterface> _asi = make_unique<ArduinoSerialInterface>(comPort.c_str(), offset);

    if (_asi->isOpen() )
    {
        cout << "arduino connected!" << endl;
        bool active(true);
        thread t( [&active, &_midiInput, &_buffer, &_asi](){
            while(active) {
                unsigned long int l = _midiInput->getMidiCommand(_buffer->getBuffer());
                if (l > 0) {

                    cout << "command(" << toHexString(_buffer->getBuffer(), l) << ")" << endl;

                    _asi->processMidiCommand(_buffer->getBuffer(), l); // so? lange her ;-)

                    // echo command
                    _midiInput->sendMidiCommand(_buffer->getBuffer(), l);
                }
                this_thread::sleep_for(chrono::milliseconds(1));
            }
        });

        cin.get();

        active = false;
        _midiInput.reset();
        t.join();
    }

    // fertig
    return 0;
}
