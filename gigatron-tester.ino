#define INSTRUCTION_CLK_DELAY 100
#define AUTOCLOCK_DELAY 100

void sendInstruction( byte _opcode, byte _operand, int _delay = INSTRUCTION_CLK_DELAY );

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

    switch ( Serial.read() )
    {
      case 'c':
        Serial.println( "clocktick" );

        clockTick( AUTOCLOCK_DELAY );
        break;

      case 'C':
        // Autoclock
        Serial.println( "toggle autoclock" );        
        g_mode = g_mode == 255 ? 0 : 255;
        break;

      default:
        // Next mode
        g_mode++;
        writeNotification();
  }

    // Read all of the data
    while ( Serial.available() )
      Serial.read();
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

      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
        // Command is set in the notification phase
        break;


      case 255:
        // autoclock
        fastClockTick( 1 );
        break;
    
    default:
      g_mode = 1;
  }
}

void clockTick( int _delay )
{
  delay( _delay );
  // clock
  PORTC &= 0b00110111;

  delay( _delay );

  // clock
  PORTC |= 0b00001000;

  delayMicroseconds( 100 );
}

void fastClockTick( int _delay )
{
  delayMicroseconds( _delay );
  //__asm__ __volatile__ ("nop\n\t");
  // clock
  PORTC &= 0b00110111;

  delayMicroseconds( _delay );

  //__asm__ __volatile__ ("nop\n\t");
  // clock
  PORTC |= 0b00001000;
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
      Serial.println( F("type 'c' followed by enter to trigger an extra clock pulse" ) );
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

    case 6:
      writeLine();
      // XL YL IX EH EL OL LD PL PH     AR0 AR1 AR2 AR3 AL  DE OE AE IE
      Serial.println( F( "Test instructions (press enter for next instruction)" ) );
      // al=!R1=!ir7 || !bcc
      // ar0=!R2=!sub || !bcc
      // ar1=!r3=!or || !xor || !sub
      // ar2=!r4=!ld || !or || !xor || !add || !bcc
      // ar3=!r5=!ld || !and || !or || !add

      // U14 pin 15 is low, 14-9 and 7 are high
      // U15 pin 3(AR3), 9(AL) and 14(AR2) are high, 7(AR1), 12(AR0), 16(LD+OL) are low (5 and 18 are for mode and condition)
      // LD =AL && !AR0 && !AR1 && AR2 && AR3     10011
      // AND=AL && !AR0 && !AR1 && !AR2 && AR3    10001
      // OR =AL && !AR0 && AR1 && AR2 && AR3      10111
      // XOR=AL && !AR0 && AR1 && AR2 && !AR3     10110
      // ADD=!AL && !AR0 && !AR1 && AR2 && AR3    00011
      // SUB=!AL && AR0 && AR1 && !AR2 && !AR3    01100
      // ST =!AL && !AR0 && !AR1 && !AR2 && !AR3  00000
      // Bcc=AL && AR0 && !AR1 && AR2 && !AR3     11010

      writeLine();
      //XL YL IX EH EL OL LD PL PH
      Serial.println( F( "LD: 3h=ar3, 5?=ix 7l=ar1, 9h=al, 12l=ar0, 14h=ar2, 16h=LD, 18?=pl " ) );

      sendInstruction( 0b00000000, 0x00 );
      break;

    case 7:
      Serial.println( F( "AND: " ) );

      sendInstruction( 0b00100000, 0x00 );
      break;

    case 8:
      Serial.println( F( "OR: " ) );

      sendInstruction( 0b01000000, 0x00 );
      break;

    case 9:
      Serial.println( F( "XOR: " ) );

      sendInstruction( 0b01100000, 0x00 );
      break;

    case 10:
      Serial.println( F( "ADD: " ) );

      sendInstruction( 0b10000000, 0x00 );
      break;

    case 11:
      Serial.println( F( "SUB: " ) );

      sendInstruction( 0b10100000, 0x00 );
      break;

    case 12:
      Serial.println( F( "ST: " ) );

      sendInstruction( 0b11000000, 0x00 );
      break;

    case 13:
      Serial.println( F( "BCC: " ) );

      sendInstruction( 0b11100000, 0x00 );
      break;

    case 14:
      writeLine();
      Serial.println( F( "Test Mode types (press enter for next instruction)" ) );
      writeLine();
      Serial.println( F( "[D], AC: " ) );

      sendInstruction( 0b00000000, 0x00 );
      break;

    case 15:
      Serial.println( F( "[X], AC: " ) );

      sendInstruction( 0b00000100, 0x00 );
      break;

    case 16:
      Serial.println( F( "[Y, D], AC: " ) );

      sendInstruction( 0b00001000, 0x00 );
      break;

    case 17:
      Serial.println( F( "[Y, X], AC: " ) );

      sendInstruction( 0b00001100, 0x00 );
      break;

    case 18:
      Serial.println( F( "[D], X: " ) );

      sendInstruction( 0b00010000, 0x00 );
      break;

    case 19:
      Serial.println( F( "[D], Y: " ) );

      sendInstruction( 0b00010100, 0x00 );
      break;

    case 20:
      Serial.println( F( "[D], OUT: " ) );

      sendInstruction( 0b00011000, 0x00 );
      break;

    case 21:
      Serial.println( F( "[Y, X++], OUT: " ) );

      sendInstruction( 0b00011100, 0x00 );
      break;

    case 22:
      writeLine();
      Serial.println( F( "Test Bus access" ) );
      writeLine();
      Serial.println( F( "DE: Operand to bus" ) );

      sendInstruction( 0b00000000, 0x00 );
      break;

    case 23:
      Serial.println( F( "OE: RAM to bus" ) );

      sendInstruction( 0b00000001, 0x00 );
      break;

    case 24:
      Serial.println( F( "AE: bus gateway AC/AC to bus" ) );

      sendInstruction( 0b00000010, 0x00 );
      break;

    case 25:
      Serial.println( F( "IE: Input to bus" ) );

      sendInstruction( 0b00000011, 0x00 );
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

  clockTick( _delay );
}

void printBin( byte _var )
{
  for ( byte test = 0x80; test; test >>= 1 )
  {
    Serial.write( (_var & test) == test ? '1' : '0');
  }
}
