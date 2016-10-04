# UDP control
import socket 
import numpy as np
import math

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

class lat:
   
    
    def __init__(self):
        global sock
        self.ipLAT  = '192.168.1.102';
        self.port_tx   = 5500;
        self.port_rx   = 5501;
        self.timeout       = 2000;
        self.packetLength  = 200;
        
        # Limits the max_vel on LAT (1 = 100%)
        self.POWER = 1;
        
         
        sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
        sock.bind((self.ipLAT, self.port_rx))
    
        
    def start(self):
        print('LASR ATTITUDE PLATFORM (LAT)');
        print('----');
        print('LAT IP: ',self.ipLAT,' \n');
        print('LAT TX Port: ',self.port_tx,' \n');
        print('LAT RX Port: ',self.port_rx,' \n');
        print('----');
        
                
    def imu(self):
        # Request IMU data from LAT
        mssg = np.int8(1);
        sock.sendto(mssg, (self.ipLAT, self.port_rx))

        # Receive requested IMU data from LAT
        vals, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
        print('IMU received.')
        gyro     = vals[1:3];
        accl     = vals[4:6]
        magn     = vals[7:9];
        return gyro, accl, magn
            
        
        
    def status(self):
        # NOT IMPLEMENTED ON LAT YET
        # Request data from LAT
        mssg = np.int8(2);
        sock.sendto(mssg, (self.ipLAT, self.port_rx))
        
        # Receive requested IMU data from LAT
        vals, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
        print('Status received.')
        bus5v    = vals(1);
        busBatt  = vals(2);
        csM1     = vals(3);
        csM2     = vals(4);
        csM3     = vals(5);
        return bus5v,busBatt,csM1,csM2,csM3
        
    
        
    def velocity(self,vel):
        # Convert negative signs to CW and CCW rotations
        rot = np.zeros(1,3);
        rotStr = np.zeros(3,3);
        for i in range(1,3):
            if vel[i] >= 0:
                rot[i] = 1;
                rotStr[i,:] = 'CCW';
            else:
                rot[i] = 2;
                rotStr[i,:] = 'CW ';
            
        
        
        # Get modulus of velocities
        vel = abs(vel);
        
        # Command to send velocity
        strBase = '3:';
        
        # Prepare received velocities to be sent to LAT
        strVel = (str(vel(1)),',',str(vel(2)),',',str(vel(3)));
        strRot = (str(rot(1)),',',str(rot(2)),',',str(rot(3)));
        mssg = np.int8(strBase,strVel,',',strRot);
        
        # Display and send velocites
        for i in range(1,3):
            print('Velocity ',i,': ',vel(i),' | ',rotStr[i,:],' \n');
        
        print('----');
        # 3:vel1,vel2,vel3,rot1,rot2,rot3
        sock.sendto(mssg, (self.ipLAT, self.port_rx))        
    
        
    def torque(self,u):
        # X Axis
        vel1 = np.array[1, 0, -1]/math.sqrt(2); # divided by the norm
                    
        # Y Axis
        y_fact = 0.866025403784439;
        vel2 = np.array[-1*y_fact, 1, -1*y_fact]; # divided by the norm
        vel2 = vel2/np.linalg.norm(vel2);
                    
        # Z Axis
        vel3 = np.array[1, 1, 1]/math.sqrt(3); # divided by the norm
                    
        # Control Influence Matrix
        C = np.transpose(np.array[vel1, vel2, vel3]);
        
        # Calculate torques
        vel = np.array[u(1), u(2), u(3)]*C;
        
        # Convert negative signs to CW and CCW rotations
        rot = np.zeros(1,3);
        rotStr = np.zeros(3,3);
        for i in range(1,3):
            if vel(i) >= 0:
                rot[i] = 1;
                rotStr[i,:] = 'CCW';
            else:
                rot[i] = 2;
                rotStr[i,:] = 'CW ';
            
        
        
        # Get modulus of velocities
        vel = round(4096*self.POWER*abs(vel));
        
        # Command to send velocity
        strBase = '3:';
        
        # Prepare received velocities to be sent to LAT
        strVel = (str(vel(1)),',',str(vel(2)),',',str(vel(3)));
        strRot = (str(rot(1)),',',str(rot(2)),',',str(rot(3)));
        mssg = np.int8(strBase,strVel,',',strRot);
        
        # Display and send velocites
        for i in range(1,3):
            print('Velocity ',i,': ',vel[i],' | ',rotStr[i,:],' \n');
        print('----');
        sock.sendto(mssg, (self.ipLAT, self.port_rx))        
