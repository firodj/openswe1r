// Application.cpp

#include "Application.hpp"

#include <cassert>
#include <iostream>
#include <mutex>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Game.hpp"

static Application* current_application = nullptr;

std::timed_mutex mutex_gl;

static const char* glsl_version = "#version 330 core";

void Application::Init()
{
  current_application = this;

  glfwSetErrorCallback(Application::OnErrorCallback);
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW!" << std::endl;
    return;
  }
 
  int major, minor, revision;
  glfwGetVersion(&major, &minor, &revision);
  std::cout << "Running against GLFW " << major << "." << minor << "." << revision << std::endl;
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.3+ only
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
  
  window_width_ = 1024;
  window_height_ = 768;
  
  window_ = glfwCreateWindow(window_width_, window_height_, "OpenSWE1R", NULL, NULL);
  assert(window_ != NULL);
  
  glfwSetWindowUserPointer(window_, reinterpret_cast<void*>(this));
  
  glfwSetKeyCallback(window_, Application::OnKeyboardCallback);
  glfwSetFramebufferSizeCallback(window_, Application::OnFramebufferSizeCallback);
  
  MakeContextCurrent();
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  
  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Setup style
  ImGui::StyleColorsDark();

  glfwGetFramebufferSize(window_, &screen_width_, &screen_height_);
  glViewport(0, 0, screen_width_, screen_height_);
}

Application* Application::Get()
{
  return current_application;
}

void Application::Finish()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
  
  glfwTerminate();
}

void Application::OnErrorCallback(int error, const char* description)
{
  std::cout << "Glfw Error " << error << ": " << description << std::endl;
}

void Application::OnKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  void* pointer = glfwGetWindowUserPointer(window);
  if (!pointer) return;
  
  auto application = reinterpret_cast<Application*>(pointer);
  
  // TODO:
}

void Application::OnFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  void* pointer = glfwGetWindowUserPointer(window);
  if (!pointer) return;
  
  auto application = reinterpret_cast<Application*>(pointer);
  
  application->screen_width_ = width;
  application->screen_height_ = height;
  glViewport(0, 0, width, height);
}

void Application::MakeContextCurrent()
{
   glfwMakeContextCurrent(window_);
}

bool Application::LockGL(int ms)
{
  if (ms == 0) {
    return mutex_gl.try_lock();
  } else {
    return mutex_gl.try_lock_for(std::chrono::milliseconds(ms));
  }
}

void Application::UnlockGL()
{
  mutex_gl.unlock();
}

void Application::CheckGLError(const char *file, int line)
{
  GLenum err;
  while((err = glGetError()) != GL_NO_ERROR)
  {
    std::string error;

    switch(err) {
      
      case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
      case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
      case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
      case GL_STACK_OVERFLOW:         error="STACK_OVERFLOW";         break;
      case GL_STACK_UNDERFLOW:        error="STACK_UNDERFLOW";        break;
      case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
      case GL_CONTEXT_LOST:           error="GL_CONTEXT_LOST";        break;
      case GL_TABLE_TOO_LARGE:        error="GL_TABLE_TOO_LARGE";     break;
    }

    std::cerr << "GL_" << error.c_str() <<" - "<<file<<":"<<line << std::endl;
  }
}

void Application::Run(Game* game)
{
  game_ = game;
  glfwSwapInterval(1);
  
  while (!glfwWindowShouldClose(window_))
  {
    glfwPollEvents();
    
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
      static float float_f = 0.0f;
      static int counter = 0;
      static bool show_demo_window = true;
      static bool show_another_window = false;
      static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
      
      if (ImGui::BeginMainMenuBar())
      {
          if (ImGui::BeginMenu("File"))
          {
              ImGui::MenuItem("(dummy menu)", NULL, false, false);
              if (ImGui::MenuItem("New")) {}
              if (ImGui::MenuItem("Open", "Ctrl+O")) {}
              ImGui::EndMenu();
          }
          if (ImGui::BeginMenu("Edit"))
          {
              if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
              if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
              ImGui::Separator();
              if (ImGui::MenuItem("Cut", "CTRL+X")) {}
              if (ImGui::MenuItem("Copy", "CTRL+C")) {}
              if (ImGui::MenuItem("Paste", "CTRL+V")) {}
              ImGui::EndMenu();
          }
      }
      ImGui::EndMainMenuBar();
      
      // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
      if (show_demo_window)
          ImGui::ShowDemoWindow(&show_demo_window);

      
    
      if (game) {
        if (ImGui::Begin("StarWars")) {
          ImGui::Image(reinterpret_cast<ImTextureID>(game->tex()), ImVec2(game->window_width(), game->window_height()),
            ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
        }
        ImGui::End();
      }
      
      // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
      if (ImGui::Begin("Hello, world!")) {                         // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &float_f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      }
      ImGui::End();
      
    }
    
    ImGui::Render();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(window_);
    
  }
}
