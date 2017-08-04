#include "Framebuffer.h"
#include <GL/glew.h>
#include "Shader.h"

Framebuffer::Framebuffer(Shader s, int width, int height)
{
    this->width = width;
    this->height = height;
    shader = s;
    glGenFramebuffers(1, &fbo); //create a frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    /*glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);*/

    glGenTextures(1, &texColorBuffer); //create and bind a texture
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0); //and attach it to the FBO

    glGenVertexArrays(1, &vao); //create and bind a VAO
    glBindVertexArray(vao);

    float vertices[] = {
        //x, y, s, t
        -1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, -1.0, 1.0, 0.0,
        -1.0, -1.0, 0.0, 0.0
    };

    int elements[] = {
        0, 1, 2,
        2, 0, 3
    };
    //put in our vertices
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); //and tie them into our shader variables
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

    s.Use();
    //s.SetFloat("strength", 0.01);
    s.SetBool("shake", false);

    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    glDeleteBuffers(1, &vbo); //delete everything
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &texColorBuffer);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}

void Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Framebuffer::BindTextureBuffer()
{
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
}

void Framebuffer::BindDefaultFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::BeginRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Framebuffer::EndRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Render(bool bindTexture)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    shader.Use();
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    if (bindTexture)
    {
        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
