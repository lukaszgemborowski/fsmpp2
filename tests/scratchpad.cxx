#include "catch.hpp"
#include "fsmpp2/state_machine.hpp"
#include <iostream>

// Try something here...
// HW interface to the oven
class OvenInterface {
public:
    void On() {}
    void Off() {}
    void SetPower(int) {}
};

// Some local data
struct State {
    int lastPowerSetting = 0;
};

struct PowerButton : fsmpp2::event {};
struct PowerIncButton : fsmpp2::event {};
struct PowerDecButton : fsmpp2::event {};
struct LockButton : fsmpp2::event {};

struct PoweredOff;
struct PoweredOn;
struct Locked;


TEST_CASE("asd", "[asd]")
{
}