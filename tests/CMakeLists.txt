#
#  CMakeLists.txt
#  lmdbw
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
#  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
#

project(tests)

# TestCRUD

add_executable(test_crud test_crud.cpp)

set_target_properties(test_crud PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_link_libraries(test_crud lm::dbw)

add_test(NAME TestCRUD COMMAND test_crud)

# TestStruct

add_executable(test_struct test_struct.cpp)

set_target_properties(test_struct PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_link_libraries(test_struct lm::dbw)

add_test(NAME TestStruct COMMAND test_struct)

# TestString

add_executable(test_string test_string.cpp)

set_target_properties(test_string PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_link_libraries(test_string lm::dbw)

add_test(NAME TestString COMMAND test_string)

# TestMultiple

add_executable(test_multiple test_multiple.cpp)

set_target_properties(test_multiple PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_link_libraries(test_multiple lm::dbw)

add_test(NAME TestMultiple COMMAND test_multiple)

# TestMultithreading

add_executable(test_multithreading test_multithreading.cpp)

set_target_properties(test_multithreading PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_link_libraries(test_multithreading lm::dbw)

add_test(NAME TestMultithreading COMMAND test_multithreading)