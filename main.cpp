#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include <functional>
using namespace std;

const string EMPTY_LEXEME = "^";
const string OUTPUT_FILE_NAME = "words.txt"; //BECUASE SIR SPECIFIED IT IN ASSIGNMENT.

// On how state machines are represented:
// They follow the method tuaght by sir, with state numbers coming from DFAs submitted earlier.
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
    ASSIGNMENT,
    INVALID,
    FILEEND, // specia token for parser to know EOF.
    NULLPROD // special token for parser to know numm productions.
};

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
    {TokenName::ASSIGNMENT, ":="},
    {TokenName::INVALID, "ERROR"},
    {TokenName::FILEEND, "EOF"}};

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
public:
    TokenName token;
    string lexeme;

    Token(TokenName inToken, string inLexeme = EMPTY_LEXEME) : token(inToken), lexeme(inLexeme) {}
    Token() : token(TokenName::NULLPROD), lexeme("^"){};
    friend ostream &operator<<(ostream &out, const Token &token);
    bool operator==(Token &other)
    {
        return this->token == other.token && this->lexeme == other.lexeme;
    }
    bool CheckOnlyTokenType(Token &other)
    {
        return this->token == other.token;
    }
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
                lexeme += fileStream.get();
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
                lexeme = "Invalid character in literal encounrtered\t" + c;
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
                lexeme = "Unterminated string literal";
                state = -1;
            }
            break;
        case 43:
            return Token(TokenName::LITERAL, lexeme);
        default:
            return Token(TokenName::INVALID, lexeme);
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
                lexeme = "Unterminated string literal";
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
            return Token(TokenName::STRING, lexeme);
        default:
            return Token(TokenName::INVALID, lexeme);
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
                lexeme += fileStream.get();
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
                lexeme = "Unexpected EOF encounceted. Untermianted comment";
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
                lexeme = "Unexpected EOF encounceted. Untermianted comment";
                state = -1;
            }
            else
            {
                lexeme += fileStream.get();
                state = 3;
            }
            break;
        case 5:
            return Token(TokenName::COMMENT, lexeme);
        default:
            return Token(TokenName::INVALID, lexeme);
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
            return Token(TokenName::INVALID);
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
void analyze(string inputFileName, string outputFileName)
{
    ifstream inFile;
    inFile.open(inputFileName);

    if (!inFile.is_open())
    {
        cout << "File does not exist or you do not have permissions" << endl;
        return;
    }
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
        case ',':
            tokens.push_back(LexemeLessCreator(TokenName::COMMA, inFile));
            break;
        case ';':
            tokens.push_back(LexemeLessCreator(TokenName::SEMI_COLON, inFile));
            break;
        case ':':
            tokens.push_back(isDeclarationOrAssignment(inFile));
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
                tokens.push_back(Token(TokenName::INVALID, "" + c));
                inFile.get();
            }
            break;
        }
    }

    ofstream outFile;
    outFile.open(outputFileName);

    if (!outFile.is_open())
    {
        cout << "Output file could not be opened. It is likely that you do not have permissions" << endl;
    }
    for (auto token : tokens)
    {
        if (token.token == TokenName::INVALID)
        {
            cout << token << endl;
        }
        else
        {
            outFile << token << endl;
        }
    }
}

class TokenReader
{
private:
    ifstream tokenFile;
    Token currToken;
    Token parseNextToken()
    {
        string token;
        string lexeme;
        string buffer;
        getline(tokenFile, buffer, '(');
        getline(tokenFile, token, ',');
        if (tokenFile.peek() == '\'')
        {
            //handles the special case when the token is comma. it is of the form ','
            tokenFile.get();
            token = "','";
        }
        getline(tokenFile, lexeme, ')');
        for (auto it = outputMapper.begin(); it != outputMapper.end(); ++it)
        {
            if (it->second == token)
            {
                return Token(it->first, lexeme);
            }
        }
        if (tokenFile.eof())
        {
            return Token(TokenName::FILEEND, lexeme);
        }
        else
        {
            return Token(TokenName::INVALID, lexeme);
        }
    }

public:
    TokenReader(string fileName)
    {
        analyze(fileName, OUTPUT_FILE_NAME);
        tokenFile.open(OUTPUT_FILE_NAME);
        if (!tokenFile.is_open())
        {
            cout << "Token file could not be generated properly." << endl;
        }
        currToken = parseNextToken();
    }
    Token peekNextToken()
    {
        return currToken;
    }
    Token consumeNextToken()
    {
        auto returner = currToken;
        currToken = parseNextToken();
        while (currToken.token == TokenName::COMMENT)
        {
            currToken = parseNextToken();
        }
        return returner;
    }
};

class Parser
{
private:
    int depth = -1;
    TokenReader tokenReader;

public:
    Parser(string fileName) : tokenReader(fileName) {}

private:
    void P()
    {
        EnterFunction("P");
        auto token = tokenReader.peekNextToken();
        switch (token.token)
        {
        case TokenName::NUM:
        case TokenName::IDENTIFIER:
            Mark(tokenReader.consumeNextToken());
            break;
        case TokenName::OPEN_PARANTHESIS:
        {
            Mark(tokenReader.consumeNextToken());
            E();
            auto anotherToken = tokenReader.peekNextToken();
            MatchToken("P", Token(TokenName::CLOSE_PARANTHESIS));
            break;
        }
        default:
            UnexptedToken("P", token);
            break;
        }
        LeaveFunction();
    }
    void M_PRIME()
    {
        EnterFunction("M'");
        auto token = tokenReader.peekNextToken();
        switch (token.token)
        {
        case TokenName::PRODUCT:
        case TokenName::DIVIDE:
            Mark(tokenReader.consumeNextToken());
            P();
            M_PRIME();
            break;
        default:
            Mark(Token());
            break;
        }
        LeaveFunction();
    }
    void M()
    {
        EnterFunction("M");
        P();
        M_PRIME();
        LeaveFunction();
    }
    void E_PRIME()
    {
        EnterFunction("E'");
        auto token = tokenReader.peekNextToken();
        switch (token.token)
        {
        case TokenName::PLUS:
        case TokenName::MINUS:
            Mark(tokenReader.consumeNextToken());
            M();
            E_PRIME();
            break;
        default:
            Mark(Token());
            break;
        }
        LeaveFunction();
    }
    void E()
    {
        EnterFunction("E");
        M();
        E_PRIME();
        LeaveFunction();
    }
    void N()
    {
        const string functionName = "N";
        EnterFunction(functionName);
        Token token = tokenReader.peekNextToken();
        TokenName matchableTokens[] = {TokenName::STRING, TokenName::IDENTIFIER, TokenName::LITERAL, TokenName::NUM};
        bool found = false;
        for (auto it : matchableTokens)
        {
            if (token.token == it)
            {
                found = true;
                MatchToken(functionName, it);
            }
        }
        if (!found)
        {
            UnexptedToken(functionName, token);
        }
        LeaveFunction();
    }
    void L()
    {
        string functionName = "L";
        EnterFunction(functionName);
        Token token = tokenReader.peekNextToken();
        switch (token.token)
        {
        case TokenName::PRINT:
        case TokenName::PRINTLN:
            MatchToken(functionName, token.token == TokenName::PRINT ? TokenName::PRINT : TokenName::PRINTLN);
            MatchToken(functionName, TokenName::OPEN_PARANTHESIS);
            N();
            MatchToken(functionName, TokenName::CLOSE_PARANTHESIS);
            MatchToken(functionName, TokenName::SEMI_COLON);
            break;
        default:
            break;
        }
        LeaveFunction();
    }
    void G()
    {
        const string functionName = "G";
        EnterFunction(functionName);
        Token tok = tokenReader.peekNextToken();
        if (tok.token == TokenName::LITERAL)
        {
            MatchToken(functionName, tok);
        }
        else
        {
            E();
        }
        LeaveFunction();
    }
    void B()
    {
        const string functionName = "B";
        EnterFunction(functionName);
        Token token = tokenReader.peekNextToken();
        switch (token.token)
        {
        case TokenName::PRINT:
        case TokenName::PRINTLN:
            L();
            S();
            break;
        case TokenName::IF:
            C();
            S();
            break;
        case TokenName::WHILE:
            W();
            B();
            break;
        case TokenName::INTEGER:
        case TokenName::CHAR:
            D();
            B();
            break;
        case TokenName::IDENTIFIER:
            E();
            MatchToken(functionName, Token(TokenName::SEMI_COLON));
            S();
            break;
        default:
            Mark(Token());
            break;
        }
        LeaveFunction();
    }
    void H()
    {
        const string functionName = "H";
        EnterFunction(functionName);
        Token token = tokenReader.peekNextToken();
        switch (token.token)
        {
        case TokenName::ELSE:
            MatchToken(functionName, TokenName::ELSE);
            MatchToken(functionName, TokenName::OPEN_BRACES);
            B();
            MatchToken(functionName, TokenName::CLOSE_BRACES);
            break;
        case TokenName::ELIF:
            MatchToken(functionName, TokenName::ELIF);
            G();
            MatchToken(functionName, TokenName::RO);
            G();
            MatchToken(functionName, TokenName::DECLARATION);
            MatchToken(functionName, TokenName::OPEN_BRACES);
            B();
            MatchToken(functionName, TokenName::CLOSE_BRACES);
            H();
            break;
        default:
            MatchToken(functionName, Token());
            break;
        }
        LeaveFunction();
    }
    void C()
    {
        const string functionName = "C";
        EnterFunction(functionName);
        MatchToken(functionName, TokenName::IF);
        G();
        MatchToken(functionName, TokenName::RO);
        G();
        MatchToken(functionName, TokenName::DECLARATION);
        MatchToken(functionName, TokenName::OPEN_BRACES);
        B();
        MatchToken(functionName, TokenName::CLOSE_BRACES);
        H();
        LeaveFunction();
    }
    void X()
    {
        G();
    }
    void W()
    {
        const string functionName = "W";
        EnterFunction(functionName);
        MatchToken(functionName, TokenName::WHILE);
        X();
        MatchToken(functionName, TokenName::RO);
        X();
        MatchToken(functionName, TokenName::DECLARATION);
        MatchToken(functionName, TokenName::OPEN_BRACES);
        B();
        MatchToken(functionName, TokenName::CLOSE_BRACES);
        LeaveFunction();
    }
    void T()
    {
        const string functionName = "Y";
        EnterFunction(functionName);
        Token tok = tokenReader.peekNextToken();
        if (tok.token == TokenName::INTEGER)
        {
            MatchToken(functionName, TokenName::INTEGER);
        }
        else if (tok.token == TokenName::CHAR)
        {
            MatchToken(functionName, TokenName::CHAR);
        }
        else
        {
            UnexptedToken(functionName, tok);
        }
        LeaveFunction();
    }
    void F()
    {
        const string functionName = "F";
        EnterFunction(functionName);
        Token token = tokenReader.peekNextToken();
        if (token.token == TokenName::COMMA)
        {
            MatchToken(functionName, TokenName::COMMA);
            MatchToken(functionName, TokenName::IDENTIFIER);
            F();
        }
        else
        {
            Mark(Token());
        }
        LeaveFunction();
    }
    void D()
    {
        const string functionName = "D";
        EnterFunction(functionName);
        T();
        MatchToken(functionName, TokenName::DECLARATION);
        MatchToken(functionName, TokenName::IDENTIFIER);
        F();
        MatchToken(functionName, TokenName::SEMI_COLON);
        LeaveFunction();
    }
    void S()
    {
        const string functionName = "S";
        EnterFunction(functionName);
        auto token = tokenReader.peekNextToken();
        switch (token.token)
        {
        case TokenName::PRINT:
        case TokenName::PRINTLN:
            L();
            S();
            break;
        case TokenName::IF:
            C();
            S();
            break;
        case TokenName::WHILE:
            W();
            S();
            break;
        case TokenName::INTEGER:
        case TokenName::CHAR:
            D();
            S();
            break;
        case TokenName::IDENTIFIER:
            E();
            MatchToken(functionName, Token(TokenName::SEMI_COLON));
            S();
            break;
        case TokenName::FILEEND:
        default:
            Mark(Token());
            break;
        }
        LeaveFunction();
    }

    void MatchToken(string functionName, Token token)
    {
        if (token.token == TokenName::NULLPROD)
        {
            cout << "Trying to match null production this should probably be marked instead" << endl;
        }
        auto consumeToken = tokenReader.consumeNextToken();
        if (token.CheckOnlyTokenType(consumeToken))
        {
            if (consumeToken.token == TokenName::RO && consumeToken.lexeme == "EQ")
            {
                tokenReader.consumeNextToken();
            }
            Mark(consumeToken);
        }
        else
        {
            cout << "Expected token " << token << "in " << functionName << " but found " << consumeToken << endl;
            exit(1);
        }
    }

    void MarkDepth()
    {
        cout << "|";
        for (int i = 0; i < depth; i++)
        {
            cout << "-";
        }
    }
    void Mark(string functionName)
    {
        if (functionName.length() == 0)
        {
            cout << "ERROR: FUNCTION NAME CANNOT BE EMPTY" << endl;
        }
        MarkDepth();
        cout << functionName << endl;
    }
    void Mark(Token token)
    {
        depth++;
        MarkDepth();
        cout << token << endl;
        depth--;
    }

    void EnterFunction(string functionName)
    {
        depth++;
        Mark(functionName);
    }
    void LeaveFunction()
    {
        depth--;
    }
    void UnexptedToken(string functionName, Token token)
    {
        cout << "Unexpected token " << token << "occured in" << functionName << endl;
        exit(1);
    }

public:
    void parse()
    {
        S();
    }
};

int mainlex()
{
    string fileName;
    cout << "Please enter the name of the input go file complete with extension" << endl;

    cin >> fileName;

    if (fileName.substr(fileName.length() - 3, 3) != ".go")
    {
        cout << "The given file is not a go source code file" << endl;
        return 1;
    }
    analyze(fileName, OUTPUT_FILE_NAME);
    return 0;
}

int main()
{
    Parser parser = Parser("test2.go");
    parser.parse();
}
