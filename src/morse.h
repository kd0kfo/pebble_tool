#ifndef MORSE_H
#define MORSE_H

/**
 * Encodes and decodes characters using Morse code.
 * 
 * Encoding scheme:
 * The three LSB's are the number of Morse bits, e.g. dits and dahs.
 *
 * The 5 MSB's are the Morse bits, starting from the MSB and working 
 * towards zero.
 *
 * For example, the character, 'p', which is ".--.", would be encoded
 * as 0x64. The character, '2', which is "..---", would be encoded as
 * 0x3d.
 */

/**
 * Takes an ascii character and gets the encoded value.
 * 
 * @param ascii char ASCII character to be encoded.
 */
char morse_encode(char ascii);

/**
 * Takes an encoded value and returns the ascii equivalent.
 *
 * @param encoded_val char encoded value
 */
char morse_decode(char encoded_val);

/**
 * Takes a null-terminated series of '.' and '-' characters and translates it to an alphanumeric based on morse.
 *
 * @param morse cstring representation of morse.
 */
char morse2char(char *morse);

#endif
