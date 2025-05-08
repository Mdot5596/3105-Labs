#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;
#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() :
    tPrev(0), angle(0.0f), rotSpeed(glm::pi<float>()/8.0f)
{

}

void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    angle = glm::pi<float>() / 2.0f;
    
    numSprites = 50;
    locations = new float[numSprites * 3];
    srand((unsigned int)time(0));

    for (int i = 0; i < numSprites; i++)
    {
        vec3 p(((float)rand() / RAND_MAX * 2.0f) - 1.0f,
            ((float)rand() / RAND_MAX * 2.0f) - 1.0f,
            ((float)rand() / RAND_MAX * 2.0f) - 1.0f);
        locations[i * 3] = p.x;
        locations[i * 3+1] = p.y;
        locations[i * 3+2] = p.z;

    }


    GLuint handle;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, numSprites * 3 * sizeof(float), locations, GL_STATIC_DRAW);
    delete[] locations;
    glGenVertexArrays(1, &sprites);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    const char* texName = "media/texture/flower.png";
    Texture::loadTexture(texName);
    prog.setUniform("SpriteTex", 0);
    prog.setUniform("Size2", 0.15f);


}




void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.compileShader("shader/basic_uniform.gs");
        prog.link();
        prog.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
   
   float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += rotSpeed * deltaT;
    if (angle > glm::two_pi<float>())angle -= glm::two_pi<float>();
    
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vec3 cameraPos(2.0f*sin(angle), 0.0f, 2.0f*cos(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    model = mat4(1.0f);
    setMatrices();
    glBindVertexArray(sprites);
    glDrawArrays(GL_POINTS, 0, numSprites);
    glFinish();
}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model; // Compute the Model-View matrix

    prog.setUniform("ModelViewMatrix", mv);
  //  prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("ProjectionMatrix", projection);
}

