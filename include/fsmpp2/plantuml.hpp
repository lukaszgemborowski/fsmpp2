#ifndef FSMPP2_PLANTUML_HPP
#define FSMPP2_PLANTUML_HPP

#include "fsmpp2/reflection.hpp"
#include <algorithm>

namespace fsmpp2::plantuml
{

namespace detail
{

inline auto sanitize_name(std::string str)
{
    std::replace(str.begin(), str.end(), ':', '_');
    return str;
}

inline void print_one_level(std::ostream &os, std::vector<fsmpp2::reflection::state_description> const& level)
{
    os << "[*] --> " << sanitize_name(level[0].name) << std::endl;
    for (auto &s : level) {
        for (auto &t : s.event_transitions) {
            for (auto &d : t.states) {
                os <<  sanitize_name(s.name) << " --> " << sanitize_name(d) << " : " << sanitize_name(t.event) << std::endl;
            }
        }

        if (s.substates.size()) {
            os << "state " << sanitize_name(s.name) << " {" << std::endl;
            print_one_level(os, s.substates);
            os << "}" << std::endl;
        }
    }
}

} // namespace detail

template<class States, class Events>
void print_state_diagram(std::ostream &os) {
    using Description = fsmpp2::reflection::state_machine_description<States, Events>;
    auto descr = Description::get();
    os << "@startuml\n";
    detail::print_one_level(os, descr);
    os << "@enduml\n";
}

struct seq_diagrm_trace {
    seq_diagrm_trace(seq_diagrm_trace &&) = default;
    seq_diagrm_trace(seq_diagrm_trace const&) = delete;

    seq_diagrm_trace(std::ostream &os)
        : os_ {os}
    {
    }


    template<class State, class E>
    void begin_event_handling () {
        current_state_ = reflection::get_type_name<State>();
        current_event_ = reflection::get_type_name<E>();
    }

    void end_event_handling(bool) {
    }

    template<class State>
    void transition() {
        os_ << detail::sanitize_name(current_state_)
            << " -> "
            << detail::sanitize_name(reflection::get_type_name<State>())
            << " : "
            << detail::sanitize_name(current_event_)
            << "\n";
    }

    void begin()
    {
        os_ << "@startuml\n";
    }

    void end()
    {
        os_ << "@enduml\n";
    }

private:
    std::string current_state_;
    std::string current_event_;
    std::ostream& os_;
};

} // namespace fsmpp2::plantuml

#endif // FSMPP2_PLANTUML_HPP