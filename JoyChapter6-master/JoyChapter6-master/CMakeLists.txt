cmake_minimum_required(VERSION 3.5)
project(
    JoyChapter6
    LANGUAGES C
)

include(CTest)

add_executable(hash_test hash_test.c hash.c)
add_test(
    NAME    hash_test 
    COMMAND hash_test shift2 identity 1024 512 0
)

add_executable(hash_map hash_map.c hash.c)
add_test(
    NAME    hash_map 
    COMMAND hash_map shift2 identity 1024 512 0
)

add_executable(hash_words_test hash_words_test.c hash_words.c)
add_test(
    NAME    hash_words_test 
    COMMAND hash_words_test jenkins
)

add_executable(performance performance.c hash.c)
add_test(
    NAME    performance
    COMMAND performance
)
