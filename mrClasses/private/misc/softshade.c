/******************************************************************************
 * Copyright 1986-2001 by mental images GmbH & Co.KG, Fasanenstr. 81, D-10623
 * Berlin, Germany. All rights reserved.
 ******************************************************************************
 * Author:	thomas
 * Created:	24.6.94
 * Module:	shader
 * Purpose:	Softimage shaders for raylib
 *
 * Exports:
 *    material:
 *	soft_material			flat, lambert, phong, blinn
 *	soft_material_photon		for global illumination
 *
 *    texture shaders:
 *	soft_color			look up 2D RGBA color
 *	soft_vertex_color		interpret XYZ texture coordinate as RGB
 *	soft_vertex_color_alpha		interpret XYZ texture coord as RGBA
 *	soft_vertex_color_alpha2	same but with separate alpha space
 *	soft_vertex_color_alpha3	same but with separate texture space
 *
 *    lights:
 *	soft_infinite			old infinite light for backwards compat
 *	soft_point			old point light for backwards compat
 *	soft_spot			old spot light for backwards compat
 *
 *    misc:
 *	soft_displace			displacement shader
 *	soft_staticblur			displacement shader for static blur
 *	soft_shadow			shadow shader
 *	soft_env_sphere			spherical environment shader
 *	soft_fog			fog shader
 *	soft_layered_fog		layered fog shader
 *	soft_dof			depth of field lens shader
 *	lens_depth_of_field		other name for soft_dof
 *
 *
 * Description:
 * Shaders that implement compatibility with Softimage|3D.
 *****************************************************************************/

/*: Globillum notes:
 *
 * For purely diffuse surfaces, BRDF f_r = rho / pi, where rho is the albedo.
 * For real surfaces, rho is between 0 and 1, and f_r is between 0 and 1/pi.
 *
 * 1) There is an inherent conflict in Softimage's interpretation of the
 * 'diffuse' parameters.  On one hand, SI would like to allow them to be
 * up to 1 (as it is the case for other shaders), in which case they should
 * be interpreted as the surface albedos.  On the other hand, if they were
 * albedos, they should have been divided by pi in mi_mtl_illumination().
 * Therefore, the diffuse reflection coefficients must be interpreted as
 * the diffuse components f_{r,d} of the BRDF f_r.  This is the interpretation
 * used for direct illumination.
 *
 * 2) For indirect illumination (the photon shader), the diffuse reflection
 * coefficients are interpreted as albedo rho (0 < rho < 1).  If we
 * interpreted them as f_{r,d}, we would have to multiply them by pi to
 * get the probability for diffuse reflection, and clip them if they were
 * more than 1 (that is, if they were larger than 1/pi (= 0.318...) before
 * the multiplication).
 *
 * 3) Since 'diffuse' is interpreted as BRDF f_{r,d} in soft_material() and
 * as albedo (rho) in soft_material_photon(), the energy statement in
 * the physical light source physical_light should be pi times the color.
 * If a softimage light source is used, there is no relation between
 * direct and indirect illumination, so energy and color can be set to
 * anything.
:*/


#ifdef HPUX
#pragma OPT_LEVEL 1	/* workaround for HP/UX optimizer bug, +O2 and +O3 */
#endif

#include <stdio.h>
#include <stdlib.h>		/* for abs */
#include <float.h>		/* for FLT_MAX */
#include <math.h>
#include <string.h>
#include <assert.h>
#include <shader.h>
#include <geoshader.h>
#include <mi_softshade.h>
#include <mi_version.h>

#define BLACK(c)	((c).r < 1e-6 && (c).g < 1e-6 && (c).b < 1e-6)
#define INTENSITY(c)	(((c).r + (c).g + (c).b) * 0.333333)
#define mi_MIN(a,b)	((a) < (b) ? (a) : (b))
#define mi_MAX(a,b)	((a) > (b) ? (a) : (b))
#define mi_MAX3(a,b,c)	mi_MAX(mi_MAX(a,b), (c));
#define miEPS		0.0001

#ifdef DEBUG
#define miASSERT assert
#else
#define miASSERT(x)
#endif

static miColor black = {0.0, 0.0, 0.0, 0.0};

char id[] = "==@@== compiled for ray " MI_VERSION_STRING " on " MI_DATE_STRING;


char *mi_soft_shader_version(void)
	{ return("=@@= softimage.so:\tversion 1.2" + 5); }


/*
 * dummies, for now, should be in softmisc.c
 */

int out_raisedcosine_version(void)	{return(1);}
int out_underlay_version(void)	{return(1);}
int out_overlay_version(void)		{return(1);}

miBoolean out_raisedcosine(
	miOutstate	*state,		/* output shader state vars */
	void		*paras)		/* shader parameters */
{
	mi_error("out_raisedcosine shader not supported");
	state; paras;
	return(miFALSE);
}

miBoolean out_underlay(
	miOutstate	*state,		/* output shader state vars */
	char		**name)
{
	mi_error("out_underlay shader not supported");
	state; name;
	return(miFALSE);
}

miBoolean out_overlay(
	miOutstate	*state,		/* output shader state vars */
	char		**name)
{
	mi_error("out_overlay shader not supported");
	state; name;
	return(miFALSE);
}


/*****************************************************************************
 **									    **
 **			     support functions				    **
 **									    **
 *****************************************************************************/

#define EPS 1e-4

/*
 * mapping methods: 0=XY, 1=XZ, 2=YZ, 3=UV, all others: cylindrical or
 * spherical. 666 is for 3D textures; they have no projections and must
 * be handled separately by the caller. This function ignores them.
 */

void mi_mtl_bump_basis(
	miState		  *state,
	miInteger	  method,		/* mapping method */
	int		  ntex,			/* texture number */
	register miVector *bump_u,		/* returned bump basis vecs */
	register miVector *bump_v)		/* returned bump basis vecs */
{
	miVector	  *v0, *v1, *v2;	/* triangle points in space */
	miVector	  ov0, ov1, ov2;	/* tri points in object space*/
	miVector	  *t0, *t1, *t2;	/* triangle texture verts */
	register double	  t, a, b;
	miBoolean	  have_v = miTRUE;

	if (method > 2) {
		if (!mi_tri_vectors(state, 'p', 0,    &v0, &v1, &v2) ||
		    !mi_tri_vectors(state, 't', ntex, &t0, &t1, &t2))
			return;
		mi_point_to_object(state, &ov0, v0);
		mi_point_to_object(state, &ov1, v1);
		mi_point_to_object(state, &ov2, v2);
	}
	switch(method) {
	  case 0:						/* XY */
		bump_u->x = 1.0;
		bump_u->y = 0.0;
		bump_u->z = 0.0;
		bump_v->x = 0.0;
		bump_v->y = 1.0;
		bump_v->z = 0.0;
		break;

	  case 1:						/* XZ */
		bump_u->x = 1.0;
		bump_u->y = 0.0;
		bump_u->z = 0.0;
		bump_v->x = 0.0;
		bump_v->y = 0.0;
		bump_v->z = 1.0;
		break;

	  case 2:						/* YZ */
		bump_u->x = 0.0;
		bump_u->y = 1.0;
		bump_u->z = 0.0;
		bump_v->x = 0.0;
		bump_v->y = 0.0;
		bump_v->z = 1.0;
		break;

	  case 5:						/* UV wrapped*/
		t0->x -= floor(t0->x);
		t0->y -= floor(t0->y);
		t1->x -= floor(t1->x);
		t1->y -= floor(t1->y);
		t2->x -= floor(t2->x);
		t2->y -= floor(t2->y);

	  case 3:						/* UV */
		/* find a line of constant u on the triangle */
		if (fabs(t0->x - t1->x) < EPS) {
			if (t0->y > t1->y)
				mi_vector_sub(bump_v, &ov0, &ov1);
			else
				mi_vector_sub(bump_v, &ov1, &ov0);
		} else if (fabs(t0->x - t2->x) < EPS) {
			if (t0->y > t2->y)
				mi_vector_sub(bump_v, &ov0, &ov2);
			else
				mi_vector_sub(bump_v, &ov2, &ov0);
		} else if (fabs(t1->x - t2->x) < EPS) {
			if (t1->y > t2->y)
				mi_vector_sub(bump_v, &ov1, &ov2);
			else
				mi_vector_sub(bump_v, &ov2, &ov1);

		/* find a line of constant v on the triangle */
		} else if (fabs(t0->y - t1->y) < EPS) {
			have_v = miFALSE;
			if (t0->x > t1->x)
				mi_vector_sub(bump_u, &ov0, &ov1);
			else
				mi_vector_sub(bump_u, &ov1, &ov0);
		} else if (fabs(t0->y - t2->y) < EPS) {
			have_v = miFALSE;
			if (t0->x > t2->x)
				mi_vector_sub(bump_u, &ov0, &ov2);
			else
				mi_vector_sub(bump_u, &ov2, &ov0);
		} else if (fabs(t1->y - t2->y) < EPS) {
			have_v = miFALSE;
			if (t1->x > t2->x)
				mi_vector_sub(bump_u, &ov1, &ov2);
			else
				mi_vector_sub(bump_u, &ov2, &ov1);
		} else {
			miVector tmp;
			tmp.x = t0->x;
			tmp.y = t0->y + 0.1;
			t = 1 / ((t1->x - t0->x) * (t2->y - t0->y) -
				 (t1->y - t0->y) * (t2->x - t0->x));
			a = t *-((t2->x - t0->x) * (tmp.y - t0->y));
			b = t * ((t1->x - t0->x) * (tmp.y - t0->y));
			bump_v->x = a * (ov1.x - ov0.x) + b * (ov2.x - ov0.x);
			bump_v->y = a * (ov1.y - ov0.y) + b * (ov2.y - ov0.y);
			bump_v->z = a * (ov1.z - ov0.z) + b * (ov2.z - ov0.z);
		}
		if (have_v) {
			mi_vector_normalize(bump_v);
			mi_vector_prod(bump_u, &state->normal, bump_v);
		} else {
			mi_vector_normalize(bump_u);
			mi_vector_prod(bump_v, bump_u, &state->normal);
		}
		break;

	  case 333:						/* UV */
		/* find a line of constant u on the triangle */
		if (fabs(t0->x - t1->x) < EPS)
			if (t0->y > t1->y)
				mi_vector_sub(bump_v, &ov0, &ov1);
			else
				mi_vector_sub(bump_v, &ov1, &ov0);
		else if (fabs(t0->x - t2->x) < EPS)
			if (t0->y > t2->y)
				mi_vector_sub(bump_v, &ov0, &ov2);
			else
				mi_vector_sub(bump_v, &ov2, &ov0);
		else
			if (t1->y > t2->y)
				mi_vector_sub(bump_v, &ov1, &ov2);
			else
				mi_vector_sub(bump_v, &ov2, &ov1);
		mi_vector_normalize(bump_v);

		/* find a line of constant v on the triangle */
		mi_vector_prod(bump_u, &state->normal, bump_v);
		break;

	  case 4: {					/* cyl, spherical */
		/* sort the verts */
		miVector tmp, *ptm;
		register float y0 = ov0.y, y1 = ov1.y, y2 = ov2.y;

		if (y0 < y1 && y1 < y2 || y2 < y1 && y1 < y0) {
			tmp = ov0; ov0 = ov1; ov1 = ov2; ov2 = tmp;
			ptm = t0;  t0  = t1;  t1  = t2;  t2  = ptm;

		} else if (y0 < y2 && y2 < y1 || y1 < y2 && y2 < y0) {
			tmp = ov2; ov2 = ov1; ov1 = ov0; ov0 = tmp;
			ptm = t2;  t2  = t1;  t1  = t0;  t0  = ptm;
		}

		/* find a line of constant u on the triangle */
		if (fabs(t1->x - t2->x) > EPS) {
			t = t0->x / (t2->x - t1->x);
			bump_v->x = ov0.x - ov1.x - t * (ov2.x - ov1.x);
			bump_v->y = ov0.y - ov1.y - t * (ov2.y - ov1.y);
			bump_v->z = ov0.z - ov1.z - t * (ov2.z - ov1.z);
		} else if (fabs(t0->x - t2->x) > EPS) {
			t = t1->x / (t0->x - t2->x);
			bump_v->x = ov1.x - ov2.x - t * (ov0.x - ov2.x);
			bump_v->y = ov1.y - ov2.y - t * (ov0.y - ov2.y);
			bump_v->z = ov1.z - ov2.z - t * (ov0.z - ov2.z);
		} else {
			bump_v->x = 0;
			bump_v->y = 0;
			bump_v->z = 0;
		}
		mi_vector_normalize(bump_v);

		/* check for consistent orientation of bump_v */
		if (bump_v->x < 0.0) {
			bump_v->x = -bump_v->x;
			bump_v->y = -bump_v->y;
			bump_v->z = -bump_v->z;
		}
		/* find a line of constant v on the triangle */
		mi_vector_prod(bump_u, &state->normal, bump_v);
		}
	}
}


/*
 * Return nonzero if the current light is in the material's light list. This
 * is used in soft_material to determine if a material casts shadows from the
 * current light. This function ONLY works for material shaders that use
 * struct soft_material.
 */

miBoolean mi_mtl_is_casting_shadow(
	register miState		*state,
	register struct soft_material	*paras)
{
	register int			num;
	miInstance			*inst;		/* leaf instance */

	inst = (miInstance *)mi_db_access(state->light_instance);
	for (num=0; num < paras->n_light; num++) {
		miTag light = paras->light[paras->i_light + num];
		if (light == inst->parent || light == state->light_instance) {
			mi_db_unpin(state->light_instance);
			return(miTRUE);
		}
	}
	for (num=0; num < paras->n_difflight; num++) {
		miTag light = paras->difflight[paras->i_difflight + num];
		if (light == inst->parent || light == state->light_instance) {
			mi_db_unpin(state->light_instance);
			return(miTRUE);
		}
	}
	mi_db_unpin(state->light_instance);
	return(miFALSE);
}


/*
 * calculate the index of refraction of the incoming (ior_in) and the
 * outgoing (ior_out) ray, relative to the current intersection. This
 * function ONLY works for material shaders that use struct soft_material.
 *
 * It first counts how many times the ray has previously been transmitted
 * through the material we have just intersected. If this number is even,
 * we are entering the material. If the number is odd, we are leaving it.
 * While counting the number of transmissions, the state where the material
 * was first entered is found. This is used for the case of an exiting ray
 * to find what the index of refraction was outside that material.
 */

void mi_mtl_refraction_index(
	miState			*state,
	struct soft_material	*paras,
	miScalar		*ior_in,
	miScalar		*ior_out)
{
	register int		num = 0;
	register miState	*s, *s_in = NULL;/*for finding enclosing mtl */

	for (s=state; s; s=s->parent)			/* history? */
		if ((s->type == miRAY_TRANSPARENT	   ||
		     s->type == miRAY_REFRACT		   ||
		     s->type == miPHOTON_TRANSMIT_SPECULAR ||
		     s->type == miPHOTON_TRANSMIT_GLOSSY   ||
		     s->type == miPHOTON_TRANSMIT_DIFFUSE) &&
		     s->parent && s->parent->shader == state->shader) {
			num++;
			if (!s_in)
				s_in = s->parent;
		}

	if (!(num & 1)) {				/* even: entering */
		*ior_out = paras->ior;
		*ior_in	 = state->parent && state->parent->ior != 0 ?
				state->parent->ior : 1;
		if (!state->refraction_volume)
			state->refraction_volume = state->volume;
	} else {					/* odd: exiting */
		*ior_in	 = paras->ior;
		*ior_out = s_in && s_in->ior_in != 0 ? s_in->ior_in : 1;
		state->refraction_volume = s_in ? s_in->volume
						: state->camera->volume;
	}

	state->ior_in = *ior_in;
	state->ior    = *ior_out;
}


/*
 * this functions loops over the textures and updates the value of
 * each parameter in the data structure, it also computes the bump
 * vector. The parameters are written back to a different paras
 * struct, m, so that future calls to the same shader can work with
 * the original paras.
 * Note that tex->blend can be larger than 1 (it is sick, but the user
 * might want it).  Then the resulting reflection coefficients (specular,
 * diffuse) may be negative.
 */

void mi_mtl_textures(
	miState			*state,		/* ray tracer state */
	register struct soft_material *m,	/* returned updated paras */
	struct soft_material	*paras,		/* raw shader paras */
	miVector		*normal,	/* bump-mapped state->normal */
	miScalar		*dot_nd)	/* normal <dot> state->dir */
{
	int			n;		/* texture counter */
	miColor			result;		/* color from light source */
	miColor			color;		/* neighbor texture sample */
	register struct Tex	*tex;		/* current texture */
	register double		blend, iblend;	/* blend, 1-blend factor */
	register double		effect,ieffect;	/* effect, 1-effect from comp*/
	double			intensity;	/* intensity from texture */

	*normal = state->normal;
	*dot_nd = state->dot_nd;

	for (n=0; n < paras->n_texture; n++) {
		tex = &paras->texture[paras->i_texture + n];
		miASSERT(tex);
		if (!mi_lookup_color_texture(&result, state, (miTag)tex->map,
						&state->tex_list[tex->space])
							|| result.a < -0.001)
			continue;
		blend = tex->blend;
		if (tex->mask == 2 || tex->comp == 2)
			intensity = INTENSITY(result);
		if (tex->mask == 1)
			blend *= result.a;
		else if (tex->mask == 2)
			blend *= intensity;
		iblend	= 1 - blend;
		effect	= tex->comp == 1 ? result.a : intensity;
		ieffect = 1 - effect;

		if (tex->diffuse != 0)
		    if (tex->mask == 3) {
			m->diffuse.r  = m->diffuse.r * tex->diffuse * result.r;
			m->diffuse.g  = m->diffuse.g * tex->diffuse * result.g;
			m->diffuse.b  = m->diffuse.b * tex->diffuse * result.b;
		    } else {
			m->diffuse.r  = iblend * m->diffuse.r
				      +	 blend * (tex->diffuse	* result.r);
			m->diffuse.g  = iblend * m->diffuse.g
				      +	 blend * (tex->diffuse	* result.g);
			m->diffuse.b  = iblend * m->diffuse.b
				      +	 blend * (tex->diffuse	* result.b);
		    }

		if (tex->transp != 0)
			m->transp     = iblend * m->transp
				      +	 blend * (tex->transp  >= 0
						?  tex->transp	*  effect
						: -tex->transp	* ieffect);
		if (tex->blackwhite && (result.r <= 0.004 &&
					result.g <= 0.004 &&
					result.b <= 0.004 ||
					result.r >= 0.996 &&
					result.g >= 0.996 &&
					result.b >= 0.996))
			m->transp = 1;

		if (state->type == miRAY_SHADOW)
			continue;

		if (tex->ambient != 0) {
			m->ambient.r  = iblend * m->ambient.r
				      +	 blend * (tex->ambient	* result.r);
			m->ambient.g  = iblend * m->ambient.g
				      +	 blend * (tex->ambient	* result.g);
			m->ambient.b  = iblend * m->ambient.b
				      +	 blend * (tex->ambient	* result.b);
		}
		if (tex->specular != 0) {
			m->specular.r = iblend * m->specular.r
				      +	 blend * (tex->specular * result.r);
			m->specular.g = iblend * m->specular.g
				      +	 blend * (tex->specular * result.g);
			m->specular.b = iblend * m->specular.b
				      +	 blend * (tex->specular * result.b);
		}
		if (tex->reflect != 0)
			m->reflect    = iblend * m->reflect
				      +	 blend * (tex->reflect >= 0
						?  tex->reflect *  effect
						: -tex->reflect * ieffect);

		if (tex->bump != 0) {
			double	 udiff, vdiff, wdiff, rem;
			miVector coord, bump_x, bump_y;

			if (tex->method == 666) {		/* 3D tex */
				state->point.x += tex->u_unit;
				mi_lookup_color_texture(&color, state,
						(miTag)tex->map,&state->point);
				state->point.x -= tex->u_unit;
				udiff = tex->bump * (tex->comp == 2
					? intensity - INTENSITY(color)
					: result.a - color.a);

				state->point.y += tex->v_unit;
				mi_lookup_color_texture(&color, state,
						(miTag)tex->map,&state->point);
				state->point.y -= tex->v_unit;
				vdiff = tex->bump * (tex->comp == 2
					? intensity - INTENSITY(color)
					: result.a - color.a);

				state->point.z += tex->v_unit;
				mi_lookup_color_texture(&color, state,
						(miTag)tex->map,&state->point);
				state->point.z -= tex->v_unit;
				wdiff = tex->bump * (tex->comp == 2
					? intensity - INTENSITY(color)
					: result.a - color.a);

				normal->x += udiff;
				normal->y += vdiff;
				normal->z += wdiff;
			} else do {				/* 2D tex */
				int sign;
				coord = state->tex_list[tex->space];
				if (coord.x + tex->u_unit < 1.0) {
					coord.x += tex->u_unit;
					sign = 1;
				} else {
					if (tex->u_wrap == 0.0) {
						coord.x -= tex->u_unit;
						sign = -1;
					} else {
						coord.x += tex->u_wrap;
						sign = 1;
					}
				}

				if (!mi_lookup_color_texture(&color, state,
						(miTag)tex->map, &coord))
					break;
				udiff = sign * tex->bump * (tex->comp == 2
					? intensity - INTENSITY(color)
					: result.a - color.a);

				coord = state->tex_list[tex->space];
				if (coord.y + tex->v_unit < 1.0) {
					coord.y += tex->v_unit;
					sign = 1;
				} else {
					if (tex->v_wrap == 0.0) {
						coord.y -= tex->v_unit;
						sign = -1;
					} else {
						coord.y += tex->v_wrap;
						sign = 1;
					}
				}

				if (!mi_lookup_color_texture(&color, state,
						(miTag)tex->map, &coord))
					break;
				vdiff = sign * tex->bump * (tex->comp == 2
					? intensity - INTENSITY(color)
					: result.a - color.a);

				mi_mtl_bump_basis(state, tex->method,
						tex->space, &bump_x, &bump_y);
				if (tex->method == 4) {
					udiff = -udiff;
					vdiff = -vdiff;
				}
				rem = 1 - udiff - vdiff;
				normal->x = rem * normal->x + udiff * bump_x.x
							    + vdiff * bump_y.x;
				normal->y = rem * normal->y + udiff * bump_x.y
							    + vdiff * bump_y.y;
				normal->z = rem * normal->z + udiff * bump_x.z
							    + vdiff * bump_y.z;
			} while (0);
			mi_vector_normalize(normal);
			*dot_nd = mi_vector_dot(normal, &state->dir);
		}
	}
	m->ambient.r *= m->ambience.r;
	m->ambient.g *= m->ambience.g;
	m->ambient.b *= m->ambience.b;
}


/*
 * Compute the amount of shadow cast by an object according to the diffuse
 * component and the transparency
 */

miBoolean mi_mtl_compute_shadow(
	register miColor	*result,
	struct soft_material	*m)
{
	register double		f, omf;

	if (m->transp < 0.5) {
		f = 2 * m->transp;
		result->r *= f * m->diffuse.r;
		result->g *= f * m->diffuse.g;
		result->b *= f * m->diffuse.b;
	} else {
		f = 2 * (m->transp - 0.5);
		omf = 1 - f;
		result->r *= f + omf * m->diffuse.r;
		result->g *= f + omf * m->diffuse.g;
		result->b *= f + omf * m->diffuse.b;
	}
	return(!BLACK(*result));
}


/*
 * modify transparency according to static blur settings
 */

void mi_mtl_static_blur(
	miState			*state,		/* ray tracer state */
	struct soft_material	*m)		/* textured material paras */
{
	if (m->sblur)
		m->transp = 1 - (1 - m->transp) *
			pow(fabs(mi_vector_dot(&state->dir, &state->normal)),
			    m->sblurdecay);
}


/*
 * calculate illumination of a material by its lights. m and paras are both
 * passed because m contains all the colors updated by textures, and paras
 * has the arrays (m is a copy of the fixed struct without appended arrays).
 *
 * Since 'diffuse' is not divided by Pi here, it must be interpreted as
 * the diffuse component f_{r,d} of the BRDF f_r.
 */

void mi_mtl_illumination(
	register miColor	*result,	/* returned illum color */
	miState			*state,		/* ray tracer state */
	struct soft_material	*m,		/* textured material paras */
	struct soft_material	*paras,		/* original material paras */
	double			dummy1,		/* 1.9 compatibility */
	double			dummy2)
{
	int			n;		/* light counter */
	miInteger		samples;	/* # of samples taken */
	miColor			color;		/* color from light source */
	miColor			sum;		/* summed sample colors */
	miVector		dir;		/* direction towards light */
	miScalar		dot_nl;		/* dot prod of normal and dir*/
	miScalar		ns;		/* specular factor */

	dummy1; dummy2;
	if (m->transp >= 1)
		result->r = result->g = result->b = result->a = 0;
	else {
		if (m->mode == 0)
		    *result = m->diffuse;
		else {
		    *result = m->ambient;
		    for (n=0; n < paras->n_light; n++) {
			sum.r = sum.g = sum.b = 0;
			samples = 0;
			while (mi_sample_light(&color, &dir, &dot_nl, state,
				paras->light[paras->i_light + n], &samples)) {

			    /*: Note that there is no division by Pi here :*/
			    sum.r += dot_nl * m->diffuse.r * color.r;
			    sum.g += dot_nl * m->diffuse.g * color.g;
			    sum.b += dot_nl * m->diffuse.b * color.b;
			    if (m->mode == 2) {	  /* Phong reflection model */
				ns = mi_phong_specular(m->shiny, state, &dir);
				if (ns > miEPS) {
				    sum.r += ns * m->specular.r * color.r;
				    sum.g += ns * m->specular.g * color.g;
				    sum.b += ns * m->specular.b * color.b;
				}
			    } else if (m->mode == 3) { /*SI Blinn/Phong model*/
				ns = mi_blong_specular(m->shiny, state, &dir);
				if (ns > miEPS) {
				    sum.r += ns * m->specular.r * color.r;
				    sum.g += ns * m->specular.g * color.g;
				    sum.b += ns * m->specular.b * color.b;
				}
			    }
			}
			if (samples) {
			    result->r += sum.r / samples;
			    result->g += sum.g / samples;
			    result->b += sum.b / samples;
			}
		    }
		    for (n=0; n < paras->n_difflight; n++) {
			sum.r = sum.g = sum.b = 0;
			samples = 0;
			while (mi_sample_light(&color, &dir, &dot_nl, state,
				paras->difflight[paras->i_difflight + n],
				&samples)) {

			    sum.r += dot_nl * m->diffuse.r * color.r;
			    sum.g += dot_nl * m->diffuse.g * color.g;
			    sum.b += dot_nl * m->diffuse.b * color.b;
			}
			if (samples) {
			    result->r += sum.r / samples;
			    result->g += sum.g / samples;
			    result->b += sum.b / samples;
			}
		    }
		}
		result->a = 1;
	}
}


/*
 * add contribution from reflection ray to result color. The reflection depth
 * is checked here because it should also apply to mi_trace_environment, but
 * mental ray only checks it in mi_trace_reflection. Softimage also applies it
 * when no ray tracing is required.
 */

void mi_mtl_reflection(
	register miColor	*result,	/* returned illum color */
	miState			*state,		/* ray tracer state */
	struct soft_material	*m)		/* textured material paras */
{
	miColor			color;		/* color from reflected ray */
	miVector		dir;		/* direction towards light */
	miBoolean		ok;		/* secondary ray successful? */

	/*
	 * At least one Oz shader attempts to reflect a shadow ray.  This
	 * is incorrect, and since state->dot_nd is not set correctly for
	 * shadow rays, mi_reflection_dir() cannot (and should not) compute
	 * a correct reflection direction.  Therefore the following assumption
	 * is beneficial to protect against abuse of mi_mtl_reflection() and
	 * the problems such abuse causes elsewhere in raylib:
	 */
	miASSERT(state->type != miRAY_SHADOW);

	if (m->reflect > 0) {
		miScalar f = 1 - m->reflect;
		result->r *= f;
		result->g *= f;
		result->b *= f;

		if (state->reflection_level < state->options->reflection_depth
		  && state->reflection_level + state->refraction_level <
						state->options->trace_depth) {
			miScalar save_ior = state->ior;
			state->ior = state->ior_in;
			mi_reflection_dir(&dir, state);
			if (m->notrace) {
				miTag savevol = state->volume;
				state->volume = 0;
				ok = mi_trace_environment(&color, state, &dir);
				state->volume = savevol;
			} else
				ok = mi_trace_reflection (&color, state, &dir);
			if (ok) {
				result->r += m->reflect * color.r;
				result->g += m->reflect * color.g;
				result->b += m->reflect * color.b;
			}
			state->ior = save_ior;
		}
	}
}


/*
 * Add contribution from refraction or dissolve ray to result color. In
 * case of total reflection, cast a reflection ray instead.  When trace is
 * off refraction is approximated by transmission.
 */

void mi_mtl_refraction(
	register miColor	*result,	/* returned illum color */
	miState			*state,		/* ray tracer state */
	struct soft_material	*m,		/* textured material paras */
	double			ior_in,		/* index of refraction in */
	double			ior_out)	/* index of refraction out */
{
	miColor			color;		/* color from reflected ray */
	miVector		dir;		/* direction towards light */
	miBoolean		ok;		/* ray suceeded */

	miASSERT(state->type != miRAY_SHADOW);

	if (m->transp > 0) {
		miScalar f = 1 - m->transp;
		result->r *= f;
		result->g *= f;
		result->b *= f;
		result->a *= f;
		state->refraction_level--;

		if (!state->options->trace ||
		    mi_refraction_dir(&dir, state, ior_in, ior_out)) {
			/* Trace refraction (unless no change in index of
			   refraction, then trace transparent ray).
			   If trace is off, mi_trace_refraction() will issue
			   a warning and then call mi_trace_transparent(). */
			ok = ior_in == ior_out
				? mi_trace_transparent(&color, state)
				: mi_trace_refraction (&color, state, &dir);
		} else {
			/* Total internal reflection, so trace reflection
			   instead of refraction */
			state->ior = state->ior_in;
			mi_reflection_dir(&dir, state);
			if (m->notrace) {
				miTag savevol = state->volume;
				state->volume = 0;
				ok = mi_trace_environment(&color, state, &dir);
				state->volume = savevol;
			} else
				ok = mi_trace_reflection (&color, state, &dir);
		}
		state->refraction_level++;
		if (ok) {
			result->r += m->transp * color.r;
			result->g += m->transp * color.g;
			result->b += m->transp * color.b;
			result->a += m->transp * color.a;
		}
	}
}


/*
 * Add contribution from global illumination and caustics
 */

void mi_mtl_globillum(
	register miColor	*result,	/* returned illum color */
	miState			*state,		/* ray tracer state */
	struct soft_material	*m)		/* textured material paras */
{
	miColor			irrad;

	if BLACK(m->diffuse)
		return;

	/* Compute irradiance using the photon map or final gather */
	mi_compute_irradiance(&irrad, state);

	/* Multiply irradiance by the diffuse BRDF f_{r,d} = albedo/pi
	   and add to result. */
	/*: See comment above at 'Globillum notes' :*/
	result->r += m->diffuse.r * (1.0/M_PI) * irrad.r;
	result->g += m->diffuse.g * (1.0/M_PI) * irrad.g;
	result->b += m->diffuse.b * (1.0/M_PI) * irrad.b;
}


/*****************************************************************************
 **									    **
 **			      material shaders				    **
 **									    **
 *****************************************************************************/

/*
 * SOFTIMAGE has four illumination models: constant, lambert, phong and
 * blinn. The only difference between the first three appears to be that
 * phong does everything, lambert doesn't do specular computations and
 * constant doesn't do either diffuse or specular computations. Since
 * the cost of turning these computations off is low (if the corresponding
 * components are zero in the material a flag will be set for the triangle),
 * we just use one shader here and set the appropriate components to zero.
 *
 * This shader is also used as shadow shader. The shadow shader is not
 * called by newer translators at all; for backwards-compatibility it
 * still exists and does nothing but call the material shader. Shadow
 * shaders make sure that the shadow-casting object is in our light list
 * (if not, return immediately). The texture loop is shared, but shadow
 * shaders don't do lighting and secondary rays. This is a shadow shader
 * if state->type == miRAY_SHADOW, and a material shader otherwise.
 */

/*
#define INTENSITY(c) (0.299*(c).r + 0.587*(c).g + 0.114*(c).b)
*/
#define INTENSITY(c) (((c).r + (c).g + (c).b) * 0.333333)

/*
 * Softimage material shader.
 * Illumination paths of the types ...S*E are simulated, as well as direct
 * glossy illumination LGS*E.  That is, glossy reflections are only possible
 * directly from the light source (and are incorrectly called 'specular':
 * Phong and Blinn reflection is actually glossy).
 */

int soft_material_version(void) {return(1);}

miBoolean soft_material(
	register miColor	*result,
	register miState	*state,
	struct soft_material	*paras)
{
	struct soft_material	m;		/* work area for material */
	miScalar		ior_in,ior_out;	/* index of refraction */

	if (miRAY_PHOTON(state->type))
		 /* Called as a photon shader */
		 return soft_material_photon(result, state, paras);

	if (state->type == miRAY_SHADOW) {
				/* Is material casting any shadow at all? */
		if (!mi_mtl_is_casting_shadow(state, paras))
			return(miTRUE);
				/* Shadow segments */
		if (state->options->shadow == 's') {
			mi_mtl_refraction_index(state, paras,
						&ior_in, &ior_out);
			mi_trace_shadow_seg(result, state);
		}
	} else
		mi_mtl_refraction_index(state, paras, &ior_in, &ior_out);

	m = *paras;
	mi_mtl_textures(state, &m, paras, &state->normal, &state->dot_nd);
	mi_mtl_static_blur(state, &m);

	if (state->type == miRAY_SHADOW)
		return(mi_mtl_compute_shadow(result, &m));

	mi_mtl_illumination(result, state, &m, paras, 0.0, 0.0);
	mi_mtl_globillum(result, state, &m);
	mi_mtl_reflection(result, state, &m);
	mi_mtl_refraction(result, state, &m, ior_in, ior_out);

	return(miTRUE);
}


/*
 * Get the reflection parameters into a decent range such that they
 * are energy preserving
 */

void make_energy_preserving(
	miState			*state,
	struct soft_material	*m)
{
	miScalar		scatter, r, g, b, max;

	/* Check scattering coefficients (transp + reflect) */

	state;
	if (m->transp < 0.0f) m->transp = 0.0f;
	if (m->reflect < 0.0f) m->reflect = 0.0f;
	scatter = m->transp + m->reflect;
	if (scatter > 1.0f) {
		/* Too high transmittance and reflectance: normalize */
		m->transp /= scatter;
		m->reflect /= scatter;
	} else {
		/* Set reflection */
		m->reflect = 1.0f - m->transp;
	}
	miASSERT(fabs(m->transp + m->reflect - 1.0f) < miEPS);

	/* Check reflection coefficients.  The reflection coeffiecients
	   can add to more than 1 just because that is the tradition in
	   Softimage materials.	 The coefficients can even be negative if
	   a texture is blended with a blend larger than 1! */
	r = m->specular.r + m->diffuse.r;
	g = m->specular.g + m->diffuse.g;
	b = m->specular.b + m->diffuse.b;
	max = mi_MAX3(r, g, b);

	if (max > 1.0f) {
		/* The specified reflection coefficients are too large.
		   Divide them by max (plus miEPS to protect against
		   numerical imprecision) */
		miScalar inv_max = 1.0f / (max + miEPS);
		m->specular.r *= inv_max;
		m->specular.g *= inv_max;
		m->specular.b *= inv_max;
		m->diffuse.r *= inv_max;
		m->diffuse.g *= inv_max;
		m->diffuse.b *= inv_max;

		r = m->specular.r + m->diffuse.r;
		g = m->specular.g + m->diffuse.g;
		b = m->specular.b + m->diffuse.b;
		max = mi_MAX3(r, g, b);
	}
	miASSERT(max <= 1.0);
}


/*
 * The photon material shader (for global illumination and caustics)
 * Illumination paths of the types L(S|D)*D... are simulated.
 */

int soft_material_photon_version(void) {return(1);}

miBoolean soft_material_photon(
	miColor               *energy,   /* not "*result" */
	miState		      *state,
	struct soft_material  *paras)
{
	miColor color;
	miVector dir;
	miScalar ior_in, ior_out;
	struct soft_material m = *paras;
	miRay_type type;

	if (!miRAY_PHOTON(state->type))
		 /* Not called as a photon shader! */
		 return soft_material(energy, state, paras);

	mi_mtl_textures(state, &m, paras, &state->normal, &state->dot_nd);
	mi_mtl_static_blur(state, &m);

	if (paras->mode == 0)	/* no photons reflected from ambient material*/
		return(miTRUE); /* (ambient is somewhat like an emitter) */

	if (paras->mode < 2)
		m.specular = black;

	/* No light suckers for global illumination, please */
	miASSERT(energy->r >= 0 && energy->g >= 0 && energy->b >= 0);

	/* Get the reflection parameters into a decent range */
	make_energy_preserving(state, &m);

	/*
	 * Insert photon in map if this is a diffuse surface (and not
	 * direct illumination from a light source)
	 */

	if (m.diffuse.r > miEPS || m.diffuse.g > miEPS || m.diffuse.b >miEPS)
		mi_store_photon(energy, state);

	/*
	 * Shoot new photon
	 */

	/* Choose scatter type */
	type = mi_choose_scatter_type(state, m.transp,
				      &m.diffuse, &black, &m.specular);

	/* Compute new photon color (compensating for Russian roulette) and
	   shoot new photon in a direction determined by the scattering type */
	switch (type) {

	  case miPHOTON_REFLECT_SPECULAR:   /* specular reflection (mirror) */
		color.r = energy->r * m.specular.r;
		color.g = energy->g * m.specular.g;
		color.b = energy->b * m.specular.b;
		mi_reflection_dir_specular(&dir, state);
		return(mi_photon_reflection_specular(&color, state, &dir));

	  case miPHOTON_REFLECT_DIFFUSE:   /* diffuse reflection (Lamberts
					       cosine law)*/
		color.r = energy->r * m.diffuse.r;
		color.g = energy->g * m.diffuse.g;
		color.b = energy->b * m.diffuse.b;
		mi_reflection_dir_diffuse(&dir, state);
		return(mi_photon_reflection_diffuse(&color, state, &dir));

	  case miPHOTON_TRANSMIT_SPECULAR:   /* specular transmission
						  (Snell's law) */
		color.r = energy->r * m.specular.r;
		color.g = energy->g * m.specular.g;
		color.b = energy->b * m.specular.b;
		mi_mtl_refraction_index(state, &m, &ior_in, &ior_out);
		miASSERT(ior_in > 0.0 && ior_out > 0.0);
		if (ior_out == ior_in) {
			return mi_photon_transparent( &color, state );
		} else {
			if (mi_transmission_dir_specular(&dir, state,
							 ior_in, ior_out))
				return(mi_photon_transmission_specular(&color,
								       state,
								       &dir));
			else
				return(miFALSE);
		}
	  case miPHOTON_TRANSMIT_DIFFUSE:   /* diffuse transmission
						   (translucency) */
		color.r = energy->r * m.diffuse.r;
		color.g = energy->g * m.diffuse.g;
		color.b = energy->b * m.diffuse.b;
		mi_transmission_dir_diffuse(&dir, state);
		return(mi_photon_transmission_diffuse(&color, state, &dir));

	  default:   /* no reflection or transmission */
		miASSERT(type == miPHOTON_ABSORB);
		return(miTRUE);
	}
}


/*
 * Accumulate the shadow filter along the path to the light. This is called
 * indirectly by the light shaders when they call mi_trace_shadow, which
 * determines whether there is some triangle between the light source and
 * the point to be illuminated. If there is one, mi_trace_shadow calls this
 * routine to find out how much lights will pass through the triangle,
 * and what color.
 * Return 0 if no light passes through the triangle; this means that we
 * can stop looking for more occluding triangles because no light will pass.
 *
 * This is no longer used. New mi files call the material shader as shadow
 * shader directly. It's only here for old .mi files that call "soft_shadow".
 */

int soft_shadow_version(void) {return(1);}

miBoolean soft_shadow(
	register miColor		*result,
	register miState		*state,
	register struct soft_material	*paras)
{
	return(soft_material(result, state, paras));
}



/*****************************************************************************
 **									    **
 **				 2D  shaders				    **
 **									    **
 *****************************************************************************/

/*
 * Only the part of the map within the cropping boundaries is used, and this
 * is expanded as if it were the full map.
 * Mapping is always within the (0, 1) range. Anything less than 0 is mapped
 * to black; anything in the range [1, 2] is folded to [0, 1]. Folding allows
 * texture seams to be in the middle of triangles; in these cases one or two
 * vertices have U/V > 1 to make the texture run in the right direction. To
 * avoid problems due to rounding errors, we actually expand the range by EPS.
 */

int soft_color_version(void) {return(1);}

miBoolean soft_color(
	register miColor	*result,
	register miState	*state,
	struct soft_color	*paras)
{
	miVector		u;

	if (mi_remap_parameter(&u, &state->tex, state, paras)) {
		return(mi_lookup_color_texture(result, state,
						(miTag)paras->texture, &u));
	} else {
		result->r = result->g = result->b = result->a = 0;
		return(miFALSE);
	}
}


/*
 * this texture shader is called by a vertex color texture definition.
 * Such a texture is defined for each material using Vertex Color shading
 * in Softimage. In this case, the translated shading model is Constant
 * and a special texture is defined (containing a reference to this shader)
 * in order to handle the computation of the material diffuse value for the
 * current ray / triangle intersection point.
 *
 */

int soft_vertex_color_version(void) {return(1);}

miBoolean soft_vertex_color(
	register miColor	*result,
	register miState	*state,
	void			*paras)
{
	paras;
	result->r = state->tex.x;
	result->g = state->tex.y;
	result->b = state->tex.z;
	result->a = 1;
	return(miTRUE);
}


/*
 * the values defined in the texture space for each triangle vertex are
 * composited with the following fashion:
 *   new r = (alpha << 8) | r
 *   new g = (alpha << 8) | g
 *   new b = (alpha << 8) | b
 * the value that gets interpolated thus contains both the unmodified alpha
 * channel (because the same value has been passed for the 3 vertices)
 * and the interpolated color.
 */

int soft_vertex_color_alpha_version(void) {return(1);}

miBoolean soft_vertex_color_alpha(
	register miColor	*result,
	register miState	*state,
	void			*paras)
{
	paras;
	result->r = (1./255.) *	 (((int)state->tex.x) & 0x000000ff);
	result->g = (1./255.) *	 (((int)state->tex.y) & 0x000000ff);
	result->b = (1./255.) *	 (((int)state->tex.z) & 0x000000ff);
	result->a = (1./255.) * ((((int)state->tex.x) & 0x0000ff00) >> 8);
	return(miTRUE);
}


/*
 * this is the same as the previous except that alpha is not taken from
 * bits 8-15 of the Z texture vector component, but from bits 0-7 of the
 * X component of the texture vector in the next texture space. Added in
 * 1.9.12.4, 2.0.28.11, and 2.1.33.38.
 */

int soft_vertex_color_alpha2_version(void) {return(1);}

miBoolean soft_vertex_color_alpha2(
	register miColor	*result,
	register miState	*state,
	int			*paras)
{
	state;
	result->r = (1./255.) * (((int)state->tex.x) & 0x000000ff);
	result->g = (1./255.) * (((int)state->tex.y) & 0x000000ff);
	result->b = (1./255.) * (((int)state->tex.z) & 0x000000ff);
	result->a = (1./255.) * (((int)state->tex_list[*paras+1].x)
						     & 0x000000ff);
	return(miTRUE);
}


/*
 * this is the same as the previous except that alpha is taken from its
 * own texture space not the previous one + 1!
 * The way it is now, compare to soft_vertex_color_alpha2, it's that there
 * is always 2 texture space, but we can't garantee that its offset is x,y,z
 * position +1. So, to be able to push the rgba in mr from Sumatra, we've
 * created a soft_vertex_color_alpha3 spdl so the tspace component type is
 * not an integer anymore, but a texturespace. This index (tspace), doesn't
 * need to have the same value has the texturespace index found in
 * soft_material texturelist (space component)...
 * Then, with this modification the soft_material push one texture space
 * (RGB values) & soft_vertex_color_alpha3 shader will then push a the other
 * texture space (Alpha value)
 */

int soft_vertex_color_alpha3_version(void) {return(1);}

miBoolean soft_vertex_color_alpha3(
	register miColor	*result,
	register miState	*state,
	int			*paras)
{
	result->r = (1./255.) * (((int)state->tex.x) & 0x000000ff);
	result->g = (1./255.) * (((int)state->tex.y) & 0x000000ff);
	result->b = (1./255.) * (((int)state->tex.z) & 0x000000ff);
	result->a = (1./255.) * (((int)state->tex_list[*paras].x) & 0xff);
	return(miTRUE);
}


#if 0	/* Softimage doesn't like the merged version for Sumatra 1.0 */
	/* Use this and delete the #else part after March 16, 2000 */
/*
 * Remap the parameter space, taking into account the repetition factors,
 * alternation flags and cropping boundaries. Texture coordinates < 0 are
 * clipped to 0, but texture coordinates > 1 wrap around into [0, 1].
 * This is done because Softimage has spherical and cylindrical projections
 * that often put the texture seam into the middle of the triangle, which
 * means that one end is < 1 and the other end > 1. Doing the modulo in
 * the translator would make the texture "rewind" and go the wrong way.
 * This routine is called by the 2D texture shaders above.
 * The new method 32 (why "& 32"?! It can't be combined with the others) is
 * a reimplementation of a new Sumatra method designed by Softimage in 3/00.
 * This reimplementation isn't redundant and fixes potential divisions by 0.
 */

#define EPS 1e-4

miBoolean mi_remap_parameter(
	register miVector	*u_out,		/* ret parameter space coord */
	miVector		*u_in,		/* texture coords to remap */
	register miState	*state,		/* for uv coord */
	register struct soft_color *paras)	/* user parameters */
{
	miVector		uraw;		/* parameter space coord */
	miVector		uin;		/* transformed parm spc coord*/
	register int		i;
	miBoolean		weird = paras->method & 32 ? miTRUE : miFALSE;
	float			*mat = paras->transform;

	state;
	miASSERT(paras);
	uraw = *u_in;
	if (weird) {
		if (uraw.x < 0) uraw.x -= floor(uraw.x);
		if (uraw.y < 0) uraw.y -= floor(uraw.y);

	} else if (paras->method != 3 && paras->method != 5) { /* not UV */
		if (uraw.x < 0) uraw.x += 1;
		if (uraw.y < 0) uraw.y += 1;
	}
	uin = uraw;
	if (mat[15]) {
		if (weird) {
			miMatrix tmp;
			mi_matrix_ident(tmp);
			tmp[0] = fabs(mat[0]) > 1e-10 ? 1 / mat[0] : 1e10;
			tmp[5] = fabs(mat[0]) > 1e-10 ? 1 / mat[5] : 1e10;
			if (paras->swap) {
				tmp[12] = -((1-mat[0]) - mat[12]) / mat[0];
				tmp[13] = -mat[13] / mat[5];
			} else {
				tmp[12] = -mat[12]/mat[0];
				tmp[13] = -((1-mat[5]) - mat[13]) / mat[5];
			}
			mat = tmp;
		}
		uin.x = mat[0] * uraw.x +
			mat[4] * uraw.y +
			mat[8] * uraw.z +
			mat[12];
		uin.y = mat[1] * uraw.x +
			mat[5] * uraw.y +
			mat[9] * uraw.z +
			mat[13];

		if (paras->u_torus)
			if (uin.x < 0)
				uin.x += weird ? fabs(mat[0]) : mat[0];
			else if (uin.x > 1)
				uin.x -= weird ? fabs(mat[0]) : mat[0];
		if (paras->v_torus)
			if (uin.y < 0)
				uin.y += weird ? fabs(mat[5]) : mat[5];
			else if (uin.y > 1)
				uin.y -= weird ? fabs(mat[5]) : mat[5];
	} else {
		if (paras->u_torus)
			if (uin.x < 0)
				uin.x += 1.0;
			else if (uin.x > 1)
				uin.x -= 1.0;
		if (paras->v_torus)
			if (uin.y < 0)
				uin.y += 1.0;
			else if (uin.y > 1)
				uin.y -= 1.0;

	}
	if (paras->method != 5 && (uin.x < -EPS || uin.x > 1 && uraw.x <= 1 ||
				   uin.y < -EPS || uin.y > 1 && uraw.y <= 1))
		return(miFALSE);

	if (paras->method != 3) {
		uin.x -= floor(uin.x);
		uin.y -= floor(uin.y);
	}
	if (paras->swap) {
		u_out->y = uin.x <= 0 ? 0 : uin.x * paras->repu;
		u_out->x = uin.y <= 0 ? 0 : uin.y * paras->repv;
	} else {
		u_out->x = uin.x <= 0 ? 0 : uin.x * paras->repu;
		u_out->y = uin.y <= 0 ? 0 : uin.y * paras->repv;
	}

	i = (int)u_out->x;
	u_out->x -= i;
	if (paras->altu && i%2 == 1)
		u_out->x = 1 - u_out->x;
	u_out->x = (paras->maxu - paras->minu) * u_out->x + paras->minu;

	i = (int)u_out->y;
	u_out->y -= i;
	if (paras->altv && i%2 == 0)
		u_out->y = 1 - u_out->y;
	u_out->y = (paras->maxv - paras->minv) * u_out->y + paras->minv;

	if ((paras->method == 3 || weird) &&
			(uin.x < 0 || uin.x > 1 || uin.y < 0 || uin.y > 1))
		return(miFALSE);

	return(miTRUE);
}
#else		/* old junk, ugly, slow, and buggy, but Softimage loves it */
/*
 * Remap the parameter space, taking into account the repetition factors,
 * alternation flags and cropping boundaries. Texture coordinates < 0 are
 * clipped to 0, but texture coordinates > 1 wrap around into [0, 1].
 * This is done because Softimage has spherical and cylindrical projections
 * that often put the texture seam into the middle of the triangle, which
 * means that one end is < 1 and the other end > 1. Doing the modulo in
 * the translator would make the texture "rewind" and go the wrong way.
 * This routine is called by the 2D texture shaders above.
 */

#define EPS 1e-4

struct sib_remap_parameter {
	miVector	input;		/* texture shader (pic file) */
	miTag		texture;	/* texture shader (pic file) */
	miInteger	repu, repv;	/* repetition factor, >= 1 */
	miBoolean	altu, altv;	/* flip map on odd repetitions */
	miBoolean	swap;		/* swap U and V coordinates */
	miScalar	minu, maxu;	/* horizontal cropping boundaries */
	miScalar	minv, maxv;	/* vertical cropping boundaries */
	miMatrix	transform;	/* transformation matrix */
	miBoolean	u_torus;	/* set if map is cyl or spherical */
	miBoolean	v_torus;	/* same thing for V, for patches only*/
};

static miBoolean sib_remap_parameter(
	register miVector	*result,
	register miState	*state,
	struct sib_remap_parameter *paras);

miBoolean mi_remap_parameter(
	register miVector	*u_out,		/* ret parameter space coord */
	miVector		*u_in,		/* texture coords to remap */
	register miState	*state,		/* for uv coord */
	register struct soft_color *paras)	/* user parameters */
{
	state;
	miASSERT(paras);
	if (paras->method & 32) {
		struct sib_remap_parameter sib_params;

		sib_params.input = *u_in;
		sib_params.texture = paras->texture;
		sib_params.repu = paras->repu;
		sib_params.repv = paras->repv;
		sib_params.altu = paras->altu;
		sib_params.altv = paras->altv;
		sib_params.swap = paras->swap;
		sib_params.minu = paras->minu;
		sib_params.maxu = paras->maxu;
		sib_params.minv = paras->minv;
		sib_params.maxv = paras->maxv;
		
		mi_matrix_ident(sib_params.transform);
		/* danger, division by 0 possible and likely! thomas 3.10.00 */
		sib_params.transform[0] = 1/paras->transform[0];
		sib_params.transform[5] = 1/paras->transform[5];
		if (paras->swap) {
			sib_params.transform[12] = -((1-paras->transform[0]) -
				paras->transform[12])/paras->transform[0];
			sib_params.transform[13] =
				-paras->transform[13]/paras->transform[5];
		} else {
			sib_params.transform[12] =
				-paras->transform[12]/paras->transform[0];
			sib_params.transform[13] = -((1-paras->transform[5]) -
				paras->transform[13])/paras->transform[5];
		}
		sib_params.u_torus = paras->u_torus;
		sib_params.v_torus = paras->v_torus;

		return sib_remap_parameter(u_out, state, &sib_params);
	} else {
		miVector	uraw;		/* parameter space coord */
		miVector	uin;		/* transformed parm spc coord*/
		register int	i;

		uraw = *u_in;
		if (paras->method != 3 && paras->method != 5) { /* not UV */
			if (uraw.x < 0) uraw.x += 1;
			if (uraw.y < 0) uraw.y += 1;
		}
		uin = uraw;
		if (paras->transform[15]) {
			uin.x = paras->transform[0] * uraw.x +
				paras->transform[4] * uraw.y +
				paras->transform[8] * uraw.z +
				paras->transform[12];
			uin.y = paras->transform[1] * uraw.x +
				paras->transform[5] * uraw.y +
				paras->transform[9] * uraw.z +
				paras->transform[13];

			if (paras->u_torus)
				if (uin.x < 0)
					uin.x += paras->transform[0];
				else if (uin.x > 1)
					uin.x -= paras->transform[0];
			if (paras->v_torus)
				if (uin.y < 0)
					uin.y += paras->transform[5];
				else if (uin.y > 1)
					uin.y -= paras->transform[5];
		} else {
			if (paras->u_torus)
				if (uin.x < 0)
					uin.x += 1.0;
				else if (uin.x > 1)
					uin.x -= 1.0;
			if (paras->v_torus)
				if (uin.y < 0)
					uin.y += 1.0;
				else if (uin.y > 1)
					uin.y -= 1.0;

		}
		if (paras->method != 5 &&
		   (uin.x < -EPS || uin.y < -EPS || uin.x > 1 && uraw.x <= 1
						 || uin.y > 1 && uraw.y <= 1))
				return(miFALSE);

		if (paras->method != 3) {  /* either not UV, or UV-wrapped */
			uin.x -= floor(uin.x);
			uin.y -= floor(uin.y);
		}
		if (paras->swap) {
			u_out->y = uin.x <= 0 ? 0 : uin.x * paras->repu;
			u_out->x = uin.y <= 0 ? 0 : uin.y * paras->repv;
		} else {
			u_out->x = uin.x <= 0 ? 0 : uin.x * paras->repu;
			u_out->y = uin.y <= 0 ? 0 : uin.y * paras->repv;
		}

		i = (int)u_out->x;
		u_out->x -= i;
		if (paras->altu && i%2 == 1)
			u_out->x = 1 - u_out->x;
		u_out->x = (paras->maxu - paras->minu) * u_out->x + paras->minu;

		i = (int)u_out->y;
		u_out->y -= i;
		if (paras->altv && i%2 == 0)
			u_out->y = 1 - u_out->y;
		u_out->y = (paras->maxv - paras->minv) * u_out->y + paras->minv;

		if (paras->method == 3 && (uin.x < 0 || uin.x > 1 ||
					   uin.y < 0 || uin.y > 1))
			return(miFALSE);

		return(miTRUE);
	}
}


static miBoolean sib_remap_parameter(
	register miVector	*result,
	register miState	*state,
	struct sib_remap_parameter *paras)
{
	miVector	uraw;		/* parameter space coord */
	miVector	uin;		/* transformed parm spc coord*/
	register int	i;

	uraw = paras->input;
	if (uraw.x < 0) uraw.x -= floor(uraw.x);
	if (uraw.y < 0) uraw.y -= floor(uraw.y);
	uin = uraw;
	if (paras->transform[15]) {
		uin.x = paras->transform[0] * uraw.x +
				paras->transform[4] * uraw.y +
				paras->transform[8] * uraw.z +
				paras->transform[12];
		uin.y = paras->transform[1] * uraw.x +
				paras->transform[5] * uraw.y +
				paras->transform[9] * uraw.z +
				paras->transform[13];

		if (paras->u_torus)
			if (uin.x < 0)
				uin.x += fabs(paras->transform[0]);
			else if (uin.x > 1)
				uin.x -= fabs(paras->transform[0]);
		if (paras->v_torus)
			if (uin.y < 0)
				uin.y += fabs(paras->transform[5]);
			else if (uin.y > 1)
				uin.y -= fabs(paras->transform[5]);
	} else {
		if (paras->u_torus)
			if (uin.x < 0)
				uin.x += 1.0;
			else if (uin.x > 1)
				uin.x -= 1.0;
		if (paras->v_torus)
			if (uin.y < 0)
				uin.y += 1.0;
			else if (uin.y > 1)
				uin.y -= 1.0;

	}
	if (uin.x < -EPS || uin.y < -EPS || uin.x > 1 && uraw.x <= 1
			|| uin.y > 1 && uraw.y <= 1)
			return(miFALSE);
	
	uin.x -= floor(uin.x);
	uin.y -= floor(uin.y);

	if (paras->swap) {
		result->y = uin.x <= 0 ? 0 : uin.x * paras->repu;
		result->x = uin.y <= 0 ? 0 : uin.y * paras->repv;
	} else {
		result->x = uin.x <= 0 ? 0 : uin.x * paras->repu;
		result->y = uin.y <= 0 ? 0 : uin.y * paras->repv;
	}
	
	i = (int)result->x;
	result->x -= i;
	if (paras->altu && i%2 == 1)
		result->x = 1 - result->x;
	result->x = (paras->maxu - paras->minu) * result->x + paras->minu;

	i = (int)result->y;
	result->y -= i;
	if (paras->altv && i%2 == 0)
		result->y = 1 - result->y;
	result->y = (paras->maxv - paras->minv) * result->y + paras->minv;

	if (uin.x < 0 || uin.x > 1 || uin.y < 0 || uin.y > 1) return(miFALSE);

	return(miTRUE);
}
#endif


/*
 * Point light source. Must have an origin in the input file. Ignores
 * the direction.
 *
 * If this light shader is called because the light source is a visible area
 * light source and was hit, the result is simply paras->color (the color of
 * the light source).
 */

int soft_point_version(void) {return(1);}

miBoolean soft_point(
	register miColor	*result,
	register miState	*state,
	register struct soft_point *paras)
{
	*result = paras->color;

	if (state->type != miRAY_LIGHT) return (miTRUE);/* visible area light*/

	if (paras->atten)					/* dist atten*/
		if (state->dist >= paras->stop)
			return(miFALSE);

		else if (state->dist > paras->start &&
				fabs(paras->stop - paras->start) > EPS) {
			register double t = 1 - (state->dist - paras->start) /
						(paras->stop - paras->start);
			result->r *= t;
			result->g *= t;
			result->b *= t;
		}

	if (paras->shadow && paras->factor < 1) {		/* shadows: */
		miColor filter;
		filter.r = filter.g = filter.b = filter.a = 1;
		if (!mi_trace_shadow(&filter, state) ||
		    BLACK(filter)) {				/* opaque */
			result->r *= paras->factor;
			result->g *= paras->factor;
			result->b *= paras->factor;
			if (paras->factor == 0)
				return(miFALSE);
		} else {					/* transparnt*/
			double omf = 1 - paras->factor;
			result->r *= paras->factor + omf * filter.r;
			result->g *= paras->factor + omf * filter.g;
			result->b *= paras->factor + omf * filter.b;
		}
	}

	return(miTRUE);
}



/*
 * Spot light source. Must have an origin and direction in the input
 * file. Takes a variable number of parameters. The first is a boolean
 * which says whether the light casts a shadow or not. The second is the
 * shadow factor. The third and fourth are the cosine of the cone and
 * spread angles, respectively. The fifth is a boolean which turns
 * distance attenuation on or off. If on, the starting and ending
 * distances are the final two floating parameters.
 *
 * If this light shader is called because the light source is a visible area
 * light source and was hit, the result is simply paras->color (the color of
 * the light source).
 */

int soft_spot_version(void) {return(1);}

miBoolean soft_spot(
	register miColor	*result,
	register miState	*state,
	register struct soft_spot *paras)
{
	register double	d, t;

	*result = paras->color;

	if (state->type != miRAY_LIGHT) return (miTRUE);/* visible area light*/

	d = mi_vector_dot(&state->dir, &paras->direction);	/*angle atten*/
	if (d <= 0)
		return(miFALSE);
	if (d < paras->spread)
		return(miFALSE);
	if (d < paras->cone) {
		t = 1 - (d - paras->cone) / (paras->spread - paras->cone);
		result->r *= t;
		result->g *= t;
		result->b *= t;
	}
	if (paras->atten)					/* dist atten*/
		if (state->dist >= paras->stop)
			return(miFALSE);

		else if (state->dist > paras->start &&
				fabs(paras->stop - paras->start) > EPS) {
			t = 1 - (state->dist - paras->start) /
				(paras->stop - paras->start);
			result->r *= t;
			result->g *= t;
			result->b *= t;
		}
	if (paras->shadow && paras->factor < 1) {		/* shadows: */
		miColor filter;
		filter.r = filter.g = filter.b = filter.a = 1;
		if (!mi_trace_shadow(&filter, state) ||
		    BLACK(filter)) {				/* opaque */
			result->r *= paras->factor;
			result->g *= paras->factor;
			result->b *= paras->factor;
			if (paras->factor == 0)
				return(miFALSE);
		} else {					/* transparnt*/
			double omf = 1 - paras->factor;
			result->r *= paras->factor + omf * filter.r;
			result->g *= paras->factor + omf * filter.g;
			result->b *= paras->factor + omf * filter.b;
		}
	}

	return(miTRUE);
}



/*
 * Infinite light source. Must have a direction in the input file.
 * Takes two parameters. The first is a boolean which says whether the
 * light casts a shadow or not. The second is the shadow factor.
 */

int soft_infinite_version(void) {return(1);}

miBoolean soft_infinite(
	register miColor	*result,
	register miState	*state,
	register struct soft_infinite *paras)
{
	*result = paras->color;

	if (paras->shadow && paras->factor < 1) {		/* shadows: */
		miColor filter;
		filter.r = filter.g = filter.b = filter.a = 1;
		if (!mi_trace_shadow(&filter, state) ||
		    BLACK(filter)) {				/* opaque */
			result->r *= paras->factor;
			result->g *= paras->factor;
			result->b *= paras->factor;
			if (paras->factor == 0)
				return(miFALSE);
		} else {					/* transparnt*/
			miScalar omf = 1 - paras->factor;
			result->r *= paras->factor + omf * filter.r;
			result->g *= paras->factor + omf * filter.g;
			result->b *= paras->factor + omf * filter.b;
		}
	}

	return(miTRUE);
}



/*****************************************************************************
 **									    **
 **			    displacement shaders			    **
 **									    **
 *****************************************************************************/

/*
 * Accumulate the displacement from all listed textures, by accumulating
 * alphas or intensities from the color map lookups. This is done during
 * triangulation of surfaces, long before material shaders are called.
 */

int soft_displace_version(void) {return(1);}

miBoolean soft_displace(
	register miScalar	*result,
	register miState	*state,
	register struct soft_displace *paras)
{
	register int		n;		/* texture/light countdown */
	register struct DisTex	*tex;		/* current texture */
	miColor			color;		/* result from texture lookup*/

	*result = 0;
	color = black;

	for (n=0; n < paras->n_texture; n++) {
		tex = &paras->texture[paras->i_texture + n];
		if (tex->displace) {
			mi_lookup_color_texture(&color, state, (miTag)tex->map,
						&state->tex_list[tex->space]);
			*result += tex->displace * (tex->comp==1
					? color.a
					: INTENSITY(color));
		}
	}
	return(miTRUE);
}


/*
 * displace the surface by a constant distance given in the parameters.
 * This is used for static blurring of surfaces. The translator takes
 * care of polygonal objects, but surfaces must be done here because
 * the translator knows neither normals nor vertices of surfaces.
 */

int soft_staticblur_version(void) {return(1);}

miBoolean soft_staticblur(
	miScalar		*result,
	miState			*state,
	struct soft_staticblur	*paras)
{
	state;
	*result = paras->width;
	return(miTRUE);
}



/*****************************************************************************
 **									    **
 **			     environment shaders			    **
 **									    **
 *****************************************************************************/

int soft_env_sphere_version(void) {return(1);}

miBoolean soft_env_sphere(
	register miColor		*result,
	register miState		*state,
	register struct soft_env_sphere *paras)
{
	register double		theta;		/* for spherical mapping */
	miVector		dir;		/* state->dir in world space */
	miVector		coord;		/* coordinate in texture */
	miColor			color;		/* texture lookup result */
	int			n;		/* texture countdown */
	register struct EnvTex	*tex;		/* current texture */
	register double		blend, iblend;	/* blend, 1-blend factor */
	miScalar		in_range_fix;

	mi_vector_to_world(state, &dir, &state->dir);
	in_range_fix = fabs(dir.z) < 1.0
		? fabs(dir.z)*miHUGE_SCALAR
		: miHUGE_SCALAR;
	/* simon: this old line broke on DEC's ALPHA processor, causing a
	 *	  floating point exception (invalid number):
	 *	  theta = fabs(dir.z)*HUGE < fabs(dir.x) */
	theta = in_range_fix < fabs(dir.x)
		? dir.x > 0
			? 1.5 * M_PI
			: 0.5 * M_PI
		: dir.z > 0
			? 1.0 * M_PI + atan(dir.x / dir.z)
			: 2.0 * M_PI + atan(dir.x / dir.z);
	theta += 3.5 * M_PI - paras->rotate;
	while (theta > 2 * M_PI)
		theta -= 2 * M_PI;

	coord.x = 1 - theta / (2 * M_PI);
	coord.y = 1 - 0.5 * (dir.y + 1.0);
	coord.z = 0;

	result->r = result->g = result->b = result->a = 0;

	for (n=0; n < paras->n_texture; n++) {
		tex = &paras->texture[paras->i_texture + n];
		mi_lookup_color_texture(&color, state, (miTag)tex->map,&coord);

		blend = tex->blend;
		if (tex->mask == 1)
			blend *= color.a;
		else if (tex->mask == 2)
			blend *= INTENSITY(color);
		iblend = (blend < 1) ? (1 - blend) : 0;

		result->r = iblend * result->r + blend * color.r;
		result->g = iblend * result->g + blend * color.g;
		result->b = iblend * result->b + blend * color.b;
		result->a = iblend * result->a + blend * color.a;

		/* if (result->r > 1) result->r = 1;
		if (result->g > 1) result->g = 1;
		if (result->b > 1) result->b = 1;
		if (result->a > 1) result->a = 1; */
	}
	return(!BLACK(*result));
}



/*****************************************************************************
 **									    **
 **			     atmosphere shaders				    **
 **									    **
 *****************************************************************************/

/*
 * SOFTIMAGE-like atmosphere shader. SOFTIMAGE does things as follows:
 * they consider the total distance traveled along the ray path since the
 * eye. They start attenuating towards a given color at a given start
 * distance and end up with the given color and transparency at a given
 * end distance. We don't have enough information to emulate that here,
 * but it's a bogus hack anyway. So we substitute another bogus hack -
 * but at least it's ever so slightly more physical. We consider the eye
 * to be surrounded by sphericaly shaped layer of fog whose inner radius
 * is the start distance and whose outer layer is the end distance. We
 * attenuate as a linear function of the distance traveled through this
 * fog. Actually, this is the same as SOFTIMAGE for first generation rays.
 * The remaining bogus things about this are that the fog is always centered
 * on the eye and stays centered even if the eye moves around in an animation,
 * and that the attenuation is a linear function of distance.
 *
 * There are three required floating point parameters: start distance, ending
 * distance and fog transparency. The fog varies between totally transparent
 * and totally opaque. The given transparency is attained when a ray traverses
 * the distance "end - start". The color of the fog is taken from the
 * transmission color in the view.
 */

int soft_fog_version(void) {return(1);}

miBoolean soft_fog(
	register miColor	 *result,
	register miState	 *state,
	register struct soft_fog *paras)
{
	miVector org, dir;
	double a, twoa, b, bsq, c, foura, cin, cout, radin, radout;
	double t1, t2, t3, t4;

	if (!paras->lightrays && state->type == miRAY_LIGHT)
		return(miTRUE);

	if (state->dist <= 0) {		/* infinite distance */
		a = 1 - paras->rate;
		if (a > 1) a = 1;
		result->r = a * paras->transmit.r;
		result->g = a * paras->transmit.g;
		result->b = a * paras->transmit.b;
		result->a = a;
		return(miTRUE);
	}

	mi_point_to_camera (state, &org, &state->org);
	mi_vector_to_camera(state, &dir, &state->dir);
	a = mi_vector_dot(&dir, &dir);
	b = mi_vector_dot(&org, &dir) * 2;
	c = mi_vector_dot(&org, &org);
	cin  = c - paras->start * paras->start;
	cout = c - paras->stop	* paras->stop;

	bsq = b*b;
	foura = 4 * a;

	radin  = bsq - foura * cin;
	radout = bsq - foura * cout;

	if (radin < 0 && radout < 0)
		return(miTRUE);			/* never intersects fog */

	twoa = 2 * a;

	if (radin > 0) {
		radin = sqrt(radin);
		t1 = (-b - radin)/twoa;
		if (t1 > state->dist)
			t1 = t2 = state->dist;
		else {
			if (t1 < 0) t1 = 0;
			t2 = (-b + radin)/twoa;
			if (t2 < 0) t2 = 0;
			else if (t2 > state->dist) t2 = state->dist;
		}
	} else
		t1 = t2 = 0;

	if (radout > 0) {
		radout = sqrt(radout);
		t3 = (-b - radout)/twoa;
		if (t3 > state->dist)
			t3 = t4 = state->dist;
		else {
			if (t3 < 0) t3 = 0;
			t4 = (-b + radout)/twoa;
			if (t4 < 0) t4 = 0;
			else if (t4 > state->dist) t4 = state->dist;
		}
	} else
		t3 = t4 = 0;

	if ((t1 == 0 && t2 == 0 && t3 == 0 && t4 == 0) ||
	    (t1 == state->dist && t2 == state->dist &&
	     t3 == state->dist && t4 == state->dist))
		return(miTRUE);			/* never intersects fog */

	if (paras->stop > paras->start){
		a = (t4 - t3 - (t2 - t1))/(paras->stop - paras->start);
		a *= 1 - paras->rate;
		if (a > 1) a = 1;
	} else {
		a = (1-paras->rate) ? 1 : 0;
	}
	b = 1 - a;

	result->r = a * paras->transmit.r + b * result->r;
	result->g = a * paras->transmit.g + b * result->g;
	result->b = a * paras->transmit.b + b * result->b;
	result->a = a			  + b * result->a;
	return(miTRUE);
}


/*
 * SOFTIMAGE-like layered fog shader. This is an attempt to emulate the
 * SOFTIMAGE layered fog shader as accurate as possible while trying to
 * avoid its worst bugs at the same time. A layer of fog with y-extent
 * [base, base + thickness] in world space is simulated. The density of
 * the fog is given by a poisson (?) distribution (numerically given)
 * that is centered around y = base + thickness / 2 in world space. The
 * density drops to half of the maximum value at y = base and y = base +
 * thickness, respectively.
 * The extent of the fog is not only specified by the y-extent in world
 * space, but one can also give a start and stop distance of the fog
 * along the ray. This is of course in conflict with the specification
 * of the fog by its y-extent. It is included nevertheless in order to
 * maintain SOFTIMAGE compatibility. The best thing one can do is to use
 * start = 0.0 and stop = a large number to switch this option off.
 * Otherwise strange effects can occur. There are five required floating
 * point parameters: start distance, ending distance, base of fog layer,
 * thickness of fog layer and the fog density. The color of the fog is
 * also given. In addition the lightrays-flag specifies whether the fog
 * is applied to lightrays also.
 */

static double poisson[40] = {
	0.5000, 0.5398, 0.5793, 0.6179, 0.6554,
	0.6915, 0.7258, 0.7580, 0.7881, 0.8159,
	0.8413, 0.8643, 0.8849, 0.9032, 0.9192,
	0.9332, 0.9452, 0.9554, 0.9641, 0.9713,
	0.9772, 0.9821, 0.9861, 0.9893, 0.9918,
	0.9938, 0.9953, 0.9965, 0.9974, 0.9981,
	0.9987, 0.9990, 0.9992, 0.9995, 0.9997,
	0.9998, 0.9998, 0.9999, 0.9999, 1.0000 };

int soft_layered_fog_version(void) {return(1);}

miBoolean soft_layered_fog(
	register miColor		 *result,
	register miState		 *state,
	register struct soft_layered_fog *paras)
{
	miVector	dir;
	miVector	org;
	miScalar	a, b, DirY, PointY;
	miScalar	top, bottom, thick;
	miScalar	start, stop;
	double		dist;
	double		length;
	double		t_min, t_max, t_bottom, t_top;
	double		pntInt, pntCam;
	double		middle, step, intGbl;
	miInteger	niveauC, niveauI;

	if (!paras->lightrays && state->type == miRAY_LIGHT)
		return(miTRUE);

	/*
	 * transform from camera to world space
	 */
	mi_vector_to_world(state, &dir, &state->dir);
	mi_point_to_world (state, &org, &state->org);
	dist = state->dist;

	if (dist > 0.0) {
		miVector point;
		mi_point_to_world(state, &point, &state->point);
		PointY = point.y;
		pntCam = org.y;
	} else {
		dist = 0.5 * FLT_MAX;
		PointY = org.y + dist * dir.y;
		pntCam = org.y;
	}
	DirY = dir.y;
	pntInt = PointY;

	/*
	 * get shader parameters
	 */
	start  = paras->start;
	stop   = paras->stop;
	bottom = paras->base;
	thick  = paras->thickness;
	top    = bottom + thick;

	if ((pntInt > top    && pntCam > top  ) ||
	    (pntInt < bottom && pntCam < bottom))
		return(miTRUE);

	/*
	 * calculate length of the ray
	 */
	if (fabs(DirY) > 1e-6) {
		t_top = (top - pntCam) / DirY;
		t_bottom = (bottom - pntCam) / DirY;
		if (t_top > t_bottom) {
			t_max = t_top;
			t_min = t_bottom;
		} else {
			t_max = t_bottom;
			t_min = t_top;
		}
		if (t_max < 0.0)
			return(miTRUE);
		if (t_min < start)
			t_min = start;
		if (t_max > stop)
			t_max = stop;
	} else {
		t_min = start;
		t_max = stop;
	}
	if (t_min > t_max)
		return(miTRUE);

	pntInt = pntCam + t_max * DirY;
	length = t_max - t_min;

	/*
	 * calculate the attenuation
	 */
	middle = bottom + (thick / 2.0);
	step = (middle - bottom) / 30.0;		/* division to 3 */

	/* get the level for camera and point (middle == 0) */
	niveauC = fabs(pntCam - middle) / step;
	if (niveauC > 39)
		niveauC = 39;
	niveauI = fabs(pntInt - middle) / step;
	if (niveauI > 39)
		niveauI = 39;

	/*
	 * camera and point of interest are both on the same side of the max
	 */
	if (((pntCam >= middle) == (pntInt >= middle)) && niveauC != 0) {
		miInteger i,nb;
		double val = 0.0;

		nb = abs(niveauC-niveauI) + 1;
		if (niveauC < niveauI)
			for (i=niveauC; i < niveauC+nb; i++)
				val += 1.0 - poisson[i];
		else
			for (i=niveauI; i < niveauI+nb; i++)
				val += 1.0 - poisson[i];
		intGbl = val/nb;
	}
	/*
	 * camera and point of interest are on the opposite side of the maximum
	 */
	else {
		miInteger i;
		double valC = 0.0, valI = 0.0;

		for (i=0; i<=niveauC; i++)
			valC += 1.0 - poisson[i];
		for (i=0; i<=niveauI; i++)
			valI += poisson[i];
		intGbl = (valC + valI)/(niveauC + niveauI + 2);
	}
	a = 0.2 * paras->density * intGbl * length;

	/*
	 * color clipping of a
	 */
	if (a < 0.0)
		a = 0.0;
	else if (a > 1.0)
		a = 1.0;
	b = 1.0 - a;

	result->r = b * result->r + a * paras->fogcolor.r;
	result->g = b * result->g + a * paras->fogcolor.g;
	result->b = b * result->b + a * paras->fogcolor.b;
	return(miTRUE);
}


/*
 * Softimage depth of field shader. Goes by two different names for
 * historical reasons.
 */

void dof(
	register miColor	*result,
	register miState	*state,
	register struct soft_dof*paras);

int soft_dof_version(void) {return(1);}

miBoolean soft_dof(
	register miColor	*result,
	register miState	*state,
	register struct soft_dof*paras)
{
	dof(result, state, paras);
	return(miTRUE);
}

int lens_depth_of_field_version(void) {return(1);}

miBoolean lens_depth_of_field(
	register miColor	*result,
	register miState	*state,
	register struct soft_dof*paras)
{
	dof(result, state, paras);
	return(miTRUE);
}


/*
 * New depth of field shader using low discrepancy sampling for the
 * simulation of depth of field. It does supersampling of the lens before the
 * ordinary oversampling. A fixed number of four samples is used.
 */

#define NO_SAMPLES 4

void dof(
	register miColor	*result,
	register miState	*state,
	register struct soft_dof*paras)
{
	miVector org, dir;   /* (transf. of) original state->org, state->dir */
	miVector rayorg, raydir;   /* ray origin and direction */
	miVector dest;
	double t, radius, angle;
	miColor average;
	int i;
	double rx = state->raster_x, ry = state->raster_y;
	double time = state->time;
	double pad[2];
	miUint n;

	average = black;

	mi_point_to_camera (state, &org, &state->org);
	mi_vector_to_camera(state, &dir, &state->dir);
	t = (paras->plane - org.z) / dir.z;
	dest.x = org.x + t * dir.x;
	dest.y = org.y + t * dir.y;
	dest.z = paras->plane;

	i = 0;
	n = NO_SAMPLES;

	while(mi_sample(pad, &i, state, 2, &n)) {
		radius = paras->radius * sqrt(pad[0]);
		angle = 2.0 * M_PI * pad[1];

		rayorg.x = org.x + radius * cos(angle);
		rayorg.y = org.y + radius * sin(angle);
		rayorg.z = org.z;
		raydir.x = dest.x - rayorg.x;
		raydir.y = dest.y - rayorg.y;
		raydir.z = dest.z - rayorg.z;
		mi_point_from_camera (state, &rayorg, &rayorg);
		mi_vector_from_camera(state, &raydir, &raydir);

		(void)mi_trace_eye(result, state, &rayorg, &raydir);

		average.r += result->r;
		average.g += result->g;
		average.b += result->b;
		average.a += result->a;
	}
	result->r = average.r / NO_SAMPLES;
	result->g = average.g / NO_SAMPLES;
	result->b = average.b / NO_SAMPLES;
	result->a = average.a / NO_SAMPLES;
}
