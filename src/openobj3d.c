/*
    A simple obj 3D file format mesh loader library    
    
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

#include "openobj3d.h"

int ooj_loadmesh (char *obj_filename, OOJ_Mesh *mo)
{

FILE *fp = NULL;
FILE *ff = NULL;
char scanline[1024];
char mat_file[128];
char obj_file[128];
long i = 0;

OOJ_MaterialG mg;
long current_texture = 1;
OOJ_Vertex vg;
OOJ_TexCoordG tg;
OOJ_Vertex nm;
char current_material[128];
int take_tex = 0;

mg.slot = 0;
mg.m = NULL;

memset (scanline, '\0', 1024);

memset (mat_file, '\0', 128);
strcat (mat_file, obj_filename);
strcat (mat_file, ".mtl");

fp = fopen (mat_file, "r");
  
if (fp == NULL) {
return 1;
}

while (fgets (scanline, 1023, fp) != NULL) {
    if (scanline[0] == 'n' && scanline[1] == 'e' && scanline[2] == 'w') {
        char *mnone = NULL;
        char tmpstr[128];
        mg.m = (OOJ_Material *) realloc (mg.m, ((mg.slot + 1) * sizeof (OOJ_Material)));
        if (mg.m == NULL) {
            fclose(fp);
            return (1);
        }
        memset (mg.m[mg.slot].name, '\0', 128);
        sscanf (scanline, "newmtl %s", mg.m[mg.slot].name);
        memset (mg.m[mg.slot].texture, '\0', 128);
        memset (tmpstr, '\0', 128);
        strcpy (tmpstr, mg.m[mg.slot].name);
        mnone = strtok (tmpstr, "_");
        mnone = strtok (NULL, "_");
        if (mnone == NULL) take_tex = 1;
        else {
            if (strcmp (mnone, "NONE") == 0) {
                take_tex = 0;
                mg.m[mg.slot].texture_id = -1;
            }
            else take_tex = 1;
        }
        mg.slot += 1;
    }
    else if (scanline[0] == 'K' && scanline[1] == 'a') {
        sscanf (scanline, "Ka %f %f %f", &mg.m[mg.slot-1].ambient.red, &mg.m[mg.slot-1].ambient.green, &mg.m[mg.slot-1].ambient.blue);
    }
    else if (scanline[0] == 'K' && scanline[1] == 'd') {
        sscanf (scanline, "Kd %f %f %f", &mg.m[mg.slot-1].diffuse.red, &mg.m[mg.slot-1].diffuse.green, &mg.m[mg.slot-1].diffuse.blue);
    }
    else if (scanline[0] == 'K' && scanline[1] == 's') {
        sscanf (scanline, "Ks %f %f %f", &mg.m[mg.slot-1].specular.red, &mg.m[mg.slot-1].specular.green, &mg.m[mg.slot-1].specular.blue);
    }
    else if (scanline[0] == 'm' && scanline[1] == 'a' && scanline[2] == 'p' && take_tex == 1) {
        sscanf (scanline, "map_Kd %s", mg.m[mg.slot-1].texture);
        mg.m[mg.slot-1].texture_id = current_texture;
        current_texture += 1;
    }
}

fclose (fp);

memset (obj_file, '\0', 128);
strcat (obj_file, obj_filename);
strcat (obj_file, ".obj");

ff = fopen (obj_file, "r");
  
if (ff == NULL) {
free (mg.m);
return 1;
}

vg.slot = 0;
vg.v = NULL;
nm.slot = 0;
nm.v = NULL;
tg.slot = 0;
tg.t = NULL;
mo->f = NULL;
mo->slot = 0;
mo->tex_num = 0;
mo->mat_num = 0;
memset (mo->textures, '\0', 16384);

memset (scanline, '\0', 1024);

while (fgets (scanline, 1023, ff) != NULL) {
    if (scanline[0] == 'v' && scanline[1] == ' ') {
        vg.v = (OOJ_Coord3D *) realloc (vg.v, (vg.slot + 1) * sizeof (OOJ_Coord3D));
        if (vg.v == NULL) {
            if (tg.t != NULL) free (tg.t);
            if (nm.v != NULL) free (nm.v);
            if (mo->f != NULL) free (mo->f);
            if (mg.m != NULL) free (mg.m);
            fclose(ff);
            return 1;
        }
        sscanf (scanline, "v %f %f %f", &vg.v[vg.slot].x, &vg.v[vg.slot].y, &vg.v[vg.slot].z);
        vg.slot += 1;
    }
    else if (scanline[0] == 'v' && scanline[1] == 't') {
        tg.t = (OOJ_TexCoord *) realloc (tg.t, (tg.slot + 1) * sizeof (OOJ_TexCoord));
        if (tg.t == NULL) {
            if (vg.v != NULL) free (vg.v);
            if (nm.v != NULL) free (nm.v);
            if (mo->f != NULL) free (mo->f);
            if (mg.m != NULL) free (mg.m);
            fclose(ff);
            return 1;
        }
        sscanf (scanline, "vt %f %f", &tg.t[tg.slot].u, &tg.t[tg.slot].v);
        tg.t[tg.slot].v = 1 - tg.t[tg.slot].v;
        tg.slot += 1;
    }
    else if (scanline[0] == 'v' && scanline[1] == 'n') {
        nm.v = (OOJ_Coord3D *) realloc (nm.v, (nm.slot + 1) * sizeof (OOJ_Coord3D));
        if (nm.v == NULL) {
            if (tg.t != NULL) free (tg.t);
            if (vg.v != NULL) free (vg.v);
            if (mo->f != NULL) free (mo->f);
            if (mg.m != NULL) free (mg.m);
            fclose(ff);
            return 1;
        }
        sscanf (scanline, "vn %f %f %f", &nm.v[nm.slot].x, &nm.v[nm.slot].y, &nm.v[nm.slot].z);
        nm.slot += 1;
    }
    else if (scanline[0] == 'u' && scanline[1] == 's' && scanline[2] == 'e') {
        memset (current_material, '\0', 128);
        sscanf (scanline, "usemtl %s", current_material);
    }
    else if (scanline[0] == 'f' && scanline[1] == ' ') {
        long v1, v2, v3, t1, t2, t3, n1, n2, n3;
        mo->f = (OOJ_Face *) realloc (mo->f, (mo->slot + 1) * sizeof (OOJ_Face));
        if (mo->f == NULL) {
            if (tg.t != NULL) free (tg.t);
            if (nm.v != NULL) free (nm.v);
            if (vg.v != NULL) free (vg.v);
            if (mg.m != NULL) free (mg.m);
            fclose(ff);
            return 1;
        }
        sscanf (scanline, "f %ld/%ld/%ld %ld/%ld/%ld %ld/%ld/%ld", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
        mo->f[mo->slot].v[0].x = vg.v[v1-1].x;
        mo->f[mo->slot].v[0].y = vg.v[v1-1].y;
        mo->f[mo->slot].v[0].z = vg.v[v1-1].z;
        mo->f[mo->slot].v[1].x = vg.v[v2-1].x;
        mo->f[mo->slot].v[1].y = vg.v[v2-1].y;
        mo->f[mo->slot].v[1].z = vg.v[v2-1].z;
        mo->f[mo->slot].v[2].x = vg.v[v3-1].x;
        mo->f[mo->slot].v[2].y = vg.v[v3-1].y;
        mo->f[mo->slot].v[2].z = vg.v[v3-1].z;
        mo->f[mo->slot].t[0].u = tg.t[t1-1].u;
        mo->f[mo->slot].t[0].v = tg.t[t1-1].v;
        mo->f[mo->slot].t[1].u = tg.t[t2-1].u;
        mo->f[mo->slot].t[1].v = tg.t[t2-1].v;
        mo->f[mo->slot].t[2].u = tg.t[t3-1].u;
        mo->f[mo->slot].t[2].v = tg.t[t3-1].v;
        mo->f[mo->slot].n[0].x = nm.v[n1-1].x;
        mo->f[mo->slot].n[0].y = nm.v[n1-1].y;
        mo->f[mo->slot].n[0].z = nm.v[n1-1].z;
        mo->f[mo->slot].n[1].x = nm.v[n2-1].x;
        mo->f[mo->slot].n[1].y = nm.v[n2-1].y;
        mo->f[mo->slot].n[1].z = nm.v[n2-1].z;
        mo->f[mo->slot].n[2].x = nm.v[n3-1].x;
        mo->f[mo->slot].n[2].y = nm.v[n3-1].y;
        mo->f[mo->slot].n[2].z = nm.v[n3-1].z;
        if (strlen (current_material) > 1) {
            strcpy (mo->f[mo->slot].material, current_material);
            for (i = 0; i < mg.slot; i++) {
                if (strcmp (mo->f[mo->slot].material, mg.m[i].name) == 0) {
                    mo->f[mo->slot].ambient.red = mg.m[i].ambient.red;
                    mo->f[mo->slot].ambient.green = mg.m[i].ambient.green;
                    mo->f[mo->slot].ambient.blue = mg.m[i].ambient.blue;
                    mo->f[mo->slot].diffuse.red = mg.m[i].diffuse.red;
                    mo->f[mo->slot].diffuse.green = mg.m[i].diffuse.green;
                    mo->f[mo->slot].diffuse.blue = mg.m[i].diffuse.blue;
                    mo->f[mo->slot].specular.red = mg.m[i].specular.red;
                    mo->f[mo->slot].specular.green = mg.m[i].specular.green;
                    mo->f[mo->slot].specular.blue = mg.m[i].specular.blue;
                    strcpy (mo->f[mo->slot].texture, mg.m[i].texture);
                    if (strlen (mg.m[i].texture) > 1) mo->f[mo->slot].texture_id = mg.m[i].texture_id;
                    else mo->f[mo->slot].texture_id = 0;
                }
            }
        }
        mo->slot += 1;
    }
}

mo->mat_num = mg.slot;

for (i = 0; i < mg.slot; i += 1) {
    if (mg.m[i].texture_id > 0) mo->tex_num += 1;
}

for (i = 0; i < mg.slot; i += 1) {
    if (strlen(mg.m[i].texture) > 1) {
        strcat (mo->textures, mg.m[i].texture);
        strcat (mo->textures, " ");
    }
}

fclose (ff);

free (mg.m);
free (vg.v);
free (tg.t);
free (nm.v);

return 0;
}
