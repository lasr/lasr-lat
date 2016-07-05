%% Simulation Info

clear all;
close all;
clc;

% Config vars
bodyName        = 'LAT';
VICON           = 0;
CONTROL         = 1;
IMU             = 0;
PLOT            = 1;

simTime         = 10;    % seconds
sim_dt          = 0.01;
tic % start counting time for vicon, imu, etc

%% Reading Sensor Data
% Initiate Vicon
if VICON
    
    vicon = liveVicon();
    [viconSocket,viconPacket,q_k] = vicon.start(bodyName);
    
    vicStore = []; % will store vicon data
    
end

% Initiate LAT connection
LAT = lat();
LAT.start();

IMUStore = []; % will store IMU data

% Check times
dt = toc;

while(dt<simTime)
    % Simulations dts
    dt = toc;
    
    if VICON
        % Read Vicon
        [x_k,q_k] = vicon.read(bodyName,viconSocket,viconPacket,q_k);
        timeVIC = toc;
        
        VICdata     = [timeVIC q_k' x_k'];
        vicStore    = [vicStore; VICdata];
        
    end
    
    if IMU
        % Request IMU data
        [gyro, accl, magn] = LAT.imu();
        timeIMU  = toc;
        
        IMUdata  = [timeIMU gyro' accl' magn'];
        IMUStore = [IMUStore; IMUdata];
        
    end
    
    if CONTROL
        % Control stuff
        % min_vel.: 0 | max_vel.: 4096
        [vel] = testControl2(dt, simTime);
                
        LAT.velocity(vel);
        % LAT.torque(u);
        
    end
    
    % Simulation delay
    pause(sim_dt);
    
end

brakeLAT = [0 0 0];
for i = 1:100
    LAT.velocity(brakeLAT);
    % Simulation delay
    pause(sim_dt);
    
end

if PLOT
    % PLOT QUATERNIONS
    color = ['r','b','g','k'];
    for i=2:5
        plot(vicStore(:,1),vicStore(:,i),color(i-1));
        hold on
    end
    
    title(sprintf('%s Quaternions', bodyName))
    legend('q0','q1','q2','q3')
    axis([0 simTime -1 1])
    grid on
    
end