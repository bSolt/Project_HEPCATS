% HEPCATS Geolocation Imaging Software
%
% This code will compute the Earth coordinates of a selected pixel. After  
% the script is run, an image will be displayed to the user. Select the 
% desired location with the crosshairs and the program will compute and  
% display the corresponding latitude and longitude coordinates. 
%
% Input: image and time taken from HEPCATS orbit
% Output: Lat/long of selected pixel in frame
%
% Author(s): Alex Baughman
% Date created: 2/17/19
% Last modified: 4/16/19

close all;clear all;clc

%% Read in image and request desired Earth location

% read in image
I = imread('Image5.jpg');

xmax = size(I,2);
ymax = size(I,1);

% get x,y location of user requested pixel (image will display with
% crosshairs, select a pixel in frame with cursor)
pts = readPoints(I,1);

%% Make 3D model

% using information about s/c location and orientation in orbit, generate a
% pseudo 3D model 

% define orbit parameters
a = 26000; % semi-major axis [km]
e = 0.74; % eccentricity
i = 63.4; % inclination [deg]
AOP = 270; % argument of periapsis [deg]
RAAN = 0; % right ascension of the ascending node [deg]
theta_star = 205.65; % true anomaly [deg]
theta_rot = theta_star + AOP; % third angle of rotation (used for transformation matrix)
jd = juliandate(2018,11,29,4,21,47); % julian date [days]
%jd = juliandate(2000,1,1,12,00,0); % julian date at J2000 [days]

% define transformation matrix
C11 = cosd(RAAN)*cosd(theta_rot) - sind(RAAN)*cosd(i)*sind(theta_rot);
C12 = -cosd(RAAN)*sind(theta_rot) - sind(RAAN)*cosd(i)*cosd(theta_rot);
C13 = sind(RAAN)*sind(i);
C21 = sind(RAAN)*cosd(theta_rot) + cosd(RAAN)*cosd(i)*sind(theta_rot);
C22 = -sind(RAAN)*sind(theta_rot) + cosd(RAAN)*cosd(i)*cosd(theta_rot);
C23 = -cosd(RAAN)*sind(i);
C31 = sind(i)*sind(theta_rot);
C32 = sind(i)*cosd(theta_rot);
C33 = cosd(i); 
C = [C11,C12,C13;C21,C22,C23;C31,C32,C33]; % transformation matrix

% define position vector in (r,theta,h) frame
mu_E = 3.986004415*10^5; % gravitational parameter of Earth [km^3/s^2]
h = sqrt(mu_E*a*(1-e^2)); % angular momentum [km^2/s]
r = (h^2/mu_E)*1/(1+e*cos(theta_star*(pi/180)));
r_roh = [r;0;0];

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

% account for rotating Earth axis'
delta_t_sec = (jd-2451545)*24*60*60; % time elapsed since J2000 epoch [sec]
omega_E = 7.292*10^(-5); % rotation rate - 360 deg in 365.25 days [rad/s]
X_axis = [cosd(23.5)*12000*cos(omega_E*delta_t_sec);cosd(23.5)*12000*sin(omega_E*delta_t_sec);-sind(23.5)*12000*sin(omega_E*delta_t_sec)]; % direction of Earth-fixed X-axis
Y_axis = [cosd(23.5)*-12000*sin(omega_E*delta_t_sec);cosd(23.5)*12000*cos(omega_E*delta_t_sec);-sind(23.5)*12000*cos(omega_E*delta_t_sec)]; % direction of Earth-fixed Y-axis
Z_axis = [0;12000*sind(23.5);12000*cosd(23.5)]; % direction of Earth-fixed Z-axis

% plot 3D model for single location in orbit
figure()
surf(r_E*x,r_E*y,r_E*z)
hold on
surf(r_sc*x+r_X,r_sc*y+r_Y,r_sc*z+r_Z)
arrow3d([0 X_axis(1)],[0 X_axis(2)],[0 X_axis(3)],0.8,250,500,'r') % Earth fixed X-axis
arrow3d([0 Y_axis(1)],[0 Y_axis(2)],[0 Y_axis(3)],0.8,250,500,'b') % Earth fixed Y-axis
arrow3d([0 Z_axis(1)],[0 Z_axis(2)],[0 Z_axis(3)],0.8,250,500,'g') % Earth fixed Z-axis
arrow3d([0 15000],[0 0],[0 0],0.8,250,500,'k') % Earth inertial J2000 X-axis
arrow3d([0 0],[0 15000],[0 0],0.8,250,500,'k') % Earth inertial J2000 Y-axis
arrow3d([0 0],[0 0],[0 15000],0.8,250,500,'k') % Earth inertial J2000 Z-axis


%% Map 2D projection to 3D model

% send out a ray for selected pixel then compute intersection points

% define optical parameters
theta_max = 20.4; % [deg]
phi_max = 10.74; % [deg] should be 16.24 from camera, 10.74 is relative aspect ratio from screenshots
K = 2.5; % scale factor

% convert user requested pixel into pointing angle
x_point = pts(1); % see line 24 for pts function
y_point = pts(2);

x_center = xmax/2;
y_center = ymax/2;

if x_point >= x_center
    theta_point = ((x_point-x_center)/x_center)*theta_max; % right half of image corresponds to positive theta
else
    theta_point = -((x_center-x_point)/x_center)*theta_max;
end
if y_point <= y_center
    phi_point = ((y_center-y_point)/y_center)*phi_max; % upper half of image corresponds to positive phi
else
    phi_point = -((y_point-y_center)/y_center)*phi_max;
end

% convert to radians
theta_point = theta_point*(pi/180);
phi_point = phi_point*(pi/180);

% define vectors in s/c frame [r,theta,h]
v_center_roh = [-r_mag;0;0]; % center of image
v_pointing_roh = [-r_mag*cos(phi_point)*cos(theta_point);r_mag*cos(phi_point)*sin(theta_point);r_mag*sin(phi_point)]; % user input 
v_TL_roh = [-r_mag*cosd(phi_max)*cosd(theta_max);-r_mag*cosd(phi_max)*sind(theta_max);r_mag*sind(phi_max)]; % top-left of image
v_TR_roh = [-r_mag*cosd(phi_max)*cosd(theta_max);r_mag*cosd(phi_max)*sind(theta_max);r_mag*sind(phi_max)]; % top-right of image
v_BL_roh = [-r_mag*cosd(phi_max)*cosd(theta_max);-r_mag*cosd(phi_max)*sind(theta_max);-r_mag*sind(phi_max)]; % bottom-left of image
v_BR_roh = [-r_mag*cosd(phi_max)*cosd(theta_max);r_mag*cosd(phi_max)*sind(theta_max);-r_mag*sind(phi_max)]; % bottom-right of image

% convert to Earth centered inertial frame [X,Y,Z]
v_center_xyz = C*v_center_roh;
v_pointing_xyz = C*v_pointing_roh;
v_TL_xyz = C*v_TL_roh;
v_TR_xyz = C*v_TR_roh;
v_BL_xyz = C*v_BL_roh;
v_BR_xyz = C*v_BR_roh;

% plot rays on 3D model
quiver3(r_X,r_Y,r_Z,K*v_center_xyz(1),K*v_center_xyz(2),K*v_center_xyz(3),'k') % center of image
quiver3(r_X,r_Y,r_Z,K*v_pointing_xyz(1),K*v_pointing_xyz(2),K*v_pointing_xyz(3),'r') % pointing vector
quiver3(r_X,r_Y,r_Z,K*v_TL_xyz(1),K*v_TL_xyz(2),K*v_TL_xyz(3),'k') % top-left of image
quiver3(r_X,r_Y,r_Z,K*v_TR_xyz(1),K*v_TR_xyz(2),K*v_TR_xyz(3),'k') % top-right of image
quiver3(r_X,r_Y,r_Z,K*v_BL_xyz(1),K*v_BL_xyz(2),K*v_BL_xyz(3),'k') % bottom-left of image
quiver3(r_X,r_Y,r_Z,K*v_BR_xyz(1),K*v_BR_xyz(2),K*v_BR_xyz(3),'k') % bottom-right of image

% compute intersection points
v_hat = v_pointing_xyz./r_mag; % unit vector in direction of pointing ray

a = v_hat(1)^2 + v_hat(2)^2 + v_hat(3)^2;
b = 2*dot(r_xyz,v_hat);
c = r_mag^2 - r_E^2;
d = b*b - 4*a*c;  % discriminant of quadratic

if d <  0 % then solutions are complex, so no intersections
    fprintf('No intersections. Run program again and select a different pixel.\n')
elseif d >= 0 % then solutions are real, so there are intersections
    quadRoots1 = (-b+sqrt(d))/(2*a);
    quadRoots2 = (-b-sqrt(d))/(2*a);
    
    intersection1 = r_xyz + quadRoots1.*v_hat; % first intersection
    intersection2 = r_xyz + quadRoots2.*v_hat; % second intersection

    % find intersection point that's visible from s/c i.e. shortest distance
    distance1 = sqrt((r_xyz(1)-intersection1(1))^2 + (r_xyz(2)-intersection1(2))^2 + (r_xyz(3)-intersection1(3))^2);
    distance2 = sqrt((r_xyz(1)-intersection2(1))^2 + (r_xyz(2)-intersection2(2))^2 + (r_xyz(3)-intersection2(3))^2);

    % define XYZ coordinates on surface of Earth
    if distance1 > distance2
        surface_xyz = intersection2; 
    else
        surface_xyz = intersection1;
    end

    %% Convert intersection points to Earth coordinates

    % define intersection coordinates in Earth centered inertial frame
    X_ECI = surface_xyz(1);
    Y_ECI = surface_xyz(2);
    Z_ECI = surface_xyz(3);
 
    % angle between Earth fixed x-axis and Earth inertial x-axis, 
    % Earth fixed x-axis is aligned with Greenwich Meridan
    theta_LST = omega_E*delta_t_sec; % + 73.86*(pi/180); % 73.86 corresponds to Greenwich offset from ECI at J2000

    % convert from Earth inertial to Earth fixed (aligned with
    % Greenwich Meridian)
    ECI_to_ECEF = [cos(23.5*(pi/180))*cos(theta_LST),cos(23.5*(pi/180))*sin(theta_LST),-sin(23.5*(pi/180))*sin(theta_LST);... % x transform
        cos(23.5*(pi/180))*-sin(theta_LST),cos(23.5*(pi/180))*cos(theta_LST),-sin(23.5*(pi/180))*cos(theta_LST);... % y transform
        0,sin(23.5*(pi/180)),cos(23.5*(pi/180))]; % z transform
    
    R2 = [cos(theta_LST),sin(theta_LST),0;-sin(theta_LST),cos(theta_LST),0;0,0,1];
    R1 = [0,0,1;0,cos(-23.5*(pi/180)),sin(-23.5*(pi/180));0,-sin(-23.5*(pi/180)),cos(-23.5*(pi/180))];
    ECI2ECEF = R1*R2;
    rot_loc = ECI2ECEF*[X_ECI;Y_ECI;Z_ECI];
    
    rotated_location = ECI_to_ECEF*[X_ECI;Y_ECI;Z_ECI];
    X_ECEF = X_ECI*(cos(23.5*(pi/180))*cos(theta_LST) + cos(23.5*(pi/180))*sin(theta_LST) + -sin(23.5*(pi/180))*sin(theta_LST));
    Y_ECEF = Y_ECI*(cos(23.5*(pi/180))*-sin(theta_LST) + cos(23.5*(pi/180))*cos(theta_LST) + sin(-23.5*(pi/180))*cos(theta_LST));
    Z_ECEF = Z_ECI*(sin(23.5*(pi/180)) + cos(23.5*(pi/180)));
    
    % plot the Greenwich Meridian line on 3D plot 
    GM_axis = [cosd(23.5)*12000*cos(theta_LST);cosd(23.5)*12000*sin(theta_LST);-12000*sind(23.5)*sin(theta_LST)]; 
    arrow3d([0 GM_axis(1)],[0 GM_axis(2)],[0 GM_axis(3)],0.8,250,500,'m') % Greenwich Meridian line, Earth-fixed
    hold off
    
    % compute and print the longitude
    if Y_ECEF >= 0 && X_ECEF >= 0
        long = atand(Y_ECEF/X_ECEF); % East 
        fprintf('The longitude is: %.2f deg E\n',long)
    elseif Y_ECEF >= 0 && X_ECEF < 0
        long = 180 + atand(Y_ECEF/X_ECEF);
        fprintf('The longitude is: %.2f deg E\n',long)
    elseif Y_ECEF < 0 && X_ECEF >= 0
        long = -atand(Y_ECEF/X_ECEF); % West 
        fprintf('The longitude is: %.2f deg W\n',long)
    elseif Y_ECEF < 0 && X_ECEF < 0
        long = 180 - atand(Y_ECEF/X_ECEF);
        fprintf('The longitude is: %.2f deg W\n',long)
    end

    % compute and print the latitude
    if Z_ECEF >= 0
        az_dist = sqrt(X_ECEF^2 + Y_ECEF^2);
        lat = atand(Z_ECEF/az_dist);
        fprintf('The latitude is: %.2f deg N\n',lat) % North
    else
        az_dist = sqrt(X_ECEF^2 + Y_ECEF^2);
        lat = atand(Z_ECEF/az_dist);
        fprintf('The latitude is: %.2f deg S\n',lat) % South
    end
end




