#pragma once
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>

#include <ImGui/implot.h>

#include <iostream>


//setup of gui system returns true on success
bool setupGui();

//render the gui. returns false on close
bool renderGui();