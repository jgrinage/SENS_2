function Bikedata(filename)



SENS_DATA = importdata(filename);
SENS_DATA = SENS_DATA.data;

% Columns of correspond to following
% X 
% Y
% Z

Accel_X = SENS_DATA(:,1);
Accel_Y = SENS_DATA(:,2);
Accel_Z = SENS_DATA(:,3);



Value1 = Accel_X(1)
Accel_X = double(bitcmp(uint16(Accel_X)+1,'uint16'))/8192.0;
Accel_Y = double(bitcmp(uint16(Accel_Y)+1,'uint16'))/8192.0;
Accel_Z = double(bitcmp(uint16(Accel_Z)+1,'uint16'))/8192.0;


Z_mean = mean(Accel_Z);
X_mean = mean(Accel_X);
Y_mean = mean(Accel_Y);
Tot_mean = Z_mean+X_mean+Y_mean;
Z_weight = Z_mean/Tot_mean
X_weight = X_mean/Tot_mean
Y_weight = Y_mean/Tot_mean


figure;
plot(Accel_X,'b');
hold on;
plot(Accel_Y,'m');
plot(Accel_Z,'r');
legend('X','Y','Z');


data = sqrt((Accel_Z*Z_weight).^2+(Accel_Y*Y_weight).^2+(Accel_X*X_weight).^2);
data = Accel_Y;
data = data-mean(data);% subtract DC value
Fs = 824; % Visually inspecting the oscilloscope
figure;
fftlength = 2^nextpow2(length(data));
L = length(data);
fdata = fft(data, fftlength) / L;
ctr = (fftlength / 2) + 1;
faxis = (Fs / 2) .* linspace(0,1, ctr);
mag = abs(fdata(1:ctr));
plot(faxis,mag);
title('Data');

