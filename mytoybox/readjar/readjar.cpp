// works like jar tf <jar file>

#include <stdio.h>
#include <string.h>
#include <iostream>
#include "unzip.h"

#define dir_delimter '/'

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " {jar/zip file to read}\n";
        return -1;
    }

    auto zipfile = unzOpen(argv[1]);
    if (zipfile == nullptr)
    {
        std::cerr << argv[1] << " not found\n";
        return -1;
    }

    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
    {
        std::cerr << argv[1] << " could not read file global info\n";
        unzClose(zipfile);
        return -1;
    }

    for (uLong i = 0; i < global_info.number_entry; i++)
    {
        unz_file_info file_info;
        char filename[PATH_MAX];
        if (unzGetCurrentFileInfo(zipfile, &file_info, filename, PATH_MAX, nullptr, 0, nullptr,
                                  0) != UNZ_OK)
        {
            std::cerr << argv[1] << " could not read file info\n";
            unzClose(zipfile);
            return -1;
        }

        auto filename_length = strlen(filename);
        if (filename[filename_length - 1] == dir_delimter)
        {
            std::cout << "dir: " << filename << "\n";
        }
        else
        {
            std::cout << "file: " << filename << "\n";
        }

        if ((i + 1) < global_info.number_entry)
        {
            if (unzGoToNextFile(zipfile) != UNZ_OK)
            {
                std::cerr << argv[1] << " cound not read next file\n";
                unzClose(zipfile);
                return -1;
            }
        }
    }

    unzClose(zipfile);

    return 0;
}
