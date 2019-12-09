#ifndef FS_DATATYPE
    #define FS_DATATYPE

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>

    #include "../GL_support/fs_opengl.h"
    
    #include "fs_node.h"
    #include "fs_matrix.h"

    node_head render;
    node_head render2d;

    void renderNodeIn(node* target);
    void renderNodeOut(node* target);
    int renderNode();
#endif