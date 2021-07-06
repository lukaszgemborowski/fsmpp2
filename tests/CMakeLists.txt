add_executable(
    tests
    main.cxx
    tests_meta.cxx
    tests_examples.cxx
    tests_state_hierarchy.cxx
    tests_example_1.cxx
)

target_link_libraries(tests PRIVATE fsmpp2)