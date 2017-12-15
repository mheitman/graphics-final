#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <vector>
#include "glm.hpp"


class ObjParser
{
public:
    ObjParser();

    static void load_obj(const char* filename,
                         std::vector<glm::vec3> &vertices,
                         std::vector<glm::vec2> &uvs,
                         std::vector<unsigned int> &indices);
};

#endif // OBJPARSER_H
