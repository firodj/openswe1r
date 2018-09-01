// Game.cpp

#include "Application.hpp"

#include <cassert>
#include <iostream>

static Application* current_application_ = nullptr;

void Application::Init()
{
  current_application_ = this;

  glfwSetErrorCallback(Application::OnErrorCallback);
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW!" << std::endl;
    return;
  }
 
  int major, minor, revision;
  glfwGetVersion(&major, &minor, &revision);
  std::cout << "Running against GLFW " << major << "." << minor << "." << revision << std::endl;
  
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.3+ only
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif
  
  int w = 1024;
  int h = 768;
  
  window_ = glfwCreateWindow(w, h, "OpenSWE1R", NULL, NULL);
  assert(window_ != NULL);
  
  glfwSetKeyCallback(window_, Application::OnKeyboardCallback);
  
  glfwMakeContextCurrent(window_);

  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  
  glfwGetFramebufferSize(window_, &w, &h);
  glViewport(0, 0, w, h);
}

Application* Application::Get()
{
  return current_application_;
}

void Application::Finish()
{
  glfwDestroyWindow(window_);
  
  glfwTerminate();
}

void Application::OnErrorCallback(int error, const char* description)
{
  std::cout << "Glfw Error " << error << ": " << description << std::endl;
}

void Application::OnKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  auto application = Application::Get();
  if (!application) return;
  if (!(application->window() == window)) return;
  
  // TODO:
}

void Application::Run()
{
  while (!glfwWindowShouldClose(window_))
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}
