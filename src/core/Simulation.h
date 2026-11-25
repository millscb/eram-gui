#pragma once
#include <string>
#include <vector>

namespace eram {

enum class Health { Up, Degraded, Down };
enum class Channel { A, B, Common };

struct Processor {
    std::string id;
    std::string name;
    std::string category;
    Channel channel;
    Health health = Health::Up;
};

struct Event {
    std::string id;
    std::string message;
    bool acknowledged = false;
};

struct Target {
    std::string callsign;
    std::string cid;
    int altitude = 0; // hundreds of feet (flight level)
    int assignedAltitude = 0;
    int groundspeed = 0; // knots
    double x = 0.0;      // normalized coordinates [0,1]
    double y = 0.0;
    double heading = 0.0; // true degrees, 0 north, 90 east
};

class Simulation {
public:
    Simulation();
    const std::vector<Processor>& processors() const noexcept { return processors_; }
    const std::vector<Event>& events() const noexcept { return events_; }
    const std::vector<Target>& targets() const noexcept { return targets_; }
    Channel activeChannel() const noexcept { return activeChannel_; }
    void setActiveChannel(Channel channel);
    void acknowledgeEvent(const std::string& id);
    bool setAssignedAltitude(const std::string& callsign, int altitude);
    void step(double seconds);
    void reset();
private:
    Channel activeChannel_ = Channel::A;
    std::vector<Processor> processors_;
    std::vector<Event> events_;
    std::vector<Target> targets_;
};
}
