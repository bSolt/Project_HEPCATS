% HEPCATS Geolocation Imaging Software
%
% This code will compute the Earth coordinates of each pixel. The output 
% will be a pseudo picture where each matrix cell contains geolocation data
% that corresponds to an individual pixel from the image.
%
% Author(s): Alex Baughman
% Date created: 2/17/19
% Last modified: 3/13/19

close all;clear all;clc

%% Create 2D shield

% need to identify X and Y coordinates of pixels containing the Earth
% make pixels with the Earth in them 1, space will be set to 0

% read in image
I = imread('test_image.jpg');
A = imgaussfilt(I, 15); % Gaussian filter helps detection by smoothing edges
[ymax,xmax,zmax] = size(I);
minRadius = 500; % 400
maxRadius = 600; % 700

% compute radii (in pixels) and Earth center in frame
[centers,radii] = imfindcircles(A,[minRadius maxRadius]);

% % validate radii and center location
% figure()
% imshow(I)
% viscircles(centers, radii);

% create binary 2D shield
shield2D = zeros(ymax,xmax,zmax);
for i = 1:ymax
    for j = 1:xmax
        dx = abs(j-round(centers(1)));
        dy = abs(i-round(centers(2)));
        distance_shield = sqrt(dx^2+dy^2); % distance from center to current pixel
        if distance_shield > radii
            shield2D(i,j,:) = [i,j,0]; % pixels outside Earth radii
        else
            shield2D(i,j,:) = [i,j,1]; % Earth pixels
        end
    end
end

% % display 2D shield
% figure()
% imshow(shield2D)

%% Make 3D model

% using information about s/c location and orientation in orbit, generate a
% pseudo 3D model 

% read in data about orbit ephemeral and define parameters
orbit_loc = xlsread('EXorbit_location.xlsx');
orbit_point = xlsread('EXorbit_pointing.xlsx');

a = orbit_loc(1); % semi-major axis [km]
e = orbit_loc(2); % eccentricity
i = orbit_loc(3); % inclination [deg]
AOP = orbit_loc(4); % argument of periapsis [deg]
RAAN = orbit_loc(5); % right ascension of the ascending node [deg]
%theta = orbit_loc(6)*(180/pi); % mean anomaly [deg]
time = orbit_loc(7); % epoch time

theta = 180; % mean anomaly [deg]

% ??? orientation of s/c ???
roll = orbit_point(1); % [deg]
pitch = orbit_point(2); % [deg]
yaw = orbit_point(3); % [deg]

% define transformation matrix
C11 = cosd(RAAN)*cosd(theta) - sind(RAAN)*cosd(i)*sind(theta);
C12 = -cosd(RAAN)*sind(theta) - sind(RAAN)*cosd(i)*cosd(theta);
C13 = sind(RAAN)*sind(i);
C21 = sind(RAAN)*cosd(theta) + cosd(RAAN)*cosd(i)*sind(theta);
C22 = -sind(RAAN)*sind(theta) + cosd(RAAN)*cosd(i)*cosd(theta);
C23 = -cosd(RAAN)*sind(i);
C31 = sind(i)*sind(theta);
C32 = sind(i)*cosd(theta);
C33 = cosd(i); 
C = [C11,C12,C13;C21,C22,C23;C31,C32,C33]; % transformation matrix

% define position vector in (r,theta,h) frame
mu_E = 3.986004415*10^5; % gravitational parameter of Earth [km^3/s^2]
h = sqrt(mu_E*a*(1-e^2)); % angular momentum [km^2/s]
r = (h^2/mu_E)*1/(1+e*cosd(theta));
r_roh = [r;theta;0];

% convert (r,theta,h) to (X,Y,Z)
r_xyz = C*r_roh;

% plot 3D model to verify
[x y z] = sphere;
r_E = 6378; % radius of Earth [km]
r_sc = 500; % radius of s/c (made large enough to be visible) [km]

r_X = r_xyz(1); % x position of s/c
r_Y = r_xyz(2); % y position of s/c
r_Z = r_xyz(3); % z position of s/c

r_mag = sqrt(r_X^2 + r_Y^2 + r_Z^2); % s/c distance from center of Earth

figure()
surf(r_E*x,r_E*y,r_E*z)
hold on
surf(r_sc*x+r_X,r_sc*y+r_Y,r_sc*z+r_Z)
% xlim([-50000,20000])
% ylim([-10000,10000])
% zlim([-20000,20000])
arrow3d([0 12000],[0 0],[0 0],0.8,250,500,'r') % Earth inertial X-axis
arrow3d([0 0],[0 10000],[0 0],0.8,250,500,'b') % Earth inertial Y-axis
arrow3d([0 0],[0 0],[0 12000],0.8,250,500,'g') % Earth inertial Z-axis

% % plot 3D model for different locations in orbit
% [x y z] = sphere;
% r_E = 6378; % radius of Earth [km]
% r_sc = 500; % radius of s/c (made large enough to be visible) [km]
% 
% figure()
% surf(r_E*x,r_E*y,r_E*z)
% hold on
% 
% theta = linspace(0,359,100);
% for j = 1:100
%     % define transformation matrix
%     C11 = cosd(RAAN)*cosd(theta(j)) - sind(RAAN)*cosd(i)*sind(theta(j));
%     C12 = -cosd(RAAN)*sind(theta(j)) - sind(RAAN)*cosd(i)*cosd(theta(j));
%     C13 = sind(RAAN)*sind(i);
%     C21 = sind(RAAN)*cosd(theta(j)) + cosd(RAAN)*cosd(i)*sind(theta(j));
%     C22 = -sind(RAAN)*sind(theta(j)) + cosd(RAAN)*cosd(i)*cosd(theta(j));
%     C23 = -cosd(RAAN)*sind(i);
%     C31 = sind(i)*sind(theta(j));
%     C32 = sind(i)*cosd(theta(j));
%     C33 = cosd(i); 
%     C = [C11,C12,C13;C21,C22,C23;C31,C32,C33]; % transformation matrix
% 
%     % define position vector in (r,theta,h) frame
%     mu_E = 3.986004415*10^5; % gravitational parameter of Earth [km^3/s^2]
%     h = sqrt(mu_E*a*(1-e^2)); % angular momentum [km^2/s]
%     r = (h^2/mu_E)*1/(1+e*cosd(theta(j)));
%     r_roh = [r;theta(j);0];
% 
%     % convert (r,theta,h) to (X,Y,Z)
%     r_xyz = C*r_roh;
% 
%     r_X = r_xyz(1); % x position of s/c
%     r_Y = r_xyz(2); % y position of s/c
%     r_Z = r_xyz(3); % z position of s/c
% 
%     surf(r_sc*x+r_X,r_sc*y+r_Y,r_sc*z+r_Z)
% end
% arrow3d([0 12000],[0 0],[0 0],0.8,250,500,'r') % Earth inertial X-axis
% arrow3d([0 0],[0 10000],[0 0],0.8,250,500,'b') % Earth inertial Y-axis
% arrow3d([0 0],[0 0],[0 12000],0.8,250,500,'g') % Earth inertial Z-axis
% hold off


%% Map 2D projection to 3D model

% send out a ray for each pixel that contains the Earth in it then
% from pseudo 3D model, compute intersection points

% work in progress...
t = linspace(0,10000,100);
phi = 8.15;
theta_c = 10.2;
for k = 1:100
    if r_X > 0
        x_t(k) = r_X - t(k);
    else
        x_t(k) = r_X + t(k);
    end
     if r_Y > 0
        y_t(k) = r_Y - sind(phi)*t(k);
    else
        y_t(k) = r_Y + sind(phi)t(k);
     end
     if r_X > 0
        x_t(k) = r_X - t(k);
    else
        x_t(k) = r_X + t(k);
    end
end
plot3(x_t,y_t,z_t)


% plot rays from camera FOV
% quiver3(r_X,r_Y,r_Z,-0.5*r_X,-0.5*r_Y,-0.5*r_Z) % center of image
% quiver3(r_X,r_Y,r_Z,-c_X,-sind(10.2)*,-c_Z) % top-left of image
hold off

%% Convert intersection points to Earth coordinates









