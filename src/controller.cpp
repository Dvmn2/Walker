#include "controller.h"

////////////////////////////////////////////////////////////
Controller::Controller(std::vector<std::string>& param) : parametrs(param) {}

////////////////////////////////////////////////////////////
void Controller::run() {
    if (parametrs.empty()) {
        std::cout << "No arguments provided. Use -help for usage information.\n";
        return;
    }

    for (const std::string& parametr : parametrs) {
        bool examed = false;
        examed = helpExam(parametr);
        if (!examed) examed = stateExam(parametr);
        if (!examed) fileExam(parametr);
    }
}

////////////////////////////////////////////////////////////
bool Controller::helpExam(const std::string& param) {
    if (param == "-help") {
        std::cout << "Usage: program [options] [files]\n"
                  << "Options:\n"
                  << "  -help              Show this help message\n"
                  << "  -c, -compression   Set mode to compression (default)\n"
                  << "  -d, -decompression Set mode to decompression\n"
                  << "Examples:\n"
                  << "  program -c file.txt        Compress file.txt -> file.txt.rle\n"
                  << "  program -d file.txt.rle    Decompress file.txt.rle -> file.txt\n";
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////
bool Controller::stateExam(const std::string& param) {
    if (param == "-c" || param == "-compression") {
        mode = Mode::Compress;
        return true;
    } else if (param == "-d" || param == "-decompression") {
        mode = Mode::Decompress;
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////
void Controller::fileExam(const std::string& param) {
    bool isAbsolute = param.size() >= 3 &&
                      param[1] == ':' &&
                      (param[2] == '/' || param[2] == '\\');

    if (isAbsolute) {
        compressionExam(param);
    } else {
        std::string parametr = getExeDir().string() + "\\" + param;
        compressionExam(parametr);
    }
}

////////////////////////////////////////////////////////////
void Controller::compressionExam(const std::string& param) {
    if (mode == Mode::Decompress) {
        decompression(param);
    } else {
        compression(param);
    }
}

////////////////////////////////////////////////////////////
std::string Controller::compressOnce(const std::string& input) {
    if (input.empty()) return input;

    std::string result;
    result.reserve(input.size());

    size_t i = 0;
    while (i < input.size()) {
        char current = input[i];
        size_t count = 1;
        while (i + count < input.size() && input[i + count] == current) {
            ++count;
        }
        result += current;
        if (count > 1) result += std::to_string(count);
        i += count;
    }

    return result;
}

////////////////////////////////////////////////////////////
void Controller::compression(const std::string& param) {
    std::ifstream inputFile(param, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: cannot open file: " << param << "\n";
        return;
    }

    std::string content((std::istreambuf_iterator<char>(inputFile)),
                        std::istreambuf_iterator<char>());
    inputFile.close();

    // Итеративное сжатие с подсчётом количества проходов
    int passes = 0;
    std::string compressed = compressOnce(content);
    while (compressed.size() < content.size()) {
        content = compressed;
        compressed = compressOnce(content);
        ++passes;
    }

    // Сохраняем количество проходов в начале файла для корректной декомпрессии
    std::string outputPath = param + ".rle";
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: cannot create file: " << outputPath << "\n";
        return;
    }

    // Первая строка — число проходов, затем сжатые данные
    outputFile << passes << "\n" << content;
    outputFile.close();

    std::cout << "Compressed: " << param << " -> " << outputPath
              << " (passes: " << passes << ")\n";
}

////////////////////////////////////////////////////////////
std::string Controller::decompressOnce(const std::string& input) {
    if (input.empty()) return input;

    std::string result;
    result.reserve(input.size() * 2);

    size_t i = 0;
    while (i < input.size()) {
        char current = input[i];
        ++i;

        // Читаем число, если оно следует за символом
        std::string numStr;
        while (i < input.size() && std::isdigit(static_cast<unsigned char>(input[i]))) {
            numStr += input[i];
            ++i;
        }

        if (!numStr.empty()) {
            int count = std::stoi(numStr);
            result.append(count, current);
        } else {
            result += current;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////
void Controller::decompression(const std::string& param) {
    std::ifstream inputFile(param, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: cannot open file: " << param << "\n";
        return;
    }

    // Читаем количество проходов из первой строки
    int passes = 0;
    std::string passesLine;
    std::getline(inputFile, passesLine);
    try {
        passes = std::stoi(passesLine);
    } catch (...) {
        std::cerr << "Error: invalid file format (missing passes count): " << param << "\n";
        return;
    }

    std::string content((std::istreambuf_iterator<char>(inputFile)),
                        std::istreambuf_iterator<char>());
    inputFile.close();

    // Применяем декомпрессию столько раз, сколько было проходов сжатия
    std::string decompressed = content;
    for (int i = 0; i < passes; ++i) {
        decompressed = decompressOnce(decompressed);
    }

    // Убираем расширение .rle если есть, иначе добавляем .dec
    std::string outputPath;
    const std::string ext = ".rle";
    if (param.size() > ext.size() &&
        param.compare(param.size() - ext.size(), ext.size(), ext) == 0) {
        outputPath = param.substr(0, param.size() - ext.size());
    } else {
        outputPath = param + ".dec";
    }

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: cannot create file: " << outputPath << "\n";
        return;
    }

    outputFile << decompressed;
    outputFile.close();

    std::cout << "Decompressed: " << param << " -> " << outputPath << "\n";
}

////////////////////////////////////////////////////////////
std::filesystem::path Controller::getExeDir() {
    wchar_t buffer[32767];  // Поддержка длинных путей (> MAX_PATH)
    GetModuleFileNameW(nullptr, buffer, 32767);
    return std::filesystem::path(buffer).parent_path();
}
