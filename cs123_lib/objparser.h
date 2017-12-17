#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <vector>
#include "glm.hpp"
#include <map>

struct vert_uv_pair {
    unsigned int vertex;
    unsigned int uv;
};

class ObjParser
{
public:
    ObjParser();

    static void load_obj(const char* filename,
                         std::vector<float> &vertices,
                         std::vector<float> &uvs,
                         std::vector<float> &tangents,
                         std::vector<float> &bitangents,
                         std::vector<unsigned int> &indices);

    static void add_tangent_for_face(int i0, int i1, int i2, int current_index,
                                     std::vector<glm::vec3> &non_adjusted_vertices,
                                     std::vector<glm::vec2> &non_adjusted_uvs,
                                     vert_uv_pair pair[4],
                                     std::map<int, std::vector<glm::vec3>> &tangents_by_vertex,
                                     std::map<int, std::vector<glm::vec3>> &bitangents_by_vertex, bool create_list);
};

#endif // OBJPARSER_H
