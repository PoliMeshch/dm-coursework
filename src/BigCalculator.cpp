#include "BigCalculator.h"

#include <map>
#include <utility>

typedef std::vector<char> Digits;
typedef std::pair<char, char> Pair;

const std::size_t kLen = 8;
const char kZero = 'a';
const char kOne = 'b';
const char kTwo = 'e';

const std::map<char, char> kNext = {
    {'a', 'b'}, {'b', 'e'}, {'c', 'f'}, {'d', 'c'}, {'e', 'h'}, {'f', 'j'},
    {'g', 'k'}, {'h', 'g'}, {'i', 'd'}, {'j', 'a'}, {'k', 'i'} };

struct Tables {
    std::map<Pair, char> sum;
    std::map<Pair, char> diff;
    std::map<Pair, bool> carry;
};

static Tables buildTables() {
    Tables t;
    char x = kZero;
    do {
        char s = x;
        char y = kZero;
        bool wrapped = false;
        do {
            t.sum[std::make_pair(x, y)] = s;
            t.carry[std::make_pair(x, y)] = wrapped;
            y = kNext.at(y);
            s = kNext.at(s);
            wrapped = wrapped || s == kZero;
        } while (y != kZero);
        x = kNext.at(x);
    } while (x != kZero);
    for (std::map<Pair, char>::const_iterator it = t.sum.begin(); it != t.sum.end(); ++it) {
        t.diff[std::make_pair(it->second, it->first.second)] = it->first.first;
    }
    return t;
}

static const Tables& tables() {
    static const Tables t = buildTables();
    return t;
}

static char digitAdd(char x, char y) { return tables().sum.at(std::make_pair(x, y)); }
static char digitSub(char x, char y) { return tables().diff.at(std::make_pair(x, y)); }
static bool digitWraps(char x, char y) { return tables().carry.at(std::make_pair(x, y)); }
static char digitOf(bool flag) { return flag ? kOne : kZero; }

static bool digitLess(char x, char y) {
    for (char c = kZero; c != y; c = kNext.at(c)) {
        if (c == x) return true;
    }
    return false;
}

static Digits zero() { return Digits(kLen, kZero); }

static Digits one() {
    Digits d = zero();
    d.back() = kOne;
    return d;
}

static Digits two() {
    Digits d = zero();
    d.back() = kTwo;
    return d;
}

static bool isZero(const Digits& d) {
    for (std::size_t i = 0; i < d.size(); ++i) {
        if (d[i] != kZero) return false;
    }
    return true;
}

static int compare(const Digits& x, const Digits& y) {
    for (std::size_t i = 0; i < x.size(); ++i) {
        if (x[i] != y[i]) {
            return digitLess(x[i], y[i]) ? -1 : 1;
        }
    }
    return 0;
}

static Digits addMag(const Digits& x, const Digits& y, bool* overflow = nullptr) {
    Digits out(x.size(), kZero);
    bool carry = false;
    for (int i = static_cast<int>(x.size()) - 1; i >= 0; --i) {
        char cin = digitOf(carry);
        char s = digitAdd(x[i], y[i]);
        out[i] = digitAdd(s, cin);
        carry = digitWraps(x[i], y[i]) || digitWraps(s, cin);
    }
    if (carry && overflow) *overflow = true;
    return out;
}

static Digits subMag(const Digits& x, const Digits& y) {
    Digits out(x.size(), kZero);
    bool borrow = false;
    for (int i = static_cast<int>(x.size()) - 1; i >= 0; --i) {
        char bin = digitOf(borrow);
        char t = digitAdd(y[i], bin);
        char d = digitSub(x[i], t);
        borrow = digitWraps(y[i], bin) || digitWraps(d, t);
        out[i] = d;
    }
    return out;
}

static Digits shiftLeft(const Digits& x, bool* overflow) {
    if (x.front() != kZero && overflow) *overflow = true;
    Digits out(x.begin() + 1, x.end());
    out.push_back(kZero);
    return out;
}

static Digits mulDigit(const Digits& x, char d, bool* overflow) {
    Digits acc = zero();
    for (char k = kZero; k != d; k = kNext.at(k)) acc = addMag(acc, x, overflow);
    return acc;
}

static Digits mulMag(const Digits& x, const Digits& y, bool* overflow) {
    Digits acc = zero();
    for (std::size_t j = 0; j < y.size(); ++j) {
        acc = shiftLeft(acc, overflow);
        acc = addMag(acc, mulDigit(x, y[j], overflow), overflow);
    }
    return acc;
}

static std::pair<Digits, Digits> divMod(const Digits& x, const Digits& y) {
    Digits divisor;
    divisor.push_back(kZero);
    divisor.insert(divisor.end(), y.begin(), y.end());

    Digits rest(kLen + 1, kZero);
    Digits quotient = zero();
    for (std::size_t i = 0; i < x.size(); ++i) {
        rest.erase(rest.begin());
        rest.push_back(x[i]);
        char digit = kZero;
        while (compare(rest, divisor) >= 0) {
            rest = subMag(rest, divisor);
            digit = kNext.at(digit);
        }
        quotient[i] = digit;
    }
    Digits remainder(rest.begin() + 1, rest.end());
    return std::make_pair(quotient, remainder);
}

static Digits gcdMag(Digits x, Digits y) {
    while (!isZero(y)) {
        Digits r = divMod(x, y).second;
        x = y;
        y = r;
    }
    return x;
}

struct Number {
    bool neg;
    Digits mag;
};

static Number makeNumber(bool neg, const Digits& mag) {
    Number result;
    result.neg = neg && !isZero(mag);
    result.mag = mag;
    return result;
}

static Number parse(const Digits& v) {
    bool neg = !v.empty() && v.front() == '-';
    Digits mag = zero();
    std::size_t dst = mag.size();
    for (std::size_t src = v.size(); src > 0 && dst > 0; --src) {
        if (v[src - 1] != '-') mag[--dst] = v[src - 1];
    }
    return makeNumber(neg, mag);
}

static Digits toDigits(const Number& n) {
    std::size_t first = 0;
    while (first < n.mag.size() && n.mag[first] == kZero) ++first;
    if (first == n.mag.size()) return Digits(1, kZero);
    Digits out;
    if (n.neg) out.push_back('-');
    out.insert(out.end(), n.mag.begin() + first, n.mag.end());
    return out;
}

static std::string text(const Number& n) {
    Digits d = toDigits(n);
    return std::string(d.begin(), d.end());
}

static Number addNum(const Number& x, const Number& y, bool* overflow) {
    if (x.neg == y.neg) return makeNumber(x.neg, addMag(x.mag, y.mag, overflow));
    if (compare(x.mag, y.mag) >= 0) return makeNumber(x.neg, subMag(x.mag, y.mag));
    return makeNumber(y.neg, subMag(y.mag, x.mag));
}

BigCalculator::BigCalculator() : normalSize(true) {}

std::vector<char> BigCalculator::add(const std::vector<char>& num1, const std::vector<char>& num2) {
    bool overflow = false;
    Number r = addNum(parse(num1), parse(num2), &overflow);
    normalSize = !overflow;
    return toDigits(r);
}

std::vector<char> BigCalculator::subtract(const std::vector<char>& num1, const std::vector<char>& num2) {
    Number y = parse(num2);
    Number minusY = makeNumber(!y.neg, y.mag);
    bool overflow = false;
    Number r = addNum(parse(num1), minusY, &overflow);
    normalSize = !overflow;
    return toDigits(r);
}

std::vector<char> BigCalculator::multiply(const std::vector<char>& num1, const std::vector<char>& num2) {
    Number x = parse(num1);
    Number y = parse(num2);
    bool overflow = false;
    Number r = makeNumber(x.neg != y.neg, mulMag(x.mag, y.mag, &overflow));
    normalSize = !overflow;
    return toDigits(r);
}

std::string BigCalculator::division(const std::vector<char>& num1, const std::vector<char>& num2) {
    normalSize = true;
    Number x = parse(num1);
    Number y = parse(num2);
    if (isZero(y.mag)) return isZero(x.mag) ? "[-max, max]" : "empty";

    std::pair<Digits, Digits> dr = divMod(x.mag, y.mag);
    Digits quotient = dr.first;
    Digits rest = dr.second;
    if (x.neg && !isZero(rest)) {
        quotient = addMag(quotient, one());
        rest = subMag(y.mag, rest);
    }
    return "quotient = " + text(makeNumber(x.neg != y.neg, quotient)) +
        ", remainder = " + text(makeNumber(false, rest));
}

std::string BigCalculator::pow(const std::vector<char>& num1, const std::vector<char>& num2) {
    normalSize = true;
    Number base = parse(num1);
    Number exponent = parse(num2);
    if (exponent.neg) return "error";
    if (isZero(exponent.mag)) return "b";
    if (isZero(base.mag)) return "a";
    if (base.mag == one()) {
        bool odd = !isZero(divMod(exponent.mag, two()).second);
        return (base.neg && odd) ? "-b" : "b";
    }

    Number result = makeNumber(false, one());
    bool overflow = false;
    for (Digits k = zero(); k != exponent.mag; k = addMag(k, one())) {
        result = makeNumber(result.neg != base.neg, mulMag(result.mag, base.mag, &overflow));
        if (overflow) {
            normalSize = false;
            return "overflow";
        }
    }
    return text(result);
}

std::vector<char> BigCalculator::gcd(const std::vector<char>& num1, const std::vector<char>& num2) {
    Number x = parse(num1);
    Number y = parse(num2);
    normalSize = true;
    return toDigits(makeNumber(false, gcdMag(x.mag, y.mag)));
}

std::vector<char> BigCalculator::lcm(const std::vector<char>& num1, const std::vector<char>& num2) {
    Number x = parse(num1);
    Number y = parse(num2);
    Number result = makeNumber(false, zero());
    bool overflow = false;
    if (!isZero(x.mag) && !isZero(y.mag)) {
        Digits q = divMod(x.mag, gcdMag(x.mag, y.mag)).first;
        result = makeNumber(false, mulMag(q, y.mag, &overflow));
    }
    normalSize = !overflow;
    return toDigits(result);
}