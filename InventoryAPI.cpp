/**
 * ============================================================
 * QUESTION 1 — Inventory Report API (Date Filter)
 * Endpoint : GET /api/v1/inventory/details?from=YYYY-MM-DD&to=YYYY-MM-DD
 *
 * Stack Used:
 *  - C++ (Backend Logic)
 *  - REST style design
 *  - SQLite (simulated with in-memory data)
 *
 * MODULE BREAKDOWN
 * ------------------------------------------------------------
 * 1. Date              → Handles parsing & comparison
 * 2. Inventory         → Stores inventory base data
 * 3. InventoryDetail   → Stores extra details
 * 4. InventoryRecord   → Combined output (JOIN result)
 * 5. Repository        → Handles data fetching
 * 6. Service           → Applies business rules
 * 7. ApiResponse       → Standard response format
 * 8. Controller        → Entry point for API request
 * ------------------------------------------------------------
 */

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// ─── DATE HELPER STRUCT ──────────────────────────────────────
struct Date {
    int year{}, month{}, day{};

    // Convert string → Date object
    static Date parse(const std::string& s) {
        if (s.size() != 10 || s[4] != '-' || s[7] != '-')
            throw std::invalid_argument("Expected format YYYY-MM-DD, got: " + s);

        Date d;
        d.year  = std::stoi(s.substr(0, 4));
        d.month = std::stoi(s.substr(5, 2));
        d.day   = std::stoi(s.substr(8, 2));

        if (d.month < 1 || d.month > 12 || d.day < 1 || d.day > 31)
            throw std::invalid_argument("Invalid date: " + s);

        return d;
    }

    // Convert date → integer for easy comparison
    int toNumber() const {
        return year * 10000 + month * 100 + day;
    }

    bool operator<=(const Date& other) const {
        return toNumber() <= other.toNumber();
    }

    bool operator>=(const Date& other) const {
        return toNumber() >= other.toNumber();
    }
};

// ─── CORE DATA MODELS ────────────────────────────────────────
struct Inventory {
    int id{};
    std::string purchase_dt;
    double cost{};
};

struct InventoryDetail {
    int id{};
    int inventory_id{};
    std::string inventory_details;
};

// Final structure returned in API
struct InventoryRecord {
    int id{};
    int inventory_id{};
    std::string purchase_dt;
    double cost{};
    std::string details;
};

// ─── DATA ACCESS LAYER ───────────────────────────────────────
class InventoryRepo {
public:
    // Simulates DB JOIN + filtering
    std::vector<InventoryRecord> fetchByDateRange(const Date& from,
                                                   const Date& to) const {

        // Dummy database tables
        static const std::vector<Inventory> inventoryData = {
            {1,"2024-01-15",1500.00}, {2,"2024-03-22",3200.50},
            {3,"2024-06-10", 800.75}, {4,"2024-09-05",4100.00},
            {5,"2025-01-20",2750.25}
        };

        static const std::vector<InventoryDetail> detailData = {
            {1,1,"Industrial pump – batch A"}, {2,2,"Server rack 2U"},
            {3,3,"Office supplies bundle"},    {4,4,"CNC machine components"},
            {5,5,"Networking cables Cat6"}
        };

        std::vector<InventoryRecord> output;

        for (const auto& inv : inventoryData) {
            Date current = Date::parse(inv.purchase_dt);

            if (current >= from && current <= to) {
                for (const auto& det : detailData) {
                    if (det.inventory_id == inv.id) {
                        output.push_back({
                            det.id,
                            inv.id,
                            inv.purchase_dt,
                            inv.cost,
                            det.inventory_details
                        });
                    }
                }
            }
        }

        // Sort result based on purchase date
        std::sort(output.begin(), output.end(),
                  [](const auto& a, const auto& b) {
                      return a.purchase_dt < b.purchase_dt;
                  });

        return output;
    }
};

// ─── BUSINESS LOGIC LAYER ────────────────────────────────────
class InventoryService {
    InventoryRepo repo_;

public:
    std::vector<InventoryRecord> getDetails(const std::string& fromStr,
                                             const std::string& toStr) {

        Date from = Date::parse(fromStr);
        Date to   = Date::parse(toStr);

        if (to.toNumber() < from.toNumber())
            throw std::invalid_argument("from_date cannot be after to_date");

        return repo_.fetchByDateRange(from, to);
    }
};

// ─── GENERIC RESPONSE STRUCT ─────────────────────────────────
template<typename T>
struct ApiResponse {
    int status;
    std::string message;
    T data;

    std::string toJson() const;
};

// JSON for vector response
template<>
std::string ApiResponse<std::vector<InventoryRecord>>::toJson() const {
    std::ostringstream os;

    os << "{\n  \"status\": " << status
       << ",\n  \"message\": \"" << message << "\","
       << "\n  \"data\": [\n";

    for (size_t i = 0; i < data.size(); ++i) {
        const auto& r = data[i];

        os << "    {\"id\":" << r.id
           << ",\"inventory_id\":" << r.inventory_id
           << ",\"purchase_dt\":\"" << r.purchase_dt
           << "\",\"cost\":" << r.cost
           << ",\"details\":\"" << r.details << "\"}";

        if (i != data.size() - 1) os << ",";
        os << "\n";
    }

    os << "  ]\n}";
    return os.str();
}

// JSON for string response
template<>
std::string ApiResponse<std::string>::toJson() const {
    std::ostringstream os;

    os << "{\n  \"status\": " << status
       << ",\n  \"message\": \"" << message << "\","
       << "\n  \"data\": \"" << data << "\"\n}";

    return os.str();
}

// ─── CONTROLLER LAYER ────────────────────────────────────────
class InventoryController {
    InventoryService service_;

public:
    std::string getInventory(const std::string& from,
                              const std::string& to) {
        try {
            auto result = service_.getDetails(from, to);

            if (result.empty()) {
                return ApiResponse<std::string>{
                    404,
                    "No data available for given range",
                    ""
                }.toJson();
            }

            return ApiResponse<std::vector<InventoryRecord>>{
                200,
                "Data fetched successfully",
                result
            }.toJson();

        } catch (const std::invalid_argument& e) {
            return ApiResponse<std::string>{400, e.what(), ""}.toJson();
        } catch (...) {
            return ApiResponse<std::string>{
                500,
                "Something went wrong",
                ""
            }.toJson();
        }
    }
};

// ─── MAIN FUNCTION (Testing) ─────────────────────────────────
int main() {
    InventoryController controller;

    std::cout << "=== Inventory API Testing ===\n\n";

    std::cout << "[CASE 1] Valid range\n";
    std::cout << controller.getInventory("2024-01-01","2024-12-31") << "\n\n";

    std::cout << "[CASE 2] No records\n";
    std::cout << controller.getInventory("2023-01-01","2023-12-31") << "\n\n";

    std::cout << "[CASE 3] Invalid range\n";
    std::cout << controller.getInventory("2024-12-31","2024-01-01") << "\n\n";

    std::cout << "[CASE 4] Wrong format\n";
    std::cout << controller.getInventory("20-Jan-2024","2024-12-31") << "\n";

    return 0;
}