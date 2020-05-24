#include "Query.h"

//
// TODO: Think a little bit harder about reading QueryType from console
QueryType Validate_Query_Type(QueryType type)
{
    switch(type)
    {
        case AND:
        {
            return AND;
        } break;

        case OR:
        {
            return OR;
        } break;

        case UNDEFINED:
        default:
        {
            return UNDEFINED;
        } break;
    }
}

u32 Validate_ALL_Query(Query &query, Book &book)
{
    u32 author_id_equality = (query.book.author_id == book.author_id);
    u32 name_equality = (query.book.name == book.name);
    u32 genre_equality = (query.book.genre == book.genre);
    if(author_id_equality && name_equality && genre_equality)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

u32 Validate_OR_Query(Query &query, Book &book)
{
    u32 author_id_equality = (query.book.author_id == book.author_id);
    u32 name_equality = (query.book.name == book.name);
    u32 genre_equality = (query.book.genre == book.genre);
    if(author_id_equality || name_equality || genre_equality)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

Query::Query(QueryType _type, Book &_book)
{
    type = Validate_Query_Type(_type);
    // TODO: check if copying std::string works correctly (pretty sure it is)
    book = _book;
}

