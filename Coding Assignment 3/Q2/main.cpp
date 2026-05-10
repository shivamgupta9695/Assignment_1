#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// ==========================================
// Inventory Class
// ==========================================

class Inventory {
private:
    string ip;
    string os;
    int memory;
    double cpu;
    string disk;

public:

    Inventory(string ip, string os,
              int memory, double cpu,
              string disk) {

        this->ip = ip;
        this->os = os;
        this->memory = memory;
        this->cpu = cpu;
        this->disk = disk;
    }

    int getMemory() const {
        return memory;
    }

    double getCPU() const {
        return cpu;
    }

    string getOS() const {
        return os;
    }

    void display() const {

        cout << "IP      : " << ip << endl;
        cout << "OS      : " << os << endl;
        cout << "Memory  : " << memory << " GB" << endl;
        cout << "CPU     : " << cpu << " Ghz" << endl;
        cout << "Disk    : " << disk << endl;

        cout << "-----------------------------"
             << endl;
    }
};

// ==========================================
// Inventory Manager Class
// ==========================================

class InventoryManager {

private:

    vector<Inventory> systems;

    // Extract value from JSON line
    string extractValue(string line) {

        int firstQuote =
            line.find(":");

        string value =
            line.substr(firstQuote + 1);

        // Remove spaces
        value.erase(remove(value.begin(),
                           value.end(), ' '),
                           value.end());

        // Remove quotes
        value.erase(remove(value.begin(),
                           value.end(), '"'),
                           value.end());

        // Remove comma
        value.erase(remove(value.begin(),
                           value.end(), ','),
                           value.end());

        return value;
    }

public:

    void loadData(string filename) {

        ifstream file(filename);

        if (!file) {
            throw runtime_error(
                "Unable to open JSON file");
        }

        string line;

        string ip, os, memoryStr,
               cpuStr, disk;

        while (getline(file, line)) {

            if (line.find("\"ip\"") !=
                string::npos) {

                ip = extractValue(line);
            }

            else if (line.find("\"os\"")
                     != string::npos) {

                os = extractValue(line);
            }

            else if (line.find("\"memory\"")
                     != string::npos) {

                memoryStr =
                    extractValue(line);
            }

            else if (line.find("\"cpu\"")
                     != string::npos) {

                cpuStr =
                    extractValue(line);
            }

            else if (line.find("\"disk\"")
                     != string::npos) {

                disk = extractValue(line);

                // Convert Memory
                int memory =
                    stoi(memoryStr.substr(
                        0,
                        memoryStr.find("GB")));

                // Convert CPU
                double cpu =
                    stod(cpuStr.substr(
                        0,
                        cpuStr.find("Ghz")));

                // Add object
                systems.push_back(
                    Inventory(
                        ip,
                        os,
                        memory,
                        cpu,
                        disk
                    )
                );
            }
        }

        file.close();
    }

    void filterData(string criteria) {

        if (criteria.empty()) {

            throw invalid_argument(
                "Filter criteria missing");
        }

        // Convert lowercase
        transform(criteria.begin(),
                  criteria.end(),
                  criteria.begin(),
                  ::tolower);

        // ==========================
        // MAX MEMORY
        // ==========================

        if (criteria == "memory") {

            auto maxSystem =
                max_element(
                    systems.begin(),
                    systems.end(),

                [](const Inventory& a,
                   const Inventory& b) {

                    return a.getMemory()
                         < b.getMemory();
                });

            cout << "\n===== SYSTEM WITH "
                 << "MAX MEMORY =====\n";

            maxSystem->display();
        }

        // ==========================
        // MAX CPU
        // ==========================

        else if (criteria == "cpu") {

            auto maxSystem =
                max_element(
                    systems.begin(),
                    systems.end(),

                [](const Inventory& a,
                   const Inventory& b) {

                    return a.getCPU()
                         < b.getCPU();
                });

            cout << "\n===== SYSTEM WITH "
                 << "MAX CPU =====\n";

            maxSystem->display();
        }

        // ==========================
        // WINDOWS
        // ==========================

        else if (criteria == "windows") {

            cout << "\n===== WINDOWS "
                 << "SYSTEMS =====\n";

            for (const auto& system
                 : systems) {

                if (system.getOS()
                    == "Windows") {

                    system.display();
                }
            }
        }

        // ==========================
        // LINUX
        // ==========================

        else if (criteria == "linux") {

            cout << "\n===== LINUX "
                 << "SYSTEMS =====\n";

            for (const auto& system
                 : systems) {

                if (system.getOS()
                    == "Linux") {

                    system.display();
                }
            }
        }

        else {

            throw invalid_argument(
                "Invalid filter criteria");
        }
    }
};

// ==========================================
// MAIN FUNCTION
// ==========================================

int main() {

    try {

        InventoryManager manager;

        manager.loadData(
            "inventory.json");

        string criteria;

        cout << "Enter Filter Criteria "
             << "(Memory/CPU/Linux/"
             << "Windows): ";

        cin >> criteria;

        manager.filterData(criteria);
    }

    catch (exception& e) {

        cout << "\nException : "
             << e.what() << endl;
    }

    return 0;
}