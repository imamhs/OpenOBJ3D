/*
    A simple obj 3D file format parser and 3D mesh loader library    
    
    Copyright (c) 2015 Md Imam Hossain

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

    Md Imam Hossain
    emamhd@gmail.com
*/

#ifndef OPENOBJ3D_H
#define	OPENOBJ3D_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
    
#define OPENOBJ3D_VERSION 1

struct ooj_coord3d {
    float x, y, z;
};
typedef struct ooj_coord3d OOJ_Coord3D;

struct ooj_vertex {
    OOJ_Coord3D *v;
    long slot;
};
typedef struct ooj_vertex OOJ_Vertex;

struct ooj_color {
    float red, green, blue;
};
typedef struct ooj_color OOJ_Color;

struct ooj_material {
    OOJ_Color ambient, diffuse, specular;
    char name[128];
    char texture[128];
    long texture_id;
};
typedef struct ooj_material OOJ_Material;
    
struct ooj_material_g {
    OOJ_Material *m;
    long slot;
};
typedef struct ooj_material_g OOJ_MaterialG;

struct ooj_texture_coord {
    float u, v;
};
typedef struct ooj_texture_coord OOJ_TexCoord;

struct ooj_texture_coord_g {
    OOJ_TexCoord *t;
    long slot;
};

typedef struct ooj_texture_coord_g OOJ_TexCoordG;

struct ooj_face {
    OOJ_Coord3D v[3];
    OOJ_Coord3D n[3];
    OOJ_Color ambient, diffuse, specular;
    OOJ_TexCoord t[3];
    char material[128];
    char texture[128];
    long texture_id;
};
typedef struct ooj_face OOJ_Face;

struct ooj_mesh {
    OOJ_Face *f;
    long slot;
    long tex_num;
    long mat_num;
    char textures[16384];
};
typedef struct ooj_mesh OOJ_Mesh;

extern int ooj_loadmesh (char *obj_filename, OOJ_Mesh *mo);

#ifdef __cplusplus
}
#endif

#endif	/* OPENOBJ3D_H */
