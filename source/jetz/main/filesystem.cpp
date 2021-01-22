/*=============================================================================
filesystem.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <fstream>

#include "jetz/main/filesystem.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC STATIC METHODS
=============================================================================*/

std::vector<char> filesystem::read_all
    (
    const std::string&              filename
    )
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file || !file.is_open()) 
    {
        LOG_FATA_FMT("Failed to open file %s.", filename);
    }

    size_t file_size = (size_t)file.tellg();
    std::vector<char> buffer(file_size);
    file.seekg(0);
    file.read(buffer.data(), file_size);
    file.close();

    return buffer;
}

}   /* namespace jetz */
