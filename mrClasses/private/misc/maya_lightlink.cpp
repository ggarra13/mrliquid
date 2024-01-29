
declare shader
    color "myshader" (
        color "color",        #: default 1 1 1
        shader "lightLink"    # connect me to maya light linker
        )
    version 1
    apply material
    #: nodeid 500
end declare


//  Here's some code to use the lightLinker. You have to do...
//      connectAttr lightLinker1.message myshader1.lightLink;
//  ...for it to work. Otherwise, it'll just use the global light list.
//  It'll also check each light for emitSpecular and emitDiffuse
//  parameters. It's sort of a hack, and I've only tried compiling it
//  with GCC on Linux.

#include <mi_version.h>
#include <shader.h>
#include <geoshader.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <vector>


using namespace std;

static void get_light_emit_flags(miTag, miBoolean*, miBoolean*);

class LightInfo {
public:
    LightInfo(miTag tag) : light(tag) {        
        get_light_emit_flags(light, &emitSpecular, &emitDiffuse);    
    }
    miTag light;
    miBoolean emitSpecular, emitDiffuse;
};
typedef struct {        // shader instance data
    vector<LightInfo> *lights;
} inst_data_t;

typedef struct {        // shader parameters
    miColor color;
    miTag lightLink;
} param_t;

static int find_param_offset(miTag shader, char *name, int sz) {

    char buf[256];
    char *dp;
    miTag decl;
    mi_query(miQ_FUNC_DECL, NULL, shader, &decl);
    mi_query(miQ_DECL_PARAM, NULL, decl, &dp);

    int offset = 0;
    bool okay = 0;
    char *start = NULL;
    
    for (; *dp != '\0'; dp++) {
        
        if (!okay) {    
            if (*dp == '=')
                okay = 1;
            continue;
        }
        
        if (*dp == '\"') {
            if (!start) {
                start = dp+1;
            } else {
                strncpy(buf, start, dp-start);
                buf[dp-start] = '\0';            
                start = NULL;
                if (!strcmp(buf, name))
                    return offset-sz;
            }

        } else if (!start) {                
            switch (*dp) {        
            case 's': offset += sizeof(miScalar); break;
            case 'b': offset += sizeof(miBoolean); break;
            case 'i': offset += sizeof(miInteger); break;
            case 'c': offset += sizeof(miColor); break;        
            /* TODO        
            miVector
            miMatrix
            miTag
            struct
            array        
            */                
            }
        }
    }
        
    return -1;
}

template <class T>
bool find_shader_param(T *ret, miTag shader, char *name) {
    int offset = find_param_offset(shader, name, sizeof(T));
    if (offset == -1) {
        fprintf(stderr, "ERROR, can't find \"%s\" parameter on shader tag %d\n", name, shader);    
        return false;
    }
    char *par;
    mi_query(miQ_FUNC_PARAM, NULL, shader, &par);    
    *ret = *((T*)(par+offset));
    return true;
}

static void get_light_emit_flags(miTag lightInst, miBoolean *emitSpecular, miBoolean *emitDiffuse) {
        
    miTag light, shader, decl;
    char *decl_name;
    mi_query(miQ_INST_ITEM, NULL, lightInst, &light);
    mi_query(miQ_LIGHT_SHADER, NULL, light, &shader);
    mi_query(miQ_FUNC_DECL, NULL, shader, &decl);
    mi_query(miQ_DECL_NAME, NULL, decl, &decl_name);
                                
    if (    !strcmp(decl_name, "maya_spotlight") ||
        !strcmp(decl_name, "maya_directionallight") ||
        !strcmp(decl_name, "maya_pointlight") ||            
        !strcmp(decl_name, "maya_arealight")
        ) {
        
        if (emitSpecular) {
            miBoolean s;        
            if (find_shader_param(&s, shader, "emitSpecular"))
                *emitSpecular = s;
            else
                *emitSpecular = miTRUE;
        }
        if (emitDiffuse) {
            miBoolean d;        
            if (find_shader_param(&d, shader, "emitDiffuse"))
                *emitDiffuse = d;
            else
                *emitDiffuse = miTRUE;
        }
        
        /*miColor c;
        if (find_shader_param(&c, shader, "color"))
            fprintf(stderr, "light color = %f %f %f\n", c.r, c.g, c.b);*/
    } else {
    
        if (emitSpecular)
            *emitSpecular = miTRUE;
        if (emitDiffuse)
            *emitDiffuse = miTRUE;
    }
}




static void printLightLinks(inst_data_t *data) {
    if (!data->lights)
        return;
    
    fprintf(stderr, "%d lights\n", data->lights->size());
    for (int i=0; i<data->lights->size(); i++) {
        fprintf(stderr, "\tlight tag %d      emitSpecular %d   emitDiffuse %d\n",
            (*data->lights)[i].light,
            (*data->lights)[i].emitSpecular,
            (*data->lights)[i].emitDiffuse);
    }
}

static void grabGlobalLightList(inst_data_t *data) {
    int nlights;
    miTag *lights;
    fprintf(stderr, "USING GLOBAL LIGHT LIST\n");
    mi_query(miQ_NUM_GLOBAL_LIGHTS, NULL, miNULLTAG, &nlights);
    mi_query(miQ_GLOBAL_LIGHTS, NULL, miNULLTAG, &lights);        
    for (int i=0; i<nlights; i++)
        data->lights->push_back(LightInfo(lights[i]));
}

static bool getLightLinks(miState *state, param_t *param, inst_data_t *data) {

    data->lights = new vector<LightInfo>;

    // get the object this shader is attached to
    miTag obj;
    mi_query(miQ_INST_ITEM, NULL, state->instance, &obj);

    // get light link info
    int *p, i,j;
    miTag link = *mi_eval_tag(&param->lightLink);
    if (!link) {    // no connection
        grabGlobalLightList(data);
        return 0;
    }
    char *name;
    miTag decl;
    mi_query(miQ_FUNC_DECL, NULL, link, &decl);
    mi_query(miQ_DECL_NAME, NULL, decl, &name);
    if (strcmp(name, "maya_lightlink")) {
        fprintf(stderr, "***\tERROR, connection to lightLink attr must be from a maya_lightlink shader\n");
        grabGlobalLightList(data);
        return 0;
    }

    // get parameters to maya_lightlink shader
    mi_query(miQ_FUNC_PARAM, NULL, link, &p);
    int skip = 2*p[0];
    int nlinks = p[1];
    miTag lightTag, objTag;
    miTag *lightLinks = (miTag*)p + skip + 2;

    // store light tags
    for (i=0; nlinks-- > 0; i+=2) {
        mi_query(miQ_INST_ITEM, NULL, lightLinks[i+1], &objTag);
        if (obj != objTag)
            continue;
        mi_query(miQ_INST_ITEM, NULL, lightLinks[i], &lightTag);
        if (mi_db_type(lightTag) != miSCENE_LIGHT) {
            fprintf(stderr, "***\t\tbad light link, tag %d not a light\n", lightTag);
            continue;
        }        
        
        // got a link to this object
        data->lights->push_back(LightInfo(lightLinks[i]));
    }
    
    if (data->lights->size() == 0) {
        fprintf(stderr, "WARNING, no lights were linked geometry\n");
    } else {
        fprintf(stderr, "USING Maya light linker\n");
    }
    return 0;
}

extern "C" void myshader_init(miState *state, param_t *param, miBoolean *init_req) {
    if (!param) {
        if (init_req)
            *init_req = miTRUE;
    } else {
    
        inst_data_t **handle;
        mi_query(miQ_FUNC_USERPTR, state, 0, &handle);
        *handle = (inst_data_t*)mi_mem_allocate(sizeof(inst_data_t));
        inst_data_t *data = *handle;
        data->lights = NULL;

        
        // grab light links
        getLightLinks(state, param, data);
        printLightLinks(data);
                        
    }
}

extern "C" void myshader_exit(miState *state, param_t *param) {
    if (param) {
        inst_data_t **handle;
        mi_query(miQ_FUNC_USERPTR, state, 0, &handle);
        delete (*handle)->lights;
        mi_mem_release(*handle);
    }
}

extern "C" int myshader_version(void) { return 1; }

extern "C" miBoolean myshader(miColor *result, miState *state, param_t *param) {
    
    inst_data_t **handle;
    mi_query(miQ_FUNC_USERPTR, state, 0, &handle);
    inst_data_t *data = *handle;

    miColor Cl;
    miVector dir, nL;
    miScalar dot_nl;
    miColor total = {0,0,0,0};
            
    const int nlights = data->lights->size();
    for (int i=0; i<nlights; i++) {        // foreach light
        int samples = 0;        
        miColor sum = {0,0,0,0};
        
        miTag light = (*data->lights)[i].light;
        while (mi_sample_light(&Cl, &dir, &dot_nl, state, light, &samples)) {                        
            sum.r += dot_nl * Cl.r;
            sum.g += dot_nl * Cl.g;
            sum.b += dot_nl * Cl.b;
        }
        if (samples) {
            if ((*data->lights)[i].emitSpecular) {
                // todo            
            }
            if ((*data->lights)[i].emitDiffuse) {
                total.r += sum.r/samples;
                total.g += sum.g/samples;
                total.b += sum.b/samples;
            }                        
        }
    } // next light
    
    miColor *color = mi_eval_color(&param->color);
    result->r = color->r * total.r;
    result->g = color->g * total.g;
    result->b = color->b * total.b;    
    result->a = 1;
                
    return miTRUE;
}




