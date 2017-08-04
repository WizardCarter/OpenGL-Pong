#ifndef SPRITE_H
#define SPRITE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "Texture.h"
#include "Shader.h"

class Sprite
{
    public:
        Sprite(glm::vec2 Size, glm::vec2 position = glm::vec2(0.0, 0.0))
        {
            this->position = position;
            this->size = Size;
        }
        glm::vec2 position;
        float rotation = 0;
        glm::vec2 size;
        glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
        void move(glm::vec2 translation)
        {
            position += translation;
        }
        void move(float x, float y)
        {
            position += glm::vec2(x, y);
        }
        void rotate(float dr)
        {
            rotation += dr;
        }
        void scale(glm::vec2 Scale)
        {
            size += Scale;
        }
        void scale(float x, float y)
        {
            size += glm::vec2(x, y);
        }
        bool contains(glm::vec2 point)
        {
            bool checkX =  (point.x >= position.x && point.x <= position.x + size.x);
            bool checkY =  (point.y >= position.y && point.y <= position.y + size.y);
            return (checkX && checkY);
        }
        bool contains (float x, float y)
        {
            bool checkX = (x >= position.x && x <= position.x + size.x);
            bool checkY = (y >= position.y && y <= position.y + size.y);
            return (checkX && checkY);
        }
        glm::mat4 toMat4()
        {
            glm::mat4 trans;
            trans = glm::translate(trans, glm::vec3(position, 0.0));
            trans = glm::rotate(trans, rotation, glm::vec3(0.0, 0.0, 1.0));
            trans = glm::scale(trans, glm::vec3(size, 1.0));
            return trans;
        }
};

class RSprite
{
    public:
        RSprite(float radius, glm::vec2 position = glm::vec2(0.0, 0.0))
        {
            this->position = position;
            this->radius = radius;
        }
        glm::vec2 position;
        float rotation = 0;
        float radius;
        glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
        void move(glm::vec2 translation)
        {
            position += translation;
        }
        void move(float x, float y)
        {
            position += glm::vec2(x, y);
        }
        void rotate(float dr)
        {
            rotation += dr;
        }
        void scale(float Scale)
        {
            radius += Scale;
        }
        /*glm::mat4 toMat4()
        {
            glm::mat4 trans;
            trans = glm::translate(trans, glm::vec3(position, 0.0));
            trans = glm::rotate(trans, rotation, glm::vec3(0.0, 0.0, 1.0));
            trans = glm::scale(trans, glm::vec3(size, 1.0));
            return trans;
        }*/
};

class SpriteRenderer
{
    public:
        SpriteRenderer(Shader shader, glm::mat4 proj);
        ~SpriteRenderer();
        void drawSprite(Texture2D &texture, Sprite sprite);
        void drawSprite(Texture2D &texture, RSprite sprite);
        void drawSpriteNoTexture(Sprite sprite);
    private:
        Shader shader;
        GLuint quadVAO;
        GLuint quadVBO;
        GLuint quadEBO;
        glm::mat4 projection;

        void initRenderData();
};

bool checkCollision(Sprite& one, Sprite& two);
bool checkCollision(Sprite& one, RSprite &two);

#endif // SPRITE_H
