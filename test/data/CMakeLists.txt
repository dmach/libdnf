file(GLOB SPEC_FILES specs/*.spec)

add_test(NAME build_test_rpms
    COMMAND rpmbuild -ba
        "--define=_srcrpmdir ${CMAKE_CURRENT_BINARY_DIR}/rpms/src"
        "--define=_rpmdir ${CMAKE_CURRENT_BINARY_DIR}/rpms"
        "--define=_source_payload w1.gzdio"
        "--define=_binary_payload w1.gzdio"
        ${SPEC_FILES}
)
