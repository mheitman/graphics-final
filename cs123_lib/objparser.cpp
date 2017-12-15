#include "objparser.h"

#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <regex>
#include <map>

ObjParser::ObjParser()
{

}

struct vert_uv_pair {
    unsigned int vertex;
    unsigned int uv;

};

bool operator< ( vert_uv_pair a, vert_uv_pair b ) {
    return std::make_pair(a.vertex,a.uv) < std::make_pair(b.vertex,b.uv);
}


void ObjParser::load_obj(const char* filename,
                        std::vector<glm::vec3> &vertices,
                        std::vector<glm::vec2> &uvs,
                        std::vector<unsigned int> &indices) {
    std::ifstream in(filename, std::ios::in);
        if (!in)
        {
            std::cerr << "Cannot open " << filename << std::endl; exit(1);
        }

        std::map<vert_uv_pair, unsigned int> pairs;
        std::vector<glm::vec3> non_adjusted_vertices;
        std::vector<glm::vec2> non_adjusted_uvs;

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
                glm::vec2 uv; s >> uv.x; s >> uv.y;
                non_adjusted_uvs.push_back(uv);
            } else if (line.substr(0,2) == "f ") {
                vert_uv_pair pair;
                std::istringstream s(line.substr(2));
                char slash = '/';
                unsigned int adjusted_indices[4];
                for (int i = 0; i < 4; i ++) {
                    // read in the values
                    s >> pair.vertex;
                    s >> slash; // throw away a character
                    s >> pair.uv;

                    //fix indexing
                    pair.vertex --;
                    pair.uv --;

                    //make the pair
                    std::map<vert_uv_pair, unsigned int>::iterator it;
                    it = pairs.find(pair);
                    if (it != pairs.end()) {
                        // Found something
                        adjusted_indices[i] = it->second;
                    } else {
                        // Didn't find anything
                        pairs[pair] = current_index;
                        adjusted_indices[i] = current_index;
                        // get the (v[i])th vertex (vec3) and put it in the output
                        vertices.push_back(non_adjusted_vertices[pair.vertex]);
                        uvs.push_back(non_adjusted_uvs[pair.uv]);

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


//                unsigned int ind0; unsigned int uv0;
//                s >> ind0; s >> slash0; s >> uv0;
//                s >> ind1; s >> slash1; s >> uv1;
//                s >> ind2; s >> slash2; s >> uv2;
//                s >> ind3; s >> slash3; s >> uv3;
//                line = line.substr(3);
//                FILE * file = std::fopen(filename, "r");
//                int matches = std::fscanf(file, "%d/%d %d/%d %d/%d %d/%d\n",
//                                     &vertexIndex[0], &uvIndex[0],
//                        &vertexIndex[1], &uvIndex[1],
//                        &vertexIndex[2], &uvIndex[2],
//                        &vertexIndex[3], &uvIndex[3]);
//                if (matches != 8){
//                    std::cout << matches << std::endl;
//                        printf("File can't be read by our simple parser : ( Try exporting with other options\n");
//                        return false;
//                }

//                    std::cout << vertexIndex[0] << " " <<  vertexIndex[1] << " " <<  vertexIndex[2] << " " <<  vertexIndex[3] << std::endl;
//                    std::cout << uvIndex[0] << " " << uvIndex[1] << " " << uvIndex[2] << " " << uvIndex[3] << std::endl;
//                    vertIndices.push_back(ind0);
//                    vertIndices.push_back(ind1);
//                    vertIndices.push_back(ind2);
//                    vertIndices.push_back(ind3);

                    // Read in uvs as two triangles
//                    uvIndices.push_back(uv0 - 1);
//                    uvIndices.push_back(uv1 - 1);
//                    uvIndices.push_back(uv2 - 1);

//                    uvIndices.push_back(uv0 - 1);
//                    uvIndices.push_back(uv2 - 1);
//                    uvIndices.push_back(uv3 - 1);


//                    // Read in the indices as two triangles
//                    vertIndices.push_back(ind0 - 1);
//                    vertIndices.push_back(ind1 - 1);
//                    vertIndices.push_back(ind2 - 1);

//                    vertIndices.push_back(ind0 - 1);
//                    vertIndices.push_back(ind2 - 1);
//                    vertIndices.push_back(ind3 - 1);

//                    normalIndices.push_back(normalIndex[0]);
//                    normalIndices.push_back(normalIndex[1]);
//                    normalIndices.push_back(normalIndex[2]);
//                istringstream s(line.substr(2));
//                GLushort a,b,c;
//                s >> a; s >> b; s >> c;
//                a--; b--; c--;
//               elements.push_back(a); elements.push_back(b); elements.push_back(c);
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

//        normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
//        for (int i = 0; i < elements.size(); i+=3)
//        {
//            GLushort ia = elements[i];
//            GLushort ib = elements[i+1];
//            GLushort ic = elements[i+2];
//            glm::vec3 normal = glm::normalize(glm::cross(
//            glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
//            glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
//            normals[ia] = normals[ib] = normals[ic] = normal;
//        }
//    }
        std::cout << "Done..." << std::endl;
}
