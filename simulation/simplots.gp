set term qt 0
set xlabel 'Sun Angle (90 degrees = overhead)'
set ylabel 'Radiative Intensity on Spacecraft Surface (W/m^2)'
plot 'data/radiance_angles.txt' using 1:2 with lines title "Surface Facing Toward Earth", 'data/radiance_angles.txt' using 1:3 with lines title "Surface Facing Away From Earth"

set term qt 1
set xlabel 'Altitude (km)'
set ylabel 'Radiative Intensity on Spacecraft Surface (W/m^2)'
plot 'data/radiance_distance.txt' using 1:2 with lines title "Surface Facing Toward Earth"

set term qt 2
set xlabel 'Moon Angle (90 degrees = overhead)'
set ylabel 'Radiative Intensity on Spacecraft Surface (W/m^2)'
plot 'data/radiance_moonglow.txt' using 1:2 with lines title "Surface Facing Away From Earth"