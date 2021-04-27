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
    NUM,
    LITERAL,
    INTEGER,
    CHAR,
    STRING,
    COMMENT,
    RO,
    PLUS,
    MINUS,
    DIVIDE,
    PRODUCT,
    OPEN_PARANTHESIS,
    CLOSE_PARANTHESIS,
    OPEN_BRACES,
    CLOSE_BRACES,
    OPEN_SQUARE_BRACKETS,
    CLOSE_SQUARE_BRACKETS,
    DECLARATION,
    COMMA,
    SEMI_COLON,
    INPUT,
    ASSIGNMENT
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
    {TokenName::NUM, "NUM"},
    {TokenName::INTEGER, "INT"},
    {TokenName::CHAR, "CHAR"},
    {TokenName::LITERAL, "LITERAL"},
    {TokenName::STRING, "STRING"},
    {TokenName::COMMENT, "COMMENT"},
    {TokenName::RO, "RO"},
    {TokenName::PLUS, "'+'"},
    {TokenName::MINUS, "'-'"},
    {TokenName::DIVIDE, "'/'"},
    {TokenName::PRODUCT, "'*'"},
    {TokenName::OPEN_PARANTHESIS, "'('"},
    {TokenName::CLOSE_PARANTHESIS, "')'"},
    {TokenName::OPEN_BRACES, "'{'"},
    {TokenName::CLOSE_BRACES, "'}"},
    {TokenName::OPEN_SQUARE_BRACKETS,
     "'['"},
    {TokenName::CLOSE_SQUARE_BRACKETS, "']'"},
    {TokenName::DECLARATION, "':'"},
    {TokenName::COMMA, "','"},
    {TokenName::SEMI_COLON, "';'"},
    {TokenName::INPUT, ">>"},
    {TokenName::ASSIGNMENT, ":="}};

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
    Token(TokenName inToken, string inLexeme = EMPTY_LEXEME) : token(inToken), lexeme(inLexeme) {}

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

Token isNumericLiteral(ifstream &fileStream)
{
    int state = 10;
    string lexeme = "";
    bool ended = false;
    char c;

    while (!ended)
    {
        switch (state)
        {
        case 10:
            c = fileStream.peek();
            if (isdigit(c))
            {
                lexeme += fileStream.get();
                state = 11;
            }
            else
            {
                state = 12;
            }
            break;
        case 11:
            c = fileStream.peek();
            if (isdigit(c))
            {
                lexeme += fileStream.get();
            }
            else
            {
                state = 12;
            }
        case 12:
        default:
            ended = true;
            break;
        }
    }
    return Token(TokenName::NUM, lexeme);
}

Token isCharacterLiteral(ifstream &fileStream)
{
    int state = 40;
    string lexeme = "";
    char c;

    while (true)
    {
        switch (state)
        {
        case 40:
            c = fileStream.peek();
            if (c == '\'')
            {
                lexeme += fileStream.get();
                state = 41;
            }
            else
            {
                state = -1;
            }
            break;
        case 41:
            c = fileStream.peek();
            if (isalpha(c))
            {
                lexeme += fileStream.get();
                state = 42;
            }
            else
            {
                state = -1;
            }
            break;
        case 42:
            c = fileStream.peek();
            if (c == '\'')
            {
                lexeme += fileStream.get();
                state = 43;
            }
            else
            {
                state = -1;
            }
            break;
        case 43:
        case 44:
        default:
            return Token(TokenName::LITERAL, lexeme);
        }
    }
}

Token isStringLiteral(ifstream &fileStream)
{
    int state = 50;
    string lexeme = "";
    char c;
    while (true)
    {
        switch (state)
        {
        case 50:
            c = fileStream.peek();
            if (c == '"')
            {
                lexeme += fileStream.get();
                state = 51;
            }
            else
            {
                state = -1;
            }
            break;
        case 51:
            c = fileStream.peek();
            if (c == fileStream.eof())
            {
                state = -1;
            }
            else if (c == '"')
            {
                lexeme += fileStream.get();
                state = 52;
            }
            else
            {
                lexeme += fileStream.get();
            }
            break;
        case 52:
        case 53:
        default:
            return Token(TokenName::STRING, lexeme);
            break;
        }
    }
}

Token isCommentOrNotEqOrDivide(ifstream &fileStream)
{
    int state = 1;
    string lexeme = "";
    char c;
    while (true)
    {
        switch (state)
        {
        case 1:
            c = fileStream.peek();
            if (c == '/')
            {
                lexeme += fileStream.get();
                state = 2;
            }
            else
            {
                state = -1;
            }
            break;
        case 2:
            c = fileStream.peek();
            if (c == '*')
            {
                lexeme += fileStream.get();
                state = 3;
            }
            else if (c == '=')
            {
                return Token(TokenName::RO, "NE");
            }
            else
            {
                return Token(TokenName::DIVIDE);
            }
            break;
        case 3:
            c = fileStream.peek();
            if (c == '*')
            {
                lexeme += fileStream.get();
                state = 4;
            }
            else if (c == fileStream.eof())
            {
                state = -1;
            }
            else
            {
                lexeme += fileStream.get();
            }
            break;
        case 4:
            c = fileStream.peek();
            if (c == '/')
            {
                lexeme += fileStream.get();
                state = 5;
            }
            else if (c == fileStream.eof())
            {
                state = -1;
            }
            else
            {
                lexeme += fileStream.get();
                state = 3;
            }
            break;
        case 5:
        default:
            return Token(TokenName::COMMENT, lexeme);
        }
    }
}

Token isROorInput(ifstream &fileStream)
{
    int state = 18;
    char c;
    while (true)
    {
        switch (state)
        {
        case 18:
            c = fileStream.peek();
            switch (c)
            {
            case '<':
                fileStream.get();
                state = 19;
                break;
            case '>':
                fileStream.get();
                state = 23;
                break;
            case '=':
                fileStream.get();
                state = 27;
                break;
            default:
                state = -1;
                break;
            }
            break;
        case 19:
            c = fileStream.peek();
            if (c == '=')
            {
                fileStream.get();
                return Token(TokenName::RO, "LE");
            }
            else
            {
                return Token(TokenName::RO, "LT");
            }
        case 23:
            c = fileStream.peek();
            if (c == '=')
            {
                fileStream.get();
                return Token(TokenName::RO, "GE");
            }
            else if (c == '>')
            {
                fileStream.get();
                return Token(TokenName::INPUT);
            }
            else
            {
                return Token(TokenName::RO, "GT");
            }
        case 27:
            return Token(TokenName::RO, "EQ");
        default:
            break;
        }
    }
}

Token isDeclarationOrAssignment(ifstream &fileStream)
{
    char c1 = fileStream.get();
    char c2 = fileStream.peek();

    if (c1 == ':' && c2 != '=')
    {
        return Token(TokenName::DECLARATION);
    }
    else
    {
        fileStream.get();
        return Token(TokenName::ASSIGNMENT);
    }
}

Token LexemeLessCreator(TokenName tokenName, ifstream &fileStream)
{
    fileStream.get();
    return Token(tokenName);
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

        switch (c)
        {
        case ' ':
        case '\n':
            inFile.get();
            break;
        case '{':
            tokens.push_back(LexemeLessCreator(TokenName::OPEN_BRACES, inFile));
            break;
        case '}':
            tokens.push_back(LexemeLessCreator(TokenName::CLOSE_BRACES, inFile));
            break;
        case '(':
            tokens.push_back(LexemeLessCreator(TokenName::OPEN_PARANTHESIS, inFile));
            break;
        case ')':
            tokens.push_back(LexemeLessCreator(TokenName::CLOSE_PARANTHESIS, inFile));
            break;
        case '[':
            tokens.push_back(LexemeLessCreator(TokenName::OPEN_SQUARE_BRACKETS, inFile));
            break;
        case ']':
            tokens.push_back(LexemeLessCreator(TokenName::CLOSE_SQUARE_BRACKETS, inFile));
            break;
        case '+':
            tokens.push_back(LexemeLessCreator(TokenName::PLUS, inFile));
            break;
        case '-':
            tokens.push_back(LexemeLessCreator(TokenName::MINUS, inFile));
            break;
        case '*':
            tokens.push_back(LexemeLessCreator(TokenName::PRODUCT, inFile));
            break;
        case '/':
            tokens.push_back(isCommentOrNotEqOrDivide(inFile));
            break;
        case ':':
            tokens.push_back(isDeclarationOrAssignment(inFile));
            break;
        case ',':
            tokens.push_back(LexemeLessCreator(TokenName::COMMA, inFile));
            break;
        case ';':
            tokens.push_back(LexemeLessCreator(TokenName::SEMI_COLON, inFile));
            break;
        case '<':
        case '>':
        case '=':
            tokens.push_back(isROorInput(inFile));
            break;
        case '\'':
            tokens.push_back(isCharacterLiteral(inFile));
            break;
        case '"':
            tokens.push_back(isStringLiteral(inFile));
            break;
        default:
            if (isalpha(c))
            {
                tokens.push_back(isIdentifierOrKeyword(inFile));
            }
            else if (isdigit(c))
            {
                tokens.push_back(isNumericLiteral(inFile));
            }
            else
            {
                cout << "Invalid Character: " << c << endl;
                inFile.get();
            }
            break;
        }
    }

    ofstream outFile;
    outFile.open("words.txt");
    for (auto token : tokens)
    {
        outFile << token << endl;
    }
}
