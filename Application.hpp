// File: Game.hpp

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class Application {
public:
  void Init();
  void Finish();
  void Run();
  
  static void OnErrorCallback(int error, const char* description);
  static void OnKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static Application* Get();
  GLFWwindow* window() { return window_; }
  
private:
  GLFWwindow *window_;
};
