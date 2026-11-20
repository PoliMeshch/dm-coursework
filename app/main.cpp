#include "BigCalculator.h"

#include <iostream>
#include <string>
#include <vector>

static bool isValidNumber(const std::string& s) {
    std::string digits = s;
    if (!digits.empty() && digits.front() == '-') digits.erase(0, 1);
    if (digits.empty() || digits.size() > 8) return false;
    for (std::size_t i = 0; i < digits.size(); ++i) {
        if (digits[i] < 'a' || digits[i] > 'k') return false;
    }
    return true;
}

static std::string evaluate(BigCalculator& calc, const std::string& op,
    const std::vector<char>& a, const std::vector<char>& b) {
    if (op == "/") return calc.division(a, b);
    if (op == "^") return calc.pow(a, b);
    std::vector<char> result;
    if (op == "+") result = calc.add(a, b);
    else if (op == "-") result = calc.subtract(a, b);
    else if (op == "*") result = calc.multiply(a, b);
    else if (op == "gcd") result = calc.gcd(a, b);
    else if (op == "lcm") result = calc.lcm(a, b);
    else return "error";
    if (!calc.normalSize) return "overflow";
    return std::string(result.begin(), result.end());
}

int main() {
    BigCalculator calc;
    std::string left;
    std::string op;
    std::string right;
    std::cout << "Format: number op number, ops: + - * / ^ gcd lcm. Exit: Ctrl+Z then Enter\n";
    while (std::cin >> left >> op >> right) {
        if (!isValidNumber(left) || !isValidNumber(right)) {
            std::cout << "error\n";
            continue;
        }
        std::vector<char> a(left.begin(), left.end());
        std::vector<char> b(right.begin(), right.end());
        std::cout << left << ' ' << op << ' ' << right << " = " << evaluate(calc, op, a, b) << '\n';
    }
    return 0;
}