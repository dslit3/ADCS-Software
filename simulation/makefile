CC := gcc
CFLAGS := -I. -I.. -lm -Wswitch

# Source files
SOURCES := simulation.c radiance/radiance.c \
			../adcs_math/vector.c ../adcs_math/quaternion.c ../adcs_math/matrix.c


# Target executable name
TARGET := adcssim

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o $@

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)

plot:
	gnuplot simplots.gp


.PHONY: all clean
