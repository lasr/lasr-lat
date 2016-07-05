/*
LASR Attitude Platform
Vinicius Goecks
March 2016

Main control file to control the attitude platform.

*/

#include "pca9685.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "networking.h"

// Test Parameters
#define COMMAND_ON
#define MAX_LOOPS 10000

// ADC Parameters
#define CHAN_CONFIG_SINGLE  8
#define SPI_CHANNEL         0

// PWM Parameters
#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 1000

// Motor Parameters
#define CW   2
#define CCW  1

// Communication Sockets
udp_sock_struct_t lat_rx, lat_tx;

float * readADC(double ADC_5V, double ADC_BAT, double ADC_MOTOR)
{
    static float telemetry[5];

    // 5v Bus voltage. Battery voltage, M1, M2, M3 currents
    telemetry[0] = ADC_5V * myAnalogRead(SPI_CHANNEL,CHAN_CONFIG_SINGLE,0);
    telemetry[1] = ADC_BAT * myAnalogRead(SPI_CHANNEL,CHAN_CONFIG_SINGLE,1);
    telemetry[2] = ADC_MOTOR * myAnalogRead(SPI_CHANNEL,CHAN_CONFIG_SINGLE,2);
    telemetry[3] = ADC_MOTOR * myAnalogRead(SPI_CHANNEL,CHAN_CONFIG_SINGLE,3);
    telemetry[4] = ADC_MOTOR * myAnalogRead(SPI_CHANNEL,CHAN_CONFIG_SINGLE,4);

    printf("5V Bus voltage = %f V\n", telemetry[0]);
    printf("Battery Bus voltage = %f V\n", telemetry[1]);
    printf("M1 current = %f A\n",telemetry[2]);
    printf("M2 current = %f A\n", telemetry[3]);
    printf("M3 current = %f A\n", telemetry[4]);

    return telemetry;
}

int main()
{
    printf("*** LASR Attitude Platform (LAT) ***\n");

    // Initialize motors and its pins on Raspi
    int vels_rots[6];
    int vel[3];
    int rot[3];
    int lastRot[3] = {1,1,1}; // Will store the last rotation sent to improve braking

    wiringPiSetupGpio();
    int inApin[3] = {21, 16, 12};  // INA: Clockwise input
    int inBpin[3] = {26, 13, 05}; // INB: Counter-clockwise input
    int enpin[3] = {20, 19, 06}; // EN: Status of switches output (Analog pin)

    motorSetup(inApin, inBpin, enpin);
    printf("Motors initialized.\n");

    // Initialize ADC
    double ADC_5V = ((990+215)/215)*3.31/1024; // Vref = 3.3 V, 1024 counts
    double ADC_BAT = ((9900+3250)/3250)*3.31/1024;
    double ADC_MOTOR = 3.31*11370/(1450*1024); // 11370 (constant from CS, datasheet)
    spiSetup(SPI_CHANNEL);

    printf("ADC initialized.\n");
    float *adc;
    adc = readADC(ADC_5V, ADC_BAT, ADC_MOTOR);

    // Initialize IMU
    //
    //
    //

    // Initialize PWM
    int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
    if (fd < 0)	{
        printf("Error in setup.\n");
        return fd;
    }
    pca9685PWMReset(fd); // Reset all PWM outputs
    printf("PWM initialized.\n");

    // Initialize Communication
	char rx_buff[99], tx_buff[999], *tok;
	int rxport = 5500, txport = 5501, rx_comm, tx_sz, ret;
	char rxip[] = "192.168.1.102", txip[] = "192.168.1.60";

    printf("UDP Communication initialized.\n");
	printf("RX Addr: \t%s:%d\n",rxip,rxport);
	printf("TX Addr: \t%s:%d\n",txip,txport);

    // Create Send/Receive Sockets
	setupReceiveSocket(&lat_rx,rxip,rxport);
	setupTransmitSocket(&lat_tx,txip,txport);
    printf("Communication sockets created.\n");

    // Main Loop
    printf("Started.\n");

    for (int k = 0; k < MAX_LOOPS; k++) {
        // RX
        while(1){
            ret = udp_recv(lat_rx, rx_buff, sizeof(rx_buff));
            if(ret==SOCKET_ERROR) {
                printf("Relay: Socket error on RX recvfrom.\n");
            }

            rx_buff[ret]=0;
            tok = strtok(rx_buff, ";");
            if(tok!=NULL) {
                sscanf(tok,"%d",&rx_comm);
                tok = strtok(NULL,";");
            }

            printf("Command received: %d.\n",rx_comm);

            if(rx_comm == 1 || 2 || 3){
                break;
            }
        }

        // Processing RX
        printf("Processing RX command ...\n");
        //  If first string == 1: requesting IMU readings
        //                  == 2: requesting telemetry
        //                  == 3: sending velocity commands
        if (rx_comm <= 2){
            if(rx_comm == 1){
                // Read IMU
                float imu = {.245,.035,.0525,.15691,.90221,-9.5321,-22.273,4.3636,49.082}; // sample test
                tx_sz = sprintf(tx_buff,"%f,%f,%f,%f,%f,%f,%f,%f,%f;",imu);
            }
            if(rx_comm == 2){
                // Read ADC
                adc = readADC(ADC_5V, ADC_BAT, ADC_MOTOR);
                tx_sz = sprintf(tx_buff,"%f,%f,%f,%f,%f;",adc);
            }
            // TX Response
            ret = udp_send(lat_tx, tx_buff, tx_sz);
            if(ret == SOCKET_ERROR){
                printf("Error sending requested data.\n");
            }
            else {
                printf("IMU/Telemetry sent.\n");
            }
        }

        if (rx_comm == 3){
            // Receive the velocity commands
            // Basic structure:
            // 3;vel[0],vel[1],vel[2],rot[0],rot[1],rot[2]
            // velocity: min. 0, max. 4096 | rotation: 1 - CCW, 2 - CW

            // FUNCTION NOT WORKING: vels_rots = getSpeeds(rx_buff);
            // Trying it directly:

            char tempBuff[strlen(rx_buff)];
            const char s[2] = ",";
            char *token;

            // Remove '3;' mark from velocity command
            for(int i = 2; i < strlen(rx_buff); i++){
                tempBuff[i-2] = rx_buff[i];
            }

            // get 3 vels and 3 rotations
            token = strtok(tempBuff, s);
            vels_rots[0] = atoi(token);

            for(int i = 1; i < 6; i++){
                token = strtok(NULL, s);
                vels_rots[i] = atoi(token);
            }

            for (int i = 0; i <= 2; i++){
                vel[i] = vels_rots[i];
                rot[i] = vels_rots[i+3];
            }

            // Print received velocities and rotations
            printf("Received velocities and rotations: (CCW = 1, CW = 2)\n");
            for (int i = 0; i <= 2; i++) {
                printf("Velocity %i: %i.\n", i+1, vel[i]);
                printf("Rotation %i: %i.\n", i+1, rot[i]);

                #ifdef COMMAND_ON
                    // Command velocites
                    pwmWrite(PIN_BASE + i, vel[i]);
                    motorGo(i+1, rot[i], vel[i], lastRot[i]);
                    lastRot[i] = rot[i];
                #endif
            }
        }
    }

    // // Brake - sending all motors 1% of max_vel for a couple loops
    // printf("Braking...\n");
    // for (int i = 0; i < 1000; i++) {
    //     pwmWrite(PIN_BASE + 16, 0.01*MAX_PWM);
    //     for (int j = 1 ; j <= 3; j++) {
    //         motorGo(j, CW, lastRot[j-1]);
    //     }
    // }

    // Shutdown
    printf("Shutting down...\n");
    for (int i = 1 ; i <= 3; i++) {
        motorOff(i, inApin, inBpin, enpin);
    }
    pca9685PWMReset(fd);

    return 0;
}
