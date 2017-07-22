/*

Book:      	Game and Graphics Programming for iOS and Android with OpenGL(R) ES 2.0
Author:    	Romain Marucchi-Foino
ISBN-10: 	1119975913
ISBN-13: 	978-1119975915
Publisher: 	John Wiley & Sons	

Copyright (C) 2011 Romain Marucchi-Foino

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of
this software. Permission is granted to anyone who either own or purchase a copy of
the book specified above, to use this software for any purpose, including commercial
applications subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that
you wrote the original software. If you use this software in a product, an acknowledgment
in the product would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be misrepresented
as being the original software.

3. This notice may not be removed or altered from any source distribution.

*/

#include "templateApp.h"

/* The main structure of the template. This is a pure C struct, you initialize the structure
   as demonstrated below. Depending on the type of your type of app simply comment / uncomment
   which event callback you want to use. */

TEMPLATEAPP templateApp = {
							/* Will be called once when the program start. */
							templateAppInit,
							
							/* Will be called every frame. This is the best location to plug your drawing. */
							templateAppDraw,
							
							/* This function will be triggered when a new touche is recorded on screen. */
							templateAppToucheBegan,
							
							/* This function will be triggered when an existing touche is moved on screen. */
							templateAppToucheMoved,
							
							/* This function will be triggered when an existing touche is released from the the screen. */
							templateAppToucheEnded,
							
							/* This function will be called everytime the accelerometer values are refreshed. Please take
							not that the accelerometer can only work on a real device, and not on the simulator. In addition
							you will have to turn ON the accelerometer functionality to be able to use it. This will be
							demonstrated in the book later on. */
							//templateAppAccelerometer // Turned off by default.
						  };


PROGRAM* program;
int WIDTH;
int HEIGHT;

void templateAppInit( int width, int height )
{
	WIDTH = width;
	HEIGHT = height;
	// Setup the exit callback function.
	atexit( templateAppExit );
	
	// Initialize GLES.
	GFX_start();
	
	// Setup a GLES viewport using the current width and height of the screen.
	glViewport( 0, 0, width, height );
	
	/* Insert your initialization code here */

	program = PROGRAM_init("DEFAULT");
	program->fragment_shader = SHADER_init("color.frag", GL_FRAGMENT_SHADER);
	program->vertex_shader = SHADER_init("color.vert", GL_VERTEX_SHADER);

	MEMORY* m = mopen("color.frag", 1);
	if (!SHADER_compile(program->fragment_shader, (char*)m->buffer, 1)) {
		printf("color.frag compile error");
		exit(1);
	}
	mclose(m);

	m = mopen("color.vert", 1);
	if (!SHADER_compile(program->vertex_shader, (char*)m->buffer, 1)) {
		printf("color.vert compile error");
		exit(2);
	}
	mclose(m);

	if (!PROGRAM_link(program, 1)) {
		printf("link error");
		exit(3);
	}
}


void templateAppDraw( void )
{
	/* Insert your drawing code here */
	static const float POSITION[8] = {
		0, 0, 
		1, 0, 
		0, 1, 
		1, 1
	};

	static const float COLOR[16] = {
		1, 0, 0, 1,
		0, 1, 0, 1,
		0, 0, 1, 1,
		0, 1, 1, 1
	};

	glClearColor(1, 0., 0., 1);
	// Clear the depth, stencil and colorbuffer.
	glClear(GL_COLOR_BUFFER_BIT);

	GFX_set_matrix_mode(PROJECTION_MATRIX);
	GFX_load_identity();
	GFX_set_orthographic_2d(-WIDTH / 2, WIDTH / 2, -HEIGHT / 2, HEIGHT / 2);
	GFX_translate(-WIDTH / 2, -HEIGHT / 2, 0);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	GFX_set_matrix_mode(MODELVIEW_MATRIX);
	GFX_load_identity();
	GFX_scale(100, 100, 0);

	if (program->pid) {
		char attr, uniform;
		glUseProgram(program->pid);
		uniform = PROGRAM_get_uniform_location(program, (char*)"MVP");
		glUniformMatrix4fv(uniform, 1, GL_FALSE, (float*)GFX_get_modelview_projection_matrix());

		attr = PROGRAM_get_vertex_attrib_location(program, "POSITION");
		glEnableVertexAttribArray(attr);
		glVertexAttribPointer(attr, 2, GL_FLOAT, GL_FALSE, 0, POSITION);

		attr = PROGRAM_get_vertex_attrib_location(program, "COLOR");
		glEnableVertexAttribArray(attr);
		glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, 0, COLOR);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	GFX_error();
}


void templateAppToucheBegan( float x, float y, unsigned int tap_count )
{
	/* Insert code to execute when a new touche is detected on screen. */
	printf("down");
}


void templateAppToucheMoved( float x, float y, unsigned int tap_count )
{
	/* Insert code to execute when a touche move on screen. */
	printf("moving...");
}


void templateAppToucheEnded( float x, float y, unsigned int tap_count )
{
	/* Insert code to execute when a touche is removed from the screen. */
	printf("up");
}


void templateAppAccelerometer( float x, float y, float z )
{
	/* Insert code to execute with the accelerometer values ( when available on the system ). */
}


void templateAppExit( void )
{
	/* Code to run when the application exit, perfect location to free everything. */
	exit(0);
}
