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
    tPrev(0), time(0.0f), plane(13.0, 10.0f, 200, 2)
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
    angle = glm::half_pi<float>();


    prog.setUniform("LineColor", vec4(0.05f, 1.0f, 0.05f, 1.0f));
    prog.setUniform("Light.La", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Light.L", vec3(1.0f, 1.0f, 1.0f));
   // prog.setUniform("Light.Ls", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));


}




void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
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
   /*
   float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += rotSpeed * deltaT;
    if (angle > glm::two_pi<float>())angle -= glm::two_pi<float>();
    */
    time = t;
}

void SceneBasic_Uniform::render()
{
    prog.setUniform("Time", time);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vec3 cameraPos(10.0f*cos(angle), 4.0f, 10.0f*sin(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
    //vec3 cameraPos(0.0f, 0.0f, 1.5f);
    //view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(50.0f), vec3(1.0f, 0.0f, 0.0f));

    prog.setUniform("Material.Ka", vec3(0.2f, 0.5f, 0.9f));
    prog.setUniform("Material.Kd", vec3(0.2f, 0.5f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("Material.Shininess", 100.0f);

    setMatrices();
    plane.render();

}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;

    projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model; // Compute the Model-View matrix
   // prog.use();
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection*mv);
    prog.setUniform("ProjectionMatrix", projection);
}

