TARGET = main

CC     = gcc
CFLAGS = -g -lm -Wall -O2 -std=c99 -lwiringPi
all: $(TARGET)

$(TARGET): $(TARGET).c networking.c adc.c motor.c pca9685.c

networking.c: networking.h
pca9685.c: pca9685.h

clean:
	$(RM) $(TARGET)
