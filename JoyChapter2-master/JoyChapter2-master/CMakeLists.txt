cmake_minimum_required(VERSION 3.5)
project(
    JoyChapter2
    LANGUAGES C
)

include(CTest)

add_library(is_free_hash is_free_hash.c)
add_library(special_key_hash special_key_hash.c)
add_library(hash_flex hash_flex.c)

add_executable(is_free_hash_test is_free_hash_test.c)
target_link_libraries(is_free_hash_test is_free_hash)
add_test(
    NAME is_free_hash_test 
    COMMAND is_free_hash_test 10
)

add_executable(special_key_hash_test special_key_hash_test.c)
target_link_libraries(special_key_hash_test special_key_hash)
add_test(
    NAME special_key_hash_test 
    COMMAND special_key_hash_test 10
)

add_executable(hash_flex_test hash_flex_test.c)
target_link_libraries(hash_flex_test hash_flex)
add_test(
    NAME hash_flex_test 
    COMMAND hash_flex_test
)