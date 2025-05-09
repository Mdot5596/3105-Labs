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
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);

   // float c = 3.5f;
   // projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);
    angle = glm::pi<float>() / 3.0f;


    prog.setUniform("TessLevel", 4);
    prog.setUniform("LineWidth",0.8f);
    prog.setUniform("LineColor", vec4(0.05f, 1.0f, 0.05f, 1.0f));
    prog.setUniform("Light.La", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Light.L", vec3(0.9f, 0.9f, 1.0f));
   // prog.setUniform("Light.Ls", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));

    prog.setUniform("Material.Ka", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Kd", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Shininess", 100.0f);
    float v[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f };

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
    glPatchParameteri(GL_PATCH_VERTICES, 16);
    GLint maxVerts;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxVerts);
    printf("Max patch vertices:%d\n", maxVerts);



  
}




void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.compileShader("shader/basic_uniform.gs");
        prog.compileShader("shader/basic_uniform.tcs");
        prog.compileShader("shader/basic_uniform.tes");
        prog.link();
        prog.use();
        /*
       solidProg.compileShader("shader/solid.vert");
       solidProg.compileShader("shader/solid.frag");
       solidProg.link();
       */
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
    vec3 cameraPos(4.25f*sin(angle), 3.0f, 4.25f*cos(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    //vec3 cameraPos(0.0f, 0.0f, 1.5f);
    //view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -1.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

    setMatrices();
    teapot.render();

    glBindVertexArray(vaoHandle);
    prog.use();
    glDrawArrays(GL_PATCHES, 0, 16);
    //solidProg.use();
   // glDrawArrays(GL_POINTS, 0, 4);
    glFinish();
}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    float w2 = w/2.0f;
    float h2 = h/2.0f;
    viewport = mat4(vec4(w2, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, h2, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 1.0f, 0.0f),
        vec4(w2 + 0, h2 + 0, 0.0f, 1.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model; // Compute the Model-View matrix
   // prog.use();
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection*mv);
    prog.setUniform("ViewportMatrix", viewport);
   // solidProg.use();
   // solidProg.setUniform("MVP", projection * mv);
}

