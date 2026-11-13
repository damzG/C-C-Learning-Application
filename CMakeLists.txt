cmake_minimum_required(VERSION 3.25)
project(C_C___Learning_Application C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(APP_BUILD_TESTS "Build the unit tests" ON)

include(FetchContent)

# Keep the raylib build small: no bundled examples
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 6.2
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

# Logic with no raylib dependency, shared by the app and the tests
add_library(core STATIC
        Account.c
        Levels.c
        questions.c
)
target_include_directories(core PUBLIC src)

add_executable(C_C___Learning_Application
        main.c
        App.c
        Ui.c
        screen_loading.c
        screen_menu.c
        screen_profiles.c
        screen_create.c
        screen_map.c
        screen_puzzles.c
        screen_lesson.c
        screen_lesson.c
        avatar.c
        avatar.h
        avatar_draw.c
        avatar_draw.h
        test_avatar.c
)
#add_custom_command(TARGET C_C___Learning_Application POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:C_C___Learning_Application>/assets)
target_include_directories(C_C___Learning_Application PRIVATE src)
target_link_libraries(C_C___Learning_Application PRIVATE core raylib)

if (APP_BUILD_TESTS)
    enable_testing()
    add_executable(test_accounts test_accounts.c)
    target_link_libraries(test_accounts PRIVATE core)
    add_test(NAME accounts COMMAND test_accounts)
endif ()