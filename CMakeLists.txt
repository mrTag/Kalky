cmake_minimum_required(VERSION 3.25)
project(Kalky)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG        v2.4.11
)
FetchContent_Declare(
        signals
        GIT_REPOSITORY https://github.com/TheWisp/signals.git
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_Declare(
        tinyexpr
        GIT_REPOSITORY https://github.com/codeplea/tinyexpr.git
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_MakeAvailable(doctest signals tinyexpr)
FetchContent_GetProperties(signals tinyexpr)

add_library(signals INTERFACE)
target_include_directories(signals INTERFACE ${signals_SOURCE_DIR})

add_library(tinyexpr ${tinyexpr_SOURCE_DIR}/tinyexpr.c)
target_include_directories(tinyexpr PUBLIC ${tinyexpr_SOURCE_DIR})

add_executable(Kalky main.cpp BaseValue.h FlatValue.h ValueCollection.h SumValue.h ExpressionValue.h)
target_link_libraries(Kalky doctest signals tinyexpr)