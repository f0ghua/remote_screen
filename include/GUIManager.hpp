/*
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期五 09/08/2019 15:18.
 * Last Revision: 星期五 09/08/2019 15:18.
 */                     

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Shader.hpp>
#include <cstring>
#include <mutex>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

typedef unsigned char uchar;

class GUIManager
{
private:
    GLFWwindow* window;
    Shader textshader;
    GLuint VAO;
    GLuint VBO;
    GLuint texture;

    int m_height;
    int m_width;

public:
    GUIManager();
    ~GUIManager();
    bool CreateWindow(const int height, const int width, const std::string title);
    bool is_Window_Created();
    void updateFrameData(uchar* data, const int height, const int width);
    void Draw();
    bool shouldclose();

    void set_current_size(const int height, const int width);
    inline GLFWwindow* getWindow(){ return window;}
};


#endif   /* End of GUIMANAGER.H */




