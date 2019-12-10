#pragma once

#include "operations.h"

#define MAX_TOKENS_COUNT 100000
#define MAX_OPERATION_LENGTH 100000

enum TokenType {
    Op, Num, BracketOn, BracketOff, END
};

struct Token {
    enum OperationType operation;
    enum TokenType type;
    double operand;
};


// Split given string to tokens
struct Token** parse_string(const char* str);
