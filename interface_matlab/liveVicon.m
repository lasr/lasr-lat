classdef liveVicon
    
    %{ 
    LIVE VICON CLASS
    INPUT: bodyName
    OUTPUT: x_k, q_k (position and attitude)
    
    Usage:
    1) Initialize VICON and create object: (INPUT: bodyName)
    
        bodyName = 'LAT';  % Change according to the body being tracked    
        vicon = liveVicon();
        [viconSocket,viconPacket,q_k] = vicon.start(bodyName);
    
    2) Read data: (OUTPUT: x_k = position (x,y,z)
                           q_k = attitude (quaternions, scalar first)
    
        [x_k,q_k] = vicon.read(bodyName,viconSocket,viconPacket,q_k);
    
    Points of attention:
    1) Make sure your computer is connected to the WIRED network.
    2) Make sure Vicon, the Nexus and ViconBroadcaster softwares are ON on 
       the VICON computer.
    3) Make sure that ONLY the body you want to be tracked is selected on 
       Vicon software.
    
    %}
    
    properties
        
        multicastGroup  = '233.0.20.1';
        viconPort       = 9999;
        timeout         = 100;
        packetLength    = 200;
        
    end
    
    methods
        
        function [socket,packet,q_i] = start(obj,bodyName)
            
            import java.io.*
            import java.net.DatagramSocket
            import java.net.DatagramPacket
            import java.net.InetAddress
            import java.net.MulticastSocket
            
            disp('LASR Live VICON Initialized.');
            disp('----');
            disp('Created Java bindings and enabled MulticastSocket.');
            fprintf('Tracking body: %s \n', bodyName);
            fprintf('Vicon Multicast IP: %s \n', obj.multicastGroup);
            fprintf('Vicon Port: %d \n', obj.viconPort);
            disp('----');
            
            disp('Establishing initial configurations ...');
            group           = InetAddress.getByName(obj.multicastGroup);
            socket          = MulticastSocket(obj.viconPort);
            socket.joinGroup(group);
            socket.setSoTimeout(obj.timeout);
            socket.setReuseAddress(1);
            packet          = DatagramPacket(zeros(1,obj.packetLength,'int8'),obj.packetLength);
            
            disp('Receiving initial VICON packets ...');
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
            
            disp('Localizing desired Vicon body ...');
            if strcmp(bodyName,name) % Check if Vicon locates desired body (multiple bodies)
                x_i = x_temp;
                q_i = q_temp;
            else
                error('ERROR: Please remove additional bodies from Vicon-space ...');
            end
            
            disp('Checking if we are receiving correct data from VICON ...');
            % Read Vicon Buffer (Do right before the loop)
            % If time between successive vicon packets is > 1/200 then good data being
            % passed. If less than that then bad data being passed.
            % Set times
            tic;
            time       = toc;
            now        = time;
            
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
            
            disp('Vicon sending correct data and ready to use.');
            
        end
        
        function [x_k,q_k] = read(obj,bodyName,socket,packet,q_i)
            
            % From initial Vicon
            q_k = q_i;
            
            % Read Vicon
            socket.receive(packet);
            mssg        = packet.getData;
            mssg        = mssg(1:packet.getLength);
            inetAddress = packet.getAddress;
            sourceHost  = char(inetAddress.getHostAddress);
            s           = char(mssg)';
            [vals, ~, ~, nextindex] = sscanf(s,'%f;%s %d,%d,%f,%f,%f,%f,%f,%f,%f;');
            %[frame,name,id,quality,x,y,z,q0,q1,q2,q3]
            frame       = vals(1);
            name        = char(vals(2:end-9))';
            id          = vals(end-8);
            quality     = vals(end-7);
            x_temp      = vals(end-6:end-4);
            q_temp      = vals(end-3:end);
            if strcmp(bodyName,name)
                x_k     = x_temp;
                q_k     = 0.8*q_k+0.2*q_temp;
                q_k         = q_k./norm(q_k);
                
                % TO DO - Add new class function: Quaternion to EA
%                 ea_k    = EP2Euler321(q_k);
%                 state   = [x_k;q_k;ea_k];
                %state = [x_k;q_k];
            end
                        
        end
        
    end
    
end