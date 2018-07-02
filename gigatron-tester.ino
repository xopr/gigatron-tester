#define DELAY 100

void sendInstruction( byte _opcode, byte _operand, int _delay = DELAY );

byte g_mode = 0;

#if !defined(ARDUINO_AVR_UNO)
#error "The pinmapping is opinly tested with Arduino Uno"
#endif

void setup()
{
  
  // Pin 8-12 output (13 is LED)
  DDRB = 0b00011111;
  // ADC0-5
  DDRC = 0b00111111;
  // Pin 1-7 (1 is TxD)
  DDRD = 0b11111110;

  Serial.begin( 115200 );
  writeNotification();
}

void loop()
{
  if ( Serial.available() )
  {
    // Cache all bytes
    delay( 20 );

    // Read all of the data
    while ( Serial.available() )
      Serial.read();

    // Next mode
    g_mode++;

    writeNotification();
  }
  
  switch ( g_mode )
  {
    case 0:
      break;

      case 1:
        // Send toggling bit pattern
        sendInstruction( 0xAA, 0xAA );
        sendInstruction( 0x55, 0x55 );
        break;

      case 2:
        // Do the LED  blink!
        sendInstruction( 0x00, 0xAA );
        sendInstruction( 0x18, 0b10101010 ); // Order of HSYNC is important here (OUT6)
        sendInstruction( 0x18, 0b01010101 );
        sendInstruction( 0x00, 0x55 );
        sendInstruction( 0x18, 0b10101010 ); // Order of HSYNC is important here (OUT6)
        sendInstruction( 0x18, 0b01010101 );
        break;

      case 3:
        // RAM writer        
        // ST #D, Y, X++
        sendInstruction( 0xDC, 0xAA );
        sendInstruction( 0xDC, 0x55 );
        break;

      case 4:
        // RAM reader
        sendInstruction( 0x1D, 0xAA );
        sendInstruction( 0x1D, 0x55 );
        break;

      case 5:
        // Input
        // LD IN
        sendInstruction( 0x03, 0x00 );
        //
        sendInstruction( 0x18, 0b10101010 ); // Order of HSYNC is important here (OUT6)
        sendInstruction( 0x18, 0b01010101 );
        break;

    default:
      g_mode = 1;
  }
}

void writeNotification()
{
  switch ( g_mode )
  {
    case 0:
      Serial.println( F("Remove the EPROM (U7) and connect the following pins:") );
      Serial.println( F("Arduino - Gigatron") );
      Serial.println( F("2 - 19") );
      Serial.println( F("3 - 18") );
      Serial.println( F("4 - 17") );
      Serial.println( F("5 - 16") );
      Serial.println( F("6 - 15") );
      Serial.println( F("7 - 14") );
      Serial.println( F("ADC4 - 13") );
      Serial.println( F("ADC5 - 12") );
      Serial.println( F("8 - 10") );
      Serial.println( F("9 - 9") );
      Serial.println( F("10 - 8") );
      Serial.println( F("11 - 7") );
      Serial.println( F("13 - 6") );
      Serial.println( F("ADC0 - 5") );
      Serial.println( F("ACD1 - 4") );
      Serial.println( F("ACD2 - 3") );
      Serial.println( F("ACD3 - resistor R1 left side") );
      Serial.println( F("13 - 21 (remember this indicator cable)") );
      Serial.println( F("GND - GND (use the same USB power supply)") );
      Serial.println( F("Press enter to start the test" ) );
      break;

    case 1:
      writeLine();
      Serial.println( F( "Test U1 (CLK1) U3, U4 (PC), (U5, U6, PC), U8 (IR), U9 (D), U11 (Bus access decoder, JMP detector), U13 (Mode decoder), U14 (Instruction decoder)" ) );
      writeLine();
      Serial.println( F( "The Arduino LED should now be blinking with a 200ms interval" ) );
      Serial.println( F("Move Gigatron pin 21 to 22 and verify that the blinking speed halves..") );
      Serial.println( F("..and move the indicator again for pin 23 (up to 29 and 31-37)." ) );
      Serial.println( F("" ) );
      Serial.println( F("After this test, use the indicator to probe U11 pin 2 and 3 (IR0, IR1: they should blink consecutively at full speed." ) );
      Serial.println( F("U11 pin 5 and 6 (OE, AE): should also blink consecutively at full speed." ) );
      Serial.println( F("Same goes for U13 pins 1, 2 and 3 (IR2, IR3, IR4: odd bus number blinks separate from even)" ) );
      Serial.println( F("U13 pin 10 and 13 ([D,Y], [Y, D, AC]): should also blink consecutively at full speed." ) );
      Serial.println( F("Also for U14 pins 1, 2 and 3 (IR5, IR6, IR7: odd bus number blinks separate from even)" ) );
      Serial.println( F("U14 pin 10 and 13 (SUB, OR): should also blink consecutively at full speed." ) );
      Serial.println( F("Finally for U10 pins 2, 4, 6, 8, 11, 13, 15 and 17 (D6, D7, D5, D3, D1, D0, D2 and D4: again, odd/even)" ) );
      Serial.println( F("Press enter for the next step" ) );     
      break;
      
    case 2:
      writeLine();
      // NOTE: the tests are partial
      // Do the LED blink
      Serial.println( F( "Test U1 (CLK2), U10 (D-Bus buffer), U12 (condition decoder), U15 (inverter), U16 (OR gates), U17-U26 (ALU), U27 (AC), U37 (OUT), U38 (XOUT)" ) );
      writeLine();
      Serial.println( F("This test will show the blinkenlights (odd and even sequentually every 600ms)" ) );
      Serial.println( F("These are U38 pins 2, 5, 6 and 9, and also the audio DAC (pins 12, 15, 16 and 19)" ) );
      Serial.println( F("Also U37 pins 2, 5, 6, 9, 12, 15, 16 and 19 will show this pattern, but with a different duty cycle" ) );
      
      Serial.println( F("Press enter for the next step" ) );
      break;
      
    case 3:
      // Write 0x55, 0xAA to RAM: Y, X++
      writeLine();
      Serial.println( F( "Test U28 (Bus gateway AC), U29, U30 (X), U31 (Y), U32-U35 (MAU)" ) );
      writeLine();
      Serial.println( F( "Check that U29 pins 14-11 blink and verify that the blinking speed halves every next pin.." ) );
      Serial.println( F( "Same goes for U30 pins 14-11 following the speed halving." ) );
      Serial.println( F( "U31 pins 5, 9 ,12, 16 are on while 2, 6 15 and 19 are off." ) );
      Serial.println( F( "U36 Pin 10 blinks" ) );
      Serial.println( F( "U36 pins 10 to 3 will blink each at half of the speed of the previous pin..") );
      Serial.println( F( "pins 11, 12, 13, 15, 16, 17, 18 and 19 will blink sequentially and pin 27 will blink fast.") );


      // Set custom Y register
      // LD #D, X
      sendInstruction( 0x10, 0x00 );
      // LD #D, Y
      sendInstruction( 0x14, 0xAA );
      break;

    case 4:
      // Read RAM: Y, D
      writeLine();
      Serial.println( F( "Test U36 (RAM)" ) );
      writeLine();
      Serial.println( F("U37 pins 2, 5, 6, 9, 12, 15, 16 and 19 will blink (odd and even sequentually every 600ms)," ) );

      // Set custom Y register
      // LD #D, X
      sendInstruction( 0x10, 0x00 );
      // LD #D, Y
      sendInstruction( 0x14, 0xAA );      
      break;
      
    case 5:
      // Read RAM: Y, D
      writeLine();
      Serial.println( F( "Test U39 (input)" ) );
      writeLine();
      Serial.println( F("Now, press and hold button A on the controller and verify that the LEDs slowly go off one by one. When releasing, they go on in the same order." ) );
      Serial.println( F("Press enter for the next step" ) );
      break;

  }
}

void writeLine()
{
    Serial.println( F( "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" ) );
}

void sendInstruction( byte _opcode, byte _operand, int _delay /*= DELAY*/ )
{
  byte pd = ( _opcode << 2 );

  // ADC
  byte adcOpcode = (( _opcode & 0b11000000 ) >> 2 );
  byte adcOperand = ( ( _operand & 0b11100000 ) >> 5 );
  
  byte pc = adcOpcode | adcOperand;
  
  byte pb = (0b00011111 & _operand);

  PORTD = pd;
  PORTC = pc;
  PORTB = pb;

  delay( _delay );

  // clock
  PORTC |= 0b00001000;
  
  delay( _delay );

  // clock
  PORTC &= 0b00110111;

  delayMicroseconds( 100 );
}

void printBin( byte _var )
{
  for ( byte test = 0xff; test; test >>= 1 )
  {
    Serial.write( (_var & test) == test ? '1' : '0');
  }
}
