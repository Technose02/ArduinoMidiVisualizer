/* teVirtualMIDI C# interface
 *
 * Copyright 2009-2019, Tobias Erichsen
 * All rights reserved, unauthorized usage & distribution is prohibited.
 *
 *
 * File: TeVirtualMIDITest.cs
 *
 * This file contains a sample using the TeVirtualMIDI-class, which
 * implements a simple loopback-MIDI-port.
 */

using MidiToArduinoLight;
using System;
using System.Threading;


namespace TobiasErichsen.teVirtualMIDI.test {

	class Program {

		private static TeVirtualMIDI port = null;
		private static ArduinoSerialInterface asi = null;
		private static void WorkThreadFunction() {

			byte[] command;
			try {
				port = new TeVirtualMIDI("Midi Visualizer");
				Console.WriteLine("Virtual port created");
				while ( true ) {

					command = port.getCommand();

					asi.processMidiCommand(command);

					// echo command
					port.sendCommand(command);
				Console.WriteLine( "command: " + byteArrayToString( command ) );

				}
			} catch ( Exception ex ) {

				Console.WriteLine( "thread aborting: " + ex.Message );

			}
		}

		public static string byteArrayToString( byte[] ba ) {

			string hex = BitConverter.ToString( ba );

			return hex.Replace( "-" , ":" );

		}

		private static void reset()
		{
			if (port != null)
			{
				port.shutdown();
				port = null;
			}
			asi.clearLeds();
			new Thread(new ThreadStart(WorkThreadFunction)).Start();
		}

	static void Main( string[] args ) {

			//Console.WriteLine( "teVirtualMIDI C# loopback sample" );
			Console.WriteLine( "Welcome to Midi Visualizer 0.1" );
			Console.WriteLine( "using dll-version:    " + TeVirtualMIDI.versionString );
			Console.WriteLine( "using driver-version: " + TeVirtualMIDI.driverVersionString );

			//TeVirtualMIDI.logging( TeVirtualMIDI.TE_VM_LOGGING_MISC | TeVirtualMIDI.TE_VM_LOGGING_RX | TeVirtualMIDI.TE_VM_LOGGING_TX );
			//Guid manufacturer = new Guid( "aa4e075f-3504-4aab-9b06-9a4104a91cf0" );
			//Guid product = new Guid( "bb4e075f-3504-4aab-9b06-9a4104a91cf0" );

			//port = new TeVirtualMIDI( "C# loopback", 65535, TeVirtualMIDI.TE_VM_FLAGS_PARSE_RX, ref manufacturer, ref product );
			//port = new TeVirtualMIDI("Arduino LED Stripe");

			//			Thread thread = new Thread( new ThreadStart( WorkThreadFunction ) );
			//					thread.Start();

			switch (args.Length)
			{
				case 1:
					asi = new ArduinoSerialInterface(args[0], 38, 0);
					break;
				case 2:
					asi = new ArduinoSerialInterface(args[0], Byte.Parse(args[1]), 0);
					break;
				case 3:
					asi = new ArduinoSerialInterface(args[0], Byte.Parse(args[1]), Byte.Parse(args[2]));
					break;
				default:
					asi = new ArduinoSerialInterface("COM7", 38, 0);
					break;
			}

			reset();
			bool alive = true;
			while (alive)
			{
				switch (Console.ReadKey().Key)
				{
					case ConsoleKey.C:
						asi.clearLeds();
						break;
					case ConsoleKey.R:
						reset();
						break;
					case ConsoleKey.RightArrow:
						Console.WriteLine("offset: " + ++asi.Offset);
						break;
					case ConsoleKey.LeftArrow:
						Console.WriteLine("offset: " + --asi.Offset);
						break;
					case ConsoleKey.UpArrow:
						Console.WriteLine("offset: " + ++asi.IsBlackOffset);
						break;
					case ConsoleKey.DownArrow:
						Console.WriteLine("offset: " + --asi.IsBlackOffset);
						break;
					case ConsoleKey.Q:
						alive = false;
						break;
				}
			}

			port.shutdown();
			asi.Close();
		}

	}

}
