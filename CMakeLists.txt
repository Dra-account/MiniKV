cmake_minimum_required(VERSION 3.10)
project(MiniKV C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_executable(minikv
    src/main.c 
    src/parser.c
    src/engine.c
    src/storage.c
    src/string_utils.c
    src/http_server.c
    src/api_handler.c
)

add_executable(test_storage
    test/test_storage.c
    src/storage.c
)

target_link_libraries(test_storage check subunit m pthread)

enable_testing()
add_test(NAME StorageTest COMMAND test_storage)
