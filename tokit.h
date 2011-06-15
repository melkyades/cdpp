/*******************************************************************
*
*  DESCRIPTION: class TokenIterator
*
*  AUTHOR:      Alejandro López 
*
*  EMAIL:       mailto:aplopez@dc.uba.ar
*
*  DATE:        10/6/2001
*
********************************************************************/

#ifndef _TOKEN_ITERATOR_HPP
#define _TOKEN_ITERATOR_HPP

#include <string>

class TokenIterator
{
private:
    typedef istream_iterator<string> base_stream;

    base_stream base;
    string current;
    string buffer;
    bool ended;
    int findInString(string &str, char c);
    
public:
    TokenIterator() : base(), current(""), buffer(""), ended(true) {};
    TokenIterator(istream &ist) : base(ist), current(""), buffer(""),
                                  ended(false) { ++(*this); };

    
    inline const string& operator*() const { return current; };
    inline const string* operator->() const { return &current; };
    //inline const char* begin() const { return current.begin(); };
    //inline const char* end() const { return current.end(); };
    TokenIterator& operator++();
    TokenIterator operator++(int);
    
    
    bool operator==(const TokenIterator& a);
    inline bool operator!=(const TokenIterator& a)
                                            { return !(*this == a); };
};

#endif //  _TOKEN_ITERATOR_HPP
