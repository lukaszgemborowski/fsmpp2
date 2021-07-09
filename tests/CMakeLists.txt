add_executable(
    tests
    main.cxx
    tests_meta.cxx
    tests_state_manager.cxx
    tests_state_machine.cxx
    tests_reflection.cxx
    # tests_examples.cxx
)

target_link_libraries(tests PRIVATE fsmpp2)