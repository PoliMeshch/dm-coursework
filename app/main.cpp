#include "BigCalculator.h"

#include <cctype>
#include <iostream>
#include <string>
#include <vector>

static const int WIDTH = 50;

static const std::string MENU[] = {
    "1. Addition",
    "2. Subtraction",
    "3. Multiplication",
    "4. Division",
    "5. Exponentiation",
    "6. GCD",
    "7. LCM",
    "0. Exit",
};

static std::string trim(const std::string& s) {
    std::string result = s;
    while (!result.empty() && std::isspace(static_cast<unsigned char>(result.back()))) {
        result.pop_back();
    }
    while (!result.empty() && std::isspace(static_cast<unsigned char>(result.front()))) {
        result.erase(result.begin());
    }
    return result;
}

static std::vector<char> toChars(const std::string& s) {
    return std::vector<char>(s.begin(), s.end());
}

static std::string toText(const std::vector<char>& v) {
    return std::string(v.begin(), v.end());
}

static bool isValidNumber(const std::string& s) {
    std::string digits = s;
    if (!digits.empty() && digits.front() == '-') digits.erase(digits.begin());
    if (digits.empty()) return false;
    for (char c : digits) {
        if (c < 'a' || c > 'k') return false;
    }
    return true;
}

static void printBorder() {
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+\n";
}

static void printRow(const std::string& text) {
    int spaces = WIDTH - 4 - static_cast<int>(text.size());
    std::cout << "| " << text << std::string(spaces, ' ') << " |\n";
}

static void printMenu() {
    printBorder();
    printRow("Calculator <Z11; +, *>");
    printRow("Numbers up to 8 symbols, alphabet a..k");
    printBorder();
    printRow("Symbol order: a<b<e<h<g<k<i<d<c<f<j");
    printBorder();
    for (const std::string& item : MENU) printRow(item);
    printBorder();
    std::cout << "Choice: ";
}

static std::string operationSymbol(const std::string& choice) {
    if (choice == "1") return "+";
    if (choice == "2") return "-";
    if (choice == "3") return "*";
    if (choice == "4") return "/";
    if (choice == "5") return "^";
    if (choice == "6") return "gcd";
    if (choice == "7") return "lcm";
    return "";
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
    else result = calc.lcm(a, b);
    if (!calc.normalSize) return "overflow";
    return toText(result);
}

int main() {
    BigCalculator calc;
    std::string line;
    while (true) {
        printMenu();
        if (!std::getline(std::cin, line)) break;
        std::string choice = trim(line);
        if (choice == "0") break;

        std::string op = operationSymbol(choice);
        if (op.empty()) {
            std::cout << "error\n";
            continue;
        }

        std::cout << "Enter A: ";
        if (!std::getline(std::cin, line)) break;
        std::string left = trim(line);
        std::cout << "Enter B: ";
        if (!std::getline(std::cin, line)) break;
        std::string right = trim(line);

        if (!isValidNumber(left) || !isValidNumber(right)) {
            std::cout << left << " " << op << " " << right << " = error\n";
            continue;
        }
        std::cout << left << " " << op << " " << right << " = "
            << evaluate(calc, op, toChars(left), toChars(right)) << "\n";
    }
    return 0;
}