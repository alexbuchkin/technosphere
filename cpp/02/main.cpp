#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <inttypes.h>
#include <exception>

class Calc
{
public:
    Calc(const std::string&);
    int64_t GetResult() const;

private:
    enum class LEX_TYPE
    {
        Number,
        Plus,
        Minus,
        Mul,
        Div,
        UnaryMinus
    };

    using LexemeIterator = std::vector<LEX_TYPE>::const_iterator;

    const std::map<char, LEX_TYPE> Char2Lex = {
        {'+', LEX_TYPE::Plus},
        {'-', LEX_TYPE::Minus},
        {'*', LEX_TYPE::Mul},
        {'/', LEX_TYPE::Div}
    };

    const std::map<LEX_TYPE, unsigned char> Lex2Prior = {
        {LEX_TYPE::Plus, 1},
        {LEX_TYPE::Minus, 1},
        {LEX_TYPE::Mul, 2},
        {LEX_TYPE::Div, 2},
        {LEX_TYPE::UnaryMinus, 3}
    };

    const std::vector<LEX_TYPE> Lexemes;
    const std::map<std::vector<LEX_TYPE>::const_iterator, int64_t> Values;

private:
    decltype(Lexemes) Parse(const std::string&);
    decltype(Values) GetValues(const std::string&); // run only after Parse

    inline bool IsSign(char c) const noexcept {return Char2Lex.find(c) != Char2Lex.cend();}

    LexemeIterator FindCurrentOp(LexemeIterator, LexemeIterator) const;
    int64_t GetResult(LexemeIterator, LexemeIterator) const;
};

Calc::Calc(const std::string& str)
    : Lexemes(Parse(str))
    , Values(GetValues(str))
    {}

decltype(Calc::Lexemes)
Calc::Parse(const std::string& str)
{
    std::vector<LEX_TYPE> result;
    for (auto it = str.cbegin(); it < str.cend(); ++it) {
        if (*it == '-' && (result.empty() || result.back() != LEX_TYPE::Number))
            result.push_back(LEX_TYPE::UnaryMinus);
        else if (IsSign(*it))
            result.push_back(Char2Lex.at(*it));
        else if (isdigit(*it)) {
            result.push_back(LEX_TYPE::Number);
            for (; it + 1 != str.cend() && isdigit(*(it + 1)); ++it); // +1 because of increment of outer for
        }
    }
    return result;
}

decltype(Calc::Values)
Calc::GetValues(const std::string& str)
{
    std::map<std::vector<LEX_TYPE>::const_iterator, int64_t> result;
    auto lexIt = Lexemes.cbegin();
    for (auto it = str.cbegin(); it < str.cend(); ++it) {
        if (isdigit(*it)) {
            auto itEnd = it + 1;
            for (; itEnd != str.cend() && isdigit(*itEnd); ++itEnd);
            int64_t value = strtoimax(std::string(it, itEnd).c_str(), nullptr, 10);

            for (; *lexIt != LEX_TYPE::Number; ++lexIt);
            result[lexIt++] = value;

            it = itEnd;
        }
    }
    return result;
}

Calc::LexemeIterator
Calc::FindCurrentOp(Calc::LexemeIterator itBegin, Calc::LexemeIterator itEnd) const
{
    unsigned char prior = 0;
    Calc::LexemeIterator opIt = itEnd;
    for (auto it = itBegin; it != itEnd; ++it) {
        if (*it == LEX_TYPE::Number)
            continue;

        unsigned char curPrior = Lex2Prior.at(*it);
        if (prior == 0 || curPrior <= prior) {
            prior = curPrior;
            opIt = it;
        }
    }
    return opIt;
}

int64_t
Calc::GetResult(Calc::LexemeIterator itBegin, Calc::LexemeIterator itEnd) const
{
    if (itBegin == itEnd)
        throw std::invalid_argument("");

    auto opIt = FindCurrentOp(itBegin, itEnd);
    if (opIt == itEnd) { // no ops left
        if (itBegin + 1 != itEnd) // two numbers in sequence
            throw std::invalid_argument("");
        return Values.at(itBegin);
    }

    if (*opIt == LEX_TYPE::UnaryMinus) {
        if (*itBegin != LEX_TYPE::UnaryMinus) // unary minus is not binary
            throw std::invalid_argument("");
        return int64_t(-1) * GetResult(itBegin + 1, itEnd);
    }

    int64_t left = GetResult(itBegin, opIt);
    int64_t right = GetResult(opIt + 1, itEnd);

    switch (*opIt) {
    case LEX_TYPE::Plus:
        return left + right;
    case LEX_TYPE::Minus:
        return left - right;
    case LEX_TYPE::Mul:
        return left * right;
    case LEX_TYPE::Div:
        if (right == 0)
            throw std::invalid_argument("");
        return left / right;
    }

    throw std::logic_error("smth wrong in GetResult");
}

int64_t
Calc::GetResult() const
{
    return GetResult(Lexemes.cbegin(), Lexemes.cend());
}

int
main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "error" << std::endl;
        return 1;
    }
    auto a = Calc(argv[1]);
    try {
        std::cout << a.GetResult() << std::endl;
    }
    catch (const std::invalid_argument&) {
        std::cout << "error" << std::endl;
        return 1;
    }
    return 0;
}