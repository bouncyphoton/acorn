#include "utils.h"
#include "core.h"
#include <fstream>

namespace utils {
    std::string load_file_to_string(const char *file_path) {
        if (std::ifstream ifs = std::ifstream(file_path, std::ios::in)) {
            std::string contents;

            ifs.seekg(0, std::ios::end);
            contents.resize(ifs.tellg());
            ifs.seekg(0, std::ios::beg);

            ifs.read(&contents[0], contents.size());
            return contents;
        }

        core->fatal("Failed to load file to string \"" + std::string(file_path) + "\"");
    }
}
