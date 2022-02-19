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

struct PoweredOff
    : fsmpp2::state<>
    , fsmpp2::access_context<OvenInterface>
{
    auto handle(PowerButton) {
        std::cout << "PoweredOff::PowerButton" << std::endl;
        get_context().On();
        return transition<PoweredOn>();
    }

    auto handle(LockButton) {
        std::cout << "PoweredOff::LockButton" << std::endl;
        return transition<Locked>();
    }
};

struct PoweredOn
    : fsmpp2::state<>
    , fsmpp2::access_context<OvenInterface, State>
{
    PoweredOn(OvenInterface &oi) {
        std::cout << "PoweredOn::enter" << std::endl;
        oi.SetPower(0);
    }

    auto handle(PowerIncButton) {
        std::cout << "PoweredOn::PowerIncButton" << std::endl;
        get_context<State>().lastPowerSetting ++;
        return handled();
    }

    auto handle(PowerDecButton) {
        std::cout << "PoweredOn::PowerDecButton" << std::endl;
        get_context<State>().lastPowerSetting --;
        return handled();
    }

    auto handle(LockButton) {
        std::cout << "PoweredOn::LockButton" << std::endl;
        return transition<Locked>();
    }
};

struct Locked
    : fsmpp2::state<>
{
    auto handle(LockButton, fsmpp2::access_context<State> c) -> fsmpp2::transitions<PoweredOff, PoweredOn> {
        c.get_context().lastPowerSetting = 42;
        //state.lastPowerSetting = 42;
        /*if (state.lastPowerSetting > 0) {
            std::cout << "Locked -> PoweredOn" << std::endl;
            return transition<PoweredOn>();
        } else {
            std::cout << "Locked -> PoweredOff" << std::endl;
            return transition<PoweredOff>();
        }*/
        return handled();
    }
};

TEST_CASE("asd", "[asd]")
{
    using events = fsmpp2::events<PowerButton, PowerIncButton, PowerDecButton, LockButton>;
    using states = fsmpp2::states<Locked>;
    using contexts = fsmpp2::contexts<State, OvenInterface>;

    State s;
    OvenInterface oi;

    fsmpp2::state_machine<states, events, contexts> sm { contexts {s, oi} };

    sm.dispatch(LockButton{});
    CHECK(s.lastPowerSetting == 42);
}