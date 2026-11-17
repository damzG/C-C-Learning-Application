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
        account.c
        levels.c
        questions.c
        problems.c
        judge.c
        practice_progress.c
        avatar.c
)
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR})

# The game. Only main.c may define main(): test_*.c files are separate programs below.
add_executable(C_C___Learning_Application
        main.c
        app.c
        ui.c
        editor.c
        avatar_draw.c
        panda.c
        screen_loading.c
        screen_menu.c
        screen_profiles.c
        screen_create.c
        screen_map.c
        screen_lesson.c
        screen_puzzles.c
        screen_practice.c
        screen_code.c
        panda.c
)
target_include_directories(C_C___Learning_Application PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(C_C___Learning_Application PRIVATE core raylib)

# Fonts and images are loaded from assets/ next to the program
add_custom_command(TARGET C_C___Learning_Application POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:C_C___Learning_Application>/assets)

if (APP_BUILD_TESTS)
    enable_testing()

    add_executable(test_accounts test_accounts.c)
    target_link_libraries(test_accounts PRIVATE core)
    add_test(NAME accounts COMMAND test_accounts)

    add_executable(test_avatar test_avatar.c)
    target_link_libraries(test_avatar PRIVATE core)
    add_test(NAME avatar COMMAND test_avatar)

    add_executable(test_practice test_practice.c)
    target_link_libraries(test_practice PRIVATE core)
    add_test(NAME practice COMMAND test_practice)
endif ()