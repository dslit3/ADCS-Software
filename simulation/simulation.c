#include <stdio.h>
#include <math.h>

#include "radiance/radiance.h"

void test_radiance_angles() {
    printf("radiance_angles.....");

    FILE *datafile = fopen("data/radiance_angles.txt", "w");

    vec3 earth_pos = {.x=0.0,.y=0.0,.z=8378138.00}; // ~2000 km above Earth surface
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

        double Fearthfacing;
        double Fspacefacing;

        radsim_receive_emission(&state, surf_earth_facing, &Fearthfacing);
        radsim_receive_emission(&state, surf_space_facing, &Fspacefacing);

        fprintf(datafile, "%d %f %f\n", i, Fearthfacing, Fspacefacing);
    }

    fclose(datafile);

    printf("OK\n");
}

int main() {
    printf("Running ADCS Simulations/Calculators.\n");

    test_radiance_angles();
    
    printf("Done!\n");
    return 0;
}