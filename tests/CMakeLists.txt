add_executable(
    tests
    main.cxx
    tests_meta.cxx
    tests_state_manager.cxx
    #tests_examples.cxx
    #ests_state_hierarchy.cxx
    #tests_example_1.cxx
    #tests_reflection.cxx
)

target_link_libraries(tests PRIVATE fsmpp2)