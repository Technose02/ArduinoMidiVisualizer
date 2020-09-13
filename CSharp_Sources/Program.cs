using System;

namespace MidiToArduinoLight
{
    class Program
    {
        /*
        public static void Main(string[] args)
        {
            Console.WriteLine("Test Arduino-Serial-Com:");

            ArduinoSerialInterface asi = new ArduinoSerialInterface();
            bool doLoop = true;
            while (doLoop)
            {
                ConsoleKeyInfo key = Console.ReadKey();
                switch(key.Key)
                {
                    case ConsoleKey.D1:
                        if (asi.setLed(0x1,0x2))
                        {
                            Console.WriteLine("Set key 1 to bright_green");
                        } else
                        {
                            Console.Error.WriteLine("Could not set key 1 to bright_green");
                        }
                        break;
                    case ConsoleKey.D2:
                        if (asi.showLeds())
                        {
                            Console.WriteLine("showLeds() triggered");
                        }
                        else
                        {
                            Console.Error.WriteLine("Could not trigger showLeds()");
                        }
                        break;
                    case ConsoleKey.D3:
                        if (asi.clearLeds())
                        {
                            Console.WriteLine("clearLeds() triggered");
                        }
                        else
                        {
                            Console.Error.WriteLine("Could not trigger clearLeds()");
                        }
                        break;
                    default:
                        doLoop = false;
                        break;
                }
            }
            asi.Close();
        }
        */
    }
}
