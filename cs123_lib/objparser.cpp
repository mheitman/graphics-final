#include "objparser.h"

#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <regex>

#include <glm/gtx/string_cast.hpp>

ObjParser::ObjParser()
{
}

bool operator < ( vert_uv_pair a, vert_uv_pair b ) {
    return std::make_pair(a.vertex,a.uv) < std::make_pair(b.vertex,b.uv);
}

void ObjParser::load_obj(const char* filename,
                        std::vector<float> &vertices,
                        std::vector<float> &uvs,
                        std::vector<float> &tangents,
                        std::vector<float> &bitangents,
                        std::vector<unsigned int> &indices) {
    std::ifstream in(filename, std::ios::in);
    if (!in)
    {
        std::cerr << "Cannot open " << filename << std::endl; exit(1);
    }

    std::map<vert_uv_pair, unsigned int> pairs;
    std::vector<glm::vec3> non_adjusted_vertices;
    std::vector<glm::vec2> non_adjusted_uvs;

    std::vector<int> original_indices;

    // Hardcoded number of vertices
    std::map<int, std::vector<glm::vec3>> tangents_by_vertex;
    std::map<int, std::vector<glm::vec3>> bitangents_by_vertex;

    unsigned int current_index = 0;

    std::string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ") {
            std::istringstream s(line.substr(2));
            glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;
            non_adjusted_vertices.push_back(v);
        } else if (line.substr(0,3) == "vt ") {
            std::istringstream s(line.substr(3));
            float a, b; s >> a; s >> b;
            non_adjusted_uvs.push_back(glm::vec2(a, 1.0 - b));
        } else if (line.substr(0,2) == "f ") {
            vert_uv_pair pair[4];
            std::istringstream s(line.substr(2));
            char slash = '/';
            unsigned int adjusted_indices[4];
            for (int i = 0; i < 4; i ++) {
                // read in the values
                s >> pair[i].vertex;
                s >> slash; // throw away a character
                s >> pair[i].uv;

                //fix indexing
                pair[i].vertex --;
                pair[i].uv --;
            }
            for (int i = 0; i < 4; i ++) {
                //make the pair
                std::map<vert_uv_pair, unsigned int>::iterator it;
                it = pairs.find(pair[i]);
                if (it != pairs.end()) {
                    // Found something
                    adjusted_indices[i] = it->second;

                    switch(i) {
                    case 1:
                        add_tangent_for_face(0, 1, 2, i, non_adjusted_vertices, non_adjusted_uvs, pair,
                                             tangents_by_vertex, bitangents_by_vertex, false);
                        break;
                    case 0:
                    case 2:
                        add_tangent_for_face(0, 1, 2, i, non_adjusted_vertices, non_adjusted_uvs, pair,
                                             tangents_by_vertex, bitangents_by_vertex, false);
                        add_tangent_for_face(0, 2, 3, i, non_adjusted_vertices, non_adjusted_uvs, pair,
                                             tangents_by_vertex, bitangents_by_vertex, false);
                        break;
                    case 3:
                        add_tangent_for_face(0, 2, 3, i, non_adjusted_vertices, non_adjusted_uvs, pair,
                                             tangents_by_vertex, bitangents_by_vertex, false);
                        break;
                    default:
                        /* impossible */
                        break;
                    }

                } else {
                    // Didn't find anything
                    pairs[pair[i]] = current_index;
                    adjusted_indices[i] = current_index;
                    // get the (v[i])th vertex (vec3) and put it in the output
                    glm::vec3 &vt = non_adjusted_vertices[pair[i].vertex];
                    glm::vec2 &uv = non_adjusted_uvs[pair[i].uv];
                    vertices.push_back(vt.x);
                    vertices.push_back(vt.y);
                    vertices.push_back(vt.z);
                    uvs.push_back(uv.x);
                    uvs.push_back(uv.y);

                    original_indices.push_back(pair[i].vertex);

                    switch(i) {
                    case 1:
                        add_tangent_for_face(0, 1, 2, i, non_adjusted_vertices, non_adjusted_uvs, pair,
                                             tangents_by_vertex, bitangents_by_vertex, true);
                        break;
                    case 0:
                    case 2:
                        add_tangent_for_face(0, 1, 2, i, non_adjusted_vertices, non_adjusted_uvs, pair,
                                             tangents_by_vertex, bitangents_by_vertex, true);
                        add_tangent_for_face(0, 2, 3, i, non_adjusted_vertices, non_adjusted_uvs, pair,
                                             tangents_by_vertex, bitangents_by_vertex, false);
                        break;
                    case 3:
                        add_tangent_for_face(0, 2, 3, i, non_adjusted_vertices, non_adjusted_uvs, pair,
                                             tangents_by_vertex, bitangents_by_vertex, true);
                        break;
                    default:
                        /* impossible */
                        break;
                    }

                    current_index ++;
                }
            }

            // 4 adjusted values have been set based on adjusted positions
            // Read in uvs as two triangles
            indices.push_back(adjusted_indices[0]);
            indices.push_back(adjusted_indices[1]);
            indices.push_back(adjusted_indices[2]);

            indices.push_back(adjusted_indices[0]);
            indices.push_back(adjusted_indices[2]);
            indices.push_back(adjusted_indices[3]);

        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
        else
        {
            /* ignoring this line */
        }
    }

    for (int i = 0; i < original_indices.size(); i++) {
        std::vector<glm::vec3> tangents_for_this_vertex = tangents_by_vertex[original_indices[i]];
        std::vector<glm::vec3> bitangents_for_this_vertex = bitangents_by_vertex[original_indices[i]];

        glm::vec3 t = glm::vec3(0);
        glm::vec3 b = glm::vec3(0);

        for (int j = 0; j < tangents_for_this_vertex.size(); j++) {
            t += tangents_for_this_vertex[j];
        }
        for (int j = 0; j < bitangents_for_this_vertex.size(); j++) {
            b += bitangents_for_this_vertex[j];
        }
        t /= tangents_for_this_vertex.size();
        b /= bitangents_for_this_vertex.size();

        tangents.push_back(t.x);
        tangents.push_back(t.y);
        tangents.push_back(t.z);

        bitangents.push_back(b.x);
        bitangents.push_back(b.y);
        bitangents.push_back(b.z);
    }

    std::cout << "Done..." << std::endl;
}


void ObjParser::add_tangent_for_face(int i0, int i1, int i2, int current_index,
                                     std::vector<glm::vec3> &non_adjusted_vertices,
                                     std::vector<glm::vec2> &non_adjusted_uvs,
                                     vert_uv_pair pair[4],
                                     std::map<int, std::vector<glm::vec3>> &tangents_by_vertex,
                                     std::map<int, std::vector<glm::vec3>> &bitangents_by_vertex, bool create_list) {
    // Calculate the tangent and binormal
    glm::vec3 &v0 = non_adjusted_vertices[pair[i0].vertex];
    glm::vec3 &v1 = non_adjusted_vertices[pair[i1].vertex];
    glm::vec3 &v2 = non_adjusted_vertices[pair[i2].vertex];

    // Shortcuts for UVs
    glm::vec2 &uv0 = non_adjusted_uvs[pair[i0].uv];
    glm::vec2 &uv1 = non_adjusted_uvs[pair[i1].uv];
    glm::vec2 &uv2 = non_adjusted_uvs[pair[i2].uv];

    // Edges of the triangle : postion delta
    glm::vec3 deltaPos1 = v1-v0;
    glm::vec3 deltaPos2 = v2-v0;

    // UV delta
    glm::vec2 deltaUV1 = uv1-uv0;
    glm::vec2 deltaUV2 = uv2-uv0;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

    int vertex_id = pair[current_index].vertex;
    if (create_list) {
        std::vector<glm::vec3> tangents_for_this_vertex;
        tangents_for_this_vertex.push_back(tangent);
        tangents_by_vertex[vertex_id] = tangents_for_this_vertex;

        std::vector<glm::vec3> bitangents_for_this_vertex;
        bitangents_for_this_vertex.push_back(bitangent);
        bitangents_by_vertex[vertex_id] = bitangents_for_this_vertex;
    } else {
        tangents_by_vertex[vertex_id].push_back(tangent);
        bitangents_by_vertex[vertex_id].push_back(bitangent);
    }
}
