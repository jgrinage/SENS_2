
function [steps_per_sec,Z_norm] = Cadence(filename)


Fs = 25; %found by manually inspect the spreadsheet

SENSOR_FUSION = importdata(filename);
SENSOR_FUSION = SENSOR_FUSION.data;
%first six columns correspond to the following
%Roll (deg)
%Pitch (deg)
%Compass (deg)
%Accelerometer (g) X
%Accelerometer (g) Y
%Accelerometer (g) Z

Roll = SENSOR_FUSION(:,1);
Pitch =SENSOR_FUSION(:,2);
Compass = SENSOR_FUSION(:,3);
Accel_X = SENSOR_FUSION(:,4);
Accel_Y = SENSOR_FUSION(:,5);
Accel_Z = SENSOR_FUSION(:,6);
% 
 % figure;
 % plot(Accel_X,'b');
 % hold on;
 % plot(Accel_Y,'m');
 % plot(Accel_Z,'r');
 % legend('X','Y','Z');

Accel = cell(3,1);
Accel{1} = Accel_X;
Accel{2} = Accel_Y;
Accel{3} = Accel_Z;

%% Frequency Domain Analysis
Z_mean = mean(Accel_Z);
X_mean = mean(Accel_X);
Y_mean = mean(Accel_Y);
Tot_mean = Z_mean+X_mean+Y_mean;
Z_weight = Z_mean/Tot_mean
X_weight = X_mean/Tot_mean
Y_weight = Y_mean/Tot_mean

data = sqrt((Accel_Z*Z_weight).^2+(Accel_Y*Y_weight).^2+(Accel_X*X_weight).^2);
data = data-mean(data);% subtract DC value


%fft original data (with DC subtracted)
fftlength = 2^nextpow2(length(data));
L = length(data);
fdata = fft(data, fftlength) / L;
ctr = (fftlength / 2) + 1;
faxis = (Fs / 2) .* linspace(0,1, ctr);
mag = abs(fdata(1:ctr));

figure; 

subplot 211;

plot(faxis, mag);hold on;
title('Data X');
% Boxcar filter the resulting spectrum
box = ones(1,5);
mag_box = conv(box,mag); %convolution of filter with signal
length(mag_box(5:end));
length(mag);
plot(faxis,mag_box(5:end),'c');hold off;
xlabel('Steps/sec');
ylabel('|S(f)|');

% BPF 1-4 Hz : butter
[b,a]= butter(10,[1 4]/(Fs/2));
M_filt = filter(b,a,data);
fdata = fft(M_filt,fftlength) / L;
subplot 212;
plot(faxis, abs(fdata(1:ctr)));
xlabel('Steps/sec');
ylabel('|S(f)|');

% Spectrogram
% figure;
% spectrogram(M_filt,256,250,Fs,30,'yaxis');
% title('Filtered 256/250');
% figure;
% specgram(data,256,Fs);
% title('Raw Data');
% figure;
% subplot 211;
% specgram(M_filt,128,Fs);
% title('Bandpassed 128');
% subplot 212;
% specgram(M_filt,256,Fs);
% title('Bandpassed 256');

figure;
nwin= 256;
wind = kaiser(nwin,10);
nlap = nwin-10;
nfft = 512;

[s,f,t,p] = spectrogram(M_filt,wind,nlap,nfft,Fs,'yaxis');
spectrogram(M_filt,wind,nlap,nfft,Fs,'yaxis');
title('BPF Spectrogram');
hold on
[q,nd] = max(p);
cdnc = mean(f(nd)) % f is cyclical frequencies
plot3(t,f(nd),q,'r','linewidth',1)
hold off
colorbar
view(2)
% figure;
% spectrogram(M_filt,wind,nlap,nfft,Fs,'yaxis');
% view(-70,45);

% figure;
% length(data)/3
% minv = minvar(data,floor(length(data)/3));
% plot(minv);

%% comparison between methods

figure;subplot 221;
[per,f] = periodogram(data,[],[],Fs);
[~,idx_4] = min(abs(f-4));
[~,idx] = max(per(1:idx_4));
plot(f,per,'r');hold on;
plot(f(idx),per(idx),'*r','MarkerSize',12);
[bart_data,new_faxis] = bart(data,10,Fs);
[~,idx_4_b] = min(abs(new_faxis-4));
[~,idx_b] = max(bart_data(1:idx_4_b));
plot(new_faxis,bart_data);
plot(new_faxis(idx_b),bart_data(idx_b),'*','MarkerSize',12);
plot(new_faxis,bart_data);
plot(new_faxis(idx_b),bart_data(idx_b),'*','MarkerSize',12);
text_p = sprintf('Cadence from Periodogram = %.2f ', f(idx));
text_b = sprintf('Cadence from BP = %.2f ', new_faxis(idx_b));
hold off; legend('Periodogram',text_p,'Bartlett Procedure(BP)',text_b);
xlabel('frequency/Hz');
title('Spectral Estimation of Original Spectrum');

subplot 223;
[per,f] = periodogram(M_filt,[],[],Fs);
[~,idx_4] = min(abs(f-4));
[~,idx] = max(per(1:idx_4));
plot(f,per,'r');hold on;
plot(f(idx),per(idx),'*r','MarkerSize',12);
[bart_data,new_faxis] = bart(M_filt,10,Fs);
[~,idx_4_b] = min(abs(new_faxis-4));
[~,idx_b] = max(bart_data(1:idx_4_b));
plot(new_faxis,bart_data);
plot(new_faxis(idx_b),bart_data(idx_b),'*','MarkerSize',12);
text_p = sprintf('Cadence from Periodogram = %.2f ', f(idx));
text_b = sprintf('Cadence from BP = %.2f ', new_faxis(idx_b));
hold off; legend('Periodogram',text_p,'Bartlett Procedure(BP)',text_b);
xlabel('frequency/Hz');
title('Spectral Estimation of BPF-ed Spectrum');

subplot 222;
plot(faxis, mag);xlabel('frequency/Hz');hold on;
[~,idx_4_o] = min(abs(mag-4));
[~,idx_o] = max(per(1:idx_4_o));
plot(faxis(idx_o),mag(idx_o),'k*','MarkerSize',12);
text_o = sprintf('Cadence from Original FFT = %.2f ', faxis(idx_o));
legend('original FFT',text_o);
title('Original Spectrum');

subplot 224
spec_b = abs(fdata(1:ctr));
plot(faxis, spec_b);xlabel('frequency/Hz');hold on;
[max_o_b,idx_o_b] = max(abs(fdata(1:ctr)));
plot (faxis(idx_o_b), max_o_b,'k*','MarkerSize',12);
text_o_b = sprintf('Cadence from original BPF FFT = %.2f ', faxis(idx_o_b));
legend('original BPF FFT',text_o_b);
title('Original BPF Spectrum');
end

function Px = minvar(x,p)

%MINVAR	Spectrum estimation using the minimum variance method.

%----

%USAGE	Px = minvar(x,p) 

%

%	The spectrum of a process x is estimated using the minimum

%	variance method (sometimes called the maximum likelihood

%	method).

%

%	x  :  Input sequence

%	p  :  Order of the minimum variance estimate - for short

%		sequences, p is typically about length(x)/3

%

%	The spectrum estimate is returned in Px using a dB scale.

%

%---------------------------------------------------------------

% copyright 1996, by M.H. Hayes.  For use with the book 

% "Statistical Digital Signal Processing and Modeling"

% (John Wiley & Sons, 1996).

%---------------------------------------------------------------

%

   x = x(:);

   R = covar(x,p);

   [v,d]=eig(R);

   U = diag(inv(abs(d)+eps));

   V  = abs(fft(v,1024)).^2;

   Px = 10*log10(p)-10*log10(V*U);

end


function [Px,frequency] = bart(x,nsect,Fs)

%BART	Bartlett's method of periodogram averaging.

%----

%USAGE	Px = bart(x,nsect) 

%

%	The spectrum of a process x is estimated using Bartlett's 

%	method of periodogrm averaging.

%	

%	x     :  Input sequence

%	nsect :	 Number of subsequences to be used in the average

%

%	The Bartlett estimate is returned in Px using a linear scale.

%

%  see also PER, MPER, WELCH, and SPER

%

%---------------------------------------------------------------

% copyright 1996, by M.H. Hayes.  For use with the book 

% "Statistical Digital Signal Processing and Modeling"

% (John Wiley & Sons, 1996).

%---------------------------------------------------------------



L  = floor(length(x)/nsect);

Px = 0;

n1 = 1;

for i=1:nsect

    Px = Px + periodogram(x(n1:n1+L-1))/nsect;
    [~,frequency] = periodogram(x(n1:n1+L-1),[],[],Fs);
    n1 = n1 + L;
    
end
end