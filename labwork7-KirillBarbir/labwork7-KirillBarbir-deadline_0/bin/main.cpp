#include<lib/Window.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "wrong number of arguments\n";
        return 1;
    }
    nlohmann::json config;
    std::ifstream fin(argv[1]);
    config = nlohmann::json::parse(fin);

    DisplayWeather(config);
}