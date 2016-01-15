#include<GL/glew.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include <vector>


GLuint glTexImageTGAFileBitmap(const char* filename, int* outWidth, int* outHeight, std::vector<long long int>& bitmap);
GLuint glTexImageTGAFile(const char* filename, int* outWidth, int* outHeight);
void glDrawSprite(GLuint tex, int x, int y, int w, int h);