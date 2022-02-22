#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
using namespace std;

vector<GLfloat> vertPos = {
  -0.3f, -0.3f, 0.0f,
  0.3f, -0.3f, 0.0f,
  -0.3f, 0.3f, 0.0f,
  -0.3f, 0.3f, 0.0f,
  0.3f, -0.3f, 0.0f,
  0.3f, 0.3f, 0.0f,
};

vector<GLfloat> vertOnly = {
    -0.4f, -0.3f, 0.0f,
    0.3f, -0.3f, 0.0f,
    -0.3f, 0.3f, 0.0f,
    0.3f, 0.3f, 0.0f
};

vector<GLuint> elements = { 0, 1, 2, 1, 3, 2 };

glm::mat4 modelMat(1.0);
glm::mat4 viewMat(1.0);
string transformString = "v";

string vertCode = R"(
#version 430 core

layout(location=0) in vec3 position;

uniform mat4 modelMat;
uniform mat4 viewMat;

void main() {
    vec4 pos = vec4(position, 1.0);
    vec4 vpos = viewMat * modelMat * pos;
    gl_Position = vpos;
}
)";

string fragCode = R"(
#version 430 core

layout(location=0) out vec4 out_color;

void main() {
    out_color = vec4(0.0, 1.0, 1.0, 1.0);
}
)";

void printRM(string name, glm::mat4 &M) {
    cout << name << ":" << endl;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            cout << M[j][i] << ", ";
        }
        cout << endl;
    }
}

static void error_callback(int e, const char* d) {
    cerr << "ERROR " << e << ": " << d << endl;
}

static void key_callback(GLFWwindow *window,
                        int key,
                        int scancode,
                        int action,
                        int mods) {
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        if(key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
        else if(key == GLFW_KEY_SPACE) {
            modelMat = glm::mat4(1.0);
            transformString = "v";
        }
        else if(key == GLFW_KEY_A) {
            modelMat = glm::translate(glm::vec3(-0.1, 0, 0))*modelMat;
            transformString = "T(-0.1,0,0)*" + transformString;
        }
        else if(key == GLFW_KEY_D) {
            modelMat = glm::translate(glm::vec3(0.1, 0, 0))*modelMat;
            transformString = "T(0.1,0,0)*" + transformString;
        }
        else if(key == GLFW_KEY_Q) {
            glm::mat4 R = glm::rotate(glm::radians(5.0f),glm::vec3(0,0,1));
            modelMat = R*modelMat;
            transformString = "R(5)*" + transformString;
        }
        else if(key == GLFW_KEY_E) {
            glm::mat4 R = glm::rotate(glm::radians(-5.0f),glm::vec3(0,0,1));
            modelMat = R*modelMat;
            transformString = "R(-5)*" + transformString;
        }
        else if(key == GLFW_KEY_F) {
            glm::mat4 S = glm::scale(glm::vec3(0.8,1.0,1.0));
            modelMat = S*modelMat;
            transformString = "S(0.8x)*" + transformString;
        }
        else if(key == GLFW_KEY_G) {
            glm::mat4 S = glm::scale(glm::vec3(1.25,1.0,1.0));
            modelMat = S*modelMat;
            transformString = "S(1.25x)*" + transformString;
        }

        printRM("Model:", modelMat);
        cout << transformString << endl;
    }
}

int main(int argc, char **argv) {
    cout << "BEGIN GRAPHICS!!!!" << endl;

    if(argc > 1) {
        string filename = argv[1];
        cout << filename << endl;
    }

    glm::vec3 a(1,4,0);
    glm::vec3 b(2,3,2);

    cout << "A.x = " << a.x << endl;
    cout << "A = " << glm::to_string(a) << endl;
    cout << "B = " << glm::to_string(b) << endl;

    glm::vec3 c = b - a;
    cout << "C = " << glm::to_string(c) << endl;
    a = 5.0f*a;
    cout << "A * 5 = " << glm::to_string(a) << endl;

    float lenA = glm::length(a);
    glm::vec3 normA = glm::normalize(a);
    float lenNormA = glm::length(normA);
    cout << "Normalized A = " << glm::to_string(normA) << endl;
    cout << "Lengths: " << lenA << ", " << lenNormA << endl;

    glm::vec3 normB = glm::normalize(b);
    cout << "Normalized B = " << glm::to_string(normB) << endl;
    float dotAB = glm::dot(normA, normB);
    float radAB = acos(dotAB);
    float degAB = glm::degrees(radAB);
    cout << "Dot(A,B) = " << dotAB << endl;
    cout << "Angle(A,B) = " << degAB << endl;

    glm::vec3 normD = glm::vec3(0,0,1);
    cout << "Normalized D = " << glm::to_string(normD) << endl;
    float dotAD = glm::dot(normA, normD);
    float degAD = glm::degrees(acos(dotAD));
    cout << "Dot(A,D) = " << dotAD << endl;
    cout << "Angle(A,D) = " << degAD << endl;

    glm::vec3 e = glm::vec3(7,8,0);
    glm::vec3 crossAE = glm::cross(a, e);
    glm::vec3 crossEA = glm::cross(e, a);
    cout << "Cross(A,E) = " << glm::to_string(crossAE) << endl;
    cout << "Cross(E,A) = " << glm::to_string(crossEA) << endl;

    glm::mat4 T = glm::translate(glm::vec3(+2, 0, 0));


    glfwSetErrorCallback(error_callback);

    if(!glfwInit()) {
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int windowWidth = 640;
    int windowHeight = 480;
    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, 
                                        "Doesn't Matter", NULL, NULL);
    
    if(!window) {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    int frameWidth, frameHeight;

    glClearColor(1.0, 0.0, 1.0, 1.0);

    glewExperimental = true;
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        cout << "GLEW ERROR: ";
        cout << glewGetErrorString(err) << endl;
        glfwTerminate();
        exit(1);
    }

    GLuint vertID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);

    char const * vertPtr = vertCode.c_str();
    char const * fragPtr = fragCode.c_str();
    glShaderSource(vertID, 1, &vertPtr, NULL);
    glShaderSource(fragID, 1, &fragPtr, NULL);

    glCompileShader(vertID);
    glCompileShader(fragID);

    GLuint progID = glCreateProgram();
    glAttachShader(progID, vertID);
    glAttachShader(progID, fragID);

    glLinkProgram(progID);

    glDeleteShader(vertID);
    glDeleteShader(fragID);

    GLint modelMatLoc = glGetUniformLocation(progID, "modelMat");
    cout << modelMatLoc << endl;

    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertOnly.size()*sizeof(GLfloat),
                    vertOnly.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
   

    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

    GLuint EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(GLuint),
                         elements.data(), GL_STATIC_DRAW);  

    glBindVertexArray(0);



    // DRAWING / MAIN RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        
        glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
        glViewport(0,0,frameWidth,frameHeight);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(progID);

        glUniformMatrix4fv(modelMatLoc, 1, false, glm::value_ptr(modelMat));

        // Draw stuff
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        this_thread::sleep_for(chrono::milliseconds(15));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &EBO);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);

    glUseProgram(0);
    glDeleteProgram(progID);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}