add_executable(
    tests
    main.cxx
    tests_meta.cxx
    test_examples.cxx
)

target_link_libraries(tests PRIVATE fsmpp2)