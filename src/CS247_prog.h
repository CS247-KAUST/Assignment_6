#ifndef CS247_PROG_H
#define CS247_PROG_H

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// framework includes
#include "glslprogram.h"
#include "vboquad.h"

//////////////////////
//  Global defines  //
//////////////////////
#define TIMER_FREQUENCY_MILLIS  20

//////////////////////
// Global variables //
//////////////////////

// window size
const unsigned int gWindowWidth = 512;
const unsigned int gWindowHeight = 512;

// Handle of the window we're rendering to
static GLFWwindow* window;

char bmModifiers;	// keyboard modifiers (e.g. ctrl,...)

int clearColor;

// data handling
char* filenames[ 3 ];
bool grid_data_loaded;
bool scalar_data_loaded;
unsigned short vol_dim[ 3 ];

vec3* vector_fields;
vec3* vector_fields_tex;
float* scalar_fields;
float* scalar_bounds;
float* noise_field;
float* pink_noise_fields;

GLuint scalar_field_texture_3D;
GLuint vector_field_texture_3D;
GLuint advection_texture;
GLuint noise_field_texture;
GLuint noise_field_texture_3D;


int num_scalar_fields;
int scalar_field_id;

int num_timesteps;
int current_timestep;
float timestep;

int loaded_file;

int view_width, view_height; // height and width of entire view

int toggle_xy;

int enable_autocycle;

int scalar_overlay;

int kernel_size;
float dt;
float blend_factor;
int colormap;
////////////////
// Prototypes //
////////////////
void NextTimestep();
void LoadData( char* base_filename );
void LoadVectorData( const char* filename );
void CreateNoiseField( void );

void InitTextures( void );
void FreeTextures( void );
void DownloadScalarFieldAs3DTexture( void );
void DownloadNoiseFieldAs2DTexture( void );
void DownloadVectorFieldAs3DTexture( void );

void ResetRenderingProperties( void );

// make quad to load texture to
VBOQuad quad;

// GLSL
GLSLProgram LICProgram;
glm::mat4 model;


#endif //CS247_PROG_H
