#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

struct Character {
    string name;
    int appearances;
};

bool compareCharacters(const Character& a,
                       const Character& b)
{
    return a.appearances > b.appearances;
}

string extractValue(const string& data,
                    size_t startPos)
{
    size_t firstQuote =
        data.find("\"", startPos);

    if (firstQuote == string::npos)
        return "";

    size_t secondQuote =
        data.find("\"", firstQuote + 1);

    if (secondQuote == string::npos)
        return "";

    return data.substr(firstQuote + 1,
                       secondQuote - firstQuote - 1);
}

int countSeasons(const string& tvSeriesBlock) {

    int count = 0;

    size_t pos = 0;

    while (true) {

        size_t seasonPos =
            tvSeriesBlock.find("Season", pos);

        if (seasonPos == string::npos)
            break;

        count++;
        pos = seasonPos + 1;
    }

    return count;
}

int main() {

    system("curl \"https://anapioficeandfire.com/api/characters?page=1&pageSize=500\" -o characters.json");

    ifstream inFile("characters.json");

    if (!inFile) {
        cout << "Unable to open characters.json\n";
        return 1;
    }

    string data((istreambuf_iterator<char>(inFile)),
                 istreambuf_iterator<char>());

    inFile.close();

    vector<Character> characters;

    size_t pos = 0;

    while (true) {

        size_t namePos =
            data.find("\"name\":", pos);

        if (namePos == string::npos)
            break;

        string name =
            extractValue(data, namePos + 7);

        size_t tvSeriesPos =
            data.find("\"tvSeries\":", namePos);

        if (tvSeriesPos == string::npos)
            break;

        size_t blockEnd =
            data.find("]", tvSeriesPos);

        string tvSeriesBlock =
            data.substr(tvSeriesPos,
                        blockEnd - tvSeriesPos);

        int appearances =
            countSeasons(tvSeriesBlock);

        if (!name.empty()) {
            characters.push_back(
                {name, appearances});
        }

        pos = blockEnd + 1;
    }

    sort(characters.begin(),
         characters.end(),
         compareCharacters);

    ofstream excelFile("characters_sorted.csv");

    excelFile
        << "Character Name,Season Appearances\n";

    for (const auto& c : characters) {

        excelFile
            << "\"" << c.name << "\","
            << c.appearances << "\n";
    }

    excelFile.close();

    cout << "Total Characters Fetched: "
         << characters.size() << endl;

    cout << "characters_sorted.csv generated successfully.\n";

    return 0;
}