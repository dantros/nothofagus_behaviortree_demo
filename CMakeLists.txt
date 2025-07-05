cmake_minimum_required(VERSION 3.20)

project(nothofagus_demo_project CXX)

option(NOTHOFAGUS_INSTALL "Disabling Nothofagus' installation" OFF)
option(NOTHOFAGUS_BUILD_EXAMPLES "Disabling Nothofagus'examples" OFF)
add_subdirectory("third_party/nothofagus")

file(GLOB_RECURSE HEADERS "source/*.h")

add_executable(nothofagus_demo
    "source/nothofagus_demo.cpp"
    
    # Only to show headers in VS
    ${HEADERS}
)
set_property(TARGET nothofagus_demo PROPERTY CXX_STANDARD 20)
target_include_directories(nothofagus_demo PRIVATE ${NOTHOFAGUS_INCLUDE})
target_link_libraries(nothofagus_demo PRIVATE nothofagus)

install(TARGETS nothofagus_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Demo_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Demo_Runtime
        NAMELINK_COMPONENT Demo_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Demo_Development
)