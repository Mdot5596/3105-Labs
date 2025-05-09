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
   // ogre = ObjMesh::loadWithAdjacency("media/bs_ears.obj");
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    float c = 3.5f;
    projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);
    glPointSize(10.0f);

    float v[] = { -1.0f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 1.0f, 1.0f };

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), v, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glPatchParameteri(GL_PATCH_VERTICES, 4);


    prog.setUniform("NumSegments", 50);
    prog.setUniform("NumStrips", 1);
    prog.setUniform("LineColor", vec4(1.0f, 1.0f, 0.05f, 1.0f));
    solidProg.use();
    solidProg.setUniform("Color", vec4(0.5f, 1.0f, 1.0f, 1.0f));

  
}




void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.compileShader("shader/basic_uniform.tcs");
        prog.compileShader("shader/basic_uniform.tes");
        prog.link();
        prog.use();

       solidProg.compileShader("shader/solid.vert");
       solidProg.compileShader("shader/solid.frag");
       solidProg.link();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
   /*
   float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += rotSpeed * deltaT;
    if (angle > glm::two_pi<float>())angle -= glm::two_pi<float>();
    */
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //vec3 cameraPos(2.0f*sin(angle), 0.0f, 2.0f*cos(angle));
    //view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    vec3 cameraPos(0.0f, 0.0f, 1.5f);
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    model = mat4(1.0f);
    glBindVertexArray(vaoHandle);
    setMatrices();

    prog.use();
    glDrawArrays(GL_PATCHES, 0, 4);
    solidProg.use();
    glDrawArrays(GL_POINTS, 0, 4);
    glFinish();
}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
  //  width = w;
  //  height = h;
  //  projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
  //  float c = 1.5f;
  //  projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model; // Compute the Model-View matrix
    prog.use();
   // prog.setUniform("ModelViewMatrix", mv);
   // prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection*mv);
    solidProg.use();
    solidProg.setUniform("MVP", projection * mv);
}

