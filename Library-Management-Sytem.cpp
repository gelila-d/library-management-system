#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include<cctype>
using namespace std;

// Define structures for books and borrow records
struct Book {
    int id;
    string title;
    string author;
    int copies;
    Book* next;
    Book* prev;
};

struct BorrowRecord {
    string borrowerName;
    string bookTitle;
    int copiesBorrowed;
    BorrowRecord* next;
    BorrowRecord* prev;
};

struct Librarian {
    string username;
    string password;
};

// Function prototypes
Book* createBook(int id, string title, string author, int copies);
Book* insertBookAtBeginning(Book* head, int id, string title, string author, int copies);
Book* insertBookAtEnd(Book* head, int id, string title, string author, int copies, bool& success);
Book* insertBookAtPosition(Book* head, int id, string title, string author, int copies, int pos);
Book* deleteFirstBook(Book* head);
Book* deleteLastBook(Book* head);
Book* deleteBookAtPosition(Book* head, int pos);
Book* searchBook(Book* head, int id);
void displayBooks(Book* head);
BorrowRecord* insertBorrowAtEnd(BorrowRecord* head, string borrowerName, string bookTitle, int copiesBorrowed);
BorrowRecord* deleteBorrowRecord(BorrowRecord* head, BorrowRecord* target);
void displayBorrowRecords(BorrowRecord* head);
void saveBooksToFile(Book* head);
void saveBorrowRecordsToFile(BorrowRecord* head);
Book* loadBooksFromFile();
BorrowRecord* loadBorrowRecordsFromFile();
void returnBook(Book*& books, BorrowRecord*& borrowRecords);
bool authenticateLibrarian(Librarian librarian);
void createLibrarianAccount(Librarian &librarian);
void libraryMenu(Librarian librarian);
int getValidInt(const string& prompt);
string getValidString(const string& prompt);

// Book management functions
Book* createBook(int id, string title, string author, int copies) {
    Book* newBook = new Book;
    newBook->id = id;
    newBook->title = title;
    newBook->author = author;
    newBook->copies = copies;
    newBook->next = NULL;
    newBook->prev = NULL;
    return newBook;
}

Book* insertBookAtBeginning(Book* head, int id, string title, string author, int copies) {
    Book* newBook = createBook(id, title, author, copies);
    if (head != NULL) {
        head->prev = newBook;
        newBook->next = head;
    }
    return newBook;
}

Book* insertBookAtEnd(Book* head, int id, string title, string author, int copies, bool& success) {
    if(searchBook(head, id) != NULL) {
        cout << "Error: Book ID already exists!\n";
        success = false;
        return head;
    }
    
    Book* newBook = createBook(id, title, author, copies);
    success = true; 
    
    if(head == NULL) return newBook;

    Book* temp = head;
    while(temp->next != NULL)
        temp = temp->next;

    temp->next = newBook;
    newBook->prev = temp;
    return head;
}

Book* insertBookAtPosition(Book* head, int id, string title, string author, int copies, int pos) {
    if (pos == 0) return insertBookAtBeginning(head, id, title, author, copies);

    Book* newBook = createBook(id, title, author, copies);
    Book* temp = head;
    for (int i = 0; i < pos - 1 && temp != NULL; i++)
        temp = temp->next;

    if (temp == NULL) {
        cout << "Invalid position!\n";
        return head;
    }

    newBook->next = temp->next;
    newBook->prev = temp;
    if (temp->next != NULL)
        temp->next->prev = newBook;
    temp->next = newBook;
    return head;
}

Book* deleteFirstBook(Book* head) {
    if (head == NULL) return NULL;

    Book* temp = head;
    head = head->next;
    if (head != NULL)
        head->prev = NULL;
    delete temp;
    return head;
}

Book* deleteLastBook(Book* head) {
    if (head == NULL) return NULL;
    if (head->next == NULL) {
        delete head;
        return NULL;
    }

    Book* temp = head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->prev->next = NULL;
    delete temp;
    return head;
}

Book* deleteBookAtPosition(Book* head, int pos, bool& success) {
    success = false; 
    if (head == NULL) return NULL;

    if (pos == 0) {
        Book* newHead = deleteFirstBook(head);
        success = true;
        return newHead;
    }

    Book* temp = head;
    for (int i = 0; i < pos && temp != NULL; i++) {
        temp = temp->next;
    }

    if (temp == NULL) {
        cout << "Invalid position!\n";
        return head; 
    }

    if (temp->prev) temp->prev->next = temp->next;
    if (temp->next) temp->next->prev = temp->prev;

    delete temp;
    success = true; 
    return head;
}

Book* searchBook(Book* head, int id) {
    Book* current = head;
    while (current != NULL) {
        if (current->id == id)
            return current;
        current = current->next;
    }
    return NULL;
}
//Display function
void displayBooks(Book* head) {
    if (!head) {
        cout << "No books available in the library.\n";
        return;
    }
    Book* current = head;
    cout << left << setw(5) << "ID" << setw(20) << "Title" << setw(20) << "Author" << setw(10) << "Copies\n";
    cout << "-----------------------------------------------------\n";
    while (current) {
        cout << left << setw(5) << current->id << setw(20) << current->title 
             << setw(20) << current->author << setw(10) << current->copies << endl;
        current = current->next;
    }
}

// Borrow record functions
BorrowRecord* insertBorrowAtEnd(BorrowRecord* head, string borrowerName, string bookTitle, int copiesBorrowed) {
    BorrowRecord* newRecord = new BorrowRecord;
    newRecord->borrowerName = borrowerName;
    newRecord->bookTitle = bookTitle;
    newRecord->copiesBorrowed = copiesBorrowed;
    newRecord->next = NULL;
    newRecord->prev = NULL;

    if (head == NULL) return newRecord;

    BorrowRecord* temp = head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newRecord;
    newRecord->prev = temp;
    return head;
}

BorrowRecord* deleteBorrowRecord(BorrowRecord* head, BorrowRecord* target) {
    if (!head || !target) return head;
    
    if (head == target) {
        head = head->next;
        if (head) head->prev = NULL;
    } else {
        target->prev->next = target->next;
        if (target->next) target->next->prev = target->prev;
    }
    
    delete target;
    return head;
}

void displayBorrowRecords(BorrowRecord* head) {
    if (!head) {
        cout << "No borrow records available.\n";
        return;
    }
    BorrowRecord* current = head;
    cout << left << setw(20) << "Borrower" << setw(20) << "Book Title" << setw(10) << "Copies\n";
    cout << "-----------------------------------------------------\n";
    while (current) {
        cout << left << setw(20) << current->borrowerName << setw(20) 
             << current->bookTitle << setw(10) << current->copiesBorrowed << endl;
        current = current->next;
    }
}

// File I/O functions
void saveBooksToFile(Book* head) {
    ofstream outFile("books.dat");
    Book* current = head;
    while (current != NULL) {
        outFile << current->id << "\n"
                << current->title << "\n"
                << current->author << "\n"
                << current->copies << "\n";
        current = current->next;
    }
    outFile.close();
}

Book* loadBooksFromFile() {
    ifstream inFile("books.dat");
    Book* head = NULL;
    if (!inFile) {
        cout << "No existing book data found. Starting fresh.\n";
        return head;
    }

    int id, copies;
    string title, author;
    while (inFile >> id) {
        inFile.ignore();
        getline(inFile, title);
        getline(inFile, author);
        if (!(inFile >> copies)) break;
        inFile.ignore();

        bool dummySuccess;
        head = insertBookAtEnd(head, id, title, author, copies, dummySuccess);
    }

    if (inFile.bad()) {
        cout << "Warning: Error reading book data file.\n";
    }
    inFile.close();
    return head;
}

void saveBorrowRecordsToFile(BorrowRecord* head) {
    ofstream outFile("borrows.dat");
    BorrowRecord* current = head;
    while (current != NULL) {
        outFile << current->borrowerName << "\n"
                << current->bookTitle << "\n"
                << current->copiesBorrowed << "\n";
        current = current->next;
    }
    outFile.close();
}

BorrowRecord* loadBorrowRecordsFromFile() {
    ifstream inFile("borrows.dat");
    BorrowRecord* head = NULL;
    if (!inFile) return head;

    string borrower, title;
    int copies;
    while (getline(inFile, borrower)) {
        getline(inFile, title);
        inFile >> copies;
        inFile.ignore();
        head = insertBorrowAtEnd(head, borrower, title, copies);
    }
    inFile.close();
    return head;
}

// Return book functionality
void returnBook(Book*& books, BorrowRecord*& borrowRecords) {
    string borrowerName = getValidString("Enter Borrower Name: ");
    int bookId = getValidInt("Enter Book ID to return: ");

    Book* book = searchBook(books, bookId);
    if(!book) {
        cout << "Book not found!\n";
        return;
    }


    BorrowRecord* current = borrowRecords;
    BorrowRecord* toDelete = NULL;
    int found = 0;
    
    while(current) {
        if(current->borrowerName == borrowerName && 
           current->bookTitle == book->title) {
            toDelete = current;
            book->copies += toDelete->copiesBorrowed;
            borrowRecords = deleteBorrowRecord(borrowRecords, toDelete);
            found++;
        }
        current = current->next;
    }

    if(found > 0) {
        cout << "Successfully returned " << found << " record(s).\n";
    } else {
        cout << "No matching borrow record found!\n";
    }
}


// Authentication functions
bool authenticateLibrarian(Librarian librarian) {
    string username, password;
    cout << "Login in to your librarian account \n";
    username = getValidString("Enter your username: ");
    password = getValidInt("Enter your PIN: ");
    return (username == librarian.username && password == librarian.password);
}

int getValidInt(const string& prompt) {
    int value;
    while(true) {
        cout << prompt;
        if(cin >> value && value >= 0) {
            cin.ignore(1000, '\n');
            return value;
        }
        cout << "Invalid input! Please enter a valid number.\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

string getValidString(const string& prompt) {
                string input;
                bool valid;
      do {
        cout << prompt;
        getline(cin, input);
        valid = true;

        // Check for empty input
        if(input.empty()) {
            cout << "Error: Input cannot be empty!\n";
            valid = false;
            continue;
        }

        // Check each character
        for (size_t i = 0; i < input.length(); i++) {
            char c = input[i];
            if(!isalpha(c) && c != ' ') { 
                cout << "Error: Input can only contain letters and spaces!\n";
                valid = false;
                break;
            }
        }
    } while(!valid);

    return input;
}

void createLibrarianAccount(Librarian &librarian) {
    cout << "No librarian account exists. Please create one.\n";
    librarian.username = getValidString("Enter a username: ");
    librarian.password = getValidInt("Enter a PIN: ");
    cout << "Librarian account created successfully!\n";
    system("cls");
}

// Main library menu
void libraryMenu(Librarian librarian) {
    Book* books = loadBooksFromFile();
    BorrowRecord* borrowRecords = loadBorrowRecordsFromFile();
    bool isLibrarian = (librarian.username != "public"); // Check if real librarian

    int choice;
        do {
        cout << "\n===============================================\n";
        cout << "|            Library Management System        |\n";
        cout << "===============================================\n";
        cout << "|  1. Display All Books                       |\n";
        cout << "|  2. Borrow Book                             |\n";
        cout << "|  3. View Borrow Records                     |\n";
        cout << "|  4. Return Book                             |\n";
        cout << "|  5. Search Book                             |\n";
        
        if(isLibrarian) {
            cout << "|  6. Add Book                              |\n";
            cout << "|  7. Remove Book                           |\n";
            cout << "|  8. Exit & Save                           |\n";
        } else {
            cout << "|  6. Exit                                  |\n";
        }
        
        cout << "===============================================\n";
        cout << "Enter your choice (1-" << (isLibrarian ? "8" : "6") << "): ";
        
        if(!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input! Please try again.\n";
            continue;
        }
        cin.ignore(1000, '\n');

        system("cls"); // Clear screen
        
        switch(choice) {
            case 1:
                displayBooks(books);
                break;
                
            case 2: { 
                string borrowerName = getValidString("Enter Borrower Name: ");
                int id = getValidInt("Enter Book ID to borrow: ");
                int copies = getValidInt("Copies to borrow: ");

                Book* book = searchBook(books, id);
                if(book && book->copies >= copies) {
                    book->copies -= copies;
                    borrowRecords = insertBorrowAtEnd(borrowRecords, borrowerName, book->title, copies);
                    cout << "Book borrowed successfully!\n";
                } else {
                    cout << "Borrow failed! Book not found or insufficient copies.\n";
                }
                break;
            }
            case 3:
                displayBorrowRecords(borrowRecords);
                break;
                
            case 4:
                returnBook(books, borrowRecords);
                break;
                
            case 5: {
                int searchId = getValidInt("Enter Book ID to search: ");
                Book* result = searchBook(books, searchId);
                if(result) {
                    cout << "Book Found:\nID: " << result->id 
                         << "\nTitle: " << result->title
      		             << "\nAuthor: " << result->author
 			             << "\nAvailable Copies: " << result->copies << "\n";
 			   } else {
  				        cout << "Book not found!\n";
   			   }
     			 break;
			   }
            
			case 6: { 
                if(!isLibrarian) {
                    saveBooksToFile(books);
                    saveBorrowRecordsToFile(borrowRecords);
                    return;
                }

                int addChoice;
                cout << "\n=== Add Book Options ===\n"
                     << "1. Add at beginning\n"
                     << "2. Add at end\n"
                     << "3. Add at specific position\n"
                     << "Enter choice: ";
                addChoice = getValidInt("");

                int id = getValidInt("Enter Book ID: ");
                string title = getValidString("Enter Book Title: ");
                string author = getValidString("Enter Author: ");
                int copies = getValidInt("Enter Copies: ");

                bool success = true;
                switch(addChoice) {
                    case 1:
                        books = insertBookAtBeginning(books, id, title, author, copies);
                        break;
                    case 2:
                        books = insertBookAtEnd(books, id, title, author, copies, success);
                        break;
                    case 3: {
                        int pos = getValidInt("Enter position (0-based): ");
                        books = insertBookAtPosition(books, id, title, author, copies, pos);
                        break;
                    }
                    default:
                        cout << "Invalid add choice!\n";
                        success = false;
                }

                if(success) cout << "Book added successfully!\n";
                break;
            }
            
            case 7: { 
                if(!isLibrarian) break;

                int removeChoice;
                cout << "\n=== Remove Book Options ===\n"
                     << "1. Delete by ID\n"
                     << "2. Delete first book\n"
                     << "3. Delete last book\n"
                     << "4. Delete at specific position\n"
                     << "Enter choice: ";
                removeChoice = getValidInt("");

                bool success = true;
                switch(removeChoice) {
                    case 1: {
                        int id = getValidInt("Enter Book ID to remove: ");
                        Book* toDelete = searchBook(books, id);
                        if(toDelete) {
                            if(toDelete == books) books = books->next;
                            if(toDelete->prev) toDelete->prev->next = toDelete->next;
                            if(toDelete->next) toDelete->next->prev = toDelete->prev;
                            delete toDelete;
                        } else {
                            success = false;
                            cout << "Book not found!\n";
                        }
                        break;
                    }
                    case 2:
                        books = deleteFirstBook(books);
                        break;
                    case 3:
                        books = deleteLastBook(books);
                        break;
                    case 4: {
                        int pos = getValidInt("Enter position (0-based): ");
                        books = deleteBookAtPosition(books, pos, success);
                        break;
                    }
                  default: {
                        cout << "Invalid remove choice!\n";
                        success = false;
                }
            }
                 if (success) {
      				  cout << "Book removed successfully!\n";
  				  } else {
       				  cout << "Failed to remove book.\n";
   		   }
   					 break;
            }
            
            case 8: {
                if(isLibrarian) {
                    saveBooksToFile(books);
                    saveBorrowRecordsToFile(borrowRecords);
                    cout << "Data saved successfully!\n";
                    return;
                }
                break;
            }
            
            default:
                cout << "Invalid choice! Please try again.\n";
        }
     } while(true);

    while(books) books = deleteFirstBook(books);
    while(borrowRecords) borrowRecords = deleteBorrowRecord(borrowRecords, borrowRecords);
}


// Main function
int main() {
    Librarian librarian;
    bool isLibrarianAccountCreated = false;
    int userChoice;

    while(true) {
        cout << "\n===============================================\n";
        cout << "|       Library Management System Login       |\n";
        cout << "===============================================\n";
        cout << "|  1. Librarian Login                         |\n";
        cout << "|  2. Normal User Access                      |\n";
        cout << "|  3. Exit System                             |\n";
        cout << "===============================================\n";
        cout << "Enter your choice (1-3): ";

        if(!(cin >> userChoice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input! Please enter 1, 2, or 3.\n";
            continue;
        }

        cin.ignore(1000, '\n');
         system("cls");
        switch(userChoice) {
            case 1: {
                if(!isLibrarianAccountCreated) {
                    createLibrarianAccount(librarian);
                    isLibrarianAccountCreated = true;
                }
                
                if(authenticateLibrarian(librarian)) {
                    libraryMenu(librarian);
                } else {
                    cout << "Authentication failed. Access denied.\n";
                }
                break;
            }
            case 2: {
                Librarian tempUser;
                tempUser.username = "public";
                libraryMenu(tempUser);
                break;
            }
       case 3: {
    char confirm;
    bool valid = false;

    do {
        cout << "Are you sure you want to exit? (y/n): ";
        string input;
        getline(cin, input); 

        if (input.length() == 1) {
            confirm = tolower(input[0]);
            if (confirm == 'y' || confirm == 'n') {
                valid = true;
            }
        }

        if (!valid) {
            cout << "Invalid input! Please enter only 'y' or 'n'.\n";
        }
    } while (!valid);

    if (confirm == 'y') {
        cout << "\n===============================================\n"
             << "|          Thank you for using the            |\n"
             << "|           Library Management System         |\n"
             << "===============================================\n";
        return 0;
    }
    break;
}
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }
}


/* 
Group 5 : Library management system
1.Gelila Dereje - 1501176
2.Ruhama Bekele - 1501588
3.Hayat Mohammed-sherif- 1503020
4.Eden Mesfin - 1501055
5.Kaafiyo sharmake - 1502294 
*/
