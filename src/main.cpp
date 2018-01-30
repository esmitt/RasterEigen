// This example is heavily based on the tutorial at https://open.gl

////////////////////////////////////////////////////////////////////////////////
// OpenGL Helpers to reduce the clutter
#include "helpers.h"
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
// Linear Algebra Library
#include <Eigen/Dense>
// Timer
#include <chrono>
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
using namespace std;
// VertexBufferObject wrapper
VertexBufferObject VBO;
enum class MODE {NONE_MODE, INSERT_MODE, TRANSLATION_MODE, DELETE_MODE};
MODE currentMode = MODE::NONE_MODE;
bool isDragging = false;
int nPoints = 0;
bool operation = false;
// Contains the vertex positions
Eigen::MatrixXf V(2,3);
Eigen::Vector2i previous(0,0);

struct Point
{
  double x, y;
};

bool isInside(Eigen::MatrixXf V, double xworld, double yworld)
{
  //check if point lies inside
  Point p1; p1.x = V(0); p1.y = V(1);
  Point p2; p2.x = V(2); p2.y = V(3);
  Point p3; p3.x = V(4); p3.y = V(5);
  Point p; p.x = xworld; p.y = yworld;
  float alpha = ((p2.y - p3.y)*(p.x - p3.x) + (p3.x - p2.x)*(p.y - p3.y)) /
    ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
  float beta = ((p3.y - p1.y)*(p.x - p3.x) + (p1.x - p3.x)*(p.y - p3.y)) /
    ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
  float gamma = 1.0f - alpha - beta;
  return (alpha > 0 && beta > 0 && gamma > 0);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// Get the position of the mouse in the window
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Get the size of the window
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Convert screen position to world coordinates
	double xworld = ((xpos/double(width))*2)-1;
	double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

	// Update the position of the first vertex if the left button is pressed
  if (currentMode == MODE::INSERT_MODE) 
  {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
      if (action == GLFW_PRESS)
      {
        if (nPoints == 0)
        {
          V.col(nPoints) << xworld, yworld;
          V.col(nPoints + 1) << xworld, yworld;
          V.col(nPoints + 2) << xworld, yworld;
          isDragging = true;
          nPoints = 1;
        }
        else if(nPoints == 1)
        {
          V.col(nPoints) << xworld, yworld;
          V.col(nPoints + 1) << xworld, yworld;
          nPoints = 2;
        }
        else if(nPoints == 2)
        {
          V.col(nPoints) << xworld, yworld;
          nPoints = 0;
          isDragging = false;
        }
      }
    }
  }
  else  if (currentMode == MODE::TRANSLATION_MODE)
  {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
      if (!isDragging) 
      {
        if(isInside(V, xworld, yworld))
        {
          isDragging = true;
          previous[0] = xpos;
          previous[1] = ypos;
        }
      }
      else
      {
        isDragging = false;
      }
    }
  }
  else  if (currentMode == MODE::DELETE_MODE) 
  {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && isInside(V, xworld, yworld))
    {
      V.col(0) << 0, 0;
      V.col(1) << 0, 0;
      V.col(2) << 0, 0;
    }
  }
	// Upload the change to the GPU
  VBO.update(V);
}

// Callback function to handle mouse movement
void handleMouseMove(GLFWwindow *window, double mouseX, double mouseY) 
{
  if (isDragging)
  {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    double xworld = ((xpos / double(width)) * 2) - 1;
    double yworld = (((height - 1 - ypos) / double(height)) * 2) - 1; // NOTE: y axis is flipped in glfw

    if (currentMode == MODE::INSERT_MODE) 
    {
      if (nPoints == 1)
      {
        V.col(nPoints) << xworld, yworld;
        V.col(nPoints + 1) << xworld, yworld;
      }
      else if (nPoints == 2)
      {
        V.col(nPoints) << xworld, yworld;
      }
    }
    else if (currentMode == MODE::TRANSLATION_MODE) 
    {
      int shiftX = xpos - previous[0];
      int shiftY = previous[1] - ypos;
      previous(0) = xpos;
      previous(1) = ypos;
      
      double xPosShift = shiftX*0.004;
      double yPosShift = shiftY*0.004;
      
      V(0) += xPosShift;
      V(1) += yPosShift;
      V(2) += xPosShift;
      V(3) += yPosShift;
      V(4) += xPosShift;
      V(5) += yPosShift;
    }
    VBO.update(V);
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Update the position of the first vertex if the keys 1,2, or 3 are pressed
  Eigen::Rotation2Df r1(10 * 3.14159265358 / 180.0);
  Eigen::Rotation2Df r2(-10 * 3.14159265358 / 180.0);
  Eigen::MatrixXf center(2, 3);
  double xcenter = (V(0) + V(2) + V(4)) / 3.0;
  double ycenter = (V(1) + V(3) + V(5)) / 3.0;
  center.col(0) << xcenter, ycenter;
  center.col(1) << xcenter, ycenter;
  center.col(2) << xcenter, ycenter;
	switch (key) {
		case GLFW_KEY_1:
			V.col(0) << -0.5,  0.5;
			break;
		case GLFW_KEY_2:
			V.col(0) << 0,  0.5;
			break;
		case GLFW_KEY_3:
			V.col(0) << 0.5,  0.5;
			break;
    case GLFW_KEY_I:
      currentMode = MODE::INSERT_MODE;
      break;
    case GLFW_KEY_O:
      currentMode = MODE::TRANSLATION_MODE;
      break;
    case GLFW_KEY_P:
      currentMode = MODE::DELETE_MODE;
      break;
    case GLFW_KEY_H:
        V = r1.toRotationMatrix() * (V - center);
        V += center;
      break;
    case GLFW_KEY_J:
        V = r2.toRotationMatrix() * (V - center);
        V += center;
      break;
    case GLFW_KEY_K:
        V = Eigen::Scaling(0.75f) * (V - center);
        V += center;
      break;
    case GLFW_KEY_L:
        V = Eigen::Scaling(1.25f) * (V - center);
        V += center;
      break;
		default:
			break;
	}

	// Upload the change to the GPU
	VBO.update(V);
}

int main(void) {
	// Initialize the GLFW library
	if (!glfwInit()) {
		return -1;
	}

	// Activate supersampling
	glfwWindowHint(GLFW_SAMPLES, 8);

	// Ensure that we get at least a 3.2 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a windowed mode window and its OpenGL context
	GLFWwindow * window = glfwCreateWindow(640, 480, "[Float] Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Load OpenGL and its extensions
	if (!gladLoadGL()) {
		printf("Failed to load OpenGL and its extensions");
		return(-1);
	}
	printf("OpenGL Version %d.%d loaded", GLVersion.major, GLVersion.minor);

	int major, minor, rev;
	major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
	printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
	printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
	printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Initialize the VAO
	// A Vertex Array Object (or VAO) is an object that describes how the vertex
	// attributes are stored in a Vertex Buffer Object (or VBO). This means that
	// the VAO is not the actual object storing the vertex data,
	// but the descriptor of the vertex data.
	VertexArrayObject VAO;
	VAO.init();
	VAO.bind();

	// Initialize the VBO with the vertices data
	// A VBO is a data container that lives in the GPU memory
	VBO.init();

	V.resize(2,3);
	V << 0,  0.5, -0.5, 0.5, -0.5, -0.5;
	VBO.update(V);

	// Initialize the OpenGL Program
	// A program controls the OpenGL pipeline and it must contains
	// at least a vertex shader and a fragment shader to be valid
	Program program;
	const GLchar* vertex_shader = R"(
		#version 150 core

		in vec2 position;

		void main() {
			gl_Position = vec4(position, 0.0, 1.0);
		}
	)";

	const GLchar* fragment_shader = R"(
		#version 150 core

		uniform vec3 triangleColor;
		out vec4 outColor;

		void main() {
		    outColor = vec4(triangleColor, 1.0);
		}
	)";

	// Compile the two shaders and upload the binary to the GPU
	// Note that we have to explicitly specify that the output "slot" called outColor
	// is the one that we want in the fragment buffer (and thus on screen)
	program.init(vertex_shader, fragment_shader, "outColor");
	program.bind();

	// The vertex shader wants the position of the vertices as an input.
	// The following line connects the VBO we defined above with the position "slot"
	// in the vertex shader
	program.bindVertexAttribArray("position", VBO);

	// Save the current time --- it will be used to dynamically change the triangle color
	auto t_start = std::chrono::high_resolution_clock::now();

	// Register the keyboard callback
	glfwSetKeyCallback(window, key_callback);

	// Register the mouse callback
	glfwSetMouseButtonCallback(window, mouse_button_callback);
  
  glfwSetCursorPosCallback(window, handleMouseMove);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		// Bind your VAO (not necessary if you have only one)
		VAO.bind();

		// Bind your program
		program.bind();

		// Set the uniform value depending on the time difference
		//auto t_now = std::chrono::high_resolution_clock::now();
		//float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
    if (isDragging)
      glUniform3f(program.uniform("triangleColor"), 0, 0, 1);
    else
      glUniform3f(program.uniform("triangleColor"), 1, 0, 0);

		// Clear the framebuffer
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw a triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Deallocate opengl memory
	program.free();
	VAO.free();
	VBO.free();

	// Deallocate glfw internals
	glfwTerminate();
	return 0;
}
