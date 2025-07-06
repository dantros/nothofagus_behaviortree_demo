cmake_minimum_required(VERSION 3.20)

project(nothofagus_demo_project CXX)

option(NOTHOFAGUS_INSTALL "Disabling Nothofagus' installation" OFF)
option(NOTHOFAGUS_BUILD_EXAMPLES "Disabling Nothofagus'examples" OFF)
add_subdirectory("third_party/nothofagus")

option(BTCPP_SHARED_LIBS "Build shared libraries" OFF)
option(BTCPP_BUILD_TOOLS "Build commandline tools" OFF)
option(BTCPP_EXAMPLES   "Build tutorials and examples" OFF)
option(BUILD_TESTING "Build the unit tests" OFF)
option(BTCPP_GROOT_INTERFACE "Add Groot2 connection. Requires ZeroMQ" OFF)
option(BTCPP_SQLITE_LOGGING "Add SQLite logging." OFF)
option(USE_V3_COMPATIBLE_NAMES  "Use some alias to compile more easily old 3.x code" OFF)
option(ENABLE_FUZZING "Enable fuzzing builds" OFF)
option(USE_AFLPLUSPLUS "Use AFL++ instead of libFuzzer" OFF)
option(ENABLE_DEBUG "Enable debug build with full symbols" OFF)
option(FORCE_STATIC_LINKING "Force static linking of all dependencies" OFF)
add_subdirectory("third_party/BehaviorTree.CPP")

add_executable(nothofagus_bt_demo
    "source/nothofagus_bt_demo.cpp"
    "source/basic_game_engine.cpp"
    "source/player.cpp"
    "source/smart_follower.cpp"
    "source/basic_game_engine.h"
    "source/player.h"
    "source/smart_follower.h"
    "source/timer.h"
    "source/velocity_component.h"
)
set_property(TARGET nothofagus_bt_demo PROPERTY CXX_STANDARD 20)
target_include_directories(nothofagus_bt_demo PRIVATE
    ${NOTHOFAGUS_INCLUDE}
    "third_party/BehaviorTree.CPP/include"
)
target_link_libraries(nothofagus_bt_demo PRIVATE
    nothofagus
    behaviortree_cpp
)

add_executable(vanilla_bt_demo
    "bt_example/dummy_nodes.h"
    "bt_example/dummy_nodes.cpp"
    "bt_example/t01_build_your_first_tree.cpp"
)
set_property(TARGET vanilla_bt_demo PROPERTY CXX_STANDARD 20)
target_include_directories(vanilla_bt_demo PRIVATE "third_party/BehaviorTree.CPP/include")
target_link_libraries(vanilla_bt_demo PRIVATE behaviortree_cpp)

install(TARGETS nothofagus_bt_demo vanilla_bt_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Demo_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Demo_Runtime
        NAMELINK_COMPONENT Demo_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Demo_Development
)