function vel = testControl2(dt, simTime)

POWER = .15; % from 0 to 1

if (dt > simTime)
    dt = simTime;
end

vel = zeros(1,3);

% Motor 1
w = (dt/simTime)*2*pi;
vel(1) = sin(w);

% Motor 2
vel(2) = sin(2*w);

% Motor 3
if (dt/simTime <= .5)
    vel(3) = 2*dt/simTime;
else
    vel(3) = 2*(1-dt/simTime);
end

% Regulate max_vel
vel = round(4096*POWER*vel);

