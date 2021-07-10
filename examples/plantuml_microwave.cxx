#include <iostream>
#include <chrono>
#include <fstream>
#include "fsmpp2/plantuml.hpp"
#include "fsmpp2/state_machine.hpp"

namespace sm
{

// Context data shared among all states
struct ContextData {
    void setTimeout(std::chrono::seconds time) {
        timeout = time;
    }

    void decreaseTimer(std::chrono::seconds delta) {
        timeout -= delta;
    }

    void clearTimer() {
        timeout = std::chrono::seconds::zero();
    }

    bool isTimerSet() const {
        return timeout != std::chrono::seconds::zero();
    }

    bool timerExpired() const {
        return timeout <= std::chrono::seconds{0};
    }

private:
    std::chrono::seconds timeout = std::chrono::seconds::zero();
};

// Event definitions, each event needs to inherit from fsmpp2::event base class
struct PowerButtonPressed : fsmpp2::event {};
struct DoorOpen : fsmpp2::event {};
struct DoorClose : fsmpp2::event {};
struct StartStopButtonPressed : fsmpp2::event {};
struct SetTimer : fsmpp2::event {
    explicit SetTimer(std::chrono::seconds t)
        : timer{t}
    {}

    std::chrono::seconds timer;
};
struct TimeElapsed : fsmpp2::event {
    explicit TimeElapsed(std::chrono::seconds d)
        : delta {d}
    {}

    std::chrono::seconds delta;
};

// states
struct PoweredOn;
struct PoweredOff;
struct Opened;
struct Microwaving;
struct Ready;

// Each event needs to to inherit from fsmpp2::state template
struct PoweredOff : fsmpp2::state<> {
    // Constructors are "state enter" indicators, when state machine transits
    // from one state to another it destructs the old state and creates new one.
    // Note: There's no dynamic allocation here as required space is statically
    // allocated when state machine is declared.
    PoweredOff() {
        std::cout << "PoweredOff state enter" << std::endl;
    }

    ~PoweredOff() {
        std::cout << "PoweredOff state exit" << std::endl;
    }

    // Event handler, one of the simplest forms, when PowerButtonPressed is passed to
    // state machine and the state machine is in PoweredOff this handler will be called.
    // For simplicity it is defined with auto return type which in this case is simply
    // deduced from a single return statement.
    auto handle(PowerButtonPressed const&) const {
        std::cout << "PoweredOff: PowerButtonPressed event" << std::endl;

        // indicate that we want to transit to PoweredOn state:
        // PoweredOff --- PowerButtonPressed ---> PoweredOn
        return transition<PoweredOn>();
    }
};

struct Opened : fsmpp2::state<> {
    Opened() {
        std::cout << "Opened state enter" << std::endl;
    }

    ~Opened() {
        std::cout << "Opened state exit" << std::endl;
    }

    // door closed, get back to ready state
    auto handle(DoorClose const&) const {
        std::cout << "Opened: DoorClose event" << std::endl;
        return transition<Ready>();
    }
};

struct Paused : fsmpp2::state<> {
    Paused() {
        std::cout << "Paused state enter" << std::endl;
    }

    ~Paused() {
        std::cout << "Paused state exit" << std::endl;
    }

    // door closed, continue microwaving
    auto handle(DoorClose const&) const {
        std::cout << "Paused: DoorClose event" << std::endl;
        return transition<Microwaving>();
    }
};

struct Microwaving : fsmpp2::state<> {
    // if the constructor accepts a single argument of Context type reference,
    // state machine will automatically provide a reference to the current Context
    Microwaving(ContextData &ctx)
        : ctx_ {ctx}
    {
        std::cout << "Microwaving state enter" << std::endl;
    }

    ~Microwaving()
    {
        std::cout << "Microwaving state exit" << std::endl;
    }


    // user canceled
    auto handle(StartStopButtonPressed const&) const {
        std::cout << "Microwaving: StartStopButtonPressed event" << std::endl;
        return transition<Ready>();
    }

    // door opened during microwaving, just pause
    auto handle(DoorOpen const&) const {
        std::cout << "Microwaving: DoorOpen event" << std::endl;
        return transition<Paused>();
    }

    auto handle(TimeElapsed const& e) -> fsmpp2::transitions<Ready> {
                // in this case we must explicitly mark that we can transit to
                // another state as the auto deduction will be ambiguous as we
                // use here two different return paths with different return types,
                // for consistency this notation could be used for every other handler
                // but we don't do that in this example
        std::cout << "Microwaving: TimeElapsed event" << std::endl;
        ctx_.decreaseTimer(e.delta);

        if (ctx_.timerExpired()) {
            // timer has expired, move back to ready state
            ctx_.clearTimer();
            return transition<Ready>();
        } else {
            // still microwaving...
            return handled();
        }
    }

    ContextData& ctx_;
};

struct Ready : fsmpp2::state<> {
    Ready(ContextData &ctx)
        : ctx_ {ctx}
    {
        std::cout << "Ready state enter" << std::endl;
    }

    ~Ready()
    {
        std::cout << "Ready state exit" << std::endl;
    }

    // can only set a timer in ready state
    auto handle(SetTimer const& e) {
        std::cout << "Ready: SetTimer event" << std::endl;
        ctx_.setTimeout(e.timer);
        return handled();
    }

    // "start" button pressed
    auto handle(StartStopButtonPressed const&) -> fsmpp2::transitions<Microwaving> {
        std::cout << "Ready: StartStopButtonPressed event" << std::endl;
        if (ctx_.isTimerSet()) {
            // is timer was set, we can move to Microwaving state
            return transition<Microwaving>();
        } else {
            // timer was not set, ignore the event, we could use handled() as well
            // but any further implementation could utilize StartStopButtonPressed
            // in parent state for any other reason/feature
            return not_handled();
        }
    }

    // door opened
    auto handle(DoorOpen const&) {
        std::cout << "Ready: DoorOpen event" << std::endl;
        return transition<Opened>();
    }

    ContextData& ctx_;
};

// Ready, Microwaving, Paused are substates of PoweredOn, this must be marked explicitly as the
// template arguments of fsmpp2::state template base class
struct PoweredOn :  fsmpp2::state<Ready, Microwaving, Paused, Opened> {
    PoweredOn() {
        std::cout << "PoweredOn state enter" << std::endl;
    }

    ~PoweredOn() {
        std::cout << "PoweredOn state exit" << std::endl;
    }

    auto handle(PowerButtonPressed const&) const {
        std::cout << "PoweredOn: PowerButtonPressed event" << std::endl;
        return transition<PoweredOff>();
    }
};

}

template<class SM>
bool read_input(SM &sm)
{
    std::string input;
    std::cout << "> ";
    std::cin >> input;

    if (input == "?" || input == "help") {
        std::cout << "PowerButtonPressed, DoorOpen, DoorClose, StartStopButtonPressed, SetTimer, TimeElapsed" << std::endl;
        return true;
    }

    if (input == "exit") {
        return false;
    }

    if (input == "PowerButtonPressed") {
        sm.dispatch(sm::PowerButtonPressed{});
    }

    if (input == "DoorOpen") {
        sm.dispatch(sm::DoorOpen{});
    }

    if (input == "DoorClose") {
        sm.dispatch(sm::DoorClose{});
    }

    if (input == "StartStopButtonPressed") {
        sm.dispatch(sm::StartStopButtonPressed{});
    }

    if (input == "SetTimer") {
        int seconds = 0;
        std::cin >> seconds;
        sm.dispatch(sm::SetTimer{std::chrono::seconds{seconds}});
    }

    if (input == "TimeElapsed") {
        int seconds = 0;
        std::cin >> seconds;
        sm.dispatch(sm::TimeElapsed{std::chrono::seconds{seconds}});
    }

    return true;
}

int main(int argc, char **argv)
{
    using States = fsmpp2::states<
        sm::PoweredOff,
        sm::PoweredOn
    >;

    using Events = fsmpp2::events<
        sm::PowerButtonPressed,
        sm::DoorOpen,
        sm::DoorClose,
        sm::StartStopButtonPressed,
        sm::SetTimer,
        sm::TimeElapsed
    >;

    if (argc == 2) {
        if (argv[1] == std::string{"state"}) {
            fsmpp2::plantuml::print_state_diagram<States, Events>(std::cout);
        } else if (argv[1] == std::string{"sequence"}) {
            sm::ContextData ctx;
            std::ofstream ofs{"sequence.txt"};
            fsmpp2::state_machine sm{States{}, Events{}, ctx, fsmpp2::plantuml::seq_diagrm_trace{ofs}};

            // run a scenarion
            sm.tracer().begin();
            sm.dispatch(sm::PowerButtonPressed{});
            sm.dispatch(sm::DoorOpen{});
            sm.dispatch(sm::DoorClose{});
            sm.dispatch(sm::SetTimer{std::chrono::seconds{30}});
            sm.dispatch(sm::StartStopButtonPressed{});
            sm.dispatch(sm::TimeElapsed{std::chrono::seconds{15}});
            sm.dispatch(sm::DoorOpen{});
            sm.dispatch(sm::DoorClose{});
            sm.dispatch(sm::TimeElapsed{std::chrono::seconds{15}});
            sm.dispatch(sm::PowerButtonPressed{});
            sm.tracer().end();
        }
    } else {
        sm::ContextData ctx;
        fsmpp2::state_machine sm{States{}, Events{}, ctx};

        while (read_input(sm))
            ;
    }
}