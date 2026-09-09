
#include "splashkit.h"

// Read string procedure
string read_string(string prompt)
{
    string user_input;

    write(prompt);
    return read_line();
}

// Read integer procedure
int read_integer(string prompt)
{
    string user_input;
    int result;

    write(prompt);
    user_input = read_line();
    while (!is_integer(user_input))
    {
        write("Incorrect input. Please enter again: ");
        user_input = read_line();
    }
    return stoi(user_input);
}

//Read double procedure
double read_double(string prompt)
{
    string user_input;

    write(prompt);
    user_input = read_line();
    while (!is_double(user_input))
    {
        write("Incorrect input. Please enter again: ");
        user_input = read_line();
    }

    return to_double(user_input);
}

// Read integer within a range
int read_integer_range(string prompt, int min, int max)
{
    int result;

    result = read_integer(prompt);
    while ((result < min) || (result > max))
    {
        write_line("Invalid number. Please enter a number from " + to_string(min) + " to " + to_string(max) + ".");
        result = read_integer(prompt);
    }
    return result;
}

// Read yes or no
bool read_yes_no(string prompt)
{
    string answer;
    
    answer = read_string(prompt);
    while ((answer != "y") && (answer != "n"))
    {
        write("Please enter y or n: ");
        answer = read_line();
    }
    return (answer == "y");
}

// Print the line
void print_line(int length)
{
    int i = 0;

    while (i < length)
    {
        write("-");
        i++;
    }
    write_line("\n");
}

// Print a repeated pattern
void print_repeated(string text, int times, bool with_newline)
{
    int i = 0;
    while (i < times)
    {
        write(text);
        i++;
    }
    if (with_newline)
    {
        write_line("\n");
    }
}

// Print the border
void print_border(int length)
{
    write("+");
    for (int i = 0; i < length - 2; i++)
    {
        write("-");
    }
    write("+\n");
}

// Print the header
void print_header(string text)
{
    int length = text.length();

    print_border(length);
    write_line(text);
    print_border(length);
}

//Print the array
void print_array(int array[], int size)
{
    for (int i = 0; i < size; i++)
    {
        write_line("Elements " + to_string(i + 1) + " has the value of " + to_string(array[i]));
    }
}

//Update every element in the array
void populate_whole(int array[], int size)
{
    for (int i = 0; i < size; i++)
    {
        array[i] = read_integer("Enter value " + to_string(i+1) + ": ");
    }
}

//Update elements in a specific range
void populate_range(int array[], int min, int max)
{
    for (int i = min; i <= max; i++)
    {
        array[i] = read_integer("Enter value " + to_string(i+1) + ": ");
    }
}
