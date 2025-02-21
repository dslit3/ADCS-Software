set xlabel 'Sun Angle (90 degrees = overhead)'
set ylabel 'Radiative Intensity on Spacecraft Surface (W/m^2)'
plot 'data/radiance_angles.txt' using 1:2 with lines title "Surface Facing Toward Earth", 'data/radiance_angles.txt' using 1:3 with lines title "Surface Facing Away From Earth"