#include <iostream>
#include <fstream>
#include <map>

using namespace std;

struct Book {
    string pages;
    string releaseDate;
    string isbn;
    string publisher;
};

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

string extractNumber(const string& data, size_t startPos) {

    string value;

    while (startPos < data.size() &&
          (data[startPos] == ' ' ||
           data[startPos] == ':'))
    {
        startPos++;
    }

    while (startPos < data.size() &&
           isdigit(data[startPos]))
    {
        value += data[startPos];
        startPos++;
    }

    return value;
}

int main() {

    system("curl \"https://anapioficeandfire.com/api/books\" -o books.json");

    ifstream inFile("books.json");

    if (!inFile) {
        cout << "Unable to open books.json\n";
        return 1;
    }

    string data((istreambuf_iterator<char>(inFile)),
                 istreambuf_iterator<char>());

    inFile.close();

    map<string, Book> books;

    size_t pos = 0;

    while (true) {

        size_t namePos = data.find("\"name\":", pos);

        if (namePos == string::npos)
            break;

        string bookName =
            extractValue(data, namePos + 7);

        size_t pagesPos =
            data.find("\"numberOfPages\":", namePos);

        string pages =
            extractNumber(data, pagesPos + 16);

        size_t releasePos =
            data.find("\"released\":", namePos);

        string released =
            extractValue(data, releasePos + 11);

        size_t isbnPos =
            data.find("\"isbn\":", namePos);

        string isbn =
            extractValue(data, isbnPos + 7);

        size_t publisherPos =
            data.find("\"publisher\":", namePos);

        string publisher =
            extractValue(data, publisherPos + 12);

        Book b;

        b.pages = pages;
        b.releaseDate = released;
        b.isbn = isbn;
        b.publisher = publisher;

        books[bookName] = b;

        pos = publisherPos + 1;
    }

    ofstream csvFile("books_dictionary.csv");

    csvFile
        << "Book Name,Pages,Release Date,ISBN,Publisher\n";

    for (auto& item : books) {

        csvFile
            << "\"" << item.first << "\","
            << "\"" << item.second.pages << "\","
            << "\"" << item.second.releaseDate << "\","
            << "\"" << item.second.isbn << "\","
            << "\"" << item.second.publisher << "\"\n";
    }

    csvFile.close();

    cout << "Total Books Fetched: "
         << books.size() << endl;

    cout << "books_dictionary.csv generated successfully.\n";

    return 0;
}