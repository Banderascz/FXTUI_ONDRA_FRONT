set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(app main.cpp)

target_link_libraries(
    app
    PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    CURL::libcurl
    nlohmann_json::nlohmann_json
    OpenSSL::Crypto
    )