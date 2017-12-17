#include "textureloader.h"

#include <QImage>
#include <iostream>

TextureLoader::TextureLoader()
{

}
void TextureLoader::loadTexture(char *filename, int textureNumber, GLuint &handler) {
    QImage image(filename);
    if (image.isNull()) {
        std::cerr << "Error loading texture" << filename << std::endl;
        exit(1);
    }
    glGenTextures(1, &handler);
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, handler);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
