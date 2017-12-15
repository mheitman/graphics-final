#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <vector>
#include "glm.hpp"


class ObjParser
{
public:
    ObjParser();

    static void load_obj(const char* filename,
                         std::vector<float> &vertices,
                         std::vector<float> &uvs,
                         std::vector<unsigned int> &indices);
};

#endif // OBJPARSER_H
