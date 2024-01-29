#include "shader.h"

struct data_lens{
};

int data_lens_version(void) {return(1);}

miBoolean data_lens(
    miColor                *result,
    miState                *state,
    struct            data_lens    *paras)
{
    miVector     start, end, raster;
    miColor        trik;

    mi_trace_eye(result, state, &state->org, &state->dir);

    if (state->child->motion.x != 0 ||
	state->child->motion.y != 0 ||
	state->child->motion.z != 0 )
    {
        mi_vector_add(&start, &state->child->point, &state->child->motion);
        mi_point_to_raster(state, &end, &start);
        mi_point_to_raster(state, &start, &state->child->point);
        mi_vector_sub(&raster, &end, &start);

        raster.z = 0;
        mi_fb_put(state, 0, &raster);
    }

    trik.r = -1*state->child->dot_nd;
    trik.g = -1*state->child->point.y;
    trik.b = state->child->dist;

    mi_fb_put(state, 1, &trik);

    return(miTRUE);
}


