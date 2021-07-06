# Intro

This is my second take on C++ state machine implementation. My first attempt can be found [here](https://github.com/lukaszgemborowski/fsmpp).
The main goals of the implementation are:

1. No dynamic allocations
2. Sub-state support (in contrast to first implementation)
3. Type safety
4. Limited stdlib dependencies (in theory easier to port on embedded platforms)

## Requirements

Currently C++20 capable compiler is required (concepts) with some part of standard library: type_traits, array and optional (to be removed).
With few minor tweaks the library can be backported to C++17 (mainly replacing concepts with some std::enable_if/SFINAE trickery).

# Overview

You can start with a more [complex example](https://github.com/lukaszgemborowski/fsmpp2/blob/master/tests/tests_example_1.cxx)
or/and read through below introduction section.

## State

State is a class derived from fsmpp2::state<> template.

```cpp
struct A_State : fsmpp2::state<> {};
```

A state class instance is created when State Machine enters that state and is destructed when it leaves.
This concept greatly increase imporatance and usage of RAII idiom. Eg. you can hold a lock or open file while in state and rely on a fact that it will be
destructed when SM leaves that state. State handle events by using overloaded handle() method accepting particular (event) type.

```cpp
struct A_State : fsmpp2::state<> {
  auto handle(AnEvent const&) {
    return handled();
  }
};
```

**IMPORTANT** states are not heap-allocated, state machine statically allocates enough storage (byte array) and reuse that storage using placement new operator.

## State context

While states are short living objects and the user have little to no control over how they are managed it is possible to pass a custom object reference (Context)
to each state's constructor while it's created. The actual context type is denoted as the first state<> template argument:

```cpp
struct CustomContext {};
struct A_State : fsmpp2::state<CustomContext> {
  A_State(CustomContext &) {}
}
```

It is important to note, that every state in state machine have to use exactly the same type of context type. If it's not used user may provide a constructor that does
not accept Context as its argument (or leave the default constructor)

```cpp
struct CustomContext {};
struct A_State : fsmpp2::state<CustomContext> {
  A_State() = default; // still valid, this state does not reference a global context
}
```

## State machine

"State machine" is simply a set of states

```cpp
struct StateA;
struct StateB;
struct StateC;

fsmpp2::states<StateA, StateB, StateC> sm;
// or
CustomContext ctx;
fsmpp2::states<StateA, StateB, StateC> sm{ctx};
// if context is defined for states
```

by default, SM enters the first state on the list, in this case `StateA`.

## Event passing

Event passing is done by calling handle() method on SM object:

```cpp
fsmpp2::states<StateA, StateB, StateC> sm;
sm.handle(AnEvent{});
```

## State transitions

In order to move from one state to another a state handle() method needs to indicate that by returing a special value, the simplest case is:

```cpp
struct StateA : fsmpp2::state<> {
  auto handle(AnEvent const&) const {
    return transition<StateB>();
  }
};
// ...
fsmpp2::states<StateA, StateB, StateC> sm;
sm.handle(AnEvent{}); // transits from StateA to StateB
```

If there are multiple `return` paths, return value needs to be explicitly stated (as there's no way to auto-deduce it).

```cpp
struct StateA : fsmpp2::state<> {
  auto handle(AnEvent const& e) const -> fsmpp2::transitions<StateB, StateC> {
    if (e.some_value == 42)
      return transition<StateB>();
    else if (e.some_value == 3)
      return transition<StateC>();
    else
      return handled();
  }
};
// ...
fsmpp2::states<StateA, StateB, StateC> sm;
sm.handle(AnEvent{3}); // transits from StateA to StateC
```

for the sake of clarity you may opt to declare your event handlers as

```cpp
struct StateA : fsmpp2::state<> {
  auto handle(EventA const&) -> fsmpp2::transitions<>;
  auto handle(EventB const&) -> fsmpp2::transitions<StateB>;
  auto handle(EventC const&) -> fsmpp2::transitions<StateC>;
  auto handle(EventD const&) -> fsmpp2::transitions<StateB, StateC, StateD>;
};
```
so it's clerly visible in the state class interface which event can lead to what transition.

## Nesteds states

The library supports state hierarchy but this sections is "To be described". For more information see [an example](https://github.com/lukaszgemborowski/fsmpp2/blob/master/tests/tests_example_1.cxx).

# Licence

MIT License, for details see [LICENSE file](https://github.com/lukaszgemborowski/fsmpp2/blob/master/LICENSE).
