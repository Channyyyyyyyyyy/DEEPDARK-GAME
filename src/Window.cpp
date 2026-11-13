#include "Window.h"
#include <algorithm>
#include <cstdio>
bool Window::create(int w, int h, const char* title){
  if(!glfwInit()) return false;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);   // B1
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  handle = glfwCreateWindow(w, h, title, nullptr, nullptr);
  if(!handle){ fprintf(stderr, "Could not create an OpenGL 4.6 window. Update your graphics driver.\n"); return false; }
  glfwMakeContextCurrent(handle); glfwSwapInterval(1);
  glewExperimental = GL_TRUE; if(glewInit() != GLEW_OK) return false;
  glfwGetFramebufferSize(handle, &width, &height);
  glfwSetWindowUserPointer(handle, this);
  glfwSetKeyCallback(handle, keyCb); glfwSetCursorPosCallback(handle, mouseCb); glfwSetFramebufferSizeCallback(handle, sizeCb);
  glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  return true;
}
void Window::poll(){ pressed.clear(); mdx = mdy = 0; glfwPollEvents(); }
void Window::keyCb(GLFWwindow* w, int key, int, int act, int){
  Window* self = (Window*)glfwGetWindowUserPointer(w);
  if(act == GLFW_PRESS){ if(key == GLFW_KEY_ESCAPE) self->close(); else self->pressed.push_back(key); }
}
void Window::mouseCb(GLFWwindow* w, double x, double y){
  Window* self = (Window*)glfwGetWindowUserPointer(w);
  if(self->first){ self->lx = x; self->ly = y; self->first = false; }
  self->mdx += x - self->lx; self->mdy += y - self->ly; self->lx = x; self->ly = y;
}
void Window::sizeCb(GLFWwindow* w, int fw, int fh){
  Window* self = (Window*)glfwGetWindowUserPointer(w); self->width = std::max(fw, 1); self->height = std::max(fh, 1);
}
