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
    tPrev(0), angle(0.0f), rotSpeed(glm::pi<float>()/8.0f), plane(50.0f, 50.0f, 1, 1), teapot(12,mat4(1.0f))
    , torus(0.7f*1.5f,0.3f*1.5f,50,50)
{

    
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    //model = mat4(1.0f);
    
    projection = mat4(1.0f);
    angle = glm::pi<float>()/4.0f;


    setupFBO();

    GLfloat verts[] =
    {
        -1.0f, -1.0f, 0.0f, 1.0f,-1.0f,0.0f, 1.0f, 1.0f,0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f,1.0f,0.0f, -1.0f, 1.0f,0.0f
    };
    GLfloat tc[] = {
        0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,
        0.0f,0.0f,1.0f,1.0f,0.0f,1.0f
    };

    unsigned int handle[2];
    glGenBuffers(2, handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0,0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0,0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    prog.setUniform("Light.L", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.2f));

    float weights[5], sum, sigma2 = 8.0f;
    weights[0] = gauss(0, sigma2);
    sum = weights[0];
    for (int i = 01; i < 5; i++)
    {
        weights[i] = gauss(float(i), sigma2);
        sum += 2 * weights[i];
    }

    for (int i = 01; i < 5; i++)
    {
        std::stringstream uniName;
        uniName << "Weight[" << i << "J";
        float val = weights[i] / sum;
        prog.setUniform(uniName.str().c_str(), val);
    }

}


void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
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
    pass1();
    pass2();
    pass3();
}

void SceneBasic_Uniform::pass1()
{

    prog.setUniform("Pass", 1);
    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    view = glm::lookAt(vec3(7.0f * cos(angle), 4.0f, 7.0f * sin(angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

   // vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
    prog.setUniform("Light.Position",glm::vec4(0.0f,0.0f,0.0f,1.0f));

    prog.setUniform("Material.Kd", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.Shininess", 100.0f);                 // Higher = sharper highlight

   
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices(); // Apply model-view-projection transformations
    teapot.render(); // Render the torus object

    prog.setUniform("Material.Kd", vec3(0.4f, 0.4f, 0.4f));
    prog.setUniform("Material.Ks", vec3(0.0f, 0.0f, 0.0f));
    prog.setUniform("Material.Ka", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.Shininess", 1.0f);                 // Higher = sharper highlight


    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.75f, 0.0f));
    setMatrices(); // Apply model-view-projection transformations
    plane.render(); // Render the torus object

    prog.setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    prog.setUniform("Material.Kd", vec3(0.2f, 0.5f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Material.Shininess", 100.0f);                 // Higher = sharper highlight


    model = mat4(1.0f);
    glm::translate(model, vec3(1.0f, 1.0f, 3.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices(); // Apply model-view-projection transformations
    torus.render(); // Render the torus object
}

void SceneBasic_Uniform::pass2()
{
    prog.setUniform("Pass", 2);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);

    setMatrices();

    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
   // glBindVertexArray(0);

}

void SceneBasic_Uniform::pass3()
{
    prog.setUniform("Pass", 3);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, intermediateTex);

    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);

    setMatrices();

    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // glBindVertexArray(0);

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

    // Send Model-View matrix to the shader
    prog.setUniform("ModelViewMatrix", mv);

    // Compute and send Normal Matrix (used for transforming normals correctly)
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    // Send Model-View-Projection matrix to the shader
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setupFBO()
{
    glGenFramebuffers(1, &renderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is complete" << endl;
    }
    else
    {
        std::cout << "Framebuffer error" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    glGenTextures(1, &intermediateTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, intermediateTex);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateTex, 0);

    glDrawBuffers(1, drawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float SceneBasic_Uniform::gauss(float x, float sigma2)
{
    double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
    double exponent = -(x * x) / (2.0 * sigma2);
    return (float)(coeff * exp(exponent));
}