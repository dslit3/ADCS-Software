#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "adcs_math/matrix.h"
#include "adcs_math/vector.h"

/*
    Simulation of nearby celestial objects' radiative energy received
    by a surface on an orbiting spacecraft (photoresistive sensor).

    All positions are in local body frame. (Spacecraft is at the origin)

    [1]: https://ntrs.nasa.gov/api/citations/19940020024/downloads/19940020024.pdf
*/

/*
    UNIT GUIDE:
    m - meters
    Wm2 - watts * meters^-2 (watts per square meter)
    <no unit> - unitless
*/

struct radsim_planet {
    vec3 pos_m;
    double radius_m;
    double k_albedo;
    int32_t resolution;
};

struct radsim_state {
    vec3 dir_to_sun;
    bool in_eclipse;
    struct radsim_planet *planets;
    int64_t planets_count;
};

void radsim_create_earth(vec3 pos_m, int32_t resolution, struct radsim_planet *earth);
void radsim_create_moon(vec3 pos_m, int32_t resolution, struct radsim_planet *moon);

void radsim_receive_sun_emission(
    vec3 sun_dir, double sun_intensity_Wm2, 
    vec3 receiving_surface_normal, double *received_intensity_Wm2
);

void radsim_receive_planet_diffuse_emission(
    vec3 sun_dir, double sun_intensity_Wm2, struct radsim_planet *planet,
    vec3 receiving_surface_normal, double *received_intensity_Wm2
);

void radsim_receive_emission(struct radsim_state *state, vec3 surface_normal, double *received_intensity_Wm2);