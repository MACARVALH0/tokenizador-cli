#ifndef TOKENIZER_CLI
#define TOKENIZER_CLI

#include <cctype>
#include <iterator>
#include <string>
#include <vector>
#include "ErrorMsg.hpp"

enum class OpTokenType
{
    INITIAL,
    UNKNOWN,
    IDENTIFIER,
    FLAG,
    STRING
};

struct Token
{
    std::string content;
    OpTokenType type;

    int col;

    Token(std::string content, OpTokenType type, int col)
    : content(std::move(content)), type(type), col(col) {}
};

std::vector<Token> tokenize(std::string&);

#endif