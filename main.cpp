#pragma warning(push, 0)
#include <iostream>
#include <vector>
#include <stdint.h>
#include <assert.h>
#pragma warning(pop)

#define DEBUG

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// NOTE: this thing doesn't have a type checking, I'm sad :c
typedef u64 BookID;

struct AuthorID
{
    std::string name;
    std::string surname;

    bool operator==(AuthorID &other)
    {
        return (name == other.name) && (surname == other.surname);
    }
};

// TODO: use this because I can't just OR Genre variables and get Genre type
// variable as a result, should reason more about this later
// use class maybe? But variable assignement would be terrible then, something like
typedef u64 Genres;

enum Genre
{
    SCIENCE_FICTION = 0x1,
    FANTASY = 0x2,
    DRAMA = 0x4,
    DETECTIVE = 0x8,
};

std::ostream& operator<<(std::ostream& os, Genre g)
{
    switch(g)
    {
        case SCIENCE_FICTION:
        {
            os << "Science fiction" << std::endl;
        } break;

        case FANTASY:
        {
            os << "Fantasy" << std::endl;
        } break;

        case DRAMA:
        {
            os << "Drama" << std::endl;
        } break;

        case DETECTIVE:
        {
            os << "Detective" << std::endl;
        } break;
    }
    return os;
}

//TODO: get rid of it
struct Query;

class Book
{
    public:
    AuthorID    author_id;
    std::string name;
    Genre       genre;

    virtual void Print(std::ostream &os)
    {
        os << author_id.name << std::endl;
        os << author_id.surname << std::endl;
        os << name << std::endl;
        os << genre << std::endl;
    }

    public:

    Book() {}
    // NOTE: maybe it is time to figure out how string copying works?
    Book(AuthorID _author_id, std::string _name, Genre _genre)
    {
        name = _name;
        author_id = _author_id;
        genre = _genre;
    }

    Book(Book &b)
    {
        name = b.name;
        author_id = b.author_id;
        genre = b.genre;
    }

    friend std::ostream& operator<<(std::ostream &os, Book &b);
    // TODO: I think this is not very OOP, ask about callback functions in OOP
    friend u32 Validate_ALL_Query(Query &query, Book &book);
    friend u32 Validate_OR_Query(Query &query, Book &book);
};

// OPTIMIZE: for now, physbooks might have a lot of data in common
class PhysBook : public Book
{
    // NOTE: overflow possible (what can i do?)
    static u32 global_book_id_counter;
    // Each book has unique id
    BookID book_id;

    void Print(std::ostream &os)
    {
        Book::Print(os);
    }

#ifdef DEV
    void DEBUG_Print(std::ostream &os)
    {
        os << book_id <<std::endl;
        Book::Print(os);
    }
#endif

    public:
    PhysBook(AuthorID _author_id, std::string _name, Genre _genre) :
        Book(_author_id, _name, _genre)
    {

        book_id = global_book_id_counter;
        global_book_id_counter++;
    }

    PhysBook(const PhysBook &pb)
    {
        book_id = pb.book_id;
        name = pb.name;
        author_id = pb.author_id;
        genre = pb.genre;
    }

    u32 Get_Book_Id()
    {
        return book_id;
    }
};

std::ostream& operator<<(std::ostream& os, Book &b)
{
    b.Print(os);
    return os;
}

class Author
{
    AuthorID id;
    // Some functionality
};

// TODO: decide include sequence
#include "Query.cpp"

enum BookState
{
    ON_HANDS = 0,
    IN_LIB = 1,
};

class Library
{
    // We need some container for books
    // For simplicity purpose, we just use dynamic array
    // We might change this any time
#ifdef DEBUG
    public:
    void DEBUG_Fill_Library_With_Books();
#endif

    std::vector<PhysBook> books;
    // NOTE: since each book has unique id and it is sequential, we can use it as an index
    // for marking if book is in the library
    std::vector<BookState> states;

    public:
    void Print()
    {
        for(u32 i = 0, len = books.size(); i < len; i++)
        {
            std::cout << books[i];
        }

        for(u32 i = 0, len = states.size(); i < len; i++)
        {
            std::cout << states[i];
        }
        std::cout << std::endl;
    }

    void Add_Book(PhysBook& book)
    {
        books.push_back(book);
        states.push_back(IN_LIB);
    }
};


std::vector<PhysBook *> Create_Response(Query &query, Library &lib)
{
#ifdef DEBUG
    assert(query.type != UNDEFINED);
#endif

    ValidateQueryFunction validate_function = ValidateQueryFunctionTable[query.type];

    std::vector<PhysBook *> result;
    for(auto &book : lib.books)
    {
        if(validate_function(query, book))
        {
            result.push_back(&book);
        }
    }

    return result;
}

class Person
{
    std::vector<PhysBook *> taken_books;

    public:

    void Interact_With_Lib(Query &query, Library &lib)
    {
        if(query.type == UNDEFINED)
        {
            std::cout << "Please, input type of the query" << std::endl;
            do
            {
                QueryType type;
                // TODO:(10.05.2020) I cant just cin type because there is no overloaded >> operator (ew)
                u32 stub_var;
                std::cin >> stub_var;
                type = static_cast<QueryType>(stub_var);
                query.type = Validate_Query_Type(type);
                if(!query.type)
                {
                    std::cout << "Incorrect type, please, repeat" << std::endl;
                }
            } while (!query.type);
        }

        std::vector<PhysBook *> response = Create_Response(query, lib);
        u32 len = response.size();

        if(len != 0)
        {
            std::cout << "What book do you want to add?" << std::endl;
            for(u32 i = 0; i < len; i++)
            {
                std::cout << i << std::endl;
                std::cout << *(response[i]) << std::endl;
            }
            u32 choice;
            std::cin >> choice;
            while(choice >= len)
            {
                std::cout << "Incorrect choice, repeat" << std::endl;
                std::cin >> choice;
            }

            PhysBook* chosen_book = response[choice];
            taken_books.push_back(chosen_book);
            lib.states[chosen_book->Get_Book_Id()] = ON_HANDS;
            return;
        }
        else
        {
            std::cout << "Sorry, we haven't found book that suit your query." << std::endl;
            return;
        }
    }
};

#ifdef DEBUG
void Library::DEBUG_Fill_Library_With_Books()
{
    AuthorID    author_id;
    std::string name;
    Genre       genre;

    author_id.name = "Stephen";
    author_id.surname = "King";
    genre = FANTASY;
    name = "Dark Tower";
    PhysBook book = PhysBook(author_id, name, genre);
    Add_Book(book);

    author_id.name = "Joanne";
    author_id.surname = "Rowling";
    genre = FANTASY;
    name = "Harry Potter: First Book";
    book = PhysBook(author_id, name, genre);
    Add_Book(book);

    author_id.name = "Joanne";
    author_id.surname = "Rowling";
    genre = FANTASY;
    name = "Harry Potter: First Book";
    book = PhysBook(author_id, name, genre);
    Add_Book(book);

    author_id.name = "Joanne";
    author_id.surname = "Rowling";
    genre = FANTASY;
    name = "Harry Potter: Second Book";
    book = PhysBook(author_id, name, genre);
    Add_Book(book);

    author_id.name = "Joanne";
    author_id.surname = "Rowling";
    genre = FANTASY;
    name = "Harry Potter: Last Book";
    book = PhysBook(author_id, name, genre);
    Add_Book(book);

    author_id.name = "John";
    author_id.surname = "Smith";
    genre = DETECTIVE;
    name = "London Stories";
    book = PhysBook(author_id, name, genre);
    Add_Book(book);
    return;
}
#endif

u32 PhysBook::global_book_id_counter = 0;
int main()
{
    Library lib;
    lib.DEBUG_Fill_Library_With_Books();
    lib.Print();

    Person p;

    AuthorID    author_id;
    std::string name;
    Genre       genre;
    author_id.name = "Joanne";
    author_id.surname = "Rowling";
    genre = FANTASY;
    name = "Harry Potter: First Book";
    Book book(author_id, name, genre);

    std::cout << "===========================================" << std::endl;
    Query q(AND, book);
    p.Interact_With_Lib(q, lib);
    lib.Print();
    return 0;
}


