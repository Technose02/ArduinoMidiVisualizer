#include <iostream>
#include <memory>
#include <thread>

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

int main(int argc, char** argv)
{
    // MidiInput-Instanz erstellen
    unique_ptr<MidiInput>         _midiInput = make_unique<MidiInput>(L"MidiVisualizer");
    unique_ptr<MidiCommandBuffer> _buffer(new MidiCommandBuffer());
    unique_ptr<ArduinoSerialInterface> _asi = make_unique<ArduinoSerialInterface>("COM5", 38);

    bool active(true);
    thread t( [&active, &_midiInput, &_buffer, &_asi](){
        while(active) {
            unsigned long int l = _midiInput->getMidiCommand(_buffer->getBuffer());
            cout << "command(" << std::to_string(l) << ")" << endl;
            if (l > 0) {

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

    // fertig
    return 0;
}
