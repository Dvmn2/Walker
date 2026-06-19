#include <string>
#include <vector>

#include "controller.h"

int main(int argc, char* argv[]) {
    std::vector<std::string> parametrs;
    for (int i = 1; i < argc; ++i) {
        parametrs.emplace_back(argv[i]);
    }

    Controller c(parametrs);
    c.run();

    return 0;
}
