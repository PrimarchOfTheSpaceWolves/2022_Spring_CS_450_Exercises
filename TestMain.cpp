#include <iostream>
#include <fstream>
#include <sstream>
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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

struct Vertex {
    glm::vec3 pos = glm::vec3(0,0,0);
    glm::vec4 color = glm::vec4(1,1,1,1);
    glm::vec3 normal = glm::vec3(0,0,0);
    glm::vec2 texcoords = glm::vec2(0,0);
    glm::vec3 tangent = glm::vec3(1,0,0);

    Vertex() {};
    Vertex(glm::vec3 p) { pos = p; };
    Vertex(glm::vec3 p, glm::vec4 c) { pos = p; color = c; };
    Vertex(glm::vec3 p, glm::vec4 c, glm::vec3 n) { pos = p; color = c; normal = n; };
};

struct PointLight {
    glm::vec4 pos = glm::vec4(0,0,0,1);
    glm::vec4 color = glm::vec4(1,1,1,1);
};

vector<Vertex> vertices;

vector<GLuint> elements = { 0, 1, 2, 1, 3, 2 };

glm::mat4 modelMat(1.0);
glm::mat4 viewMat(1.0);
glm::mat4 projMat(1.0);
glm::vec4 eye(-1,1,1,1);
glm::vec4 center(0,0,0,1);
string transformString = "v";
bool leftMouseDown = false;
glm::vec2 lastMousePos(0,0);
float angleX = 0.0;

float shininess = 10.0;

struct FBO {
    unsigned int ID;
    int width;
    int height;
    vector<unsigned int> colorIDs;
    unsigned int depthRBO;

    void clear() {
        ID = 0;
        width = height = 0;
        colorIDs.clear();
        depthRBO = 0;
    };
};

unsigned int createColorAttachment(int width, int height,
                                    int internal, int format,
                                    int type, int texFilter,
                                    int color_attach) {
    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height,
                    0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                            GL_COLOR_ATTACHMENT0 + color_attach,
                            GL_TEXTURE_2D, texID, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

unsigned int createDepthRBO(int width, int height) {
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                        width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, 
                            GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    return rbo;
}

void createFBO(FBO &fboObj, int width, int height) {
    fboObj.clear();
    fboObj.width = width;
    fboObj.height = height;
    glGenFramebuffers(1, &(fboObj.ID));
    glBindFramebuffer(GL_FRAMEBUFFER, fboObj.ID);
    fboObj.colorIDs.push_back(createColorAttachment(width, height,
                            GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
                            GL_LINEAR, 0));
    fboObj.depthRBO = createDepthRBO(width, height);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)
        != GL_FRAMEBUFFER_COMPLETE) {
            cerr << "ERROR: Framebuffer incomplete!" << endl;
            fboObj.clear();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
// OLD VERSION
string vertCode = R"(
#version 430 core

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec3 normal;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat3 normMat;

out vec4 interColor;
out vec4 interPos;
out vec3 interNorm;

void main() {
    vec4 pos = vec4(position, 1.0);
    vec4 vpos = viewMat * modelMat * pos;
    interPos = vpos;
    gl_Position = projMat * vpos;
    interColor = color;
    interNorm = normMat*normal;
}
)";

string fragCode = R"(
#version 430 core

layout(location=0) out vec4 out_color;

in vec4 interColor;
in vec4 interPos;
in vec3 interNorm;

struct PointLight {
    vec4 pos;
    vec4 color;
};
uniform PointLight light;

void main() {
    vec3 N = normalize(interNorm);
    //out_color = interColor; // vec4(0.0, 1.0, 1.0, 1.0);
    vec3 L = vec3(light.pos - interPos);
    float d = length(L);
    float at = 1.0 / (d*d + 1.0);
    //out_color = vec4(at, at, at, 1.0);

    L = normalize(L);

    //N += 1.0;
    //N /= 2.0;
    out_color = vec4(N, 1.0);
}
)";
*/

// Read from file and dump in string
string readFileToString(string filename) {
	// Open file
	ifstream file(filename);
	// Could we open file?
	if(!file || file.fail()) {
		cerr << "ERROR: Could not open file: " << filename << endl;
		const char *m = ("ERROR: Could not open file: " + filename).c_str();
		throw runtime_error(m);
	}

	// Create output stream to receive file data
	ostringstream outS;
	outS << file.rdbuf();
	// Get actual string of file contents
	string allS = outS.str();
	// Close file
	file.close();
	// Return string
	return allS;
}

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
        else if(key == GLFW_KEY_Z) {
            glm::mat4 R = glm::rotate(glm::radians(5.0f),glm::vec3(0,1,0));
            modelMat = R*modelMat;
            transformString = "Ry(5)*" + transformString;
        }
        else if(key == GLFW_KEY_C) {
            glm::mat4 R = glm::rotate(glm::radians(-5.0f),glm::vec3(0,1,0));
            modelMat = R*modelMat;
            transformString = "Ry(-5)*" + transformString;
        }
        else if(key == GLFW_KEY_9) {
            shininess *= 2.0;
        }
        else if(key == GLFW_KEY_8) {
            shininess /= 2.0;
            if(shininess < 1.0)
                shininess = 1.0;
        }

        printRM("Model:", modelMat);
        cout << transformString << endl;
    }
}

static void mouse_button_callback(  GLFWwindow *window, 
                                    int button, 
                                    int action, 
                                    int mods) {

    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        if(action == GLFW_PRESS) {
            leftMouseDown = true;
            cout << "LEFT MOUSE DOWN" << endl;
        }
        else if(action == GLFW_RELEASE) {
            leftMouseDown = false;
            cout << "LEFT MOUSE UP" << endl;
        }
    }
}

static void mouse_cursor_callback(GLFWwindow *window,
                                    double xpos,
                                    double ypos) {
    if(leftMouseDown) {
        cout << "MOUSE: " << xpos << "," << ypos << endl;
    }

    glm::vec2 curMousePos = glm::vec2(xpos, ypos);
    curMousePos -= lastMousePos;
    int fw, fh;
    glfwGetFramebufferSize(window, &fw, &fh);
    if(fw > 0 && fh > 0) {
        curMousePos.x /= fw;
        curMousePos.y /= fh;
        
        // DO SOMETHING
        float rotScale = 180.0;
        angleX += rotScale*curMousePos.y;
    }

    lastMousePos = glm::vec2(xpos, ypos);
}

void computeOneNormal(vector<Vertex> &v, int i1, int i2, int i3) {
    glm::vec3 p1 = v.at(i1).pos;
    glm::vec3 p2 = v.at(i2).pos;
    glm::vec3 p3 = v.at(i3).pos;

    glm::vec3 v1 = p2 - p1;
    glm::vec3 v2 = p3 - p1;
    glm::vec3 n = glm::normalize(glm::cross(v1, v2));

    v.at(i1).normal += n;
    v.at(i2).normal += n;
    v.at(i3).normal += n;
}

void computeAllNormals(vector<Vertex> &v, vector<GLuint> &ind) {
    for(int i = 0; i < ind.size(); i += 3) {
        computeOneNormal(v, ind.at(i), ind.at(i+1), ind.at(i+2));
    }

    for(int i = 0; i < v.size(); i++) {
        v.at(i).normal = glm::normalize(v.at(i).normal);
    }
}

void makeCylinder(vector<Vertex> &v, vector<GLuint> &ind,
                    float length, float radius, int faceCnt) {

        float angleInc = glm::pi<float>()*2.0 / faceCnt;
        v.clear();
        ind.clear();
        float halfLen = length/2.0;

        for(int i = 0; i < faceCnt; i++) {
            float angle = i*angleInc;
            float y = radius*sin(angle);
            float z = radius*cos(angle);
            glm::vec3 left = glm::vec3(-halfLen, y, z);
            glm::vec3 right = glm::vec3(+halfLen, y, z);

            Vertex leftV, rightV;
            leftV.pos = left;
            leftV.color = glm::vec4(1,0,0,1);
            leftV.texcoords = glm::vec2(0, ((float)i)/2.0);

            rightV.pos = right;
            rightV.color = glm::vec4(0,1,0,1);
            rightV.texcoords = glm::vec2(2, ((float)i)/2.0);

            v.push_back(leftV);
            v.push_back(rightV);
        }

        int tf = 2*faceCnt;
        for(int i = 0; i < faceCnt; i++) {
            int k = i*2;
            int i1 = (k)%tf;
            int i2 = (k+1)%tf;
            int i3 = (k+2)%tf;
            int i4 = (k+3)%tf;
            
            ind.push_back(i1);
            ind.push_back(i2);
            ind.push_back(i3);

            ind.push_back(i2);
            ind.push_back(i4);
            ind.push_back(i3);
        }

        computeAllNormals(v, ind);
}

unsigned int loadAndCreateTexture(string filename) {
    int twidth, theight, tnumc;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* tex_image = stbi_load(filename.c_str(), &twidth, &theight, &tnumc, 0);

    if(!tex_image) {
        cout << "COULD NOT LOAD TEXTURE: " << filename << endl;
        glfwTerminate();
        exit(1);
    }

    GLenum format;
    if(tnumc == 3) {
        format = GL_RGB;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    else if(tnumc == 4) {
        format = GL_RGBA;
    }
    else {
        cout << "UNKNOWN NUMBER OF CHANNELS: " << tnumc << endl;
        glfwTerminate();
        exit(1);
    }

    unsigned int textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, twidth, theight, 0, format, 
                    GL_UNSIGNED_BYTE, tex_image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    stbi_image_free(tex_image);

    return textureID;
}

GLuint loadAndCreateShader(string vertFile, string fragFile) {
    GLuint vertID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);

    string vertCode = readFileToString(vertFile);
    string fragCode = readFileToString(fragFile);

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

    return progID;
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

    vertices.push_back(Vertex(glm::vec3(-0.4f, -0.3f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(0.3f, -0.3f, 0.0f),
                                glm::vec4(0,1,0,1)));
    vertices.push_back(Vertex(glm::vec3(-0.3f, 0.3f, 0.0f)));
    vertices.push_back(Vertex(glm::vec3(0.3f, 0.3f, 0.0f)));

    vector<Vertex> cylinderVert;
    vector<GLuint> cylinderInd;
    makeCylinder(cylinderVert, cylinderInd, 1.0, 0.5, 10);

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

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    lastMousePos = glm::vec2(mx, my);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);

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

    string filename = "test.jpg";
    unsigned int textureID = loadAndCreateTexture(filename);
    unsigned int normalID = loadAndCreateTexture("./NormalMap.png");
    
    GLuint progID = loadAndCreateShader("Basic.vs", "Basic.fs");
    GLuint quadProgID = loadAndCreateShader("Quad.vs", "Quad.fs");

    GLint modelMatLoc = glGetUniformLocation(progID, "modelMat");
    GLint viewMatLoc = glGetUniformLocation(progID, "viewMat");
    GLint projMatLoc = glGetUniformLocation(progID, "projMat");
    GLint normMatLoc = glGetUniformLocation(progID, "normMat");
    cout << modelMatLoc << " ";
    cout << viewMatLoc << " ";
    cout << projMatLoc << " ";
    cout << normMatLoc << endl;

    GLint screenTexLoc = glGetUniformLocation(quadProgID, "screenTexture");

    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, vertOnly.size()*sizeof(GLfloat),
    //                vertOnly.data(), GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
    //                    vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, cylinderVert.size() * sizeof(Vertex),
                        cylinderVert.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
   

    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex),
                            (void*)offsetof(Vertex, pos));
    glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex),
                            (void*)offsetof(Vertex, color));
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex),
                            (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof(Vertex),
                            (void*)offsetof(Vertex, texcoords));
    glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(Vertex),
                            (void*)offsetof(Vertex, tangent));

    GLuint EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(GLuint),
    //                     elements.data(), GL_STATIC_DRAW); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinderInd.size()*sizeof(GLuint),
                         cylinderInd.data(), GL_STATIC_DRAW);  

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    PointLight light;
    light.pos = glm::vec4(0, 0.5, 0.5, 1);
    GLint lightPosLoc = glGetUniformLocation(progID, "light.pos");
    GLint lightColorLoc = glGetUniformLocation(progID, "light.color");
    cout << lightPosLoc << " " << lightColorLoc << endl;

    GLint shinyLoc = glGetUniformLocation(progID, "shininess");
    cout << "SHINY: " << shinyLoc << endl;

    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    FBO fboObj;
    createFBO(fboObj, frameWidth, frameHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalID);

    GLint uniformTextureID = glGetUniformLocation(progID, "diffuseTexture");
    GLint uniformNormalTextureID = glGetUniformLocation(progID,
                                                    "normalTexture");


    Vertex q1, q2, q3, q4, q5, q6;
    q1.pos = glm::vec3(-1,-1,0);        // Lower-left
    q1.texcoords = glm::vec2(0,0);

    q2.pos = glm::vec3(1,-1,0);        // Lower-right
    q2.texcoords = glm::vec2(1,0);

    q3.pos = glm::vec3(-1,1,0);        // Upper-left
    q3.texcoords = glm::vec2(0,1);

    q4.pos = glm::vec3(1,-1,0);        // Lower-right
    q4.texcoords = glm::vec2(1,0);

    q5.pos = glm::vec3(1,1,0);        // Upper-right
    q5.texcoords = glm::vec2(1,1);

    q6.pos = glm::vec3(-1,1,0);
    q6.texcoords = glm::vec2(0,1);      // Upper-left

    vector<Vertex> quadPoints = {q1,q2,q3,q4,q5,q6};

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, quadPoints.size()*sizeof(Vertex), 
                    quadPoints.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex),
                            (void*)offsetof(Vertex, pos));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex),
                            (void*)offsetof(Vertex, texcoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);               



    
    // DRAWING / MAIN RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        // FIRST PASS ////////////////////////////////////////////
        glBindFramebuffer(GL_FRAMEBUFFER, fboObj.ID);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalID);
        
        glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
        glViewport(0,0,frameWidth,frameHeight);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(progID);

        glUniform1i(uniformTextureID, 0);
        glUniform1i(uniformNormalTextureID, 1);
        glUniform1f(shinyLoc, shininess);

        

        glm::mat4 R = glm::rotate(glm::radians(angleX), glm::vec3(0,1,0));
        glm::mat4 modModelMat = modelMat * R; //  
        glUniformMatrix4fv(modelMatLoc, 1, false, glm::value_ptr(modModelMat));

        viewMat = glm::lookAt(glm::vec3(eye), glm::vec3(center), glm::vec3(0,1,0));
        glUniformMatrix4fv(viewMatLoc, 1, false, glm::value_ptr(viewMat));

        float fov = glm::radians(90.0f);
        float aspectRatio = 1.0;
        if(frameHeight > 0) {
            aspectRatio = ((float)frameWidth)/((float)frameHeight);
        }
        float near = 0.01f;
        float far = 1000.0f;
        projMat = glm::perspective(fov, aspectRatio, near, far);
        glUniformMatrix4fv(projMatLoc, 1, false, glm::value_ptr(projMat));

        glm::vec4 curLightPos = viewMat * light.pos;
        glUniform4fv(lightPosLoc, 1, glm::value_ptr(curLightPos));
        glUniform4fv(lightColorLoc, 1, glm::value_ptr(light.color));

        glm::mat4 modelView = viewMat * modModelMat;
        glm::mat3 normMat = glm::transpose(glm::inverse(glm::mat3(modelView)));
        glUniformMatrix3fv(normMatLoc, 1, false, glm::value_ptr(normMat));

        // Draw stuff
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, cylinderInd.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        
        // SECOND PASS ////////////////////////////////////////////
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(quadProgID);
        glUniform1i(screenTexLoc, 0);
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboObj.colorIDs.at(0));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        this_thread::sleep_for(chrono::milliseconds(15));
    }

    glDeleteFramebuffers(1, &(fboObj.ID));

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &textureID);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &EBO);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);

    glUseProgram(0);
    glDeleteProgram(progID);
    glDeleteProgram(quadProgID);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}