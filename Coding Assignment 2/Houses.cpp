#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

struct House {
    string name;
    string region;
};

bool compareHouse(const House& a, const House& b) {
    return a.name < b.name;
}

string extractValue(const string& data, size_t startPos) {

    size_t firstQuote = data.find("\"", startPos);

    if (firstQuote == string::npos)
        return "";

    size_t secondQuote = data.find("\"", firstQuote + 1);

    if (secondQuote == string::npos)
        return "";

    return data.substr(firstQuote + 1,
                       secondQuote - firstQuote - 1);
}

int main() {

    system("curl \"https://anapioficeandfire.com/api/houses?page=1&pageSize=500\" -o houses.json");

    ifstream inFile("houses.json");

    if (!inFile) {
        cout << "Unable to open JSON file.\n";
        return 1;
    }

    string data((istreambuf_iterator<char>(inFile)),
                 istreambuf_iterator<char>());

    inFile.close();

    vector<House> houses;

    size_t pos = 0;

    while (true) {

        size_t namePos = data.find("\"name\":", pos);

        if (namePos == string::npos)
            break;

        string name = extractValue(data, namePos + 7);

        size_t regionPos = data.find("\"region\":", namePos);

        string region = "";

        if (regionPos != string::npos) {
            region = extractValue(data, regionPos + 9);
        }

        if (!name.empty()) {
            houses.push_back({name, region});
        }

        pos = namePos + 1;
    }

    sort(houses.begin(), houses.end(), compareHouse);

    ofstream outFile("houses_and_regions.txt");

    outFile << "===== Houses of Ice and Fire =====\n\n";

    for (const auto& h : houses) {

        outFile << "House: " << h.name << "\n";
        outFile << "Region: " << h.region << "\n";
        outFile << "-----------------------------\n";
    }

    outFile.close();

    cout << "Total Houses Fetched: "
         << houses.size() << endl;

    cout << "houses_and_regions.txt generated successfully.\n";

    return 0;
}