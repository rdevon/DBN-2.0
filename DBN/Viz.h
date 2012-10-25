//
//  Viz.h
//  DBN
//
//  Created by Devon Hjelm on 7/23/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
#ifndef DBN_Viz_h
#define DBN_Viz_h
#include "Types.h"

#define GLFW_GL3
#include <GL/glfw.h>
#define OPENGL3
#include "opengl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Tex_Unit;
class Plot_Unit;
class Unit_Monitor;
class Feature_to_Data_Monitor;
class Monitor;
class New_Monitor;
class MLP;
class Layer;
class fMRI_Feature_Monitor;
class Border;
class fMRI_Layer_Monitor;
class Reconstruction_Cost_Monitor;
class DBN;
class MNIST_Feature_Monitor;
class MNIST_Layer_Monitor;
class Teacher;
class Visualizer;

void GLFWCALL resize(int width, int height);
void GLFWCALL keypress(int key, int state);
int GLFWCALL close(void);

extern Visualizer *the_viz;
extern Monitor    *the_monitor;

using std::vector;

class Visualizer{
public:
   
   bool                    on;
   bool                    pause;
   
   std::vector<GLuint> _texID;
   std::vector<GLuint>  _lineVAO;   //< Vertex array object for line plots
   std::vector<GLuint>  _lineBO;    //< Vertex buffer object for the position of vertices in the line plots
   
   GLuint _program;             //< Shader program handle
   GLuint _vao;                 //< Vertex array object for the vertices for the textured quad
   GLuint _vertexBuf;           //< Buffer object for the vertices for the textured quad
   GLuint _texCoordBuf;         //< Buffer object for the texture coordinates for the textured quad
   GLint  _vertexLocation;      //< Location of the vertex attribute in the shader program
   GLint  _colorLocation;       //< Location of color!!!
   GLint  _scaleLocation;       //< Location of scale!!!
   GLint  _texCoordLocation;    //< Location of the texture coordinate attribute in the shader program
   GLint  _mvpLocation;         //< model view projection matrix.  This is how we'll move things around in the window
   GLuint _texWeightId;         //< Texture object for the weights
   GLuint _negtexWeightId;
   GLuint _plotId;
   GLuint _colorFilter;
   bool   _linearFilter;        //< TODO
   GLint  _weightSamplerLoc;    //< Location of the weight texture sampler in the fragment program
   std::vector<glm::vec4> _points;   //< List of points for the textured quad
   std::vector<glm::vec2> _texCoords;//< Texture coordinates for the textured quad
   
   Visualizer(){}
   Visualizer(int width, int height){
      open_window(width, height);
      pause = false;
   }
   
   void open_window(int width, int height);
   void close_window();
   void update();
   
   //GL STUFF HERE
   
   void terminate(int exitCode);
   std::string readTextFile(const std::string& filename);
   bool shaderCompileStatus(GLuint shader);
   std::string getShaderLog(GLuint shader);
   bool programLinkStatus(GLuint program);
   std::string getProgramLog(GLuint program);
   GLuint createShader(const std::string& source, GLenum shaderType);
   GLuint createGLSLProgram();
   void init(int num_tex_maps, int num_line_plots);
   void resize_maps(int num_tex_maps, int num_line_plots);
   int draw_texture_map(Tex_Unit *tex_unit, int id);
   int draw_plot(Plot_Unit *plot_unit, int id);
   int draw_border(Border *border, int id);
   int update(Monitor*);
   void toggle_on() {on = !on;}
   void toggle_pause() {pause = !pause;}
   
};


#endif