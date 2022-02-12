#include <benchmark/benchmark.h>
#include <fsmpp2/states.hpp>
#include <fsmpp2/state_machine.hpp>

namespace
{

struct EvA : fsmpp2::event {};
struct StA : fsmpp2::state<> {};
struct StHandleEvA : fsmpp2::state<> {
    auto handle(EvA) { return handled(); }
};
struct NullCtx {};

static void BM_DispatchEventToSingleStateMachine(benchmark::State& state) {
    using events = fsmpp2::events<EvA>;
    using states = fsmpp2::states<StA>;

    fsmpp2::state_machine<states, events, NullCtx> sm;
    for (auto _ : state) {
        sm.dispatch(EvA{});
    }
}

BENCHMARK(BM_DispatchEventToSingleStateMachine);

static void BM_DispatchEventToSingleStateMachineHandlingTheEvent(benchmark::State& state) {
    using events = fsmpp2::events<EvA>;
    using states = fsmpp2::states<StHandleEvA>;

    fsmpp2::state_machine<states, events, NullCtx> sm;
    for (auto _ : state) {
        sm.dispatch(EvA{});
    }
}

BENCHMARK(BM_DispatchEventToSingleStateMachineHandlingTheEvent);

template<std::size_t I, bool Last = false> struct StPI : fsmpp2::state<> {
    auto handle(EvA) { return transition<StPI<I+1>>(); }
};
template<std::size_t I> struct StPI<I,true> : fsmpp2::state<> {
    auto handle(EvA) { return transition<StPI<0>>(); }
};

template<std::size_t... I>
struct generate_i_stpis {
    using states = fsmpp2::states<StPI<I>..., StPI<sizeof...(I), true>>;
};

template<std::size_t N>
void BM_ProgressThroughStateMachine(benchmark::State& state) {
    using events = fsmpp2::events<EvA>;
    using states = typename generate_i_stpis<10>::states;
    fsmpp2::state_machine<states, events, NullCtx> sm;
    for (auto _ : state) {
        sm.dispatch(EvA{});
    }

    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_ProgressThroughStateMachine<10>);
BENCHMARK(BM_ProgressThroughStateMachine<100>);
BENCHMARK(BM_ProgressThroughStateMachine<1000>);
BENCHMARK(BM_ProgressThroughStateMachine<10000>);

}