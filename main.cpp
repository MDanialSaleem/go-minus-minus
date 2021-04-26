#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

enum TokenName
{
    IF,
    ELIF,
    ELSE,
    WHILE,
    IN,
    FUNC,
    PRINT,
    PRINTLN,
    IDENTIFIER,
    INTEGER,
    CHAR
};

const string EMPTY_LEXEME = "^";

const map<TokenName, string> outputMapper = {
    {TokenName::IF, "IF"},
    {TokenName::ELIF, "ELIF"},
    {TokenName::ELSE, "ELSE"},
    {TokenName::WHILE, "WHILE"},
    {TokenName::IN, "IN"},
    {TokenName::FUNC, "FUNC"},
    {TokenName::PRINT, "PRINT"},
    {TokenName::PRINTLN, "PRINTLN"},
    {TokenName::IDENTIFIER, "ID"},
    {TokenName::INTEGER, "INT"},
    {TokenName::CHAR, "CHAR"}};

const map<string, TokenName> keywordsMapper = {
    {"if", TokenName::IF},
    {"elif", TokenName::ELIF},
    {"else", TokenName::ELSE},
    {"while", TokenName::WHILE},
    {"in", TokenName::IN},
    {"func", TokenName::FUNC},
    {"print", TokenName::PRINT},
    {"println", TokenName::PRINTLN},
    {"integer", TokenName::INTEGER},
    {"char", TokenName::CHAR}};

class Token
{
    TokenName token;
    string lexeme;

public:
    Token(TokenName inToken, string inLexeme) : token(inToken), lexeme(inLexeme) {}

    friend ostream &operator<<(ostream &out, const Token &token);
};

ostream &operator<<(ostream &out, const Token &token)
{
    auto it = outputMapper.find(token.token);
    string output;
    if (it == outputMapper.end())
    {
        output = "Output mapping not defined for " + token.token;
    }
    else
    {
        output = it->second;
    }
    out << '(' << output << ',' << token.lexeme << ')';
    return out;
}
Token isIdentifierOrKeyword(ifstream &fileStream)
{
    int state = 7;
    string lexeme = "";
    bool ended = false;
    char c;
    while (!ended)
    {
        switch (state)
        {
        case 7:
            c = fileStream.peek();
            if (isalpha(c))
            {
                fileStream.get();
                lexeme += c;
                state = 8;
            }
            else
            {
                state = 9;
            }
            break;
        case 8:
            c = fileStream.peek();
            if (isalnum(c))
            {
                fileStream.get();
                lexeme += c;
            }
            else
            {
                state = 9;
            }
            break;

        case 9:
        default:
            ended = true;
            break;
        }
    }
    auto it = keywordsMapper.find(lexeme);
    if (it == keywordsMapper.end())
    {
        return Token(TokenName::IDENTIFIER, lexeme);
    }
    else
    {
        return Token(it->second, EMPTY_LEXEME);
    }
}

int main()
{
    ifstream inFile;
    inFile.open("a.go");
    vector<Token> tokens;

    while (true)
    {
        char c;
        c = inFile.peek();

        if (inFile.eof())
        {
            break;
        }

        if (isalpha(c))
        {
            tokens.push_back(isIdentifierOrKeyword(inFile));
        }
        else
        {
            inFile.get();
        }
    }

    ofstream outFile;
    outFile.open("words.txt");
    for (auto token : tokens)
    {
        outFile << token << endl;
    }
}