#ifndef FSMPP2_PLANTUML_HPP
#define FSMPP2_PLANTUML_HPP

#include "fsmpp2/reflection.hpp"

namespace fsmpp2::plantuml
{

void print_one_level(std::ostream &os, std::vector<fsmpp2::reflection::state_description> const& level)
{
    auto repl = [](auto i) { std::replace(i.begin(), i.end(), ':', '_'); return i; };

     os << "[*] --> " << repl(level[0].name) << std::endl;
    for (auto &s : level) {
        for (auto &t : s.event_transitions) {
            for (auto &d : t.states) {
                os <<  repl(s.name) << " --> " << repl(d) << " : " << repl(t.event) << std::endl;
            }
        }

        if (s.substates.size()) {
            os << "state " << repl(s.name) << " {" << std::endl;
            print_one_level(os, s.substates);
            os << "}" << std::endl;
        }
    }
}

template<class States, class Events>
void print_state_diagram(std::ostream &os) {
    using Description = fsmpp2::reflection::state_machine_description<States, Events>;
    auto descr = Description::get();
    os << "@startuml\n";
    print_one_level(os, descr);
    os << "@enduml\n";
}

} // namespace fsmpp2::plantuml

#endif // FSMPP2_PLANTUML_HPP