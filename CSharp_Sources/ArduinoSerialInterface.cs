using System;
using System.IO.Ports;

namespace MidiToArduinoLight
{
    class ArduinoSerialInterface
    {
        /*
           LED Palette defined in Arduino-Code:
          
           const byte palette[] = {
             0,0,0,   // off
             0,0,100, // bright_blue  CH01 (w)
             0,0,71,  // dark_blue    CH01 (k)
             0,100,0, // bright_green CH02 (w)
             0,71,0,  // dark_green   CH02 (k)
             100,0,0, // bright_red   CH03 (w)
             71,0,0,  // dark_red     CH03 (k)
             71,71,0, //              CH04 (w)
             36,36,0, //              CH04 (k)
             71,0,71, //              CH05 (w)
             36,0,36, //              CH05 (k)
             0,71,71, //              CH06 (w)
             0,36,36, //              CH06 (k)
             61,61,20,//              CH07 (w)
             31,31,10,//              CH07 (k)
             20,61,61,//              CH08 (w)
             10,31,31,//              CH08 (k)
             61,20,61,//              CH09 (w)
             31,10,31,//              CH09 (k)
             58,57,58,//              CH10 (w)
             29,28,29 //              CH10 (k)
           };
        */


        const byte COMMAND_SHOW_LEDS = 145;
        const byte COMMAND_CLEAR_LEDS = 146;
        
        private byte offset;
        public byte Offset {
            get { return offset; }
            set { offset = value; }
        }

        public byte IsBlackOffset { get; set; }

        private bool[] isBlackMap = { false, true, false, true, false, false, true, false, true, false, true, false};
        private bool IsBlack (byte key)
        {
            return isBlackMap[(key + IsBlackOffset) % 12];
        }

        byte[] keyMap = new byte[] {
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26,
            27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51,
            53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77,
            79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103,
            105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125,
            127, 129, 131, 133, 135, 137, 139, 141, 143
        };
        
        private SerialPort _serialPort;

        public ArduinoSerialInterface(string port, byte offset, byte isBlackOffset)
        {
            Offset = offset;
            IsBlackOffset = isBlackOffset;
            _serialPort = new SerialPort(port, 115200); // Standardwerte für angeschlossenen Arduino
            _serialPort.Open();
        }
        
        public void Close()
        {
            if (_serialPort.IsOpen)
            {
                clearLeds();
                _serialPort.Close();
            }
        }
        
        public bool setLed(byte idx, byte col)
        {
            if ( idx >= offset && idx < keyMap.Length + offset)
            {
                byte key = keyMap[idx - offset];
                byte[] buf = new byte[] { key, col };
                _serialPort.Write(buf, 0, 2);
                _serialPort.Read(buf, 0, 2);
                if (buf[0] == key && buf[1] == col)
                {
                    return true;
                }
            }
            return false;
        }
        
        public bool showLeds()
        {
            byte[] buf = new byte[] { COMMAND_SHOW_LEDS };
            _serialPort.Write(buf, 0 ,1);
            _serialPort.Read(buf, 0, 1);
            if (buf[0] == COMMAND_SHOW_LEDS)
            {
                return true;
            }
            return false;
        }
        
        public bool clearLeds()
        {
            byte[] buf = new byte[] { COMMAND_CLEAR_LEDS };
            _serialPort.Write(buf, 0, 1);
            _serialPort.Read(buf, 0, 1);
            if (buf[0] == COMMAND_CLEAR_LEDS)
            {
                return true;
            }
            return false;
        }
        
        private void activateLed(byte key, byte col)
        {
            if (setLed(key, col))
            {
                Console.WriteLine("Set led " + key + " to 'on'");
            }
            else
            {
                Console.Error.WriteLine("Error setting led " + key + " to 'on'");
            }
        }
        
        private void deactivateLed(byte key)
        {
            if (setLed(key, 0x0))
            {
                Console.WriteLine("Set led " + key + " to 'off'");
            }
            else
            {
                Console.Error.WriteLine("Error setting led " + key + " to 'off'");
            }
        }
        
        public void processMidiCommand(byte[] cmd)
        {
            if (cmd.Length < 3)
            {
                return;
            }
            byte first = cmd[0];
            byte key = cmd[1];
            byte vel = cmd[2];
            byte upper = (byte)(first / 16);
            byte channelNumber = (byte)(first % 16);
            
            if (upper == 8 || upper == 9 && vel == 0x0)
            {
                // NoteOff
                deactivateLed(key);
            } else if (upper == 9)
            {
                // NoteOn
                byte color = (byte)((IsBlack(key) ? (2 + 2 * channelNumber) : (1 + 2 * channelNumber)));
                string bok = IsBlack(key) ? "k" : "w";
                Console.WriteLine("Setze für Kanal " + (channelNumber + 1) + " Note " + key + "(" + bok + ") auf Farbcode " + color);
                activateLed(key, color);
            }
        }
    }
}
