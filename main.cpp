#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <map>
#include "Shader.cpp"
#include "Sprite.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "ParticleSystem.h"
#define GLSL(src) "#version 330 core\n" #src

using namespace std;
using namespace sf;
using namespace glm;

const GLchar* vertexSource = GLSL(
    layout(location=0) in vec2 pos;
    layout(location=1) in vec2 texc;

    out vec2 texcoord;

    uniform mat4 model;
    uniform mat4 proj;
    void main()
    {
        texcoord = texc;
        gl_Position = proj * model * vec4(pos, 0.0, 1.0);
    }
);

const GLchar* fragmentSource = GLSL(

    in vec2 texcoord;

    out vec4 outColor;

    uniform vec3 color;
    uniform sampler2D tex;
    void main()
    {
        outColor = texture(tex, texcoord) * vec4(color, 1.0);
    }
);

const GLchar* frameVSource = GLSL(
    layout (location = 0) in vec2 pos;
    layout (location = 1) in vec2 texc;

    out vec2 texcoord;

    uniform bool shake;
    uniform float time;
    void main()
    {
        texcoord = texc;
        gl_Position = vec4(pos, 0.0, 1.0);

        if (shake)
        {
            float strength = 0.01;
            gl_Position.x += strength * cos(10*time);
            gl_Position.y += strength * cos(15*time);
        }
    }
);

const GLchar* frameFSource = GLSL(
    in vec2 texcoord;

    out vec4 outColor;

    uniform sampler2D scene;
    uniform bool invert = false;
    uniform bool gray = false;
    void main()
    {
        if (invert) {
            outColor = vec4(1.0 - vec3(texture(scene, texcoord)), 1.0);
        } else if (gray) {
            vec4 origColor = texture(scene, texcoord);
            float average = 0.2126 * origColor.r + 0.7152 * origColor.g + 0.0722 * origColor.b;
            outColor = vec4(average, average, average, 1.0);
        } else {
            outColor = vec4(vec3(texture(scene, texcoord)), 1.0);
        }
    }
);

const GLchar* particleVSource = GLSL(
    layout(location=0) in vec2 position;


    uniform mat4 proj;
    uniform mat4 model;
    void main()
    {
        gl_Position = proj * model * vec4(position, 0.0, 1.0);
    }
);

const GLchar* particleGSource = GLSL(
    layout (points) in;
    layout (line_strip, max_vertices = 4) out;

    uniform float squareSize = 0.1;
    void main()
    {
        gl_Position = gl_in[0].gl_Position + vec4(-squareSize, squareSize, 0.0, 0.0);
        EmitVertex();

        gl_Position = gl_in[0].gl_Position + vec4(squareSize, squareSize, 0.0, 0.0);
        EmitVertex();

        gl_Position = gl_in[0].gl_Position + vec4(squareSize, -squareSize, 0.0, 0.0);
        EmitVertex();

        gl_Position = gl_in[0].gl_Position + vec4(-squareSize, -squareSize, 0.0, 0.0);
    }
);

const GLchar* particleFSource = GLSL(

    out vec4 outColor;

    uniform vec3 color;
    uniform float alpha = 1.0;
    void main()
    {
        outColor = vec4(color, alpha);
    }
);

void initGL()
{
    glewExperimental = GL_TRUE;
    glewInit();
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);
}


enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
};

int randUInt(int rmin, int rmax)
{
    int mod = rmax - rmin + 1;
    int randint = (rand()%mod) + rmin;
    return randint;
}

/*int randInt(int rmin, int rmax) //TO BE IMPLEMENTED
{
    if (rmin < 0)
    {
        int rand_sign = (rand()%2) ? -1 : 1;
    } else {
        return randUInt(rmin, rmax);
    }
}*/

class Game
{
    public:
        // Game state
        GameState  state;

        int  width, height;

        SpriteRenderer *renderer;

        Texture2D BLANK;
        map <string, Texture2D> textures;

        Framebuffer *fb;

        float shakeTime = 0.0;
        bool invert;

        Sprite *playButton;
        Sprite *quitButton;

        Sprite *player1;
        Sprite *ball;
        vec2 ballVel;
        Sprite *cursorSpr;

        //ParticleSystem *ps;

        vec2 mousePos;

        vector<Event> events;

        double totalTime;
        //Clock speedClock;

        bool lost = false;

        // Constructor/Destructor
        Game(int w, int h);
        ~Game();
        // Initialize game state (load all shaders/textures/levels)
        void init();
        // GameLoop
        void update(float dt);
        void render();
};

Game::Game(int w, int h)
{
    width = w;
    height = h;
}

void Game::init()
{
    //initialize textures and such here...
    srand(time(NULL)); //seed

    mat4 proj = ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f,  1.0f); //projection

    Shader spriteShader; //shader for sprites
    spriteShader.Compile(vertexSource, fragmentSource);

    Shader frameShader; //frame buffer shader
    frameShader.Compile(frameVSource, frameFSource);

    Shader particleShader;
    particleShader.Compile(particleVSource, particleFSource, particleGSource);

    fb = new Framebuffer(frameShader, width, height);

    playButton = new Sprite(vec2(130, 45), vec2(335, 250)); //button for playing


    player1 = new Sprite(vec2(45, 130), vec2(30, 20));
    player1->color = vec3(0.0f, 1.0f, 0.0f); //player 1

    ball = new Sprite(vec2(70, 70), vec2(randUInt(70, 700), randUInt(70, 500)));
    //ball->color = vec3(255, 0, 216);
    float magnitude = randUInt(600, 900);
    int ang = randUInt(25, 70);
    ballVel = vec2(magnitude * cos(ang), magnitude*sin(ang));

    //ps = new ParticleSystem(ball->position, particleShader, 10, proj, 3, ballVel);

    cursorSpr = new Sprite(vec2(30, 30));

    Texture2D Face;
    int w, h;
    unsigned char* image =
    SOIL_load_image("textures\\awesomeface.png", &w, &h, 0, SOIL_LOAD_RGBA);
    Face.Generate(w, h, image);
    SOIL_free_image_data(image);
    textures["face"] = Face;

    Texture2D Cat;
    image = SOIL_load_image("textures\\cat.jpg", &w, &h, 0, SOIL_LOAD_RGBA);
    Cat.Generate(w, h, image);
    SOIL_free_image_data(image);
    textures["cat"] = Cat;

    BLANK.GenerateBlank(); //blank texture

    renderer = new SpriteRenderer(spriteShader, proj); //renderer


    state = GAME_ACTIVE;
    fb->BindTextureBuffer();
}

void Game::update(float dt)
{
    totalTime += dt;
    fb->shader.SetFloat("time", totalTime);
    switch (state)
    {
        case GAME_ACTIVE:
        {
            while (events.size() != 0)
            {
                Event ev = events[events.size() - 1];
                events.pop_back();
                switch (ev.type)
                {
                    case Event::MouseButtonPressed:
                        if (player1->contains(mousePos) && (!lost))
                        {
                            player1->color = (player1->color == vec3(0.0, 1.0, 0.0)) ?
                            vec3(1.0, 0.6666, 0.98) : vec3(0.0, 1.0, 0.0);
                        }

                        break;
                    case Event::KeyReleased:
                        if (ev.key.code == Keyboard::I)
                        {
                            if (invert)
                            {
                                invert = false;
                                fb->shader.SetBool("invert", false);
                            } else {
                                invert = true;
                                fb->shader.SetBool("invert", true);
                            }
                        }
                        break;
                }
            }

            if (shakeTime > 0.0)
            {
                shakeTime -= dt;
            } else {
                fb->shader.SetBool("shake", false);
            }

            if (Keyboard::isKeyPressed(Keyboard::Up)) //handle input
            {
                if ((player1->position.y >= 0.0f) && (!lost))
                {
                    player1->position.y -= 450.0f*dt;
                }
            }
            if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                if ((player1->position.y + player1->size.y <= 600.0f) && (!lost))
                {
                    player1->position.y += 450.0f*dt;
                }
            }

            //do collisions
            vec2 ballCenter = ball->position + (0.5f*ball->size);
            float radius = 0.5f*ball->size.x;
            if (!lost)
            {
                if (ballCenter.x + radius >= width) //keep the ball inside the court
                {
                    ballVel.x *= -1.0f;
                    ball->move(-0.5, 0.0);
                } else if (ballCenter.x - radius <= 0) {
                    ballVel.x *= -1.0f;
                    ball->move(0.5, 0.0);
                    lost = true;
                    fb->shader.SetBool("gray", true);
                }
                if (ballCenter.y - radius <= 0)
                {
                    ballVel.y *= -1.0f;
                    ball->move(0, 0.5);
                } else if (ballCenter.y + radius >= height) {
                    ballVel.y *= -1.0f;
                    ball->move(0, -0.5);
                }
            }

            if (checkCollision(*player1, *ball) && !(lost)) //handle collisions
            {
                shakeTime = 0.07;
                fb->shader.SetBool("shake", true);
                if (ballCenter.x <= player1->position.x + player1->size.x)
                {
                    ballVel.y *= -1.0f;
                } else {
                    ballVel.x *= -1.0f;
                }
            }

            if (!lost)
            {
                ball->move(ballVel*dt);
                ball->rotate(dt); //spin ball
            }

            cursorSpr->position = mousePos - (0.5f*cursorSpr->size);
            break;
        }
        case GAME_MENU:
        {
            while (events.size() != 0)
            {
                Event ev = events[events.size() - 1];
                events.pop_back();
                switch (ev.type)
                {

                }
            }
            break;
        }
    }
}

void Game::render()
{
    fb->BeginRender();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    switch (state)
    {
        case GAME_ACTIVE:
            renderer->drawSprite(textures["face"], *ball);
            renderer->drawSprite(BLANK, *player1);
            break;
    }
    fb->EndRender();
    fb->Render(true);
}

Game::~Game()
{
    delete renderer;
    delete player1;
    delete ball;
    delete cursorSpr;
}

int main()
{
    ContextSettings settings; //Create a window
    settings.depthBits = 24;
    settings.stencilBits = 8;
    Game game(800, 600);
    game.state = GAME_MENU;
    Window window(VideoMode(800, 600), "Pong", Style::Default, settings);

    initGL(); //initialize OpenGL
    game.init();

    Clock clock; //loop
    bool running = true;
    while (running)
    {
        Event ev;
        while (window.pollEvent(ev))
        {
            switch(ev.type) //handle events
            {
                case Event::Closed:
                    running = false;
                    break;
                case Event::KeyPressed:
                    if (ev.key.code == Keyboard::Escape)
                    {
                        running = false;
                    } else {
                        game.events.push_back(ev);
                    }
                    break;
                default:
                    game.events.push_back(ev);
                    break;
            }
        }

        game.mousePos = vec2(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
        //update
        game.update(clock.restart().asSeconds());
        //render
        game.render();

        window.display();
    }
    window.close();
    //cin.ignore();
    //cin.ignore();
    return 0;
}
