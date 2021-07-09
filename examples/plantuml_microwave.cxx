#include <iostream>
#include <chrono>
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

// Each event needs to to inherit from fsmpp2::state template, first template
// argument is the common Context data that is shared among all the state, if
// you use that Context type in one of the state you need to use it consistently
// in any other state in the same state machine
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
// second template argument to fsmpp2::state template base class
struct PoweredOn :  fsmpp2::state<fsmpp2::states<Ready, Microwaving, Paused, Opened>> {
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

int main(int argc, char **argv)
{
    using StateMachine = fsmpp2::states<sm::PoweredOff, sm::PoweredOn>;
    using Events = fsmpp2::events<sm::PowerButtonPressed, sm::DoorOpen, sm::DoorClose, sm::StartStopButtonPressed, sm::SetTimer, sm::TimeElapsed>;

    fsmpp2::plantuml::print_state_diagram<StateMachine, Events>(std::cout);
}