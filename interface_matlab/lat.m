classdef lat
    
    properties
        ipLAT  = '192.168.1.102';
        port_tx   = 5500;
        port_rx   = 5501;
        timeout       = 2000;
        packetLength  = 200;
        
        % Limits the max_vel on LAT (1 = 100%) - RECOMMENDED .5
        POWER = .5;
    end
    
    methods
        
        function obj = start(obj)
            disp('LASR ATTITUDE PLATFORM (LAT)');
            disp('----');
            fprintf('LAT IP: %s \n', obj.ipLAT);
            fprintf('LAT TX Port: %d \n', obj.port_tx);
            fprintf('LAT RX Port: %d \n', obj.port_rx);
            disp('----');
        end
                
        function [gyro, accl, magn] = imu(obj)
            % Request IMU data from LAT
            mssg = '1';
            judp('SEND',obj.port_tx,obj.ipLAT,int8(mssg));
            
            % Receive requested IMU data from LAT
            vals = judp('RECEIVE',obj.port_rx,obj.packetLength,obj.timeout);
            disp('IMU received.')
            gyro     = vals(1:3);
            accl     = vals(4:6);
            magn     = vals(7:9);
            
        end
        
        function [bus5v,busBatt,csM1,csM2,csM3] = status(obj)
            % NOT IMPLEMENTED ON LAT YET
            % Request data from LAT
            mssg = '2';
            judp('SEND',obj.port_tx,obj.ipLAT,int8(mssg));
            
            % Receive requested IMU data from LAT
            vals = judp('RECEIVE',obj.port_rx,obj.packetLength,obj.timeout);
            disp('Status received.')
            bus5v    = vals(1);
            busBatt  = vals(2);
            csM1     = vals(3);
            csM2     = vals(4);
            csM3     = vals(5);
            
        end
        
        function [] = velocity(obj,vel)
            % Convert negative signs to CW and CCW rotations
            rot = zeros(1,3);
            rotStr = zeros(3,3);
            for i=1:3
                if vel(i) >= 0
                    rot(i) = 1;
                    rotStr(i,:) = 'CCW';
                else
                    rot(i) = 2;
                    rotStr(i,:) = 'CW ';
                end
            end
            
            % Get modulus of velocities
            vel = abs(vel);
            
            % Command to send velocity
            strBase = '3:';
            
            % Prepare received velocities to be sent to LAT
            strVel = strcat(num2str(vel(1)),',',num2str(vel(2)),',',num2str(vel(3)));
            strRot = strcat(num2str(rot(1)),',',num2str(rot(2)),',',num2str(rot(3)));
            mssg = strcat(strBase,strVel,',',strRot);
            
            % Display and send velocites
            for i = 1:3
                fprintf('Velocity %d: %d | %s \n',i,vel(i),rotStr(i,:));
            end
            disp('----');
            judp('SEND',obj.port_tx,obj.ipLAT,int8(mssg));
            
        end
        
        function [] = torque(obj,u)
            % X Axis
            vel1 = [1 0 -1]./sqrt(2); % divided by the norm
                        
            % Y Axis
            y_fact = 0.866025403784439;
            vel2 = [-1*y_fact 1 -1*y_fact]; % divided by the norm
            vel2 = vel2./norm(vel2);
                        
            % Z Axis
            vel3 = [1 1 1]./sqrt(3); % divided by the norm
                        
            % Control Influence Matrix
            C = [vel1; vel2; vel3];
            
            % Calculate torques
            vel = [u(1) u(2) u(3)]*C;
            
            % Convert negative signs to CW and CCW rotations
            rot = zeros(1,3);
            rotStr = zeros(3,3);
            for i=1:3
                if vel(i) >= 0
                    rot(i) = 1;
                    rotStr(i,:) = 'CCW';
                else
                    rot(i) = 2;
                    rotStr(i,:) = 'CW ';
                end
            end
            
            % Get modulus of velocities
            vel = round(4096*obj.POWER*abs(vel));
            
            % Command to send velocity
            strBase = '3:';
            
            % Prepare received velocities to be sent to LAT
            strVel = strcat(num2str(vel(1)),',',num2str(vel(2)),',',num2str(vel(3)));
            strRot = strcat(num2str(rot(1)),',',num2str(rot(2)),',',num2str(rot(3)));
            mssg = strcat(strBase,strVel,',',strRot);
            
            % Display and send velocites
            for i = 1:3
                fprintf('Velocity %d: %d | %s \n',i,vel(i),rotStr(i,:));
            end
            disp('----');
            judp('SEND',obj.port_tx,obj.ipLAT,int8(mssg));
            
        end
        
    end
    
end