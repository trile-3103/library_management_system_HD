#include "splashkit.h"
#include "utilities.h"
#include <iostream>
#include <fstream>

// Constants
const int MAX_BOOKS = 100;
const int MAX_MEMBER = 50;
const int BORROW_LIMIT = 3;
const int MAX_LOANS = MAX_MEMBER * BORROW_LIMIT;
const int OVERDUE_LIMIT = 14;
// Enums
enum genre
{
    FICTION,
    NON_FICTION,
    SCI_FI,
    MYSTERY,
    ROMANCE,
    THRILLER,
    FANTASY
};

enum menu
{
    ADD_MEMBER = 1,
    ADD_BOOK,
    PRINT_MEMBER,
    PRINT_BOOK,
    PRINT_LOAN,
    CHECKOUT,
    RETURN,
    RECOMMEND,
    ADVANCE,
    QUIT
};

enum loan_status
{
    ACTIVE,
    OVERDUE,
    RETURNED
};

// Declaring structs
struct book
{
    int book_id;
    string title;
    string author;
    genre book_genre;
    int copies;
    int borrows_total;
};

struct member
{
    int member_id;
    string name;
    int books_borrowed;
};

struct loan
{
    int member_id;
    string book_title;
    int borrow_days;
    loan_status status;
};

struct library
{
    book books[MAX_BOOKS];
    int book_count;

    member members[MAX_MEMBER];
    int member_count;

    loan loans[MAX_LOANS];
    int loan_count;
};

/**
 * A loan node that contains a pointer to the next node
 */
template<typename T>
struct loan_node
{
    T data;
    loan_node<T> *next;
};

/**
 * A linked list used for storing returned loan records
 * 
 */
template <typename T>
struct returned_linked_list
{
    loan_node<T> *first;
    loan_node<T> *last;
};

// Procedures, functions

/**
 * Creates a new linked list storing list.
 */
template<typename T>
returned_linked_list<T> *new_returned_list()
{
    returned_linked_list<T> *list =(returned_linked_list<T> *)malloc(sizeof(returned_linked_list<T>));
    list -> first = nullptr;
    list -> last = nullptr;
    return list;
}


/**
 * Adding nodes to the end of the linked list
 * 
 */
template <typename T>
void add_returned_records(returned_linked_list<T> *list, const library &data)
{
    int i;
    for (i = 0; i < data.loan_count; i++)
    {
        if (data.loans[i].status == RETURNED)
        {
            loan_node<T> *new_node = (loan_node<T> *)malloc(sizeof(loan_node<T>));

            new(&new_node -> data) T();

            new_node -> data = data.loans[i];
            new_node -> next = nullptr;

            if (list -> first == nullptr)
            {
                list -> first = new_node;
                list -> last = new_node;
            }
            else 
            {
                list -> last -> next = new_node;
                list -> last = new_node;
            }
        }
    }
}


/**
 * Update the loan status 
 * If the borrow_days exceeds 14
 * @param data
 */
void update_status(library &data)
{
    for (int i = 0 ; i < data.loan_count; i++)
    {
        if (data.loans[i].status == ACTIVE && data.loans[i].borrow_days > OVERDUE_LIMIT)
        {
            data.loans[i].status = OVERDUE;
        }
    }
}

/**
 * Advance every active and overdue loan's borrow_days by a number of days based on user's input
 * Then update every loan's status
 * @param data
 */
void advance_days(library &data)
{
    int days;
    days = read_integer("Enter the number of days you want to advance: ");

    for (int i = 0; i < data.loan_count; i++)
    {
        if (data.loans[i].status == ACTIVE || data.loans[i].status == OVERDUE)
        {
            data.loans[i].borrow_days += days;
        }
    }
    write_line("All active and overdue loan records have been added " + to_string(days) + " days.");
    update_status(data);
    write_line();
}

/**
 * Clean the string data from the CSV file
 * @param value
 * @returns a "clean" string
 */
string clean_data(string value)
{
    if (length_of(value) >= 2 && value[0] == '"' && value[length_of(value) - 1] == '"')
    {
        return value.substr(1, length_of(value) - 2);
    }
    else
    {
        return value;
    }
}

/**
 * Read the genre enum, and then return a string according to that genre
 * @param book_genre
 * @returns a string
 */
string genre_to_string(genre book_genre)
{
    switch (book_genre)
    {
    case FICTION:
        return "Fiction";
        break;
    case NON_FICTION:
        return "Non-Fiction";
        break;
    case SCI_FI:
        return "Science Fiction";
        break;
    case MYSTERY:
        return "Mystery";
        break;
    case ROMANCE:
        return "Romance";
        break;
    case THRILLER:
        return "Thriller";
        break;
    case FANTASY:
        return "Fantasy";
        break;
    default:
        return "Unknown";
        break;
    }
}

/**
 * Read the status enum, and then return a string according to that status
 * @param loan_status
 * @returns a string
 */
string status_to_string(loan_status loan_status)
{
    switch (loan_status)
    {
    case ACTIVE:
        return "Active";
        break;
    case OVERDUE:
        return "Overdue";
        break;
    case RETURNED:
        return "Returned";
        break;
    default:
        return "Unknown";
        break;
    }
}

/**
 * Read the user's input, then return an enum value based on the string
 *
 */
genre string_to_genre()
{
    bool correct_genre = false;
    genre input;
    do
    {
        string genre_string = read_string("Please enter a genre: ");
        if (genre_string == "Fiction")
        {
            correct_genre = true;
            input = FICTION;
        }
        else if (genre_string == "Science Fiction")
        {
            correct_genre = true;
            input = SCI_FI;
        }
        else if (genre_string == "Mystery")
        {
            correct_genre = true;
            input = MYSTERY;
        }
        else if (genre_string == "Thriller")
        {
            correct_genre = true;
            input = THRILLER;
        }
        else if (genre_string == "Romance")
        {
            correct_genre = true;
            input = ROMANCE;
        }
        else if (genre_string == "Non-Fiction")
        {
            correct_genre = true;
            input = NON_FICTION;
        }
        else if (genre_string == "Fantasy")
        {
            correct_genre = true;
            input = FANTASY;
        }
    } while (correct_genre == false);

    return input;
}

/**
 * Add a book to the system
 * @param data
 */
void add_book(library &data)
{
    // Checking the capacity
    if (data.book_count < MAX_BOOKS)
    {
        book new_book;

        new_book.title = read_string("Enter the name of the book: ");
        new_book.author = read_string("Enter the writer of the book: ");
        new_book.copies = read_integer("How many copies does the book have? ");
        new_book.book_id = data.book_count + 1;
        new_book.book_genre = string_to_genre();
        new_book.borrows_total = 0;

        data.books[data.book_count] = new_book;

        data.book_count++;
    }
    else if (data.book_count >= MAX_BOOKS)
    {
        write_line("Sorry. The book's storage has reached the limit, please try again later.");
    }
    write_line();
}

/**
 *
 * Add a member to the system
 * @param data
 */
void add_member(library &data)
{
    // Checking the capacity
    if (data.member_count < MAX_MEMBER)
    {
        member new_member;

        new_member.member_id = data.member_count + 1;
        new_member.name = read_string("Enter new member's name: ");
        new_member.books_borrowed = 0;

        data.members[data.member_count] = new_member;
        data.member_count++;
    }
    else if (data.member_count >= MAX_MEMBER)
    {
        write_line("Sorry. The number of members has reached the limit, please try again later.");
    }
    write_line();
}

/**
 * Find the member based on the member's ID
 * @param data (read only)
 * @param found_id of the member
 * @returns the index
 */
int find_member(const library &data, int found_id)
{
    int found_index = -1;
    for (int i = 0; i < data.member_count; i++)
    {
        if (data.members[i].member_id == found_id)
        {
            found_index = i;
            break;
        }
    }
    return found_index;
}

/**
 * Find the book based on the book's title
 * @param data (read only)
 * @param found_title of the book
 * @returns the index
 */
int find_book(const library &data, string found_title)
{
    int found_index = -1;
    for (int i = 0; i < data.book_count; i++)
    {
        if (data.books[i].title == found_title)
        {
            found_index = i;
            break;
        }
    }
    return found_index;
}

/**
 * Find the loan based on the book's title and the member_id
 * @param data
 * @param found_title of the book
 * @param found_id of the member
 * @return the loan's record index
 */
int find_loan(const library &data, string found_title, int found_id)
{
    int found_index = -1;
    int book_index = find_book(data, found_title);
    if (book_index == -1)
    {
        return found_index;
    }
    string book_title = data.books[book_index].title;

    for (int i = 0; i < data.loan_count; i++)
    {
        if (data.loans[i].book_title == book_title && data.loans[i].member_id == found_id && data.loans[i].status == ACTIVE)
        {
            found_index = i;
        }
    }
    return found_index;
}

/**
 * Print a book's current detail, or every book's detail
 * The option is based on the user's input
 * @param data (read only)
 */
void print_book(library const &data)
{
    int choice;
    choice = read_integer("Do you want to print the details of a particular book (1) or all books (2)? ");
    write_line();
    while (choice != 1 && choice != 2)
    {
        choice = read_integer("Invalid choice. Please enter 1 or 2: ");
    }
    if (choice == 1)
    {
        string needed_title = read_string("Please enter the title of the book: ");
        int found_index = find_book(data, needed_title);
        if (found_index == -1)
        {
            write_line("The book that you need does not exist.");
        }
        else
        {
            write_line("Book's ID: " + to_string(data.books[found_index].book_id));
            write_line("Book's title: " + data.books[found_index].title);
            write_line("Book's author: " + data.books[found_index].author);
            write_line("Book's genre: " + genre_to_string(data.books[found_index].book_genre));
            write_line("Book's available copies: " + to_string(data.books[found_index].copies));
            write_line("The total number of times it has been borrowed: " + to_string(data.books[found_index].borrows_total));
            write_line();
        }
    }
    else if (choice == 2)
    {
        for (int i = 0; i < data.book_count; i++)
        {
            write_line("Book's ID: " + to_string(data.books[i].book_id));
            write_line("Book's title: " + data.books[i].title);
            write_line("Book's author: " + data.books[i].author);
            write_line("Book's genre: " + genre_to_string(data.books[i].book_genre));
            write_line("Book's available copies: " + to_string(data.books[i].copies));
            write_line("The total number of times it has been borrowed: " + to_string(data.books[i].borrows_total));
            write_line();
        }
    }
}

/**
 * Print loan records of a specific member
 *
 */
void print_loan(library const &data)
{
    int choice = read_integer("Do you want print a member's all loan records (1) or a specific record (2)?  ");
    write_line();
    while (choice != 1 && choice != 2)
    {
        choice = read_integer("Invalid choice. Please enter 1 or 2: ");
    }
    if (choice == 1)
    {
        int needed_id = read_integer("Please enter the ID of the member: ");
        int found_index = find_member(data, needed_id);
        write_line();
        if (found_index == -1)
        {
            write_line("The member that you need does not exist.");
        }
        else
        {
            for (int i = 0; i < data.loan_count; i++)
            {
                if (data.loans[i].member_id == needed_id)
                {
                    write_line("Member's ID: " + to_string(data.loans[i].member_id));
                    write_line("Borrowed book's title: " + data.loans[i].book_title);
                    write_line("Days borrowed: " + to_string(data.loans[i].borrow_days));
                    write_line("Loan's status: " + status_to_string(data.loans[i].status));
                    write_line();
                }
            }
        }
    }
    else if (choice == 2)
    {
        // Reading user's input
        int member_id = read_integer("Enter member's ID: ");
        int member_index = find_member(data, member_id);
        while (member_index == -1)
        {
            member_id = read_integer("Member not found. Try again: ");
            member_index = find_member(data, member_id);
        }
        string book_title = read_string("Enter book's title: ");
        // Finding the loan record
        int loan_index = find_loan(data, book_title, member_id);
        while (loan_index == -1)
        {
            write_line("Loan record not found. Try Again. ");
            write_line();
            member_id = read_integer("Enter member's ID: ");
            member_index = find_member(data, member_id);
            while (member_index == -1)
            {
                member_id = read_integer("Member not found. Try again: ");
                member_index = find_member(data, member_id);
                write_line();
            }
            book_title = read_string("Enter book's title: ");
            loan_index = find_loan(data, book_title, member_id);
            write_line();
        }
        write_line();
        write_line("Member's ID: " + to_string(data.loans[loan_index].member_id));
        write_line("Borrowed book's title: " + data.loans[loan_index].book_title);
        write_line("Days borrowed: " + to_string(data.loans[loan_index].borrow_days));
        write_line("Loan's status: " + status_to_string(data.loans[loan_index].status));
    }
    write_line();
}

/**
 * Print a member's detail, or every member's detail (each on one row)
 * The option is based on the user's input
 * @param data (read only)
 */
void print_member(library const &data)
{
    int choice;
    choice = read_integer("Do you want to print the details of a particular member (1) or all members (2)? ");
    write_line();
    while (choice != 1 && choice != 2)
    {
        choice = read_integer("Invalid choice. Please enter 1 or 2: ");
    }
    if (choice == 1)
    {
        int needed_id = read_integer("Please enter the ID of the member: ");
        int found_index = find_member(data, needed_id);
        if (found_index == -1)
        {
            write_line("The member that you need does not exist.");
        }
        else
        {
            write_line("Member's ID: " + to_string(data.members[found_index].member_id));
            write_line("Member's name: " + data.members[found_index].name);
            write_line("The total number of times the member has borrowed books: " + to_string(data.members[found_index].books_borrowed));
            write_line();
        }
    }
    else if (choice == 2)
    {
        for (int i = 0; i < data.member_count; i++)
        {
            write_line("ID: " + to_string(data.members[i].member_id) + " | Name: " + data.members[i].name + " | Books borrowed: " + to_string(data.members[i].books_borrowed));
            write_line();
        }
    }
}

/**
 * Print the menu options
 */
void print_menu()
{
    write_line("===== LIBRARY MANAGEMENT SYSTEM =====");
    write_line("1. Add Member.");
    write_line("2. Add Book.");
    write_line("3. Print member's details.");
    write_line("4. Print book's details. ");
    write_line("5. Print loan's details. ");
    write_line("6. Checkout book.");
    write_line("7. Return book.");
    write_line("8. Recommend books.");
    write_line("9. Advance days.");
    write_line("10. Quit.");
    write_line("=====================================");
    write_line();
}

/**
 * Read menu option
 * @returns an enum value
 */
menu read_menu_option()
{
    int choice = read_integer("Option: ");
    while (choice < 1 || choice > 10)
    {
        choice = read_integer("Invalid option. Try again: ");
    }
    return menu(choice);
}

/**
 * Checkout a book
 * Fnd the books and members, checks the capacity and then updates both structs and add new loan record
 *
 * @param data
 */
void checkout(library &data)
{
    // Reading user's input and checking the condition
    int member_id = read_integer("Enter member's ID: ");
    int member_index = find_member(data, member_id);
    while (member_index == -1)
    {
        member_id = read_integer("Member not found. Try again: ");
        member_index = find_member(data, member_id);
    }
    if (data.members[member_index].books_borrowed >= BORROW_LIMIT)
    {
        write_line("Member's current number of books borrowed has reached the limit.");
    }
    else if (data.loan_count >= MAX_LOANS)
    {
        write_line("System Error. System's number of loans has reached the limit");
    }
    else
    {
        string book_title = read_string("Enter book's title: ");
        int book_index = find_book(data, book_title);
        while (book_index == -1)
        {
            book_title = read_string("Book not found. Try again: ");
            book_index = find_book(data, book_title);
        }
        if (data.books[book_index].copies <= 0)
        {
            write_line("Sorry. We are currently running out of the book you want to borrow.");
        }
        else
        {
            // Updating the two structs
            data.members[member_index].books_borrowed++;
            data.books[book_index].copies--;
            data.books[book_index].borrows_total++;
            // Add a new loan record
            loan new_loan;
            new_loan.book_title = data.books[book_index].title;
            new_loan.member_id = data.members[member_index].member_id;
            new_loan.borrow_days = 0;
            new_loan.status = ACTIVE;

            data.loans[data.loan_count] = new_loan;
            data.loan_count++;
        }
    }
    write_line();
}

/**
 * Return a book
 * Find the loan record based on the book_id and member_id, update the copies and books_borrowed and update the loan record's status
 *
 * @param data
 */
void return_book(library &data)
{
    // Reading user's input
    int member_id = read_integer("Enter member's ID: ");
    int member_index = find_member(data, member_id);
    while (member_index == -1)
    {
        member_id = read_integer("Member not found. Try again: ");
        member_index = find_member(data, member_id);
    }
    string book_title = read_string("Enter book's title: ");
    // Finding the loan record
    int loan_index = find_loan(data, book_title, member_id);
    while (loan_index == -1)
    {
        write_line("Loan record not found. Try Again. ");
        member_id = read_integer("Enter member's ID: ");
        member_index = find_member(data, member_id);
        while (member_index == -1)
        {
            member_id = read_integer("Member not found. Try again: ");
            member_index = find_member(data, member_id);
        }
        book_title = read_string("Enter book's title: ");
        loan_index = find_loan(data, book_title, member_id);
    }
    // Update the copies and books borrowed
    int book_index = find_book(data, book_title);
    data.members[member_index].books_borrowed--;
    data.books[book_index].copies++;
    // Update the loan status
    data.loans[loan_index].status = RETURNED;
    write_line();
}

/**
 * Print the top 5 books of all genres
 *
 * @param data
 * @param filter_by_genre to specify whether to only search for books of a specific genre
 * @param option of the genre
 *
 */
void print_top_5(library &data, bool filter_by_genre, genre option)
{
    {
        // Create an array storing 5 top books' index
        int top5_indexes[5] = {-1, -1, -1, -1, -1};
        //
        for (int m = 0; m < 5; m++)
        {
            int book_index = -1;
            int borrow_times = -1;

            for (int i = 0; i < data.book_count; i++)
            {
                // This if is used to for the filter logic
                if (filter_by_genre && data.books[i].book_genre != option) // Once the filter_by_genre is false, it will check every book
                {
                    continue;
                }
                // Check if the index has already in the top 5 array
                bool already_in_top = false;
                for (int index = 0; index < m; index++)
                {
                    if (i == top5_indexes[index])
                    {
                        already_in_top = true;
                        break;
                    }
                }
                // Ranking the books
                if (already_in_top)
                {
                    continue;
                }
                else if (data.books[i].borrows_total > borrow_times)
                {
                    borrow_times = data.books[i].borrows_total;
                    book_index = i;
                }
            }
            // Add the index into the array
            top5_indexes[m] = book_index;
        }
        for (int n = 0; n < 5; n++)
        {
            if (top5_indexes[n] != -1)
            {
                write_line("Top " + to_string(n + 1) + ": " + data.books[top5_indexes[n]].title);
            }
        }
    }
}

/**
 *
 * Print recommedations for top 5 based on the total borrows of each book
 * The user can request a specific genre
 *
 */
void print_recommendations(library &data)
{
    int choice = read_integer("You want to print top 5 books for (1) all genres or (2) a specific genre? ");
    write_line();
    while (choice != 1 && choice != 2)
    {
        choice = read_integer("Invalid input. Please try again: ");
        write_line();
    }
    if (choice == 1)
    {
        print_top_5(data, false, genre());
        write_line();
    }
    else if (choice == 2)
    {
        genre option = string_to_genre();
        write_line();
        print_top_5(data, true, option);
        write_line();
    }
}

//Procedures for reading CSV file at the start of the program
/**
 * A function used for reading CSV file, converting a string to genre enum value
 *@param genre_string
 *@returns a genre enum value
 */
genre string_to_genre_CSV(string genre_string)
{
    genre value;
    
    if (genre_string == "Fiction") {value = FICTION;}
    else if (genre_string == "Science Fiction") {value = SCI_FI;}
    else if (genre_string == "Mystery") {value = MYSTERY;}
    else if (genre_string == "Thriller") {value = THRILLER;}
    else if (genre_string == "Romance") {value = ROMANCE;}
    else if (genre_string == "Non-Fiction") {value = NON_FICTION;}
    else if (genre_string == "Fantasy") {value = FANTASY;}
    return value;
}

/**
 * Read the book csv file and populate data into the array
 * @param data
 * @param file_name
 */
void read_book_from_CSV(library &data, string file_name)
{
    std::ifstream csv_file(file_name);

    if (!csv_file.is_open())
    {
        write_line("Error: " + file_name + " not found.");
        return;
    }

    string line;
    std::getline(csv_file, line);

    // The loop will run as long as there is still a line to read and the array is not full
    while (std::getline(csv_file, line) && data.book_count < MAX_BOOKS)
    {
        // Create a local array to holds 6 fields of the book struct
        string fields[6];
        int field_index = 0;       // Keep track what fields we are in
        string current_field = ""; // A variable used to build of the characters of a field
        bool inside_quotes = false;

        // The loop used to run through every character in a line
        for (int i = 0; i < length_of(line); i++)
        {
            char c = line[i];
            
            if (c == '"')
            {
                inside_quotes = !inside_quotes;
                current_field += c;
            }
            else if (c == ',' && !inside_quotes)
            {
                fields[field_index] = clean_data(current_field);
                field_index++;
                current_field = "";
            }
            else
            {
                current_field += c;
            }
        }
        // Clean the last field
        fields[field_index] = clean_data(current_field);

        book new_book;
        new_book.book_id = to_integer(fields[0]);
        new_book.title = fields[1];
        new_book.author = fields[2];
        new_book.book_genre = string_to_genre_CSV(fields[3]);
        new_book.copies = to_integer(fields[4]);
        new_book.borrows_total = to_integer(fields[5]);

        data.books[data.book_count] = new_book;
        data.book_count++;
    }
}

/**
 * Read the member csv file and populate data into the array
 * @param data
 * @param file_name
 */
void read_member_from_CSV(library &data, string file_name)
{
    std::ifstream csv_file(file_name);

    if (!csv_file.is_open())
    {
        write_line("Error: " + file_name + " not found.");
        return;
    }

    string line;
    std::getline(csv_file, line);

    // The loop will run as long as there is still a line to read and the array is not full
    while (std::getline(csv_file, line) && data.member_count < MAX_MEMBER)
    {
        // Create a local array to holds 3 fields of the member struct
        string fields[3];
        int field_index = 0;       // Keep track what fields we are in
        string current_field = ""; // A variable used to build of the characters of a field
        bool inside_quotes = false;

        // The loop used to run through every character in a line
        for (int i = 0; i < length_of(line); i++)
        {
            char c = line[i];
            
            if (c == '"')
            {
                inside_quotes = !inside_quotes;
                current_field += c;
            }
            else if (c == ',' && !inside_quotes)
            {
                fields[field_index] = clean_data(current_field);
                field_index++;
                current_field = "";
            }
            else
            {
                current_field += c;
            }
        }
        // Clean the last field
        fields[field_index] = clean_data(current_field);

        member new_member;
        new_member.member_id = to_integer(fields[0]);
        new_member.name = fields[1];
        new_member.books_borrowed = to_integer(fields[2]);

        data.members[data.member_count] = new_member;
        data.member_count++;
    }
}

/**
 * A function used for reading CSV file, converting a string to status enum value
 *@param status_string
 *@returns a status enum value
 */
loan_status string_to_status_CSV(string status_string)
{
    loan_status value = ACTIVE;
    
    if (status_string == "Active") {value = ACTIVE;}
    else if (status_string == "Overdue") {value = OVERDUE;}
    else if (status_string == "Returned") {value = RETURNED;}
    
    return value;
}

/**
 * Read the loan csv file and populate data into the array
 * @param data
 * @param file_name
 */
void read_loan_from_CSV(library &data, string file_name)
{
    std::ifstream csv_file(file_name);

    if (!csv_file.is_open())
    {
        write_line("Error: " + file_name + " not found.");
        return;
    }

    string line;
    std::getline(csv_file, line);

    // The loop will run as long as there is still a line to read and the array is not full
    while (std::getline(csv_file, line) && data.loan_count < MAX_LOANS)
    {
        // Create a local array to holds 4 fields of the loan struct
        string fields[4];
        int field_index = 0;       // Keep track what fields we are in
        string current_field = ""; // A variable used to build of the characters of a field
        bool inside_quotes = false;

        // The loop used to run through every character in a line
        for (int i = 0; i < length_of(line); i++)
        {
            char c = line[i];
            
            if (c == '"')
            {
                inside_quotes = !inside_quotes;
                current_field += c;
            }
            else if (c == ',' && !inside_quotes)
            {
                fields[field_index] = clean_data(current_field);
                field_index++;
                current_field = "";
            }
            else
            {
                current_field += c;
            }
        }
        // Clean the last field
        fields[field_index] = clean_data(current_field);

        loan new_loan;
        new_loan.member_id = to_integer(fields[0]);
        new_loan.book_title = fields[1];
        new_loan.borrow_days = to_integer(fields[2]);
        new_loan.status = string_to_status_CSV(fields[3]);

        data.loans[data.loan_count] = new_loan;
        data.loan_count++;
    }
}

//Procdures for writing CSV files at the end of the program
/**
 * Wrap a value in quotes if it contains a comma
 * For correctly parsed back out into then CSV
 * @param value
 * @returns the newly modified value
 */
string csv_safe(string value)
{
    bool need_quotes = false; //By default a value will not need quotes

    //Scanning character by character
    for (int i = 0; i < length_of(value); i++)
    {
        if (value[i] == ',')
        {
            need_quotes = true;
        }
    }
    //Add quotes if there is a comma found
    if (need_quotes)
    {
        value = "\"" + value + "\"";
    }
    return value;
}

/**
 * Write the final book array out to CSV file, overwriting the existing file
 * Each book is written as one line, matching the same column order when reading the csv
 * @param data
 * @param file_name
 */
void write_books_to_CSV(const library &data, string file_name)
{
    //Overwriting the old file
    std::ofstream csv_file(file_name);

    if (!csv_file.is_open())
    {
        write_line("Error: could not open " + file_name + " for writing.");
        return;
    }
    //Write the header row
    csv_file << "book_id,title,author,genre,copies,borrows_total\n";
    //Write one line per book
    for (int i = 0; i < data.book_count; i++)
    {
        csv_file << data.books[i].book_id << ','
                    << csv_safe(data.books[i].title) << ','
                    << csv_safe(data.books[i].author) << ','
                    << genre_to_string(data.books[i].book_genre) << ','
                    << data.books[i].copies << ','
                    << data.books[i].borrows_total << "\n";
    }
    //Close the file once done writing
    csv_file.close();
}

/**
 * Write the final member array to CSV file, overwriting the existing file
 * Each member is written as one line, matching the same column order when reading the csv
 * @param data
 * @param file_name
 */
void write_members_to_CSV(const library &data, string file_name)
{
    //Overwrite the old file
    std::ofstream csv_file(file_name);

    if (!csv_file.is_open())
    {
        write_line("Error: could not open " + file_name + " for writing.");
        return;
    }
    //Write the header row
    csv_file << "member_id,name,books_borrowed\n";
    //Write one line per member
    for (int i = 0; i < data.member_count; i++)
    {
        csv_file << data.members[i].member_id << ','
                    << csv_safe(data.members[i].name) << ','
                    << data.members[i].books_borrowed << "\n";
    }
    //Close the file once done writing
    csv_file.close();
}

/**
 * Write the final loan array to CSV file, overwriting the existing file
 * Each loan is written as one line, matching the same column order when reading the csv
 * @param data
 * @param file_name
 */
void write_loans_to_CSV(const library &data, string file_name)
{
    //Overwrite the old file
    std::ofstream csv_file(file_name);

    if (!csv_file.is_open())
    {
        write_line("Error: could not open " + file_name + " for writing.");
        return;
    }
    //Write the header row
    csv_file << "member_id,book_title,borrow_days,status\n";
    //Write one line per loan
    for (int i = 0; i < data.loan_count; i++)
    {
        csv_file << data.loans[i].member_id << ','
                    << csv_safe(data.loans[i].book_title) << ','
                    << data.loans[i].borrow_days << ','
                    << status_to_string(data.loans[i].status) << "\n";
    }
    //Close the file once done writing
    csv_file.close();
}


int main()
{
    menu choice;
    library data = {};

    // Add 5 books
    //data.books[0] = {1, "Dune", "Frank Herbert", SCI_FI, 3, 5};
    //data.books[1] = {2, "1984", "George Orwell", FICTION, 1, 8};
    //data.books[2] = {3, "The Hobbit", "J.R.R. Tolkien", FANTASY, 1, 3};
    //data.books[3] = {4, "Gone Girl", "Gillian Flynn", THRILLER, 0, 6};
    //data.books[4] = {5, "Pride and Prejudice", "Jane Austen", ROMANCE, 4, 2};
    //data.book_count = 5;

    read_book_from_CSV(data, "books.csv");

    // Add 5 members
    //data.members[0] = {1, "Alice", 1};
    //data.members[1] = {2, "Bob", 1};
    //data.members[2] = {3, "Charlie", 0};
    //data.members[3] = {4, "Dana", 0};
    //data.members[4] = {5, "Evan", 0};
    //data.member_count = 5;
    read_member_from_CSV(data, "members.csv");

    // Add 2 loan records
    //data.loans[0] = {1, "1984", 3, ACTIVE};
    //data.loans[1] = {2, "Gone Girl", 10, ACTIVE};
    //data.loan_count = 2;
    read_loan_from_CSV(data, "loans.csv");
    update_status(data);

    do
    {
        print_menu();
        choice = read_menu_option();
        write_line();

        switch (choice)
        {
        case ADD_MEMBER:
            add_member(data);
            break;
        case ADD_BOOK:
            add_book(data);
            break;
        case PRINT_MEMBER:
            print_member(data);
            break;
        case PRINT_BOOK:
            print_book(data);
            break;
        case PRINT_LOAN:
            print_loan(data);
            break;
        case CHECKOUT:
            checkout(data);
            break;
        case RETURN:
            return_book(data);
            break;
        case RECOMMEND:
            print_recommendations(data);
            break;
        case ADVANCE:
            advance_days(data);
            break;
        case QUIT:
            break;
        }
    } while (choice != QUIT);

    //Update the csv files
    write_books_to_CSV(data, "books.csv");
    write_members_to_CSV(data, "members.csv");
    write_loans_to_CSV(data, "loans.csv");

    return 0;
}