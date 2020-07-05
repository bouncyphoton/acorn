#ifndef ACORN_UTILS_H
#define ACORN_UTILS_H

#include "vertex.h"
#include <string>

namespace utils {
    /// Try to load an ascii file as a string
    std::string load_file_to_string(const char *file_path);

    /// Get date and time as string
    std::string get_date_time_as_string();

    /// Generate bi-tangent and tangent vectors for vertices of a triangle
    void calculate_tangent_and_bi_tangent(Vertex &v1, Vertex &v2, Vertex &v3);
}

#endif //ACORN_UTILS_H
