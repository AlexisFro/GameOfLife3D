#pragma once

// Vertex attribute location imposées
enum VA_Locations{ VPOS_LOC = 0, VCOL_LOC = 1, VNOR_LOC = 2, VTEX_LOC = 3, LAST_LOC };

inline const char* get_vertex_attribute_name (VA_Locations loc)
{
    switch (loc) {
        case VPOS_LOC  : return "vPos";
        case VCOL_LOC  : return "vCol";
        case VNOR_LOC  : return "vNor";
        case VTEX_LOC  : return "vTex";
        default : return "";
    }
}