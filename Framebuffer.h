#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "Shader.h"

class Framebuffer
{
    public:
        Framebuffer(Shader s, int width, int height);
        ~Framebuffer();

        GLuint fbo;
        GLuint rbo;
        GLuint texColorBuffer;
        Shader shader;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;

        int width;
        int height;

        void Bind();
        void BindTextureBuffer();
        void Render(bool bindTexture);
        void BeginRender();
        void EndRender();
        static void BindDefaultFrameBuffer();
};

#endif // FRAMEBUFFER_H
