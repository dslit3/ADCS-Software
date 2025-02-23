#include <stdio.h>
#include <math.h>

#include "radiance/radiance.h"

void test_radiance_angles() {
    printf("radiance_angles.....");

    FILE *datafile = fopen("data/radiance_angles.txt", "w");

    vec3 earth_pos = {.x=0.0,.y=0.0,.z=6378137.0 + 415000.0}; // 415 km above Earth surface
    vec3 moon_pos = {.x=-221933443.5,.y=221933443.5,.z=221933443.5}; // Uneclipsed, at an appropriate distance away

    struct radsim_planet planets[2]; 
    radsim_create_earth(earth_pos, 50, &planets[0]);
    radsim_create_moon(moon_pos, 25, &planets[1]);

    struct radsim_state state;
    state.dir_to_sun.y = 0.0;
    state.in_eclipse = false;
    state.planets = planets;
    state.planets_count = 2;

    vec3 surf_earth_facing = {.x=0.0,.y=0.0,.z=1.0};
    vec3 surf_space_facing = {.x=0.0,.y=0.0,.z=-1.0};
    
    for (int i = 0; i <= 180; i++) {
        double angle = (i / 180.0) * 3.14159265358;
        state.dir_to_sun.x = cos(angle);
        state.dir_to_sun.z = -sin(angle);

        double Fearthfacing = 0.0;
        double Fspacefacing = 0.0;

        radsim_receive_emission(&state, surf_earth_facing, &Fearthfacing);
        radsim_receive_emission(&state, surf_space_facing, &Fspacefacing);

        fprintf(datafile, "%d %f %f\n", i, Fearthfacing, Fspacefacing);
    }

    fclose(datafile);

    printf("OK\n");
}

void test_radiance_distance() {
    printf("radiance_distance.....");

    FILE *datafile = fopen("data/radiance_distance.txt", "w");

    vec3 earth_pos = {.x=0.0,.y=0.0,.z=6378137.0}; // Earth surface touching spacecraft
    vec3 moon_pos = {.x=-221933443.5,.y=221933443.5,.z=221933443.5}; // Uneclipsed, at an appropriate distance away

    struct radsim_planet planets[2]; 
    radsim_create_earth(earth_pos, 50, &planets[0]);
    radsim_create_moon(moon_pos, 25, &planets[1]);

    struct radsim_state state;
    state.dir_to_sun.x = 0.0;
    state.dir_to_sun.y = 0.0;
    state.dir_to_sun.z = -1.0;
    state.in_eclipse = false;
    state.planets = planets;
    state.planets_count = 2;

    vec3 surf = {.x=0.0,.y=0.0,.z=1.0};
    
    for (int i = 0; i <= 350; i++) {
        double dist = 200000.0 + i * 50.0;
        double Fin = 0.0;

        state.planets[0].pos_m.z = 6378137.0 + dist;
        radsim_receive_emission(&state, surf, &Fin);

        fprintf(datafile, "%f %f\n", dist, Fin);
    }

    fclose(datafile);

    printf("OK\n");
}

void test_radiance_moonglow() {
    printf("radiance_moonglow.....");

    FILE *datafile = fopen("data/radiance_moonglow.txt", "w");

    vec3 earth_pos = {.x=0.0,.y=0.0,.z=6378137.0}; // Earth surface touching spacecraft
    vec3 moon_pos = {.y=0.0}; // Uneclipsed, at an appropriate distance away

    struct radsim_planet planets[2]; 
    radsim_create_earth(earth_pos, 50, &planets[0]);
    radsim_create_moon(moon_pos, 25, &planets[1]);

    struct radsim_state state;
    state.dir_to_sun.x = 0.0;
    state.dir_to_sun.y = 0.0;
    state.dir_to_sun.z = 1.0;
    state.in_eclipse = true;
    state.planets = planets;
    state.planets_count = 2;

    vec3 surf = {.x=0.0,.y=0.0,.z=-1.0};
    
    for (int i = 0; i <= 180; i++) {
        double angle = (i / 180.0) * 3.14159265358;
        double Fin = 0.0;

        state.planets[1].pos_m.x = 384400000.0 * cos(angle);
        state.planets[1].pos_m.z = 384400000.0 * -sin(angle);
        radsim_receive_emission(&state, surf, &Fin);

        fprintf(datafile, "%d %f\n", i, Fin);
    }

    fclose(datafile);

    printf("OK\n");
}

int main() {
    printf("Running ADCS Simulations/Calculators.\n");

    test_radiance_angles();
    test_radiance_distance();
    test_radiance_moonglow();
    
    printf("Done!\n");
    return 0;
}