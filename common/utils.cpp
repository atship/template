/*

GFX Lightweight OpenGLES 2.0 Game and Graphics Engine

Copyright (C) 2011 Romain Marucchi-Foino http://gfx.sio2interactive.com

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of
this software. Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that
you wrote the original software. If you use this software in a product, an acknowledgment
in the product would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be misrepresented
as being the original software.

3. This notice may not be removed or altered from any source distribution.

*/

#include "gfx.h"

/*!
	\file utils.cpp
	
	\brief Miscallenous utility functions that does not bound to any other structure of the engine.
	
	\details Provide utilities function for file path manipulation, time, frustum as well as other
	helper functions used by the engine.
*/

#ifdef _WIN32
int
gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}
#endif

/*!
	Get the current time in micro seconds.
	
	\return Return the time in micro seconds.
*/
unsigned int get_micro_time( void )
{
	struct timeval tv;

	gettimeofday( &tv, NULL );

	return ( tv.tv_sec * 1000000 ) + tv.tv_usec; 
}


/*!
	Get the current time in milli seconds.

	\return Return the time in milli seconds.
*/
unsigned int get_milli_time( void )
{
    struct timeval tv;

	gettimeofday( &tv, NULL );
  
    return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}


/*!
	Replace the \ for / in a an absolute or relative file path.

	\param[in,out] filepath The file path to adjust.
*/
void adjust_file_path( char *filepath )
{
	unsigned int i = 0,
				 l = strlen( filepath );
	
	while( i != l )
	{
		if( filepath[ i ] == '\\' ) filepath[ i ] = '/';
		++i;
	}
}


/*!
	Extract path from a file path.
	
	\param[in] filepath The file path to use to extract the path.
	\param[in,out] path The return path.
*/
void get_file_path( char *filepath, char *path )
{
	char *t = NULL;
	
	unsigned int p;
	
	adjust_file_path( filepath );

	t = strrchr( filepath, '/' );
	
	if( t )
	{
		p = ( t - filepath ) + 1;
		strncpy( path, filepath, p );
		path[ p ] = 0;
	}
}


/*!
	Extract the file name from a file path.
	
	\param[in] filepath The file path to use to extract the file name.
	\param[in,out] filename The file name.
		
*/
void get_file_name( char *filepath, char *filename )
{
	char *t = NULL;
	
	adjust_file_path( filepath );
	
	t = strrchr( filepath, '/' );
	
	if( t ) strcpy( filename, t + 1 );
	else strcpy( filename, filepath );
}


/*!
	Extract the extension of a file. And optionally convert it to uppercase.
	
	\param[in] filepath The file path to use.
	\param[in,out] ext If a valid extension is found this variable will be used as destination to store it.
	\param[in] uppercase Determine wheter or not ext should be converted to uppercase.
*/
void get_file_extension( char *filepath, char *ext, unsigned char uppercase )
{
	char *t = NULL;
	
	adjust_file_path( filepath );
	
	t = strrchr( filepath, '.' );
	
	if( t ) strcpy( ext, t + 1 );
	
	if( uppercase )
	{
		unsigned int i = 0,
					 l = strlen( ext );
		
		while( i != l )
		{
			ext[ i ] = toupper( ext[ i ] );
			++i;
		}
	}
}


/*!
	Helper function allowing you to generate a 24 bits color from an index.
	
	\param[in] index The index to use to generate the color.
	\param[in,out] color The resulting color for this index will be stored in this variable.
*/
void generate_color_from_index( unsigned int index, vec4 *color )
{
	ucol4 tmp_color = { 0, 0, 0, 255 };

	unsigned int i = index >> 16;

	memcpy( &tmp_color.r, &i, 1 );

	i = index >> 8;
	
	memcpy( &tmp_color.g, &i, 1 );

	memcpy( &tmp_color.b, &index, 1 );
	
	color->x = tmp_color.r / 255.0f;
	color->y = tmp_color.g / 255.0f;
	color->z = tmp_color.b / 255.0f;
	color->w = tmp_color.a / 255.0f;
}


/*!
	Cross platform implementation of printf.
	
	\param[in] str The string of text to print.
	\param[in] ... The list of parameters (if any).
*/
void console_print( const char *str, ... )
{
	char tmp[ 1024 ] = {""};
	
	va_list	ap;

	va_start( ap, str );
	{ 
	    vsprintf( tmp, str, ap );
	}
	va_end( ap );


	#ifndef _ANDROID_

		printf( "%s", tmp );
	#else
		__android_log_print( ANDROID_LOG_INFO, "", "%s", tmp );
	#endif	
}


/*!
	Construct a six plane frustum from an arbitrary model view and projection matrix.
	
	\param[in,out] frustum The resulting planes will be stored in this variable.
	\param[in] modelview_matrix A 4x4 matrix representing the model view matrix.
	\param[in] projection_matrix A 4x4 matrix representing the projection matrix.
*/
void build_frustum( vec4 frustum[ 6 ], mat4 *modelview_matrix, mat4 *projection_matrix )
{
	mat4 c;

	float t;
	
	c.m[ 0 ].x = modelview_matrix->m[ 0 ].x * projection_matrix->m[ 0 ].x +
				 modelview_matrix->m[ 0 ].y * projection_matrix->m[ 1 ].x + 
				 modelview_matrix->m[ 0 ].z * projection_matrix->m[ 2 ].x + 
				 modelview_matrix->m[ 0 ].w * projection_matrix->m[ 3 ].x;

	c.m[ 0 ].y = modelview_matrix->m[ 0 ].x * projection_matrix->m[ 0 ].y +
				 modelview_matrix->m[ 0 ].y * projection_matrix->m[ 1 ].y +
				 modelview_matrix->m[ 0 ].z * projection_matrix->m[ 2 ].y +
				 modelview_matrix->m[ 0 ].w * projection_matrix->m[ 3 ].y;
	
	c.m[ 0 ].z = modelview_matrix->m[ 0 ].x * projection_matrix->m[ 0 ].z +
				 modelview_matrix->m[ 0 ].y * projection_matrix->m[ 1 ].z +
				 modelview_matrix->m[ 0 ].z * projection_matrix->m[ 2 ].z +
				 modelview_matrix->m[ 0 ].w * projection_matrix->m[ 3 ].z;

	c.m[ 0 ].w = modelview_matrix->m[ 0 ].x * projection_matrix->m[ 0 ].w +
				 modelview_matrix->m[ 0 ].y * projection_matrix->m[ 1 ].w + 
				 modelview_matrix->m[ 0 ].z * projection_matrix->m[ 2 ].w + 
				 modelview_matrix->m[ 0 ].w * projection_matrix->m[ 3 ].w;

	c.m[ 1 ].x = modelview_matrix->m[ 1 ].x * projection_matrix->m[ 0 ].x + 
				 modelview_matrix->m[ 1 ].y * projection_matrix->m[ 1 ].x + 
				 modelview_matrix->m[ 1 ].z * projection_matrix->m[ 2 ].x +
				 modelview_matrix->m[ 1 ].w * projection_matrix->m[ 3 ].x;
			
	c.m[ 1 ].y = modelview_matrix->m[ 1 ].x * projection_matrix->m[ 0 ].y + 
				 modelview_matrix->m[ 1 ].y * projection_matrix->m[ 1 ].y +
				 modelview_matrix->m[ 1 ].z * projection_matrix->m[ 2 ].y +
				 modelview_matrix->m[ 1 ].w * projection_matrix->m[ 3 ].y;
			
	c.m[ 1 ].z = modelview_matrix->m[ 1 ].x * projection_matrix->m[ 0 ].z +
				 modelview_matrix->m[ 1 ].y * projection_matrix->m[ 1 ].z +
				 modelview_matrix->m[ 1 ].z * projection_matrix->m[ 2 ].z +
				 modelview_matrix->m[ 1 ].w * projection_matrix->m[ 3 ].z;
			
	c.m[ 1 ].w = modelview_matrix->m[ 1 ].x * projection_matrix->m[ 0 ].w +
				 modelview_matrix->m[ 1 ].y * projection_matrix->m[ 1 ].w +
				 modelview_matrix->m[ 1 ].z * projection_matrix->m[ 2 ].w +
				 modelview_matrix->m[ 1 ].w * projection_matrix->m[ 3 ].w;
 
	c.m[ 2 ].x = modelview_matrix->m[ 2 ].x * projection_matrix->m[ 0 ].x +
				 modelview_matrix->m[ 2 ].y * projection_matrix->m[ 1 ].x +
				 modelview_matrix->m[ 2 ].z * projection_matrix->m[ 2 ].x + 
				 modelview_matrix->m[ 2 ].w * projection_matrix->m[ 3 ].x;
			
	c.m[ 2 ].y = modelview_matrix->m[ 2 ].x * projection_matrix->m[ 0 ].y +
				 modelview_matrix->m[ 2 ].y * projection_matrix->m[ 1 ].y +
				 modelview_matrix->m[ 2 ].z * projection_matrix->m[ 2 ].y +
				 modelview_matrix->m[ 2 ].w * projection_matrix->m[ 3 ].y;
			
	c.m[ 2 ].z = modelview_matrix->m[ 2 ].x * projection_matrix->m[ 0 ].z +
				 modelview_matrix->m[ 2 ].y * projection_matrix->m[ 1 ].z +
				 modelview_matrix->m[ 2 ].z * projection_matrix->m[ 2 ].z +
				 modelview_matrix->m[ 2 ].w * projection_matrix->m[ 3 ].z;
			
	c.m[ 2 ].w = modelview_matrix->m[ 2 ].x * projection_matrix->m[ 0 ].w +
				 modelview_matrix->m[ 2 ].y * projection_matrix->m[ 1 ].w +
				 modelview_matrix->m[ 2 ].z * projection_matrix->m[ 2 ].w +
				 modelview_matrix->m[ 2 ].w * projection_matrix->m[ 3 ].w;
 
	c.m[ 3 ].x = modelview_matrix->m[ 3 ].x * projection_matrix->m[ 0 ].x +
				 modelview_matrix->m[ 3 ].y * projection_matrix->m[ 1 ].x +
				 modelview_matrix->m[ 3 ].z * projection_matrix->m[ 2 ].x +
				 modelview_matrix->m[ 3 ].w * projection_matrix->m[ 3 ].x;
			
	c.m[ 3 ].y = modelview_matrix->m[ 3 ].x * projection_matrix->m[ 0 ].y +
				 modelview_matrix->m[ 3 ].y * projection_matrix->m[ 1 ].y +
				 modelview_matrix->m[ 3 ].z * projection_matrix->m[ 2 ].y +
				 modelview_matrix->m[ 3 ].w * projection_matrix->m[ 3 ].y;
			
	c.m[ 3 ].z = modelview_matrix->m[ 3 ].x * projection_matrix->m[ 0 ].z +
				 modelview_matrix->m[ 3 ].y * projection_matrix->m[ 1 ].z +
				 modelview_matrix->m[ 3 ].z * projection_matrix->m[ 2 ].z +
				 modelview_matrix->m[ 3 ].w * projection_matrix->m[ 3 ].z;
			
	c.m[ 3 ].w = modelview_matrix->m[ 3 ].x * projection_matrix->m[ 0 ].w +
				 modelview_matrix->m[ 3 ].y * projection_matrix->m[ 1 ].w +
				 modelview_matrix->m[ 3 ].z * projection_matrix->m[ 2 ].w +
				 modelview_matrix->m[ 3 ].w * projection_matrix->m[ 3 ].w;
 
 
	frustum[ 0 ].x = c.m[ 0 ].w - c.m[ 0 ].x ;
	frustum[ 0 ].y = c.m[ 1 ].w - c.m[ 1 ].x ;
	frustum[ 0 ].z = c.m[ 2 ].w - c.m[ 2 ].x ;
	frustum[ 0 ].w = c.m[ 3 ].w - c.m[ 3 ].x ;
 
	t = 1.0f / sqrtf( frustum[ 0 ].x * frustum[ 0 ].x +
					  frustum[ 0 ].y * frustum[ 0 ].y +
					  frustum[ 0 ].z * frustum[ 0 ].z );

	frustum[ 0 ].x *= t;
	frustum[ 0 ].y *= t;
	frustum[ 0 ].z *= t;
	frustum[ 0 ].w *= t;
 

	frustum[ 1 ].x = c.m[ 0 ].w + c.m[ 0 ].x ;
	frustum[ 1 ].y = c.m[ 1 ].w + c.m[ 1 ].x ;
	frustum[ 1 ].z = c.m[ 2 ].w + c.m[ 2 ].x ;
	frustum[ 1 ].w = c.m[ 3 ].w + c.m[ 3 ].x ;
 
	t = 1.0f / sqrtf( frustum[ 1 ].x * frustum[ 1 ].x +
					  frustum[ 1 ].y * frustum[ 1 ].y +
					  frustum[ 1 ].z * frustum[ 1 ].z );

	frustum[ 1 ].x *= t;
	frustum[ 1 ].y *= t;
	frustum[ 1 ].z *= t;
	frustum[ 1 ].w *= t;

 
	frustum[ 2 ].x = c.m[ 0 ].w + c.m[ 0 ].y ;
	frustum[ 2 ].y = c.m[ 1 ].w + c.m[ 1 ].y ;
	frustum[ 2 ].z = c.m[ 2 ].w + c.m[ 2 ].y ;
	frustum[ 2 ].w = c.m[ 3 ].w + c.m[ 3 ].y ;
 
	t = 1.0f / sqrtf( frustum[ 2 ].x * frustum[ 2 ].x +
					  frustum[ 2 ].y * frustum[ 2 ].y +
					  frustum[ 2 ].z * frustum[ 2 ].z );

	frustum[ 2 ].x *= t;
	frustum[ 2 ].y *= t;
	frustum[ 2 ].z *= t;
	frustum[ 2 ].w *= t;

 
	frustum[ 3 ].x = c.m[ 0 ].w - c.m[ 0 ].y ;
	frustum[ 3 ].y = c.m[ 1 ].w - c.m[ 1 ].y ;
	frustum[ 3 ].z = c.m[ 2 ].w - c.m[ 2 ].y ;
	frustum[ 3 ].w = c.m[ 3 ].w - c.m[ 3 ].y ;
 
	t = 1.0f / sqrtf( frustum[ 3 ].x * frustum[ 3 ].x +
					  frustum[ 3 ].y * frustum[ 3 ].y +
					  frustum[ 3 ].z * frustum[ 3 ].z );

	frustum[ 3 ].x *= t;
	frustum[ 3 ].y *= t;
	frustum[ 3 ].z *= t;
	frustum[ 3 ].w *= t;
 

	frustum[ 4 ].x = c.m[ 0 ].w - c.m[ 0 ].z ;
	frustum[ 4 ].y = c.m[ 1 ].w - c.m[ 1 ].z ;
	frustum[ 4 ].z = c.m[ 2 ].w - c.m[ 2 ].z ;
	frustum[ 4 ].w = c.m[ 3 ].w - c.m[ 3 ].z ;
 
	t = 1.0f / sqrtf( frustum[ 4 ].x * frustum[ 4 ].x +
					  frustum[ 4 ].y * frustum[ 4 ].y +
					  frustum[ 4 ].z * frustum[ 4 ].z );

	frustum[ 4 ].x *= t;
	frustum[ 4 ].y *= t;
	frustum[ 4 ].z *= t;
	frustum[ 4 ].w *= t;


	frustum[ 5 ].x = c.m[ 0 ].w + c.m[ 0 ].z ;
	frustum[ 5 ].y = c.m[ 1 ].w + c.m[ 1 ].z ;
	frustum[ 5 ].z = c.m[ 2 ].w + c.m[ 2 ].z ;
	frustum[ 5 ].w = c.m[ 3 ].w + c.m[ 3 ].z ;
 

	t = 1.0f / sqrtf( frustum[ 5 ].x * frustum[ 5 ].x +
					  frustum[ 5 ].y * frustum[ 5 ].y +
					  frustum[ 5 ].z * frustum[ 5 ].z );

	frustum[ 5 ].x *= t;
	frustum[ 5 ].y *= t;
	frustum[ 5 ].z *= t;
	frustum[ 5 ].w *= t;
}


/*!
	Get the distance of an arbitrary sphere located in space.
		
	\param[in] frustum The six clipping planes data to use to test if the sphere is inside the planes.
	\param[in] location The location of the pivot point of the sphere in world coordinates.
	\param[in] radius The radius of the sphere.

	\return Return the distance of the sphere in the frustum (>0), else return 0.
*/	
float sphere_distance_in_frustum( vec4  *frustum, vec3  *location, float radius )
{
	unsigned int i = 0;
	
	float d;
	
	while( i != 6 )
	{
		d = frustum[ i ].x * location->x + 
			frustum[ i ].y * location->y + 
			frustum[ i ].z * location->z + 
			frustum[ i ].w;
		
		if( d < -radius )
		{ return 0.0f; }
		
		++i;
	}
	
	return d + radius;	
}


/*!
	Get if a point is inside the frustum planes.
		
	\param[in] frustum The six clipping planes data to use to test if the point is inside the planes.
	\param[in] location The location of the point in world coordinates.

	\return Return 1 if the point is inside the frustum, else return 0.
*/
unsigned char point_in_frustum( vec4 *frustum, vec3 *location )
{
	unsigned int i = 0;
	
	while( i != 6 )
	{
		if( frustum[ i ].x * location->x +
			frustum[ i ].y * location->y +
			frustum[ i ].z * location->z +
			frustum[ i ].w < 0.0f )
		{ return 0; }
		
		++i;
	}
	
	return 1;
}


/*!
	Get if a box is inside the frustum planes.
		
	\param[in] frustum The six clipping planes data to use to test if the point is inside the planes.
	\param[in] location The location of the pivot point of the box in world coordinates.
	\param[in] dimension The dimension of the box on the XYZ axis.
	
	\return Return 1 if the point is inside the frustum, else return 0.
*/
unsigned char box_in_frustum( vec4 *frustum, vec3 *location, vec3 *dimension )
{
	unsigned int i = 0;
	
	while( i != 6 )
	{
		if( frustum[ i ].x * ( location->x - dimension->x ) +
			frustum[ i ].y * ( location->y - dimension->y ) +
			frustum[ i ].z * ( location->z - dimension->z ) +
			frustum[ i ].w > 0.0f )
		{
			++i;
			continue;
		}
		
		
		if( frustum[ i ].x * ( location->x + dimension->x ) +
			frustum[ i ].y * ( location->y - dimension->y ) +
			frustum[ i ].z * ( location->z - dimension->z ) +
			frustum[ i ].w > 0.0f )
		{
			++i;
			continue;
		}
		
		
		if( frustum[ i ].x * ( location->x - dimension->x ) +
			frustum[ i ].y * ( location->y + dimension->y ) +
			frustum[ i ].z * ( location->z - dimension->z ) +
			frustum[ i ].w > 0.0f )
		{
			++i;
			continue;
		}
		
		
		if( frustum[ i ].x * ( location->x + dimension->x ) +
			frustum[ i ].y * ( location->y + dimension->y ) +
			frustum[ i ].z * ( location->z - dimension->z ) +
			frustum[ i ].w > 0.0f )
		{
			++i;
			continue;
		}
		
		
		if( frustum[ i ].x * ( location->x - dimension->x ) +
			frustum[ i ].y * ( location->y - dimension->y ) +
			frustum[ i ].z * ( location->z + dimension->z ) +
			frustum[ i ].w > 0.0f )
		{
			++i;
			continue;
		}
		
		
		if( frustum[ i ].x * ( location->x + dimension->x ) +
			frustum[ i ].y * ( location->y + dimension->y ) +
			frustum[ i ].z * ( location->z + dimension->z ) +
			frustum[ i ].w > 0.0f )
		{
			++i;
			continue;
		}
		
		
		if( frustum[ i ].x * ( location->x - dimension->x ) +
			frustum[ i ].y * ( location->y + dimension->y ) +
			frustum[ i ].z * ( location->z + dimension->z ) +
			frustum[ i ].w > 0.0f )
		{
			++i;
			continue;
		}
		
		
		if( frustum[ i ].x * ( location->x + dimension->x ) +
			frustum[ i ].y * ( location->y + dimension->y ) +
			frustum[ i ].z * ( location->z + dimension->z ) +
			frustum[ i ].w > 0.0f )
		{
			++i;
			continue;
		}
			
		return 0;
	}
	
	return 1;
}


/*!
	Get if a sphere is inside, outside or intersect the frustum planes.
		
	\param[in] frustum The six clipping planes data to use to test if the sphere is inside the planes.
	\param[in] location The location of the pivot point of the sphere in world coordinates.
	\param[in] radius The radius of the sphere.
	
	\return Return 0 if outside, 1 if inside and 2 if the sphere intersect with at least one plane
	of the frustum.
*/
unsigned char sphere_intersect_frustum( vec4  *frustum, vec3  *location, float radius )
{
	float d;
	
	unsigned int i = 0;

	unsigned char c = 0;

	while( i != 6 )
	{
		d = frustum[ i ].x * location->x +
		    frustum[ i ].y * location->y +
		    frustum[ i ].z * location->z +
		    frustum[ i ].w;

		if( d < -radius )
		{ return 0; }
		
		else if( d > radius )
		{ ++c; }
		
		++i;
	}

	return c == 6 ? 2: 1;
}


/*!
	Get if a box is inside, outside or intersect the frustum planes.
		
	\param[in] frustum The six clipping planes data to use to test if the point is inside the planes.
	\param[in] location The location of the pivot point of the box in world coordinates.
	\param[in] dimension The dimension of the box on the XYZ axis.
	
	\return Return 0 if outside, 1 if inside and 2 if the box intersect with at least one plane
	of the frustum.
*/
unsigned char box_intersect_frustum( vec4 *frustum,
									 vec3 *location,
									 vec3 *dimension )
{
	unsigned int i = 0;
	
	unsigned char c1,
				  c2 = 0;

	while( i != 6 )
	{
		c1 = 0;
		
		if( frustum[ i ].x * ( location->x - dimension->x ) + 
			frustum[ i ].y * ( location->y - dimension->y ) +
			frustum[ i ].z * ( location->z - dimension->z ) +
			frustum[ i ].w > 0.0f )
		{ ++c1; }
		
		
		if( frustum[ i ].x * ( location->x + dimension->x ) +
			frustum[ i ].y * ( location->y - dimension->y ) +
			frustum[ i ].z * ( location->z - dimension->z ) +
			frustum[ i ].w > 0.0f )
		{ ++c1; }
		
		
		if( frustum[ i ].x * ( location->x - dimension->x ) +
			frustum[ i ].y * ( location->y + dimension->y ) +
			frustum[ i ].z * ( location->z - dimension->z ) +
			frustum[ i ].w > 0.0f )
		{ ++c1; }
		
		
		if( frustum[ i ].x * ( location->x + dimension->x ) +
			frustum[ i ].y * ( location->y + dimension->y ) +
			frustum[ i ].z * ( location->z - dimension->z ) +
			frustum[ i ].w > 0.0f )
		{ ++c1; }
		
		
		if( frustum[ i ].x * ( location->x - dimension->x ) +
			frustum[ i ].y * ( location->y - dimension->y ) +
			frustum[ i ].z * ( location->z + dimension->z ) +
			frustum[ i ].w > 0.0f )
		{ ++c1; }
		
		
		if( frustum[ i ].x * ( location->x + dimension->x ) +
			frustum[ i ].y * ( location->y - dimension->y ) +
			frustum[ i ].z * ( location->z + dimension->z ) +
			frustum[ i ].w > 0.0f )
		{ ++c1; }
		
		
		if( frustum[ i ].x * ( location->x - dimension->x ) +
			frustum[ i ].y * ( location->y + dimension->y ) +
			frustum[ i ].z * ( location->z + dimension->z ) +
			frustum[ i ].w > 0.0f )
		{ ++c1; }
		
		
		if( frustum[ i ].x * ( location->x + dimension->x ) +
			frustum[ i ].y * ( location->y + dimension->y ) +
			frustum[ i ].z * ( location->z + dimension->z ) +
			frustum[ i ].w > 0.0f )
		{ ++c1; }
		
		
		if( !c1 )
		{ return 0; }
		
		if( c1 == 8 )
		{ ++c2; }
	
		++i;
	}

	return c2 == 6 ? 2 : 1;
}


/*!
	\return Return the next valid power of 2 for the current size.
*/
unsigned int get_next_pow2( unsigned int size )
{
	switch( size ) 
	{
		case 1:
		case 2:
		case 4:				
		case 8:
		case 16:
		case 32:
		case 64:
		case 128:
		case 256:
		case 512:
		case 1024:
		case 2048:
		case 4096:
		case 8092:		
		{ return size; }
	}	

	return ( unsigned int )( powf( 2.0f, ceilf( logf( ( float )size ) / logf( 2.0f ) ) ) );
}


/*!
	\return Return the closest valid power of 2 for the current size.
*/
unsigned int get_nearest_pow2( unsigned int size )
{
	unsigned int i = 1;

	while( 1 )
	{
		if( size == 1 ) return i;
		
		else if( size == 3 ) return i * 4;

		size >>= 1;

		i *= 2;
	}
}


/*!
	Create a normalized direction vector for an arbitraty set of rotation angles.
	
	\param[in,out] dst The resulting direction vector will be stored in this variable.
	\param[in] up_axis The up axis.
	\param[in] rotx The X angle in degree.
	\param[in] roty The Y angle in degree.
	\param[in] rotz The Z angle in degree.
*/
void create_direction_vector( vec3 *dst, vec3 *up_axis, float rotx, float roty, float rotz )
{
	vec4 rotation;

	mat4 m;

	mat4_identity( &m );

	rotation.z = 1.0f;
	rotation.x =
	rotation.y = 0.0f;
	rotation.w = rotz;

	mat4_rotate( &m, &m, &rotation );

	rotation.y = 1.0f;
	rotation.x =
	rotation.z = 0.0f;
	rotation.w = roty;

	mat4_rotate( &m, &m, &rotation );

	rotation.x = 1.0f;
	rotation.y =
	rotation.z = 0.0f;
	rotation.w = rotx;

	mat4_rotate( &m, &m, &rotation );

	vec3_invert( up_axis, up_axis );

	vec3_multiply_mat4( dst, up_axis, &m );
}
