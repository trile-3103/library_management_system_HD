#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
using std::string;


/**
 * Displays a prompt, reads the string entered at the terminal, and returns this string to the caller
 * @param prompt
 * @return
 */
string read_string(string prompt);

/**
 * Display a prompt, reads the string entered, safely converts this to an integer, and returns the value
 * @param prompt
 * @return
 */
int read_integer(string prompt);

/**
 * Display a prompt, reads the string entered, safely converts this to a double, and returns the value
 * @param prompt
 * @return 
 */
double read_double(string prompt);

/**
 * Read integer within a range
 * @param prompt, min, max
 * @return
 */
int read_integer_range(string prompt, int min, int max);

/**
 * Read user's input and retrun bool values
 * @param prompt
 * @return
 */
bool read_yes_no(string prompt);

/**
 * Print the line of '-' characters to the terminal. The number of characters to write is specified in the length parameter
 * @param length
 */
void print_line(int length);

/**
 * Writes the text to the Terminal a given number of times, adding a new line when with_newline is true
 * @param text, times, with_newline
 */
void print_repeated(string text, int times, bool with_newline);

/**
 * Print the border of '+' and '-' characters matching the given length
 * @param length
 */
void print_border(int length);

/**
 * Print the menu header, the length the line is depended on the length of the text
 * @param text
 */
void print_header(string text);

/**
 * Print the elements in an array
 * @param array
 * @param size
 */
void print_array(int array[], int size);

/**
 * 
 * Reassigned every element in the array
 * @param array
 * @param size
 */
void populate_whole(int array[], int size);

/**
 * Reassign elements in a specific range of the array
 * @param array
 * @param min
 * @param max
 */
void populate_range(int array[], int min, int max);




#endif