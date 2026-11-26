#ifndef FILECOMPARATOR_H
#define FILECOMPARATOR_H

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

template <typename SizeType>
class FileComparator
{
public:
    FileComparator();
    FileComparator(const std::string &firstName, const std::string &secondName);
    FileComparator(const std::string &firstName, const std::string &secondName, SizeType length);
    FileComparator(const FileComparator &other);
    ~FileComparator();

    bool IsOpen() const { return file1 != nullptr && file2 != nullptr; }
    SizeType CountDifferences();
    SizeType SaveDifferences(const std::string &outName);

    std::string GetName1() const { return name1; }
    std::string GetName2() const { return name2; }
    SizeType GetLength1() const { return length1; }
    SizeType GetLength2() const { return length2; }
    SizeType GetCompareLength() const;

private:
    std::string name1;
    std::string name2;
    SizeType length1;
    SizeType length2;
    SizeType compareLength;
    FILE *file1;
    FILE *file2;

    void OpenFiles();
    void CloseFiles();
    static SizeType FileLength(FILE *file);
};

template <typename SizeType>
FileComparator<SizeType>::FileComparator()
    : length1(0), length2(0), compareLength(0), file1(nullptr), file2(nullptr)
{
    std::cout << "    [default constructor]" << std::endl;
}

template <typename SizeType>
FileComparator<SizeType>::FileComparator(const std::string &firstName, const std::string &secondName)
    : name1(firstName), name2(secondName), length1(0), length2(0), compareLength(0),
      file1(nullptr), file2(nullptr)
{
    OpenFiles();
    std::cout << "    [constructor with 2 args]" << std::endl;
}

template <typename SizeType>
FileComparator<SizeType>::FileComparator(const std::string &firstName, const std::string &secondName,
                                         SizeType length)
    : name1(firstName), name2(secondName), length1(0), length2(0), compareLength(length),
      file1(nullptr), file2(nullptr)
{
    OpenFiles();
    std::cout << "    [constructor with 3 args]" << std::endl;
}

template <typename SizeType>
FileComparator<SizeType>::FileComparator(const FileComparator &other)
    : name1(other.name1), name2(other.name2), length1(0), length2(0),
      compareLength(other.compareLength), file1(nullptr), file2(nullptr)
{
    OpenFiles();
    std::cout << "    [copy constructor]" << std::endl;
}

template <typename SizeType>
FileComparator<SizeType>::~FileComparator()
{
    CloseFiles();
    std::cout << "    [destructor: files closed]" << std::endl;
}

template <typename SizeType>
void FileComparator<SizeType>::OpenFiles()
{
    file1 = std::fopen(name1.c_str(), "rb");
    file2 = std::fopen(name2.c_str(), "rb");
    length1 = file1 ? FileLength(file1) : 0;
    length2 = file2 ? FileLength(file2) : 0;
}

template <typename SizeType>
void FileComparator<SizeType>::CloseFiles()
{
    if (file1) std::fclose(file1);
    if (file2) std::fclose(file2);
    file1 = nullptr;
    file2 = nullptr;
}

template <typename SizeType>
SizeType FileComparator<SizeType>::FileLength(FILE *file)
{
    std::fseek(file, 0, SEEK_END);
    SizeType size = static_cast<SizeType>(std::ftell(file));
    std::fseek(file, 0, SEEK_SET);
    return size;
}

template <typename SizeType>
SizeType FileComparator<SizeType>::GetCompareLength() const
{
    SizeType common = std::min(length1, length2);
    if (compareLength > 0)
        return std::min(compareLength, common);
    return common;
}

template <typename SizeType>
SizeType FileComparator<SizeType>::CountDifferences()
{
    if (!IsOpen()) return 0;

    std::fseek(file1, 0, SEEK_SET);
    std::fseek(file2, 0, SEEK_SET);

    SizeType count = 0;
    SizeType limit = GetCompareLength();
    for (SizeType i = 0; i < limit; i++) {
        if (std::fgetc(file1) != std::fgetc(file2))
            count++;
    }
    return count;
}

template <typename SizeType>
SizeType FileComparator<SizeType>::SaveDifferences(const std::string &outName)
{
    if (!IsOpen()) return 0;

    std::ofstream out(outName);
    if (!out) return 0;

    std::fseek(file1, 0, SEEK_SET);
    std::fseek(file2, 0, SEEK_SET);

    SizeType count = 0;
    SizeType limit = GetCompareLength();
    for (SizeType i = 0; i < limit; i++) {
        if (std::fgetc(file1) != std::fgetc(file2)) {
            out << i + 1 << "\n";
            count++;
        }
    }
    return count;
}

#endif
