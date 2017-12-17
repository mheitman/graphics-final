#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include "GL/glew.h"

class TextureLoader
{
public:
    TextureLoader();

    static void loadTexture(char *filename, int textureNumber, GLuint &handler);
};

#endif // TEXTURELOADER_H
