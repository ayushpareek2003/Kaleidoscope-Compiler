### trying to create object for files not wokring 


et(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/llvm_code_Gen.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/parser.cpp
)

set(SOURCE_TEST_FILES ${SOURCE_FILES} PARENT_SCOPE)

add_executable(kaleidoscope main.cpp ${SOURCE_FILES})