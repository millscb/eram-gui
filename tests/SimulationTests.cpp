#include "core/Simulation.h"
#include <cassert>
#include <cmath>
#include <stdexcept>

int main() {
    eram::Simulation simulation;
    assert(simulation.activeChannel() == eram::Channel::A);
    assert(!simulation.processors().empty());
    for (const auto& processor : simulation.processors())
        assert(processor.health == eram::Health::Up);
    assert(simulation.targets().size() == 3);
    simulation.setActiveChannel(eram::Channel::B);
    assert(simulation.activeChannel() == eram::Channel::B);
    bool rejected = false;
    try { simulation.setActiveChannel(eram::Channel::Common); }
    catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    simulation.acknowledgeEvent("EVT-002");
    assert(simulation.events()[1].acknowledged);
    assert(simulation.setAssignedAltitude("AAL1244", 360));
    assert(simulation.targets()[0].assignedAltitude == 360);
    assert(!simulation.setAssignedAltitude("AAL1244", 601));
    assert(!simulation.setAssignedAltitude("UNKNOWN", 350));
    const double oldX = simulation.targets()[0].x;
    simulation.step(60);
    assert(simulation.targets()[0].x > oldX);
    rejected = false;
    try { simulation.step(-1); }
    catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    simulation.reset();
    assert(simulation.activeChannel() == eram::Channel::A);
    assert(simulation.targets()[0].assignedAltitude == 350);
}
