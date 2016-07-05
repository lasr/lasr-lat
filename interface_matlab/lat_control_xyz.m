%% Simulation Info

clear all;
close all;
clc;

bodyName        = 'LAT';
VICON           = 0;
CONTROL         = 1;
IMU             = 0;

simTime         = 10;%30;       % seconds
sim_dt          = 0.01;
tic

%% Reading Sensor Data
import java.io.*
import java.net.DatagramSocket
import java.net.DatagramPacket
import java.net.InetAddress
import java.net.MulticastSocket

% Initiate Vicon Connection
if VICON
    multicastGroup  = '233.0.20.1';
    viconPort       = 9999;
    timeout         = 100;
    packetLength    = 200;
    
    group           = InetAddress.getByName(multicastGroup);
    socket          = MulticastSocket(viconPort);
    socket.joinGroup(group);
    socket.setSoTimeout(timeout);
    socket.setReuseAddress(1);
    packet          = DatagramPacket(zeros(1,packetLength,'int8'),packetLength);
    
    % Receives Initial Vicon (passing Vicon packet)
    socket.receive(packet);
    mssg            = packet.getData;
    mssg            = mssg(1:packet.getLength);
    inetAddress     = packet.getAddress;
    sourceHost      = char(inetAddress.getHostAddress);
    s               = char(mssg)';
    [vals, ~, ~, nextindex] = sscanf(s,'%f;%s %d,%d,%f,%f,%f,%f,%f,%f,%f;');
    %[frame,name,id,quality,x,y,z,q0,q1,q2,q3]
    frame           = vals(1);
    name            = char(vals(2:end-9))';
    id              = vals(end-8);
    quality         = vals(end-7);
    x_temp          = vals(end-6:end-4);
    q_temp          = vals(end-3:end);
    if strcmp(bodyName,name) % Check if Vicon locates desired body (multiple bodies)
        x_i = x_temp;
        q_i = q_temp;
    end
    ea_i            = EP2Euler321(q_i);
    
    vicStore    = [];
    q_k         = q_i;
    state      = [x_i;q_i;ea_i];
    
    % Read Vicon Buffer (Do right before the loop)
    % If time between successive vicon packets is > 1/200 then good data being
    % passed. If less than that then bad data being passed.
    % Set times
    tic;
    time       = toc;
    now        = time;
    lastRun    = time;
    lastAct    = time;
    
    goodPackets = 0;
    while(goodPackets<10)           % loop until 9 good packets
        socket.receive(packet);
        lastRun = now;
        now     = toc;
        if now-lastRun > 1/200
            goodPackets = goodPackets + 1;
        else
            goodPackets = 0;
        end
    end
end

% Initiate LAT connection
LAT = lat();
LAT.start();


LAT.torque([0.3 0 0]);
pause(0.5);

LAT.torque([0 0 0])
pause(1);

LAT.torque([0 0.3 0]);
pause(0.5);

LAT.torque([0 0 0])
pause(1);

LAT.torque([0 0 0.3]);
pause(0.5);



brakeLAT = [0 0 0];
for i = 1:100
    LAT.velocity(brakeLAT);
    % Simulation delay
    pause(sim_dt);
    
end
