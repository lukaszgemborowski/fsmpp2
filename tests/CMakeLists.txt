if (FSMPP2_TEST_ASAN)
    add_link_options(-fsanitize=address)
    add_compile_options(-fsanitize=address)
endif ()

if (FSMPP2_TEST_UBSAN)
    add_link_options(-fsanitize=undefined)
    add_compile_options(-fsanitize=undefined)
endif ()

add_executable(
    tests
    main.cxx
    scratchpad.cxx
    tests_meta.cxx
    tests_state_manager.cxx
    tests_state_machine.cxx
    tests_reflection.cxx
    tests_context_passing.cxx
)

target_link_libraries(tests PRIVATE fsmpp2)