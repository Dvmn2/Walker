#pragma once

#include <string>
#include <vector>

class Controller {
   private:
    std::vector<std::string> parametrs;

   public:
    Controller(std::vector<std::string> param);

    void run();

   private:
};