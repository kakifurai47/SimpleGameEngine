cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME sge_editor_app)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} src/main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER sge/editor)
target_link_libraries(${PROJECT_NAME} PUBLIC sge_editor)