#include <avr/io.h>

#define _BV(bit) (1 << (bit))

#define DATA_7P     PORTB
#define DATA_6P     PORTD
#define DATA_5P     PORTD
#define DATA_4P     PORTD

#define DATA_7      _BV(PORTB0)
#define DATA_6      _BV(PORTD7)
#define DATA_5      _BV(PORTD6)
#define DATA_4      _BV(PORTD5)

#define DD_7R       DDRB
#define DD_6R       DDRD
#define DD_5R       DDRD 
#define DD_4R       DDRD 

#define DD_7        _BV(DDB0)
#define DD_6        _BV(DDD7)
#define DD_5        _BV(DDD6)
#define DD_4        _BV(DDD5)

#define EN_P        PORTB
#define EN          _BV(PORTB1)
#define DD_ENR      DDRB
#define DD_EN       _BV(DDB1)

#define RS_P        PORTB
#define RS          _BV(PORTB2)
#define DD_RSR      DDRB
#define DD_RS       _BV(DDB2)

// flags for basic functions
// ---------------------------------------------------------------------------
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

// flags for display entry mode
// ---------------------------------------------------------------------------
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off and cursor control
// ---------------------------------------------------------------------------
#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

// flags for display/cursor shift
// ---------------------------------------------------------------------------
#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

// flags for function set
// ---------------------------------------------------------------------------
#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00

const uint8_t pin[4];
volatile uint8_t *port[4];

uint8_t _cur_line;
uint8_t _line_change;

void _pulse_enable(void);
void _set_pin(volatile uint8_t *port, uint8_t pin, uint8_t value);
void _write_bits(uint8_t to_write);
void _write_nibble(uint8_t to_write);
void write_line(char* string, int line);
void lcd_init(void);
void put_char(char c, FILE *stream);
