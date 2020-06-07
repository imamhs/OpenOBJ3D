/*
 * Copyright (C) 2013
 * Md Imam Hossain (emamhd@gmail.com)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <openobj3d.h>

#include <SDL.h>
#include <GL/gl.h>


OOJ_Mesh Object1;

/* Program Control */
int shut = 0;
int fps = 0;

/* Texture Parameters */
int Mode = 0;
char *texp;
long ct = -1;
GLuint *textures = NULL;
GLfloat brightness = 1.0f;

/* GL Lighting */
GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat specularLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
GLfloat color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_diffuse[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightPos[] = { 0.0f, 1.0f, -2.0f, 1.0f };
char cm[128];
long i = 0;

/* World Camera */
GLfloat beta = 0.0f;
GLfloat alpha = 40.0f;
GLfloat x = 0.0f, y = 0.0f, z = -6.0f;

/* Moving */
int move[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
GLfloat pi = 3.1416;
GLfloat speed = 0.5f;

/* SDL speed regulator */
Uint32 fps_reg = 0;
Uint32 fps_count = 0;

/* SDL variables */
SDL_Surface *back_surface = NULL;
SDL_Surface *tex_image = NULL;
SDL_Event event;

void Draw_RectPrism (GLfloat length, GLfloat width, GLfloat thickness, GLfloat red, GLfloat green, GLfloat blue);
void Setup_RC (void);
void Render_Scene (void);
void Render_Physics (void);
void Get_Input (void);

int main (int argc, char *argv[])
{
    
printf ("Initializing SDL\n");
if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) == -1) {
    fprintf (stderr, "Could not initialize SDL %s\n", SDL_GetError());
    exit (1);
}
printf ("Success\n");  
    
SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
SDL_WM_SetCaption ("OpenGL 3D room Camera", NULL);

back_surface = SDL_SetVideoMode (640, 480, 32, SDL_OPENGL);
    
if (back_surface == NULL) {
    fprintf (stderr, "Could not set video mode %s\n", SDL_GetError());
    SDL_Quit ();
    exit (1);
}

if (ooj_loadmesh ("wel", &Object1) != 0) {
    SDL_Quit ();
    exit (1);
}

memset (cm, '\0', 128);

Setup_RC ();
    
while (shut == 0) {
    if ((SDL_GetTicks() - fps_reg) > 20) {
        Get_Input ();
        Render_Physics ();
        fps_reg = SDL_GetTicks ();
    }
    if ((SDL_GetTicks() - fps_count) > 1000) {
        printf ("FPS %d\n", fps);
        fps = 0;
        fps_count = SDL_GetTicks ();
    }
Render_Scene ();
}

glDeleteTextures (Object1.tex_num, textures);
if (textures != NULL) free (textures);

SDL_Quit();

free (Object1.f);
  
return 0;

}

void Draw_RectPrism (GLfloat length, GLfloat width, GLfloat thickness, GLfloat red, GLfloat green, GLfloat blue)
{
    
glBegin(GL_QUADS);

    glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glVertex3f(-length, -thickness, width);
    glVertex3f(length, -thickness, width);
    glVertex3f(length, thickness, width);
    glVertex3f(-length, thickness, width);
  
    glVertex3f(length, thickness, -width);
    glVertex3f(length, thickness, width);
    glVertex3f(length, -thickness, width);
    glVertex3f(length, -thickness, -width);

    glVertex3f(-length, -thickness, -width);
    glVertex3f(-length, thickness, -width);
    glVertex3f(length, thickness, -width);
    glVertex3f(length, -thickness, -width);

    glVertex3f(-length, -thickness, width);
    glVertex3f(-length, thickness, width);
    glVertex3f(-length, thickness, -width);
    glVertex3f(-length, -thickness, -width);

    glVertex3f(-length, thickness, width);
    glVertex3f(length, thickness, width);
    glVertex3f(length, thickness, -width);
    glVertex3f(-length, thickness, -width);

    glVertex3f(-length, -thickness, width);
    glVertex3f(-length, -thickness, -width);
    glVertex3f(length, -thickness, -width);
    glVertex3f(length, -thickness, width);
    
glEnd();

}

void Setup_RC (void)
{

glEnable (GL_DEPTH_TEST);

glFrontFace (GL_CCW);

glEnable (GL_CULL_FACE);

glShadeModel (GL_SMOOTH);

glEnable (GL_LIGHTING);

glLightfv (GL_LIGHT0, GL_AMBIENT, ambientLight);
glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuseLight);
glLightfv (GL_LIGHT0, GL_SPECULAR, specularLight);
glLightf (GL_LIGHT0, GL_CONSTANT_ATTENUATION, GL_TRUE);
glMaterialf (GL_FRONT, GL_SHININESS, 128.0);
glEnable (GL_LIGHT0);

glEnable (GL_TEXTURE_2D);

textures = (GLuint *) malloc (Object1.tex_num * sizeof (GLuint));

if (textures != NULL) {

glGenTextures (Object1.tex_num, textures);

texp = strtok (Object1.textures, " ");

for (i = 0; i < Object1.tex_num; i += 1) {
    if (texp != NULL) {
        glBindTexture (GL_TEXTURE_2D, textures[i]);
        
        tex_image = SDL_LoadBMP(texp);

        if (tex_image == NULL) {
            fprintf(stderr, "Couldn't load %s: %s\n", texp, SDL_GetError());
            free (Object1.f);
            SDL_Quit ();
            exit (1);
        }
        if(tex_image->format->BytesPerPixel == 4) Mode = GL_RGBA;
        else Mode = GL_RGB;
        glTexImage2D (GL_TEXTURE_2D, 0, Mode, tex_image->w, tex_image->h, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_image->pixels);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        if (tex_image != NULL) SDL_FreeSurface (tex_image);
        tex_image = NULL;
    }
    texp = strtok (NULL, " ");
}

glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

}

glClearColor (0.2f, 0.2f, 0.2f, 1.0f);

glViewport (0, 0, 640, 480);

glMatrixMode (GL_PROJECTION);
glLoadIdentity ();

glFrustum (-1.0, 1.0, -1.0, 1.0, 1.0, 200.0);

glMatrixMode (GL_MODELVIEW);

}

void Render_Scene (void)
{

fps += 1;

glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

glLoadIdentity ();

/* Define World */

glPushMatrix ();

glLightfv (GL_LIGHT0, GL_POSITION, lightPos);
glTranslatef (lightPos[0],lightPos[1],lightPos[2]);
Draw_RectPrism (0.1f, 0.1f, 0.1f, 1.0f, 0.0f, 0.0f);

glPopMatrix ();

glTranslatef (x, y, z);

glRotatef (alpha, 1.0f, 0.0f, 0.0f);
glRotatef (beta, 0.0f, 1.0f, 0.0f);

for (i = 0; i < Object1.slot; i += 1) {
    if (Object1.f[i].texture_id != ct) {
        ct = Object1.f[i].texture_id;
        if (Object1.f[i].texture_id > 0) {
            glBindTexture (GL_TEXTURE_2D, textures[Object1.f[i].texture_id-1]);
            glEnable (GL_TEXTURE_2D);
        }
        else glDisable (GL_TEXTURE_2D);
    }
    glBegin(GL_TRIANGLES);
        if (strcmp(Object1.f[i].material, cm) != 0) {
            strcpy (cm, Object1.f[i].material);
            material_ambient[0] = Object1.f[i].ambient.red;
            material_ambient[1] = Object1.f[i].ambient.green;
            material_ambient[2] = Object1.f[i].ambient.blue;
            material_diffuse[0] = Object1.f[i].diffuse.red;
            material_diffuse[1] = Object1.f[i].diffuse.green;
            material_diffuse[2] = Object1.f[i].diffuse.blue;
            material_specular[0] = Object1.f[i].specular.red;
            material_specular[1] = Object1.f[i].specular.green;
            material_specular[2] = Object1.f[i].specular.blue;
            glMaterialfv (GL_FRONT, GL_AMBIENT, material_ambient);
            glMaterialfv (GL_FRONT, GL_DIFFUSE, material_diffuse);
            glMaterialfv (GL_FRONT, GL_SPECULAR, material_specular);
        }

        glNormal3f (Object1.f[i].n[0].x, Object1.f[i].n[0].y, Object1.f[i].n[0].z);
        glTexCoord2f (Object1.f[i].t[0].u, Object1.f[i].t[0].v);
        glVertex3f (Object1.f[i].v[0].x, Object1.f[i].v[0].y, Object1.f[i].v[0].z);
        glNormal3f (Object1.f[i].n[1].x, Object1.f[i].n[1].y, Object1.f[i].n[1].z);
        glTexCoord2f (Object1.f[i].t[1].u, Object1.f[i].t[1].v);
        glVertex3f (Object1.f[i].v[1].x, Object1.f[i].v[1].y, Object1.f[i].v[1].z);
        glNormal3f (Object1.f[i].n[2].x, Object1.f[i].n[2].y, Object1.f[i].n[2].z);
        glTexCoord2f (Object1.f[i].t[2].u, Object1.f[i].t[2].v);
        glVertex3f (Object1.f[i].v[2].x, Object1.f[i].v[2].y, Object1.f[i].v[2].z);
    glEnd();
}

SDL_GL_SwapBuffers ();

}

void Render_Physics (void)
{
if (move[0] == 1) {
    if (beta < 1) beta = 359.0f;
    else beta -= 1.0f;
}
if (move[1] == 1) {
    if (beta > 359) beta = 1.0f;
    else beta += 1.0f;
}
if (move[2] == 1) {
    if (alpha < 1) alpha = 359.0f;
    else alpha -= 1.0f;
}
if (move[3] == 1) {
    if (alpha > 359) alpha = 1.0f;
    else alpha += 1.0f;
}
if (move[4] == 1) {
    x -= 0.2f;
}
if (move[5] == 1) {
    x += 0.2f;
}
if (move[6] == 1) {
    y += 0.2f;
}
if (move[7] == 1) {
    y -= 0.2f;
}
if (move[8] == 1) {
    z += 0.2f;
}
if (move[9] == 1) {
    z -= 0.2f;
}
}

void Get_Input (void)
{
while (SDL_PollEvent (&event)) {
    switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    move[0] = 1;
                    break;
                case SDLK_RIGHT:
                    move[1] = 1;
                    break;
                case SDLK_UP:
                    move[2] = 1;
                    break;
                case SDLK_DOWN:
                    move[3] = 1;
                    break;
                case SDLK_a:
                    move[4] = 1;
                    break;
                case SDLK_d:
                    move[5] = 1;
                    break;
                case SDLK_w:
                    move[6] = 1;
                    break;
                case SDLK_s:
                    move[7] = 1;
                    break;
                case SDLK_n:
                    move[8] = 1;
                    break;
                case SDLK_m:
                    move[9] = 1;
                    break;
                case SDLK_l:
                    SDL_WM_IconifyWindow ();
                    break;
                case SDLK_f:
                    SDL_WM_ToggleFullScreen (back_surface);
                    break;
                case SDLK_ESCAPE:
                    shut = 1;
                    break;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    move[0] = 0;
                    break;
                case SDLK_RIGHT:
                    move[1] = 0;
                    break;
                case SDLK_UP:
                    move[2] = 0;
                    break;
                case SDLK_DOWN:
                    move[3] = 0;
                    break;
                case SDLK_a:
                    move[4] = 0;
                    break;
                case SDLK_d:
                    move[5] = 0;
                    break;
                case SDLK_w:
                    move[6] = 0;
                    break;
                case SDLK_s:
                    move[7] = 0;
                    break;
                case SDLK_n:
                    move[8] = 0;
                    break;
                case SDLK_m:
                    move[9] = 0;
                default:
                    break;
            }
            break;
        case SDL_MOUSEMOTION:

            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
                case SDL_BUTTON_WHEELUP:
                    
                    break;
                case SDL_BUTTON_WHEELDOWN:
                    
                    break;
                default:
                    break;
            }
            break;
        case SDL_ACTIVEEVENT:
            if (event.active.gain == 0) {

            }
            if (event.active.gain == 1) {

            }
        default:
            break;
    }
}
}




 
