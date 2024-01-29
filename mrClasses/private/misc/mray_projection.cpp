
void mi_point_to_raster(
        miState  * const        state,          /* shader state */
        miVector * const        proj,           /* transformed vector */
        miVector * const        pos)            /* vector to transform */
{
        register miCamera *camera = state->camera;
        miScalar raster_x_step = camera->aperture /
                        (camera->focal * camera->x_resolution);
        miScalar raster_y_step = camera->aperture /
                        (camera->focal *
                         camera->aspect * camera->y_resolution);
        miScalar raster_x_start =
                        (camera->x_offset - 0.5f * camera->x_resolution) *
                        raster_x_step;
        miScalar raster_y_start =
                        (camera->y_offset - 0.5f * camera->y_resolution) *
                        raster_y_step;
        if (state->options->render_space != 'c') {
                if (state->type == miRAY_DISPLACE) {
                        mi_point_to_camera(state, proj, pos);
                } else {
                        miInstance *instance = (miInstance*)
                                        mi_db_access(state->camera_inst);
                        mi_point_transform(proj, pos,
                                        instance->tf.global_to_local);
                        mi_db_unpin(state->camera_inst);
                }
                if (!camera->orthographic) {
                        if (proj->z >= 0.0f) {
                                proj->x = miHUGE_SCALAR;
                                proj->y = miHUGE_SCALAR;
                                proj->z = miHUGE_SCALAR;
                        } else {
                                miScalar z = - 1.0f / proj->z;
                                proj->x *= z;
                                proj->y *= z;
                                proj->z = z;
                        }
                }
        } else {
                if (camera->orthographic) {
                        if (proj != pos)
                                *proj = *pos;
                } else {
                        if (pos->z >= 0.0f) {
                                proj->x = miHUGE_SCALAR;
                                proj->y = miHUGE_SCALAR;
                                proj->z = miHUGE_SCALAR;
                        } else {
                                miScalar z = - 1.0f / pos->z;
                                proj->x = z * pos->x;
                                proj->y = z * pos->y;
                                proj->z = z;
                        }
                }
        }
        proj->x = (proj->x - raster_x_start) / raster_x_step;
        proj->y = (proj->y - raster_y_start) / raster_y_step;
}
