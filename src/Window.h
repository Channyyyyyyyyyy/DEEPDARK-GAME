#pragma once
// Owner: R1 (R4 sends input changes to R1, who commits them)
#include "Common.h"
#include <GLFW/glfw3.h>
class Window {
public:
  GLFWwindow* handle = nullptr; int width = 1280, height = 720;
  std::vector<int> pressed;          // keys pressed since the last poll()
  double mdx = 0, mdy = 0;           // mouse movement since the last poll()
  bool create(int w, int h, const char* title);   // GL 4.6 core (B1)
  void poll();
  bool key(int k) const { return glfwGetKey(handle, k) == GLFW_PRESS; }
  bool shouldClose() const { return glfwWindowShouldClose(handle); }
  void close() { glfwSetWindowShouldClose(handle, 1); }
  void swap() { glfwSwapBuffers(handle); }
  void setVsync(bool on) { glfwSwapInterval(on ? 1 : 0); }
  float aspect() const { return (float)width / (float)height; }   // B3: the projection follows the window
private:
  bool first = true; double lx = 0, ly = 0;
  static void keyCb(GLFWwindow*, int, int, int, int);
  static void mouseCb(GLFWwindow*, double, double);
  static void sizeCb(GLFWwindow*, int, int);
};
