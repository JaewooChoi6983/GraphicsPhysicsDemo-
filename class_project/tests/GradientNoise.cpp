//Name: Hyosang Jung, Jaewoo Choi
//Assignment name: Class project
//Course name: CS250
//Term & Year : 2022&Spring
/*!
@file    GradientNoise.cpp
@author  Hyosang Jung, Jaewoo.choi
@date    04/11/2022

Note : This file is for Fourth demo that shows
       3 demo that(Gradient Noise,Gradient Plane,Gradient Sun).
       All of these demo, you can control Frequency by Imgui.
       Also If you clicked Animated, all demos are update every frame. so it looks moving to left smoothly.
       Also it has Imgui implementing for showing another demo in left top side.
       Gradient Noise is 2D gradient plane and sun is 3D, we want to show like a sun burning.

*//*__________________________________________________________________________*/
#include "GradientNoise.h"
#include <functional>
#include<random>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

Gradient_Noise::Gradient_Noise()
{}

Gradient_Noise::~Gradient_Noise()
{
}

float Gradient_Noise::smoothstepDeriv(const float& t)
{
    return t * (6 - 6 * t);
}

float Gradient_Noise::evalute(glm::vec3 p, glm::vec3& derivs)
{
    int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
    int yi0 = ((int)std::floor(p.y)) & tableSizeMask;
    int zi0 = ((int)std::floor(p.z)) & tableSizeMask;

    int xi1 = (xi0 + 1) & tableSizeMask;
    int yi1 = (yi0 + 1) & tableSizeMask;
    int zi1 = (zi0 + 1) & tableSizeMask;

    float tx = p.x - ((int)std::floor(p.x));
    float ty = p.y - ((int)std::floor(p.y));
    float tz = p.z - ((int)std::floor(p.z));

    float u = quinticstep(tx);
    float v = quinticstep(ty);
    float w = quinticstep(tz);

    float x0 = tx, x1 = tx - 1;
    float y0 = ty, y1 = ty - 1;
    float z0 = tz, z1 = tz - 1;

    float a = gradientDotV(hash(xi0, yi0, zi0), x0, y0, z0);
    float b = gradientDotV(hash(xi1, yi0, zi0), x1, y0, z0);
    float c = gradientDotV(hash(xi0, yi1, zi0), x0, y1, z0);
    float d = gradientDotV(hash(xi1, yi1, zi0), x1, y1, z0);
    float e = gradientDotV(hash(xi0, yi0, zi1), x0, y0, z1);
    float f = gradientDotV(hash(xi1, yi0, zi1), x1, y0, z1);
    float g = gradientDotV(hash(xi0, yi1, zi1), x0, y1, z1);
    float h = gradientDotV(hash(xi1, yi1, zi1), x1, y1, z1);

    float du = quinticstepDeriv(tx);
    float dv = quinticstepDeriv(ty);
    float dw = quinticstepDeriv(tz);

    float k0 = a;
    float k1 = (b - a);
    float k2 = (c - a);
    float k3 = (e - a);
    float k4 = (a + d - b - c);
    float k5 = (a + f - b - e);
    float k6 = (a + g - c - e);
    float k7 = (b + c + e + h - a - d - f - g);

    derivs.x = du * (k1 + k4 * v + k5 * w + k7 * v * w);
    derivs.y = dv * (k2 + k4 * u + k6 * w + k7 * v * w);
    derivs.z = dw * (k3 + k5 * u + k6 * v + k7 * v * w);

    return k0 + k1 * u + k2 * v + k3 * w + k4 * u * v + k5 * u * w + k6 * v * w + k7 * u * v * w;
}

void Gradient_Noise::generate_gradient(float dt)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width * 3; j++) //  *3  because  r  g  b
        {
            glm::vec3 derives{ 0,0,0 };
            float val = (evalute(glm::vec3((j / 3) + 0.5 + dt, i + 0.5, 0) * frequency, derives) + 1) * 0.5f;

            data[i][j] = static_cast<unsigned char>(val * 255.f);
        }
    }
}

Mesh Gradient_Noise::create_gradient_plane(int stacks, int slices, float )
{
    Mesh mesh;
    mesh.stack_slice[0] = stacks;
    mesh.stack_slice[1] = slices;

    for (int stack_ = 0; stack_ <= stacks; ++stack_)
    {
        float row = (float)stack_ / stacks;

        for (int slice_ = 0; slice_ <= slices; ++slice_)
        {
            float col = (float)slice_ / slices;

            Vertex v;
            v.pos = glm::vec3(col - 0.5f, 0, row - 0.5f);

            glm::vec3 derivs;
            float val = evalute(glm::vec3(v.pos.x + 0.5, 0, v.pos.z + 0.5) * frequency * 2.f, derivs);  //     /3  because  r  g  b
            v.pos.y = val;
            v.nrm = glm::vec3(-derivs.x, 1, -derivs.z);

            v.nrm /= v.nrm.length();

            v.uv = glm::vec2(col, row);

            v.color = glm::vec3(0.68, 0.87, 0.89);

            addVertex(mesh, v);
        }
    }

    BuildIndexBuffer(stacks, slices, mesh);

    return mesh;
}

Mesh Gradient_Noise::create_gradient_sphere(int stacks, int slices, float dt)
{
    Mesh mesh;
    mesh.stack_slice[0] = stacks;
    mesh.stack_slice[1] = slices;
    for (int stack_ = 0; stack_ <= stacks; ++stack_)
    {
        float row = (float)stack_ / stacks;
        float beta = PI * (row - 0.5f);

        for (int slice_ = 0; slice_ <= slices; ++slice_)
        {
            float col = (float)slice_ / slices;
            float alpha = col * PI * 2.0f;
            Vertex v;
            v.uv.x = col;
            v.uv.y = row * (-1.0f);

            v.pos.x = 0.5f * sin(alpha) * cos(beta);
            v.pos.y = 0.5f * sin(beta);
            v.pos.z = 0.5f * cos(alpha) * cos(beta);

            v.nrm.x = v.pos.x;
            v.nrm.y = v.pos.y;
            v.nrm.z = v.pos.z;

            v.nrm /= 0.5;

            glm::vec3 derivs;
            float val = evalute(glm::vec3(v.pos.x + dt, v.pos.y, v.pos.z) * frequency, derivs);  //     /3  because  r  g  b

            v.color = 2.f * abs(val) * glm::vec3(1.0, 0.5, 0.0);

            addVertex(mesh, v);
        }
    }

    BuildIndexBuffer(stacks, slices, mesh);

    return mesh;
}

void Gradient_Noise::update_plane(float dt)
{
    plane.vertexBuffer.clear();
    plane.numVertices = 0;

    for (int stack_ = 0; stack_ <= this->stack; ++stack_)
    {
        float row = (float)stack_ / this->stack;

        for (int slice_ = 0; slice_ <= this->slice; ++slice_)
        {
            float col = (float)slice_ / this->slice;

            Vertex v;
            v.pos = glm::vec3(col - 0.5f, 0, row - 0.5f);

            glm::vec3 derivs;

            float val = evalute(glm::vec3(slice_ , 0, stack_ +dt) * frequency, derivs);  //     /3  because  r  g  b
            v.pos.y = (val+1)/2.f;


            v.nrm = glm::vec3(-derivs.x, 1, -derivs.z);

            v.nrm /= v.nrm.length();

            v.uv = glm::vec2(col, row);

            v.color = glm::vec3(0.68, 0.87, 0.89);

            addVertex(plane, v);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, plane.VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(plane.numVertices) * static_cast<GLsizeiptr>(vertexSize), &plane.vertexBuffer[0], GL_DYNAMIC_DRAW);
}

void Gradient_Noise::update_sun(float dt)
{
    sun.vertexBuffer.clear();
    sun.numVertices = 0;

    for (int stack_ = 0; stack_ <= sun.stack_slice[0]; ++stack_)
    {
        float row = (float)stack_ / sun.stack_slice[0];
        float beta = PI * (row - 0.5f);

        for (int slice_ = 0; slice_ <= sun.stack_slice[1]; ++slice_)
        {
            float col = (float)slice_ / sun.stack_slice[1];
            float alpha = col * PI * 2.0f;
            Vertex v;
            v.uv.x = col;
            v.uv.y = row * (-1.0f);

            v.pos.x = 0.5f * sin(alpha) * cos(beta);
            v.pos.y = 0.5f * sin(beta);
            v.pos.z = 0.5f * cos(alpha) * cos(beta);

            v.nrm.x = v.pos.x;
            v.nrm.y = v.pos.y;
            v.nrm.z = v.pos.z;

            v.nrm /= 0.5;

            glm::vec3 derives{ 0,0,0 };

            float val = (evalute(glm::vec3((v.pos.x + 0.5 + dt * 0.005) * 255, (v.pos.y + 0.5 + dt * 0.001) * 255, (v.pos.z + 0.5 + dt * 0.0001) * 255) * frequency, derives) + 1) * 0.8f;
            v.color = glm::vec3(1.0, 0.3, 0) * val;

            addVertex(sun, v);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, sun.VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sun.numVertices) * static_cast<GLsizeiptr>(vertexSize), &sun.vertexBuffer[0], GL_DYNAMIC_DRAW);
}


void Gradient_Noise::init()
{
   view = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
    };
     projection = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    generate_random_value();
    plane = CreatePlane(stack, slice);
    sun = create_gradient_sphere(stack, slice, 0);
    plane.init("gradient_noise");
    sun.init("gradient_noise");

    currstate = gradient_state::Gradient_noise;
    generate_gradient(0);

    eye = { 0.f,  0.f, -2.f };
    light = { 0.0f,  2.f, 0.f };
    view = glm::translate(view, eye);
    projection = glm::perspective(glm::radians(45.0f), 1.f, 0.1f, 100.0f);

    glDisable(GL_CULL_FACE);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Gradient_Noise::Update(float dt)
{
    static float timer = 0;
    timer += dt;

    if (animated == true)
    {
        offset += dt * 10.f;


        switch (currstate)
        {
        case gradient_state::Gradient_noise:
        {
            generate_gradient(offset);
            break;
        }
        case gradient_state::Gradient_plane:
        {
            update_plane(offset);
            break;
        }
        case gradient_state::Sun:
        {
            update_sun(offset);
            break;
        }
        }
    }
}

void Gradient_Noise::Draw()
{
    glClearColor(0.68f, 0.87f, 0.89f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
    };

    glm::vec4 color;

    if (currstate == gradient_state::Gradient_noise || currstate == gradient_state::Gradient_plane)
    {
        if (currstate == gradient_state::Gradient_noise)
            color = { -1,0,0,-1 };
        else
        {
            model = glm::scale(model, { 2,1,2 });
            color = { 0.68, 0.0, 0.89 ,1 };
         }

        glUniform4fv(plane.colorLoc, 1, ValuePtr(color));
        glUniformMatrix4fv(plane.modelLoc, 1, GL_FALSE  ,      &model[0].x          );
        glUniformMatrix4fv(plane.viewLoc, 1, GL_FALSE,           &view[0].x                 );
        glUniformMatrix4fv(plane.projectionLoc, 1, GL_FALSE, &projection[0].x)      ;
        glUniform3fv(plane.LightLoc, 1, ValuePtr(light));
        glUniform3fv(plane.ViewPosLoc, 1, ValuePtr(-eye));
        glBindVertexArray(plane.VAO);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, plane.numIndices, GL_UNSIGNED_INT, 0);
    }

    else
    {

        glClearColor(0,0,0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        color = { -1,0,0,0 };

        glUniform4fv(sun.colorLoc, 1, ValuePtr(color));
        glUniformMatrix4fv(sun.modelLoc, 1, GL_FALSE, &model[0].x);
        glUniformMatrix4fv(sun.viewLoc, 1, GL_FALSE, &view[0].x);
        glUniformMatrix4fv(sun.projectionLoc, 1, GL_FALSE, &projection[0].x);
        glUniform3fv(sun.LightLoc, 1, ValuePtr(light));
        glUniform3fv(sun.ViewPosLoc, 1, ValuePtr(-eye));
        glBindVertexArray(sun.VAO);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, sun.numIndices, GL_UNSIGNED_INT, 0);
    }
    OnImGuiRender();

}

void Gradient_Noise::OnImGuiRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Checkbox("Animated", &animated);

    if (ImGui::SliderFloat("frequeny", &frequency, 0.f, 0.2f))
    {
        switch (currstate)
        {
        case gradient_state::Gradient_noise:
        {
            generate_gradient(0);
            break;
        }
        case gradient_state::Gradient_plane:
        {
            update_plane(0);
            break;
        }
        case gradient_state::Sun:
        {
            update_sun(0);
            break;
        }
        }
    }


    if (ImGui::Button("Gradient Noise") == true)
    {
        view = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
        };
        eye = { 0.f,  0.f, -2.f };
        view = glm::translate(view, eye);
        plane = CreatePlane(stack, slice);
        plane.init("gradient_noise");
        currstate = gradient_state::Gradient_noise;
        generate_gradient(0);
    }
    else if (ImGui::Button("Gradient plane") == true)
    {
        view = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
        };
        eye = { 2.f,  -3.f, -2.f };
        view = glm::rotate(view, QUARTER_PI, glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, QUARTER_PI, glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(view, eye);
        update_plane(0);
        currstate = gradient_state::Gradient_plane;
    }
    else if (ImGui::Button("Gradient Sun") == true)
    {
        view = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
        };
        eye = { 0.f,  0.f, -2.f };
        view = glm::translate(view, eye);
        // sun.init("gradient_noise");
        currstate = gradient_state::Sun;
        update_sun(0);
    }
}

void Gradient_Noise::UnLoad()
{

}


float Gradient_Noise::lerp(float min_, float max_, float t)
{
    return  (min_ * (1 - t)) + (max_ * t);
}

float Gradient_Noise::smoothstep(const float& t)
{
    return t * t * (3 - (2 * t));
}


float Gradient_Noise::cosinestep(const float& t)
{
    return (1.f - cos(t * PI)) * 0.5f;
}
float Gradient_Noise::quinticstep(const float& t)
{
    return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
}
float Gradient_Noise::quinticstepDeriv(const float& t)
{
    return 30.f * t * t * (t * (t - 2.f) + 1.f);
}


void Gradient_Noise::generate_random_value()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distrFloat(0, 1);
    auto dice = std::bind(distrFloat, gen);

    // create an array of random values and initialize permutation table
    for (unsigned i = 0; i < tableSize; ++i)
    {

        float theta = acos(2 * dice() - 1);
        float phi = 2 * dice() * PI;

        float x = cos(phi) * sin(theta);
        float y = sin(phi) * sin(theta);
        float z = cos(theta);
        gradients[i] = glm::vec3(x, y, z);
        permutationTable[i] = i;
    }
    std::uniform_int_distribution<unsigned> distributionInt;
    auto diceInt = std::bind(distributionInt, gen);
    // create permutation table
    for (unsigned i = 0; i < tableSize; ++i)
        std::swap(permutationTable[i], permutationTable[diceInt() & tableSizeMask]);
    // extend the permutation table in the index range [256:512]
    for (unsigned i = 0; i < tableSize; ++i) {
        permutationTable[tableSize + i] = permutationTable[i];
    }
}


