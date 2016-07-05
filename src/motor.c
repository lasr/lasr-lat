/*
VNH2SP30 LIBRARY TO WORK ON RASPI WITH WIRINGPI
Vinicius Goecks
March 2016
*/

#include <unistd.h>
#include <wiringPi.h>

/* motorGo() will set a motor going in a specific direction
the motor will continue going in that direction, at that speed
until told to do otherwise.

motor: this should be 1, 2 or 3. Selet which of the motors to be controlled

direct: Should be between 1 or 2, with the following result
1: Clockwise
2: CounterClockwise */

void motorSetup(int inApin[3], int inBpin[3], int enpin[3])
{
    // Initialize digital pins as outputs
    for (int i=0; i<3; i++)
    {
        pinMode(inApin[i], OUTPUT);
        pinMode(inBpin[i], OUTPUT);
        pinMode(enpin[i], OUTPUT);
    }
    // Initialize braked
    for (int i=0; i<3; i++)
    {
        digitalWrite(inApin[i], LOW);
        digitalWrite(inBpin[i], LOW);
        digitalWrite(enpin[i], HIGH);
    }

}

void motorGo(int motor, int direct, int vel, int lastRot)
{
    int inApin;
    int inBpin;
    if ((motor < 4) && (motor > 0))
    {
        // Made this way to be easier for the user send commands
        // only using the motor numbers and direction
        if (motor==1)
        {
            inApin = 21;
            inBpin = 26;
        }

        if (motor==2)
        {
            inApin = 16;
            inBpin = 13;
        }

        if (motor==3)
        {
            inApin = 12;
            inBpin = 05;
        }
    }

    if (direct <=2)
    {
        if (vel =! 0){
            // Set inA[motor]
            if (direct ==1)
                digitalWrite(inApin, HIGH);
            else
                digitalWrite(inApin, LOW);

            // Set inB[motor]
            if (direct==2)
                digitalWrite(inBpin, HIGH);
            else
                digitalWrite(inBpin, LOW);
        }
        else{
            // Input a reverse rotation to improve brake
            // Set inA[motor]
            if (direct ==2)
            digitalWrite(inApin, HIGH);
            else
            digitalWrite(inApin, LOW);

            // Set inB[motor]
            if (direct==1)
            digitalWrite(inBpin, HIGH);
            else
            digitalWrite(inBpin, LOW);
        }

    }
}

void motorOff(int motor, int inApin[3], int inBpin[3], int enpin[3])
{
    for (int i=0; i<3; i++)
    {
        digitalWrite(inApin[i], LOW);
        digitalWrite(inBpin[i], LOW);
        digitalWrite(enpin[i], LOW);
    }
}

// int * getSpeeds(char rx_buff[99])
// {
//     char tempBuff[strlen(rx_buff)];
//     const char s[2] = ",";
//     char *token;
//     static int splits[6];
//
//     // Remove '3;' mark from velocity command
//     for(int i = 2; i < strlen(rx_buff); i++){
//         tempBuff[i-2] = rx_buff[i];
//     }
//
//     // get 3 vels and 3 rotations
//     token = strtok(tempBuff, s);
//     splits[0] = atoi(token);
//
//     for(int i = 1; i < 6; i++){
//         token = strtok(NULL, s);
//         splits[i] = atoi(token);
//     }
//
//     return splits;
// }
