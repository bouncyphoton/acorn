#include "utils.h"
#include "types.h"
#include <fstream>

char *load_file_as_string(const char *filepath) {
    char *contents = nullptr;

    std::ifstream ifs(filepath, std::ios::in | std::ios::binary);
    if (!ifs) {
        fprintf(stderr, "[error] failed to load file \"%s\" as string\n", filepath);
        return contents;
    }

    ifs.seekg(0, std::ios::end);
    u32 len = ifs.tellg();
    contents = (char *)(malloc((len + 1) * sizeof(char)));
    contents[len] = '\0';
    ifs.seekg(0, std::ios::beg);
    ifs.read(contents, len);

    return contents;
}
