#ifndef ACORN_UTILS_H
#define ACORN_UTILS_H

#include <string>

namespace utils {
    /// Try to load an ascii file as a string
    std::string load_file_to_string(const char *file_path);

    /// Get date and time as string
    std::string get_date_time_as_string();
}

#endif //ACORN_UTILS_H
