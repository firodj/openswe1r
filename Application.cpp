// Application.cpp

#include "Application.hpp"

#include <cassert>
#include <iostream>
#include <mutex>

#include "Game.hpp"


static Application* current_application = nullptr;

std::timed_mutex mutex_gl;

static const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTexCoords;\n"
    "out vec2 TexCoords;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "  TexCoords = aTexCoords;\n"
    "}";

static const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoords;\n"
    "uniform sampler2D screenTexture;\n"
    "void main()\n"
    "{\n"
    "  FragColor = texture(screenTexture, TexCoords);\n"
    "}";

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
  
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.3+ only
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
  
  int w = 1024;
  int h = 768;
  
  window_ = glfwCreateWindow(w, h, "OpenSWE1R", NULL, NULL);
  assert(window_ != NULL);
  
  glfwSetWindowUserPointer(window_, reinterpret_cast<void*>(this));
  
  glfwSetKeyCallback(window_, Application::OnKeyboardCallback);
  glfwSetFramebufferSizeCallback(window_, Application::OnFramebufferSizeCallback);
  
  MakeContextCurrent();
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  
  glfwGetFramebufferSize(window_, &w, &h);
  glViewport(0, 0, w, h);
  
  CreateQuad();
  CompileShader();
}

void Application::CreateQuad()
{
  float top = 1.0f, left = -1.0f, right = 0.0f, bottom = 0.0f;
  
  float quad_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        left , top   ,  0.0f, 1.0f,
        left , bottom,  0.0f, 0.0f,
        right, bottom,  1.0f, 0.0f,

        left , top   ,  0.0f, 1.0f,
        right, bottom,  1.0f, 0.0f,
        right, top   ,  1.0f, 1.0f
    };
  
  glGenVertexArrays(1, &quad_vao_);
  glGenBuffers(1, &quad_vbo_);
  glBindVertexArray(quad_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindVertexArray(0);
}

void Application::CompileShader()
{
  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  
  // link shaders
  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  
  quad_shader_program_ = shaderProgram;
}

Application* Application::Get()
{
  return current_application;
}

Game* Application::CurrentGame()
{
  return current_application->game();
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
  
  glViewport(0, 0, width, height);
  
  std::cout << "OnFramebufferSizeCallback" << std::endl;
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
  
  double lastTime = glfwGetTime();
  int nbFrames = 0;
  
  while (!glfwWindowShouldClose(window_))
  {
    glfwPollEvents();
    
    /**
    if (game) {
      game->Render();
    }
    */
    
    //if (!LockGL(0)) continue;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (game) {
      glUseProgram(quad_shader_program_);
      glUniform1i(glGetUniformLocation(quad_shader_program_, "screenTexture"), 0);
  
      glBindVertexArray(quad_vao_);
      glDisable(GL_DEPTH_TEST);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, game->tex());
      glDrawArrays(GL_TRIANGLES, 0, 6);
      
      if (game->render_full()) game->ClearRenderFull();
    }
    //UnlockGL();
    
    // Measure speed
    double currentTime = glfwGetTime();
    nbFrames++;
    
    if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
      // printf and reset timer
      std::cout << 1000.0/double(nbFrames) << " ms/frame" << std::endl;
      nbFrames = 0;
      lastTime += 1.0;
    }
    
    glfwSwapBuffers(window_);
    
  }
}
