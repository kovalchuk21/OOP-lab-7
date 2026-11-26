#include "filecomparator.h"
#include "ui.h"

#ifdef _WIN32
#include <windows.h>
#endif

std::vector<std::vector<std::string>> summary;

template <typename SizeType>
void Test(const std::string &typeName, const std::string &first, const std::string &second,
          SizeType length, const std::string &resultName)
{
    PrintBox("Testing FileComparator<" + typeName + ">, sizeof(T) = " + std::to_string(sizeof(SizeType)));

    FileComparator<SizeType> full(first, second);
    SizeType all = full.CountDifferences();
    SizeType saved = full.SaveDifferences(resultName);
    FileComparator<SizeType> part(first, second, length);
    SizeType partDiff = part.CountDifferences();
    FileComparator<SizeType> copy(part);
    SizeType copyDiff = copy.CountDifferences();

    PrintTable({"Object", "Compare len", "Differences"},
               {{"full", std::to_string(full.GetCompareLength()) + " B", std::to_string(all)},
                {"part", std::to_string(part.GetCompareLength()) + " B", std::to_string(partDiff)},
                {"copy", std::to_string(copy.GetCompareLength()) + " B", std::to_string(copyDiff)}});
    Done("Positions saved to " + resultName + ": " + std::to_string(saved));

    summary.push_back({typeName, std::to_string(sizeof(SizeType)), std::to_string(all),
                       std::to_string(partDiff), std::to_string(copyDiff)});
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    SaveOutputTo("output.txt");

    std::ifstream fin("input.txt");
    if (!fin) {
        Warn("Cannot open input.txt");
        return 1;
    }

    std::string first, second, resultName;
    long long length;
    fin >> first >> second >> length >> resultName;

    PrintBox("<------- CLASS TEMPLATE FileComparator<T> (VARIANT 10) ------->");

    Test<int>("int", first, second, static_cast<int>(length), "int_" + resultName);
    Test<long long>("long long", first, second, length, "longlong_" + resultName);
    Test<unsigned short>("unsigned short", first, second, static_cast<unsigned short>(length),
                         "ushort_" + resultName);

    PrintBox("Summary for all types");
    PrintTable({"Type T", "sizeof", "Diff (all)", "Diff (" + std::to_string(length) + " B)", "Diff (copy)"}, summary);
    Done("Results saved to output.txt");
    return 0;
}
