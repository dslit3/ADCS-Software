#include "radiance.h"

#include "adcs_math/quaternion.h"
#include <math.h>
#define radsim_PI 3.1415926535897932

void radsim_create_earth(vec3 pos, int32_t resolution, struct radsim_planet *earth) {
    earth->pos_m = pos;
    earth->radius_m = 6378137.0;
    earth->k_albedo = 0.3;
    earth->resolution = resolution;
}

void radsim_create_moon(vec3 pos, int32_t resolution, struct radsim_planet *moon) {
    moon->pos_m = pos;
    moon->radius_m = 1737400.0;
    moon->k_albedo = 0.13;
    moon->resolution = resolution;
}

void radsim_receive_sun_emission(vec3 shat, double Fsun, vec3 nhat, double *Fin) {
    *Fin = Fsun * vec_dot(shat, nhat); // [1], pp. 2
    if (*Fin < 0) *Fin = 0;
}

/*
    Computes an approximation of the integral for diffusely reflecting the sun's radiation 
    off of the surface of the earth and then receiving it on the spacecraft by doing the 
    following:

    Take the side of the planet facing the spacecraft, and slice it into rings (onion).
    Then, slice each ring into radial segments (pizza/monkey bread).
    Each segment will then act as an omnidirectional point emitter of radiation with a radiative
    power (in Watts) proportional to its area.

    TH (theta): The angle up to a given ring.
    PH (phi): The angle from the top of each ring, to a given segment.

    ring_area: The area of ring i.
    dAe: The area of segment j of ring i. This area emits the radiation received by the sun.
*/
void radsim_receive_planet_diffuse_emission(vec3 shat, double Fsun, struct radsim_planet *planet, vec3 nhat, double *Fin) {
    const int concens = planet->resolution; // Concentric segments
    const int init_radials = 5;
    *Fin = 0;

    vec3 vref = {.x=0,.y=1,.z=0};
    vec3 D = planet->pos_m; vec_scalar(-1.0, D, &D);
    vec3 Dhat; vec_norm(D, &Dhat); // axis of rotation for PH (phi)
    vec3 Lhat; // Perpendicular to D/Dhat, axis of rotation for TH (theta)

    vec_cross(D, vref, &Lhat);
    if (vec_dot(Lhat, Lhat) < 0.0000001) {
        vref.x = 1;
        vref.y = 0;

        vec_cross(Dhat, vref, &Lhat);
    }
    vec_norm(Lhat, &Lhat);

    double distance = vec_mag(D);
    double THmax = acos(planet->radius_m / distance); // Max half-angle visible of the planet
    double dTH = THmax / concens;
    for (int i = 0; i < concens; i++) {
        double THi = i * dTH; // Angle theta to this ring

        double DEx = (cos(THi) - cos(THi + dTH)) * planet->radius_m;
        // Surface area of a segment of a sphere revolved around its radius' axis.
        // Solving the above gives 2*pi*r*deltaX, where deltaX is the width of the segment
        double ring_area = 2 * radsim_PI * planet->radius_m * DEx;

        // Rotation from Dhat to the angle of this ring
        // for finding the normal vectors of segments
        quat qTH;
        quat_from(i == 0 ? THi : THi + 0.5 * dTH, Lhat, &qTH);

        int radials = i == 0 ? 1 : init_radials + i; // Radial segments
        double dPHi = (2 * radsim_PI) / radials;
        for (int j = 0; j < radials; j++) {
            double PHij = j * dPHi; // Angle phi to this segment

            // Segment area: [1], pp. 2
            double dAe = ring_area / radials; // m^2
            
            // Rotation to the angle of this segment
            // for finding the segment's normal vector
            quat qPH;
            quat_from(PHij + 0.5 * dPHi, Dhat, &qPH);

            vec3 nehat; // [1], pp. 2
            quat_rotate_vec(Dhat, qTH, &nehat);
            quat_rotate_vec(nehat, qPH, &nehat);

            double Fout; // W/m2
            radsim_receive_sun_emission(shat, Fsun, nehat, &Fout);

            if (Fout <= 0.0) continue; // Area is not lit

            // Total energy in sunlit area dAe, taking albedo into account: [1], pp. 3
            double Eout = planet->k_albedo * Fout * dAe; // W/m^2 * m^2 = W

            vec3 emit_pos_planetspace; vec_scalar(planet->radius_m, nehat, &emit_pos_planetspace);
            vec3 emit_pos; vec_add(planet->pos_m, emit_pos_planetspace, &emit_pos);
            double emit_dist = vec_mag(emit_pos);

            vec3 dir_to_emitter; vec_norm(emit_pos, &dir_to_emitter);
            double receive_cosine = vec_dot(dir_to_emitter, nhat);

            if (receive_cosine < 0) receive_cosine = 0;

            // Inverse square law, cosine law, energy radiated over hemisphere: [1], pp. 3
            *Fin += Eout * receive_cosine / (radsim_PI * emit_dist * emit_dist);
        }
    }
}

void radsim_receive_emission(struct radsim_state *state, vec3 nhat, double *Fin) {
    double dFin;
    *Fin = 0;

    if (!state->in_eclipse) {
        radsim_receive_sun_emission(state->dir_to_sun, 1370, nhat, &dFin);
        *Fin += dFin;
    }

    for (int32_t i = 0; i < state->planets_count; i++) {
        radsim_receive_planet_diffuse_emission(state->dir_to_sun, 1370, &state->planets[i], nhat, &dFin);
        *Fin += dFin;
    }
}
