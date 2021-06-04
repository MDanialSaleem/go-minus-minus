#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <algorithm>
using namespace std;

const string EMPTY_LEXEME = "^";
//HARDCODED BECUASE SIR SPECIFIED THESE IN ASSIGNMENT.
const string LEX_OUTPUT_FILE_NAME = "words.txt";
const string PARSE_TREE_OUTPUT_FILE_NAME = "parsetree.txt";
const string SYMBOL_TABLE_OUTPUT_FILE_NAME = "parser-symboltable.txt";
const string TAC_FILE_NAME = "tac.txt";
const string TEMP_TAC_FILE_NAME = "temp_tac.txt";

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
    RET,
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
    {TokenName::RET, "RET"},
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
    {TokenName::FILEEND, "EOF"},
    {TokenName::NULLPROD, "^"}};

// used to differentiate between identifiers and keywords.
const map<string, TokenName> keywordsMapper = {
    {"if", TokenName::IF},
    {"elif", TokenName::ELIF},
    {"else", TokenName::ELSE},
    {"while", TokenName::WHILE},
    {"in", TokenName::IN},
    {"func", TokenName::FUNC},
    {"ret", TokenName::RET},
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
    static string getOutputMapping(const Token &token)
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
        return output;
    }
    static string getStrippedOutputMapping(const Token &token)
    {
        auto output = Token::getOutputMapping(token);
        if (output[0] == '\'' && output[output.length() - 1] == '\'')
        {
            return output.substr(1, output.length() - 2);
        }
        else
        {
            return output;
        }
    }
};

ostream &operator<<(ostream &out, const Token &token)
{

    out << '(' << Token::getOutputMapping(token) << ',' << token.lexeme << ')';
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
        case '\t':
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
    // read comment over the peekNextMultipleTokens to see the need for this.
    vector<Token> readAheadBuffer;

    Token parseNextToken()
    {
        string token;
        string lexeme;
        string buffer;
        getline(tokenFile, buffer, '(');
        getline(tokenFile, token, ',');
        if (token == "'")
        {
            //handles the special case when the token is comma. it is of the form ','
            tokenFile.get(); // remove the ending '
            token = "','";
            lexeme = EMPTY_LEXEME;
        }
        else
        {
            getline(tokenFile, lexeme, ')');
        }
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
    Token getFreshNextToken()
    {
        Token returner = parseNextToken();
        while (returner.token == TokenName::COMMENT)
        {
            returner = parseNextToken();
        }
        return returner;
    }

public:
    TokenReader(string fileName)
    {
        analyze(fileName, LEX_OUTPUT_FILE_NAME);
        tokenFile.open(LEX_OUTPUT_FILE_NAME);
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
        Token returner = currToken;
        if (readAheadBuffer.empty())
        {
            currToken = getFreshNextToken();
        }
        else
        {
            currToken = readAheadBuffer[0];
            readAheadBuffer.erase(readAheadBuffer.begin());
        }

        return returner;
    }
    vector<Token> peekNextMultiple(int N)
    {
        vector<Token> returner;
        returner.reserve(N);
        returner.push_back(currToken);
        int needed = N - 1;
        for (int i = 0; i < readAheadBuffer.size() - needed; i++)
        {
            readAheadBuffer.push_back(getFreshNextToken());
        }
        for (int i = 0; i < needed; i++)
        {
            returner.push_back(readAheadBuffer[i]);
        }
        return returner;
    }
};

const string DECLARATION_PRODUCTION_NAME = "D";
const string PARAMTER_PRODCUTION_NAME = "PAR";

class SymbolTable
{
private:
    ofstream symbolTableOutputStream;
    bool declaration = false;
    bool paramters = false;
    TokenName activeDataType;
    // jsut as an extra safety layer.
    void setActiveDataType(TokenName tokenName)
    {
        if (tokenName == TokenName::INTEGER || tokenName == TokenName::CHAR)
        {
            activeDataType = tokenName;
        }
        else
        {
            cout << "[SYMBOL TABLE] Trying to set wrong active data type." << endl;
        }
    }

public:
    SymbolTable()
    {
        symbolTableOutputStream.open(SYMBOL_TABLE_OUTPUT_FILE_NAME);
        if (!symbolTableOutputStream.is_open())
        {
            cout << "could not open symbol file" << endl;
        }
    }
    void evaluate(Token token)
    {
        if (declaration)
        {
            // this works like a simple state machine.
            switch (token.token)
            {
            case TokenName::INTEGER:
            case TokenName::CHAR:
                setActiveDataType(token.token);
                break;
            case TokenName::IDENTIFIER:

                symbolTableOutputStream << Token::getOutputMapping(activeDataType) << "---" << token.lexeme << endl;
                break;
            case TokenName::SEMI_COLON:
                declaration = false;
                break;
            default:
                break;
            }
        }
        else if (paramters)
        {
            switch (token.token)
            {
            case TokenName::INTEGER:
            case TokenName::CHAR:
                setActiveDataType(token.token);
                break;
            case TokenName::IDENTIFIER:
                symbolTableOutputStream << Token::getOutputMapping(activeDataType) << "---" << token.lexeme << endl;
                break;
            case TokenName::CLOSE_PARANTHESIS:
                paramters = false;
            default:
                break;
            }
        }
    }
    void evaluate(string functionName)
    {
        if (functionName == DECLARATION_PRODUCTION_NAME)
        {
            if (paramters || declaration)
            {
                cout << "[SYMBOL TABLE] no declaration within paramters or declaration" << endl;
            }
            else
            {
                declaration = true;
            }
        }
        else if (functionName == PARAMTER_PRODCUTION_NAME)
        {
            if (paramters || declaration)
            {
                cout << "[SYMBOL TABLE] no paramters within parameters or declaration" << endl;
            }
            else
            {
                paramters = true;
            }
        }
    }
};

class Translator
{
private:
    string currentTemp = "a";
    ofstream outFile;
    int lineNumber = 1;
    vector<tuple<int, int>> backpatches;

public:
    Translator()
    {
        outFile.open(TEMP_TAC_FILE_NAME);
        if (!outFile.is_open())
        {
            cout << "could not open translator file" << endl;
        }
    }
    string GetTemp()
    {
        string returner = currentTemp;
        if (currentTemp[currentTemp.length() - 1] == 'z')
        {
            currentTemp += "a";
        }
        else
        {
            currentTemp[currentTemp.length() - 1]++;
        }
        return returner;
    }

    int GetNextLineNumber()
    {
        return lineNumber;
    }
    void writeLineNumber()
    {
        outFile << lineNumber << ")";
        lineNumber++;
    }
    string WriteExpressionGetTemp(string value1, Token op, string value2)
    {
        auto allowedOperetos = {TokenName::PRODUCT, TokenName::PLUS, TokenName::MINUS, TokenName::DIVIDE};
        bool found = false;
        for (auto allowedOperator : allowedOperetos)
        {
            if (op.token == allowedOperator)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            cout << "Invalid operator " << op.token << " found in write expression. This is likely a logical error" << endl;
        }
        string temp = GetTemp();
        writeLineNumber();
        outFile << temp << "=" << value1 << Token::getStrippedOutputMapping(op) << value2 << endl;
        return temp;
    }
    void WriteAssignment(Token identifier, string value)
    {
        if (identifier.token != TokenName::IDENTIFIER)
        {
            cout << "Unexpected toekn " << Token::getOutputMapping(identifier.token) << " in assignment, this is likely a logical error" << endl;
        }
        writeLineNumber();
        outFile << identifier.lexeme << "=" << value << endl;
    }
    void WriteIF(string G1, Token RO, string G2)
    {
        if (RO.token != TokenName::RO)
        {
            cout << "Unexpected toekn " << Token::getOutputMapping(RO.token) << " in if, this is likely a logical error" << endl;
        }
        writeLineNumber();
        outFile << "if" << G1 << RO.lexeme << G2 << "goto " << GetNextLineNumber() + 1 << endl;
    }
    int writeGoToGetLineNumber()
    {
        int returner = lineNumber;
        writeLineNumber();
        outFile << endl;
        return returner;
    }
    void backpatch(int fromLineNumber, int toLineNumber)
    {
        backpatches.push_back(make_tuple(fromLineNumber, toLineNumber));
    }
    void backpatch()
    {
        this->outFile.close();
        ifstream inFile;
        ofstream outFile;
        inFile.open(TEMP_TAC_FILE_NAME);
        if (!inFile.is_open())
        {
            cout << "could not open temporary tac file for backpatchings" << endl;
        }
        outFile.open(TAC_FILE_NAME);
        if (!outFile.is_open())
        {
            cout << "could not open tac file" << endl;
        }
        sort(backpatches.begin(), backpatches.end());
        auto currentBackpatch = backpatches.begin();
        while (!inFile.eof())
        {
            string line;
            getline(inFile, line);
            if (line == to_string(get<0>(*currentBackpatch)) + ")")
            {
                outFile << line
                        << "goto" << get<1>(*currentBackpatch) << endl;
                currentBackpatch++;
            }
            else
            {
                outFile << line << endl;
            }
        }
    }
};

class Parser
{
private:
    int depth = -1;
    TokenReader tokenReader;
    SymbolTable symTable;
    Translator translator;
    ofstream parseTreeOutputStream;

public:
    Parser(string sourceCodeFileName) : tokenReader(sourceCodeFileName)
    {
        parseTreeOutputStream.open(PARSE_TREE_OUTPUT_FILE_NAME);
        if (!parseTreeOutputStream.is_open())
        {
            cout << "could not open parse tree file" << endl;
        }
    }

private:
    string P()
    {
        EnterFunction("P");
        auto token = tokenReader.peekNextToken();
        string finalPVal;
        switch (token.token)
        {
        case TokenName::NUM:
        case TokenName::IDENTIFIER:
            finalPVal = token.lexeme;
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
        return finalPVal;
    }
    string M_PRIME(string initMPrimeVal)
    {
        EnterFunction("M'");
        auto token = tokenReader.peekNextToken();
        string finalMPrimeVal;
        switch (token.token)
        {
        case TokenName::PRODUCT:
        case TokenName::DIVIDE:
        {
            Mark(tokenReader.consumeNextToken());
            string pVal = P();
            string mPrimeVal = M_PRIME(pVal);
            finalMPrimeVal = translator.WriteExpressionGetTemp(initMPrimeVal, token, mPrimeVal);
            break;
        }
        default:
        {
            Mark(Token());
            finalMPrimeVal = initMPrimeVal;
            break;
        }
        }
        LeaveFunction();
        return finalMPrimeVal;
    }
    string M()
    {
        EnterFunction("M");
        string finalMVal;
        string pVal = P();
        finalMVal = M_PRIME(pVal);
        LeaveFunction();
        return finalMVal;
    }
    string E_PRIME(string initEPrimeVal)
    {
        EnterFunction("E'");
        auto token = tokenReader.peekNextToken();
        string finalEPrimeVal;
        switch (token.token)
        {
        case TokenName::PLUS:
        case TokenName::MINUS:
        {
            Mark(tokenReader.consumeNextToken());
            string mVal = M();
            string ePrimeVal = E_PRIME(mVal);
            finalEPrimeVal = translator.WriteExpressionGetTemp(initEPrimeVal, token, ePrimeVal);
            break;
        }
        default:
        {
            Mark(Token());
            finalEPrimeVal = initEPrimeVal;
            break;
        }
        }
        LeaveFunction();
        return finalEPrimeVal;
    }
    string E()
    {
        EnterFunction("E");
        string mval = M();
        string val = E_PRIME(mval);
        LeaveFunction();
        return val;
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
    string G()
    {
        const string functionName = "G";
        EnterFunction(functionName);
        string finalGVal;
        Token tok = tokenReader.peekNextToken();
        if (tok.token == TokenName::LITERAL)
        {
            finalGVal = MatchToken(functionName, tok).lexeme;
        }
        else
        {
            finalGVal = E();
        }
        LeaveFunction();
        return finalGVal;
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
        case TokenName::IN:
            IN();
            B();
            break;
        case TokenName::IDENTIFIER:
            if (isIncomingAssignment())
            {
                A();
                S();
            }
            else if (isIncomingFunctionCall())
            {
                O();
                S();
            }
            else
            {
                E();
                MatchToken(functionName, Token(TokenName::SEMI_COLON));
                S();
            }

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
        {
            MatchToken(functionName, TokenName::ELIF);
            auto firstGVal = G();
            auto ROToken = MatchToken(functionName, TokenName::RO);
            auto secondGVal = G();
            MatchToken(functionName, TokenName::DECLARATION);
            translator.WriteIF(firstGVal, ROToken, secondGVal);
            auto cFalse = translator.writeGoToGetLineNumber();
            MatchToken(functionName, TokenName::OPEN_BRACES);
            B();
            auto C_Next = translator.writeGoToGetLineNumber();
            translator.backpatch(cFalse, translator.GetNextLineNumber());
            MatchToken(functionName, TokenName::CLOSE_BRACES);
            H();
            translator.backpatch(C_Next, translator.GetNextLineNumber());
            break;
        }
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
        auto firstGVal = G();
        auto ROToken = MatchToken(functionName, TokenName::RO);
        auto secondGVal = G();
        MatchToken(functionName, TokenName::DECLARATION);
        translator.WriteIF(firstGVal, ROToken, secondGVal);
        auto cFalse = translator.writeGoToGetLineNumber();
        MatchToken(functionName, TokenName::OPEN_BRACES);
        B();
        auto C_Next = translator.writeGoToGetLineNumber();
        translator.backpatch(cFalse, translator.GetNextLineNumber());
        MatchToken(functionName, TokenName::CLOSE_BRACES);
        H();
        translator.backpatch(C_Next, translator.GetNextLineNumber());
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
        const string functionName = DECLARATION_PRODUCTION_NAME;
        EnterFunction(functionName);
        T();
        MatchToken(functionName, TokenName::DECLARATION);
        MatchToken(functionName, TokenName::IDENTIFIER);
        F();
        MatchToken(functionName, TokenName::SEMI_COLON);
        LeaveFunction();
    }
    void PAR_PRIME()
    {
        const string functionName = "PAR'";
        EnterFunction(functionName);
        if (tokenReader.peekNextToken().token == TokenName::COMMA)
        {
            MatchToken(functionName, TokenName::COMMA);
            T();
            MatchToken(functionName, TokenName::DECLARATION);
            MatchToken(functionName, TokenName::IDENTIFIER);
            PAR_PRIME();
        }
        else
        {
            Mark(Token());
        }
        LeaveFunction();
    }
    void PAR()
    {
        const string functionName = "PAR";
        EnterFunction(functionName);
        Token token = tokenReader.peekNextToken();
        if (token.token == TokenName::CHAR || token.token == TokenName::INTEGER)
        {
            T();
            MatchToken(functionName, TokenName::DECLARATION);
            MatchToken(functionName, TokenName::IDENTIFIER);
            PAR_PRIME();
        }
        else
        {
            Mark(Token());
        }
        LeaveFunction();
    }
    void Q()
    {
        const string functionName = "Q";
        EnterFunction(functionName);
        if (tokenReader.peekNextToken().token == TokenName::RET)
        {
            MatchToken(functionName, TokenName::RET);
            G();
            MatchToken(functionName, TokenName::SEMI_COLON);
        }
        else
        {
            Mark(Token());
        }
        LeaveFunction();
    }
    void FN()
    {
        const string functionName = "FN";
        EnterFunction(functionName);
        MatchToken(functionName, TokenName::FUNC);
        T();
        MatchToken(functionName, TokenName::DECLARATION);
        MatchToken(functionName, TokenName::IDENTIFIER);
        MatchToken(functionName, TokenName::OPEN_PARANTHESIS);
        PAR();
        MatchToken(functionName, TokenName::CLOSE_PARANTHESIS);
        MatchToken(functionName, TokenName::OPEN_BRACES);
        B();
        Q();
        MatchToken(functionName, TokenName::CLOSE_BRACES);
        LeaveFunction();
    }
    void IN()
    {
        const string functionName = "IN";
        EnterFunction(functionName);
        MatchToken(functionName, TokenName::IN);
        MatchToken(functionName, TokenName::INPUT);
        MatchToken(functionName, TokenName::IDENTIFIER);
        MatchToken(functionName, TokenName::SEMI_COLON);
        LeaveFunction();
    }
    void A()
    {
        const string functionName = "A";
        EnterFunction(functionName);
        auto identifierToken = MatchToken(functionName, TokenName::IDENTIFIER);
        MatchToken(functionName, TokenName::ASSIGNMENT);

        Token tok = tokenReader.peekNextToken();
        if (tok.token == TokenName::LITERAL)
        {
            MatchToken(functionName, TokenName::LITERAL);
        }
        else
        {
            auto eval = E();
            translator.WriteAssignment(identifierToken, eval);
        }
        MatchToken(functionName, TokenName::SEMI_COLON);
        LeaveFunction();
    }
    void O_PRIME_PRIME()
    {
        const string functionName = "O''";
        EnterFunction(functionName);
        if (tokenReader.peekNextToken().token == TokenName::COMMA)
        {
            MatchToken(functionName, TokenName::COMMA);
            MatchToken(functionName, TokenName::IDENTIFIER);
            O_PRIME_PRIME();
        }
        else
        {
            Mark(Token());
        }
        LeaveFunction();
    }
    void O_PRIME()
    {
        const string functionName = "O'";
        EnterFunction(functionName);
        if (tokenReader.peekNextToken().token == TokenName::IDENTIFIER)
        {
            MatchToken(functionName, TokenName::IDENTIFIER);
            O_PRIME_PRIME();
        }
        else
        {
            Mark(Token());
        }
        LeaveFunction();
    }
    void O()
    {
        const string functionName = "O";
        EnterFunction(functionName);
        MatchToken(functionName, TokenName::IDENTIFIER);
        MatchToken(functionName, TokenName::OPEN_PARANTHESIS);
        O_PRIME();
        MatchToken(functionName, TokenName::CLOSE_PARANTHESIS);
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
        case TokenName::FUNC:
            FN();
            S();
            break;
        case TokenName::IN:
            IN();
            S();
            break;
        case TokenName::IDENTIFIER:
            if (isIncomingAssignment())
            {
                A();
                S();
            }
            else if (isIncomingFunctionCall())
            {
                O();
                S();
            }
            else
            {
                E();
                MatchToken(functionName, Token(TokenName::SEMI_COLON));
                S();
            }

            break;
        case TokenName::FILEEND:
        default:
            Mark(Token());
            break;
        }
        LeaveFunction();
    }

    bool isIncomingAssignment()
    {
        vector<Token> lookAhead = tokenReader.peekNextMultiple(2);
        return lookAhead.size() == 2 && lookAhead[0].token == TokenName::IDENTIFIER && lookAhead[1].token == TokenName::ASSIGNMENT;
    }
    bool isIncomingFunctionCall()
    {
        vector<Token> lookAhead = tokenReader.peekNextMultiple(2);
        return lookAhead.size() == 2 && lookAhead[0].token == TokenName::IDENTIFIER && lookAhead[1].token == TokenName::OPEN_PARANTHESIS;
    }
    Token MatchToken(string functionName, Token token)
    {
        if (token.token == TokenName::NULLPROD)
        {
            cout << "Trying to match null production this should probably be marked instead" << endl;
        }
        auto consumeToken = tokenReader.consumeNextToken();
        if (token.CheckOnlyTokenType(consumeToken))
        {
            symTable.evaluate(consumeToken);
            Mark(consumeToken);
            return consumeToken;
        }
        else
        {
            cout << "Expected token " << token << "in " << functionName << " but found " << consumeToken << endl;
            exit(1);
        }
    }

    void MarkDepth()
    {
        parseTreeOutputStream << "|";
        for (int i = 0; i < depth; i++)
        {
            parseTreeOutputStream << "-";
        }
    }
    void Mark(string functionName)
    {
        if (functionName.length() == 0)
        {
            cout << "ERROR: FUNCTION NAME CANNOT BE EMPTY" << endl;
        }
        MarkDepth();
        parseTreeOutputStream << functionName << endl;
    }
    void Mark(Token token)
    {
        depth++;
        MarkDepth();
        parseTreeOutputStream << Token::getOutputMapping(token);
        if (token.lexeme != EMPTY_LEXEME)
        {
            parseTreeOutputStream << "(" << token.lexeme << ")";
        }
        parseTreeOutputStream << endl;
        depth--;
    }

    void EnterFunction(string functionName)
    {
        symTable.evaluate(functionName);
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
        translator.backpatch();
    }
};

int main()
{
    // string fileName;
    // cout << "Please enter the name of the input go file complete with extension" << endl;

    // cin >> fileName;

    // if (fileName.substr(fileName.length() - 3, 3) != ".go")
    // {
    //     cout << "The given file is not a go source code file" << endl;
    //     return 1;
    // }
    // lexical analyzer is called by teh parser.
    Parser parser = Parser("test.go");
    parser.parse();
    return 0;
}
