#pragma once

#include <windows.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

enum class Mode { Compress, Decompress };

class Controller {
   private:
    std::vector<std::string>& parametrs;
    Mode mode = Mode::Compress;

   public:
    Controller(std::vector<std::string>& param);

    void run();

   private:
    bool helpExam(const std::string& param);
    bool stateExam(const std::string& param);
    void fileExam(const std::string& param);

    void compressionExam(const std::string& param);
    std::string compressOnce(const std::string& input);
    void compression(const std::string& param);
    std::string decompressOnce(const std::string& input);
    void decompression(const std::string& param);

    std::filesystem::path getExeDir();
};

