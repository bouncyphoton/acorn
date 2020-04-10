#include "utils.h"
#include "types.h"
#include "shader.h"
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

u32 load_shader_from_file(const char *vertex_file_path, const char *fragment_file_path) {
    char *vert = load_file_as_string(vertex_file_path);
    char *frag = load_file_as_string(fragment_file_path);

    u32 shader = shader_create(vert, frag);

    free(vert);
    free(frag);

    return shader;
}
