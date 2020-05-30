#ifndef QUERY_H
#define QUERY_H
enum QueryType
{
    UNDEFINED = 0,
    AND = 1,
    OR  = 2,
};

struct Query
{
    QueryType   type;
    Book        book;

    Query() {};
    Query(QueryType _type, Book &_book);
};

typedef u32 (*ValidateQueryFunction) (Query&, Book&);
#endif // QUERY_H guard
