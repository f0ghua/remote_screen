/*
 * File Type:     C/C++
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期五 09/08/2019 15:39.
 * Last Revision: 星期五 09/08/2019 15:39.
 */

#include <opencv2/opencv.hpp>
#include <GUIManager.hpp>
#include <TCPManager.hpp>

extern TCPManager tcp = TCPManager();

int cur_height, cur_width;

GUIManager::GUIManager(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    window = NULL;
}

GUIManager::~GUIManager(){
    glfwTerminate();
}

bool GUIManager::is_Window_Created(){
    return window != NULL;
}

void GUIManager::set_current_size(const int height, const int width){
    m_height = height;
    m_width = width;
    cur_height = height;
    cur_width = width;
}

bool GUIManager::CreateWindow(const int height=1080, const int width=1920, const std::string title="title"){
    m_height = height;
    m_width = width;
    cur_height = height;
    cur_width = width;
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_motion_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    float textcoor[] = {
        -1, -1, 0, 0, 1,
        -1, 1, 0, 0, 0,
        1, -1, 0, 1, 1,
        1, 1, 0, 1, 0,
        1, -1, 0, 1, 1,
        -1, 1, 0, 0, 0
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textcoor), textcoor, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    textshader = Shader("../shader/tvt.vs", "../shader/tft.fs");
    textshader.use();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void GUIManager::updateFrameData(uchar* data, const int height, const int width){
    textshader.use();
    glBindVertexArray(VAO);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, data);
}

void GUIManager::Draw(){
    textshader.use();
    glBindVertexArray(VAO);

    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool GUIManager::shouldclose(){
    return glfwWindowShouldClose(window);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    cur_height = height;
    cur_width = width;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    //camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //camera.ProcessMouseScroll(yoffset);
    unsigned char t = 0;
    if(yoffset > 0) t = 6;
    else if(yoffset < 0) t = 7;
    tcp.Send_Input(&t, 1);
}
/* EOF */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //if (key == GLFW_KEY_E && action == GLFW_PRESS)
        //activate_airship();
    uchar t[2] = {0};
    t[0] |= 1<<7;
    if(action == GLFW_RELEASE) t[0] |= 1<<6;
    t[1] = scancode;
    tcp.Send_Input(t, 2);

}
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos){
    static double st = cv::getTickCount();
    if( (cv::getTickCount() - st)/cv::getTickFrequency() < 0.2) return;
    st = cv::getTickCount();
    long long data = 0;
    uchar* p = (uchar*)&data;
    p[0] |= 1<<6;
    short tmp = xpos*1.0/cur_width*1920;
    *(short*)(p+1) = tmp;
    tmp = ypos*1.0/cur_height*1080;
    *(short*)(p+3) = tmp;
    tcp.Send_Input((uchar*)&data, 5);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    long long data = 0;
    uchar* p = (uchar*)&data;
    p[0] |= 1<<6;
    short tmp = xpos*1.0/cur_width*1920;
    *(short*)(p+1) = tmp;
    tmp = ypos*1.0/cur_height*1080;
    *(short*)(p+3) = tmp;
    tcp.Send_Input((uchar*)&data, 5);

    uchar t = 0;
    if(action == GLFW_RELEASE){
        switch(button){
            case GLFW_MOUSE_BUTTON_LEFT: t = 1;break;
            case GLFW_MOUSE_BUTTON_MIDDLE: t = 5;break;
            case GLFW_MOUSE_BUTTON_RIGHT: t = 3;break;
        }
    }
    else if(action == GLFW_PRESS){
        switch(button){
            case GLFW_MOUSE_BUTTON_LEFT: t = 0;break;
            case GLFW_MOUSE_BUTTON_MIDDLE: t = 4;break;
            case GLFW_MOUSE_BUTTON_RIGHT: t = 2;break;
        }
    }
    tcp.Send_Input(&t, 1);
}
