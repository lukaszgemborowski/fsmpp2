add_executable(
    plantuml_simple_flat
    plantuml_simple_flat.cxx
)

target_link_libraries(
    plantuml_simple_flat
    PRIVATE
    fsmpp2
)