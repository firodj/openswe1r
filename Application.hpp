// File: Application.hpp

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class Application {
public:
  void Init();
  void Finish();
  void Run(void* user_data);
  
  void MakeContextCurrent();
  void CompileShader();
  void CreateQuad();
  static void CheckGLError(const char *file, int line);
  
  static void OnErrorCallback(int error, const char* description);
  static void OnKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void OnFramebufferSizeCallback(GLFWwindow* window, int width, int height);
  
  static Application* Get();
  GLFWwindow* window() { return window_; }
  
  bool LockGL(int ms);
  void UnlockGL();
  
private:
  GLFWwindow *window_;
  GLuint quad_shader_program_, quad_vao_, quad_vbo_;
};
