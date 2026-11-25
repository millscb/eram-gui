#include "core/Simulation.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace eram {
Simulation::Simulation() { reset(); }

void Simulation::reset() {
    activeChannel_ = Channel::A;
    processors_ = {
        {"SDR1", "SDRP 1", "Equipment Room", Channel::A},
        {"SDR2", "SDRP 2", "Equipment Room", Channel::B},
        {"FDP1", "FDP 1", "Equipment Room", Channel::A},
        {"FDP2", "FDP 2", "Equipment Room", Channel::B},
        {"CPR1", "CPRP 1", "Equipment Room", Channel::A},
        {"CPR2", "CPRP 2", "Equipment Room", Channel::B},
        {"MCR1", "MCRP 1", "Equipment Room", Channel::A},
        {"MCR2", "MCRP 2", "Equipment Room", Channel::B},
        {"ECG1", "ECGIP 1", "Equipment Room", Channel::Common},
        {"101R", "Sector 01 Radar", "Control Room", Channel::Common},
        {"101D", "Sector 01 Data", "Control Room", Channel::Common},
        {"102R", "Sector 02 Radar", "Control Room", Channel::Common},
        {"102D", "Sector 02 Data", "Control Room", Channel::Common},
        {"401M", "OMCWS 01", "SOC", Channel::A},
        {"402M", "OMCWS 02", "SOC", Channel::B},
    };
    events_ = {
        {"EVT-001", "CHANNEL A SYNCHRONIZATION VERIFIED", true},
        {"EVT-002", "SIMULATED SURVEILLANCE FEEDS ONLINE", false},
        {"EVT-003", "SIMULATION INITIALIZED - TRAINING ONLY", false}
    };
    targets_ = {
        {"AAL1244", "042", 334, 350, 480, .42, .38, 85},
        {"DAL892", "115", 370, 370, 465, .65, .55, 260},
        {"UAL405", "078", 280, 240, 390, .31, .72, 140}
    };
}

void Simulation::setActiveChannel(Channel channel) {
    if (channel == Channel::Common) throw std::invalid_argument("Common is not an active channel");
    activeChannel_ = channel;
}

void Simulation::acknowledgeEvent(const std::string& id) {
    for (auto& event : events_) if (event.id == id) event.acknowledged = true;
}

bool Simulation::setAssignedAltitude(const std::string& callsign, int altitude) {
    if (altitude < 0 || altitude > 600) return false;
    for (auto& target : targets_) {
        if (target.callsign == callsign) {
            target.assignedAltitude = altitude;
            return true;
        }
    }
    return false;
}

void Simulation::step(double seconds) {
    if (!std::isfinite(seconds) || seconds < 0.0) throw std::invalid_argument("Step must be nonnegative and finite");
    // Training-only simplified kinematics: 1.0 normalized display unit = 600 NM.
    // No real-world airspace, terrain, or operational flight dynamics implied.
    constexpr double displayWidthNm = 600.0;
    constexpr double radians = 3.14159265358979323846 / 180.0;
    for (auto& target : targets_) {
        const double distance = target.groundspeed * seconds / 3600.0 / displayWidthNm;
        target.x += std::sin(target.heading * radians) * distance;
        target.y -= std::cos(target.heading * radians) * distance;
        target.x = std::clamp(target.x, 0.0, 1.0);
        target.y = std::clamp(target.y, 0.0, 1.0);
    }
}
}
