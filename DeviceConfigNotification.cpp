/**
 * ============================================================
 * QUESTION 2 — Device Configuration Alert System
 * Endpoint : GET /api/v1/devices/configNotification
 *
 * Tech:
 *  - C++ backend
 *  - REST-style architecture
 *  - In-memory DB (simulating SQLite)
 *  - Observer Pattern for notifications
 *
 * FLOW OVERVIEW
 * ------------------------------------------------------------
 * DeviceRepo → Service → Publisher → Channels → Response
 *
 * COMPONENTS:
 * 1. Device                → Entity model
 * 2. NotificationMessage   → Payload sent to client
 * 3. INotificationChannel  → Abstract interface
 * 4. ConsoleChannel        → Implementation (stdout)
 * 5. Publisher             → Broadcast mechanism
 * 6. Repository            → Data provider
 * 7. Service               → Business flow
 * 8. ApiResponse           → Response wrapper
 * 9. Controller            → API entry point
 * ------------------------------------------------------------
 */

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// ─── HELPER: CURRENT TIME (ISO FORMAT) ───────────────────────
static std::string getCurrentTimeUTC() {
    auto now = std::time(nullptr);
    std::ostringstream os;
    os << std::put_time(std::gmtime(&now), "%Y-%m-%dT%H:%M:%SZ");
    return os.str();
}

// ─── DEVICE ENTITY ───────────────────────────────────────────
struct Device {
    int id{};
    std::string ip;
    std::string details;
    bool isConfigChanged{false};
};

// ─── NOTIFICATION STRUCT ─────────────────────────────────────
struct NotificationMessage {
    std::string type;
    int deviceId{};
    std::string ip;
    std::string details;
    std::string timestamp;

    std::string toJson() const {
        std::ostringstream os;
        os << "{\n"
           << "  \"event_type\": \"" << type << "\",\n"
           << "  \"device_id\": " << deviceId << ",\n"
           << "  \"device_ip\": \"" << ip << "\",\n"
           << "  \"device_details\": \"" << details << "\",\n"
           << "  \"timestamp\": \"" << timestamp << "\"\n"
           << "}";
        return os.str();
    }
};

// ─── CHANNEL INTERFACE ───────────────────────────────────────
class INotificationChannel {
public:
    virtual ~INotificationChannel() = default;
    virtual void notify(const NotificationMessage& msg) = 0;
};

// ─── CONSOLE IMPLEMENTATION ──────────────────────────────────
class ConsoleChannel : public INotificationChannel {
public:
    void notify(const NotificationMessage& msg) override {
        std::cout << "[ALERT GENERATED]\n"
                  << msg.toJson() << "\n";
    }
};

// ─── PUBLISHER (OBSERVER CORE) ───────────────────────────────
class Publisher {
    std::vector<std::shared_ptr<INotificationChannel>> subscribers;

public:
    void registerChannel(std::shared_ptr<INotificationChannel> ch) {
        subscribers.push_back(std::move(ch));
    }

    void broadcast(const Device& dev) {
        NotificationMessage msg;
        msg.type      = "CONFIG_CHANGE";
        msg.deviceId  = dev.id;
        msg.ip        = dev.ip;
        msg.details   = dev.details;
        msg.timestamp = getCurrentTimeUTC();

        for (auto& sub : subscribers)
            sub->notify(msg);
    }
};

// ─── DATA ACCESS ─────────────────────────────────────────────
class DeviceRepository {
    std::vector<Device> devices = {
        {1, "192.168.1.10", "Router-CoreA", false},
        {2, "192.168.1.20", "Switch-FloorB", true},
        {3, "192.168.1.30", "Firewall-EdgeC", false},
        {4, "10.0.0.5",     "AP-Lobby", true},
    };

public:
    std::vector<Device> getChangedDevices() const {
        std::vector<Device> res;
        for (const auto& d : devices)
            if (d.isConfigChanged)
                res.push_back(d);
        return res;
    }

    void clearFlag(int id) {
        for (auto& d : devices)
            if (d.id == id) {
                d.isConfigChanged = false;
                break;
            }
    }
};

// ─── SERVICE LAYER ───────────────────────────────────────────
class DeviceService {
    DeviceRepository repo;
    Publisher publisher;

public:
    DeviceService() {
        publisher.registerChannel(
            std::make_shared<ConsoleChannel>());
    }

    int processNotifications() {
        auto list = repo.getChangedDevices();
        int total = 0;

        for (const auto& d : list) {
            publisher.broadcast(d);
            repo.clearFlag(d.id);
            total++;
        }
        return total;
    }
};

// ─── API RESPONSE WRAPPER ────────────────────────────────────
template<typename T>
struct ApiResponse {
    int status;
    std::string message;
    T data;

    std::string toJson() const;
};

template<>
std::string ApiResponse<std::string>::toJson() const {
    std::ostringstream os;
    os << "{\n  \"status\": " << status
       << ",\n  \"message\": \"" << message << "\","
       << "\n  \"data\": \"" << data << "\"\n}";
    return os.str();
}

// ─── CONTROLLER ──────────────────────────────────────────────
class DeviceController {
    DeviceService service;

public:
    std::string getConfigNotifications() {
        try {
            int count = service.processNotifications();

            std::string msg = (count == 0)
                ? "No config updates found."
                : std::to_string(count) + " alert(s) sent.";

            return ApiResponse<std::string>{200, msg, ""}.toJson();

        } catch (...) {
            return ApiResponse<std::string>{
                500, "Server error occurred.", ""
            }.toJson();
        }
    }
};

// ─── MAIN DRIVER ─────────────────────────────────────────────
int main() {
    DeviceController controller;

    std::cout << "=== Device Notification API ===\n\n";

    std::string res = controller.getConfigNotifications();
    std::cout << "\n[RESPONSE]\n" << res << "\n\n";

    std::cout << "[Second Attempt — No Changes Expected]\n";
    res = controller.getConfigNotifications();
    std::cout << "\n[RESPONSE]\n" << res << "\n";

    return 0;
}