#ifndef FSMPP2_PLANTUML_HPP
#define FSMPP2_PLANTUML_HPP

#include "fsmpp2/reflection.hpp"

namespace fsmpp2::plantuml
{

template<class States, class Events>
void print_state_diagram(std::ostream &os) {
    using Description = fsmpp2::reflection::state_machine_description<States, Events>;
    auto descr = Description::get();

    auto repl = [](auto i) { std::replace(i.begin(), i.end(), ':', '_'); return i; };

    os << "@startuml\n";
    os << "[*] --> " << repl(descr[0].name) << std::endl;
    for (auto &s : descr) {
        for (auto &t : s.event_transitions) {
            for (auto &d : t.states) {
                os <<  repl(s.name) << " --> " << repl(d) << " : " << repl(t.event) << std::endl;
            }
        }
    }
    os << "@enduml\n";
}

} // namespace fsmpp2::plantuml

#endif // FSMPP2_PLANTUML_HPP