#include <stdio.h>
#include <string.h>

#define MORSE_COUNTER_MASK 0x7
#define MORSE_CODE_MASK 0xf8

const char morse_table[] = "?etianmsurwdkgohvf\x7Fl\x0apjbxcyzq? 54?3???2??+????16=/?????7???8!90";

const unsigned char morse_encoding[] =
  {
    0b01000010,    
    0b10000100,  // b
    0b10100100,  // c
    0b10000011,  // d
    0b00000001,  // e
    0b00100100,  // f
    0b11000011,  // g
    0b00000100,  // h
    0b00000010,  // i
    0b01110100,  // j
    0b10100011,  // k
    0b01000100,  // l
    0b11000010,  // m
    0b10000010,  // n
    0b11100011,  // o
    0b01100100,  // p
    0b11010100,  // q
    0b01000011,  // r
    0b00000011,  // s
    0b10000001,  // t
    0b00100011,  // u
    0b00010100,  // v
    0b01100011,  // w
    0b10010100,  // x
    0b10110100,  // y
    0b11000100,  // z
    0b11111101,  // 0
    0b01111101,  // 1
    0b00111101,  // 2
    0b00011101,  // 3
    0b00001101,  // 4
    0b00000101,  // 5
    0b10000101,  // 6
    0b11000101,  // 7
    0b11100101,  // 8
    0b11110101,  // 9
    0b11110100,  // SPACE
    0b11101101   // Exclamation point
  };

/**
 * Gets the encoding for an ascii alpha numeric character
 */
char morse_encode(char ascii)
{
  if(ascii >= 'A' && ascii <= 'Z')
    return morse_encoding[ascii - 'A'];
  else if(ascii >= 'a' && ascii <= 'z')
    return morse_encoding[ascii - 'a'];
  else if(ascii >= '0' && ascii <= '9')
    return morse_encoding[ascii - 0x17];// '0' + Offset to skip alpha characters in array
  
  switch(ascii) {
  case ' ':
    return morse_encoding[0x23];
  case '!':
    return morse_encoding[0x24];
  default:
    break;
  }

  return morse_encoding['t'-'a'];

}

char morse_decode(char encoded_val) {
  size_t num_chars = strlen(morse_table);
  unsigned char counter = encoded_val & MORSE_COUNTER_MASK;// 3 LSB's are the number of morse bits
  unsigned char place = 0;
  unsigned char morse_data = encoded_val & MORSE_CODE_MASK;// 5 MSB's are the morse bits, starting at MOST SIGNIFICANT BIT
  unsigned char index = 0;

  while(counter > 0) {
    index <<= 1;
    index++;
    if(morse_data & 0x80)
      index++;
    place++;
    morse_data <<= 1;
    counter--;
  }

  if(index > num_chars)
    return '?';

  return morse_table[index];
}

char morse2char(char *morse) {
  unsigned char encoding = 0;
  unsigned char counter = 0;
  unsigned char mask = 0x80;
  while(morse != NULL && *morse != 0) {
    if(*morse == '-')
      {
	encoding |= mask;
      }
    mask >>= 1;
    counter++;
    encoding = (encoding & MORSE_CODE_MASK) | counter;
    morse++;
  }

  return morse_decode(encoding);
}

char* char2morse(char ch, char *morse) {
  char encoding = 0;
  char *retval = morse;
  unsigned char counter = 0;
  unsigned char code = 0;
  static const unsigned char mask = 0x80;
  
  if(morse == NULL)
    return morse;

  encoding = morse_encode(ch);
  counter = (encoding & MORSE_COUNTER_MASK);
  code = (encoding & MORSE_CODE_MASK);
  
  for(;counter > 0;counter--) {
    if(mask & code) {
      *morse = '-';
    } else {
      *morse = '.';
    }
    code <<= 1;
    morse++;
  }
  *morse = 0;
  
  return retval;
}
