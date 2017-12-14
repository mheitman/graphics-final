#include "objparser.h"

#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <regex>

ObjParser::ObjParser()
{

}

void ObjParser::load_obj(const char* filename,
              std::vector<glm::vec3> &vertices,
              std::vector<glm::vec2> &uvs,
              std::vector<unsigned int> &vertIndices,
              std::vector<unsigned int> &uvIndices) {
    std::ifstream in(filename, std::ios::in);
        if (!in)
        {
            std::cerr << "Cannot open " << filename << std::endl; exit(1);
        }

        std::string line;
        while (getline(in, line))
        {
            if (line.substr(0,2) == "v ") {
                std::istringstream s(line.substr(2));
                glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;
                vertices.push_back(v);
            } else if (line.substr(0,3) == "vt ") {
                std::istringstream s(line.substr(3));
                glm::vec2 uv; s >> uv.x; s >> uv.y;
                uvs.push_back(uv);
            } else if (line.substr(0,2) == "f ") {
                std::string vertex1, vertex2, vertex3;
                unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
                std::istringstream s(line.substr(2));
                unsigned int ind0; char slash0 = '/'; unsigned int uv0;
                unsigned int ind1; char slash1 = '/'; unsigned int uv1;
                unsigned int ind2; char slash2 = '/'; unsigned int uv2;
                unsigned int ind3; char slash3 = '/'; unsigned int uv3;
//                unsigned int ind0; unsigned int uv0;
                s >> ind0; s >> slash0; s >> uv0;
                s >> ind1; s >> slash1; s >> uv1;
                s >> ind2; s >> slash2; s >> uv2;
                s >> ind3; s >> slash3; s >> uv3;
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
                    vertIndices.push_back(ind0);
//                    std::cout << ind0 << " " << ind1 << " " << ind2 << " " << ind3 << std::endl;
//                    std::cout << uv0 << " " << uv1 << " " << uv2 << " " << uv3 << std::endl;
                    vertIndices.push_back(ind1);
                    vertIndices.push_back(ind2);
                    vertIndices.push_back(ind3);
                    uvIndices    .push_back(uv0);
                    uvIndices    .push_back(uv1);
                    uvIndices    .push_back(uv2);
                    uvIndices    .push_back(uv3);
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
