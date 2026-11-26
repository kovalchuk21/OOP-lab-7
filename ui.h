#ifndef UI_H
#define UI_H

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class TeeBuf : public std::streambuf
{
public:
    TeeBuf(std::streambuf *first, std::streambuf *second) : a(first), b(second) {}

protected:
    int overflow(int c) override
    {
        if (c == EOF) return 0;
        if (a->sputc(c) == EOF || b->sputc(c) == EOF) return EOF;
        return c;
    }
    int sync() override
    {
        return (a->pubsync() == 0 && b->pubsync() == 0) ? 0 : -1;
    }

private:
    std::streambuf *a;
    std::streambuf *b;
};

struct RestoreCout
{
    std::streambuf *old;
    ~RestoreCout()
    {
        std::cout.flush();
        std::cout.rdbuf(old);
    }
};

inline void SaveOutputTo(const std::string &fileName)
{
    static std::ofstream file(fileName);
    static TeeBuf tee(std::cout.rdbuf(), file.rdbuf());
    static RestoreCout restore{std::cout.rdbuf(&tee)};
}

inline std::string Center(const std::string &text, int width)
{
    int spaces = width - static_cast<int>(text.size());
    if (spaces < 0) spaces = 0;
    int left = spaces / 2;
    return std::string(left, ' ') + text + std::string(spaces - left, ' ');
}

inline std::string Num(double value, int digits = 2)
{
    std::ostringstream s;
    s << std::fixed << std::setprecision(digits) << value;
    return s.str();
}

inline void PrintBox(const std::string &title, int width = 78)
{
    std::cout << "\n" << std::string(width, '-') << "\n";
    std::cout << "|" << Center(title, width - 2) << "|\n";
    std::cout << std::string(width, '-') << "\n";
}

inline void PrintStatus(const std::vector<std::pair<std::string, std::string>> &rows)
{
    int keyWidth = 15;
    int valueWidth = 17;
    for (const auto &row : rows) {
        keyWidth = std::max(keyWidth, static_cast<int>(row.first.size()) + 4);
        valueWidth = std::max(valueWidth, static_cast<int>(row.second.size()) + 4);
    }
    std::string border(keyWidth + valueWidth + 3, '-');
    std::cout << "\n" << border << "\n";
    for (const auto &row : rows)
        std::cout << "|" << Center(row.first, keyWidth) << "|" << Center(row.second, valueWidth) << "|\n";
    std::cout << border << "\n";
}

inline void PrintTable(const std::vector<std::string> &head, const std::vector<std::vector<std::string>> &rows)
{
    std::vector<int> widths;
    for (const std::string &cell : head)
        widths.push_back(static_cast<int>(cell.size()) + 2);
    for (const auto &row : rows)
        for (size_t i = 0; i < row.size() && i < widths.size(); i++)
            widths[i] = std::max(widths[i], static_cast<int>(row[i].size()) + 2);

    std::string line = "+";
    for (int w : widths)
        line += std::string(w, '-') + "+";

    std::cout << "\n" << line << "\n|";
    for (size_t i = 0; i < head.size(); i++)
        std::cout << Center(head[i], widths[i]) << "|";
    std::cout << "\n" << line << "\n";
    for (const auto &row : rows) {
        std::cout << "|";
        for (size_t i = 0; i < widths.size(); i++)
            std::cout << Center(i < row.size() ? row[i] : "", widths[i]) << "|";
        std::cout << "\n";
    }
    std::cout << line << "\n";
}

inline void Done(const std::string &text = "Done")
{
    std::cout << ">> " << text << "\n";
}

inline void Warn(const std::string &text)
{
    std::cout << "[!] " << text << "\n";
}

inline void WaitEnter()
{
    std::cout << "\nPress ENTER to continue...";
    std::cin.ignore(1000, '\n');
    std::cin.get();
}

inline bool ReadValue(const std::string &prompt, double &value)
{
    std::cout << prompt;
    if (std::cin >> value)
        return true;
    std::cin.clear();
    std::cin.ignore(1000, '\n');
    Warn("Invalid input! Please enter numbers only.");
    return false;
}

#endif
