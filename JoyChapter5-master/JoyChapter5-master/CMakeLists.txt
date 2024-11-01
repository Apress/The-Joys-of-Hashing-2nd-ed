cmake_minimum_required(VERSION 3.5)
project(
    JoyChapter5
    LANGUAGES C
)

include(CTest)

add_executable(generated_list_test generated_list_test.c)
add_test(
    NAME    generated_list_test 
    COMMAND generated_list_test
)

add_executable(generated_hash_test generated_hash_test.c)
add_test(
    NAME    generated_hash_test 
    COMMAND generated_hash_test 191
)

add_executable(open_addressing_map_test open_addressing_map_test.c open_addressing_map.c)
add_test(
    NAME    open_addressing_map_test 
    COMMAND open_addressing_map_test 191
)

add_executable(str2int str2int.c open_addressing_map.c)
