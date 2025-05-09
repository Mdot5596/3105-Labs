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
    tPrev(0), time(0.0f), particleLifeTime(5.5f), nParticles(8000), emitterPos(1, 0, 0), emitterDir(-1,2,0)
{
   // ogre = ObjMesh::loadWithAdjacency("media/bs_ears.obj");
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

   // float c = 3.5f;
   // projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);
    angle = glm::half_pi<float>();
    initBuffers();
    glActiveTexture(GL_TEXTURE0);
    Texture::loadTexture("media/texture/bluewater.png");
    prog.use();

    prog.setUniform("ParticleTex", 0);
    prog.setUniform("ParticleLifeTime", particleLifeTime);
    prog.setUniform("ParticleSize", 0.05f);
    prog.setUniform("Gravity", vec3(0.0f, -0.2f, 0.0f));
    prog.setUniform("EmitterPos", emitterPos);

    flatProg.use();
    flatProg.setUniform("Color", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

}




void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        prog.use();
        
       flatProg.compileShader("shader/solid.vert");
       flatProg.compileShader("shader/solid.frag");
       flatProg.link();
       
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
    angle = std::fmod(angle + 0.01f, glm::two_pi<float>());
}

void SceneBasic_Uniform::render()
{
    prog.setUniform("Time", time);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vec3 cameraPos(3.0f*cos(angle), 1.5f, 3.0f*sin(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
    //vec3 cameraPos(0.0f, 0.0f, 1.5f);
    //view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    /*
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(50.0f), vec3(1.0f, 0.0f, 0.0f));

    prog.setUniform("Material.Ka", vec3(0.2f, 0.5f, 0.9f));
    prog.setUniform("Material.Kd", vec3(0.2f, 0.5f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("Material.Shininess", 100.0f);

    setMatrices();
    plane.render();
    */
    model = mat4(1.0f);
    flatProg.use();
    setMatrices(flatProg);
    grid.render();

    glDepthMask(GL_FALSE);
    prog.use();
    setMatrices(prog);
    glBindVertexArray(particles);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;

    projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices(GLSLProgram &prog )
{
    mat4 mv = view * model; // Compute the Model-View matrix
   // prog.use();
    prog.setUniform("ModelViewMatrix", mv);
 //   prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection*mv);
    prog.setUniform("ProjectionMatrix", projection);
}

void SceneBasic_Uniform::initBuffers()
{
    glGenBuffers(1, &initVel);
    glGenBuffers(1, &startTime);
    int size = nParticles * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferData(GL_ARRAY_BUFFER, size * 3, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);

    glm::mat3 emitterBasis = ParticleUtils::makeArbitraryBasis(emitterDir);
    vec3 v(0.0f);
    float velocity, theta, phi;
    std::vector<GLfloat> data(nParticles * 3);

    for (uint32_t i = 0; i < nParticles; i++)
    {
        theta = glm::mix(0.0f, glm::pi<float>() / 20.0f, randFloat());
        phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());
        v.x = sinf(theta) * cosf(phi);
        v.y = cosf(theta);
        v.z = sinf(theta) * sinf(phi);
        velocity = glm::mix(1.25f, 1.5f, randFloat());
        v = glm::normalize(emitterBasis * v) * velocity;
        data[3 * i] = v.x;
        data[3 * i + 1] = v.y;
        data[3 * i + 2] = v.z;
    }
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3, data.data());
    float rate = particleLifeTime / nParticles;
    for (int i = 0; i < nParticles; i++)
    {
        data[i] = rate * i;
    }
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenVertexArrays(1, &particles);
    glBindVertexArray(particles);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glBindVertexArray(0);

}

float SceneBasic_Uniform::randFloat()
{
    return rand.nextFloat();

}