% Robert Karam
% 7/21/2015
% CAT_basic.m
% Basic implementation of CAT algorithm including initial EMA filtering,
% DWT, and adaptive thresholding.


Fc = 0.01;  % cutoff frequency in hertz for low pass filter (EMA)
fs = 0.01;  % sample period (100 Hz) in seconds

% time in seconds to analyze at once
windowTimeSeconds = 90;

% reading in the file (in the real implementation you'll get
% 1 sample at a time (I think every 10 ms) instead
res_file_name = sprintf('res_a/cat/res_%02i.csv',fileIndex);
result = zeros(num_comps, 7);
[Time, Pabd, Pves, fileName] = read_data_A(fileIndex);

% calculate filtering coefficient
coA = 1 - exp(-(Fc / fs) * pi * 2.0);

% % this is median filtering, you can skip this step
% winwid = 7;
% medianFilt = zeros (1, sigLen);
% m_wind = zeros(64);
% for i = 1 : sigLen - winwid
%     left = i;
%     right = i + winwid;
%     m_wind = Pves(left:right);
%     med = median(m_wind);
%     Pves(i + (floor(winwid/2))) = med;
% end

% this is the EMA filter
% when applying in real time, your 'input' sample will be Pves(index)
% and  your filtered output will be in the sumLog buffer (probably easiest
% if it's a circular buffer)
sum = 0;
sumLog = zeros(1, sigLen);
events = zeros(1, sigLen);

for index = 2 : sigLen
    sumLog(index) = sumLog(index - 1) + coA * (Pves(index) - sumLog(index - 1));
end

% this is the DWT - you'll be using the c lib to implement this in real
% time. Passing a portion of the buffer (we may need to play around with
% that to get the best size)
mw = 'db2';
s0 = sumLog;

[a0, d0] = dwt(s0,   mw);
[a1, d1] = dwt(a0,   mw);
[a2, d2] = dwt(a1,   mw);
[a3, d3] = dwt(a2,   mw);
[a4, d4] = dwt(a3,   mw);
[a5, d5] = dwt(a4,   mw);

% wtLevel = wavelet transform level
% initialize wtFS to sample frequency (100 Hz in this case)
wtLevel = 5;
wtFS = fs;

% because you downsample by 2 every step of the transform, you divide
% the time resolution in half
for i = 1 : wtLevel
    wtFS = wtFS / 2;
end



% this extracts a portion of the EMA filtered + DWT transformed data
% (wtWindowLen samples) computed as
% [(wavelet transform sample frequency) * (target window time in seconds)]
wtWindowLen = ceil(wtFS * windowTimeSeconds);
wtMax = wtLen - wtWindowLen;

% process signal with adaptive thresholding
% wtWindowLen = length of window (in samples) of the transformed data
winEnd = index  + wtWindowLen;

% window signal (approximation) and details
sigInWindow = wtSigA(index : winEnd);
detInWindow = wtSigD(index : winEnd);

% calculate quantiles (approximation quantiles)
% be_percentile = bladder event percentile, a double from 0 to 1 (e.g.
% 0.95, where 95% of data is below that value. check the matlab
% documentation for the algorithm they use
appQuant = quantile(sigInWindow,be_percentile);
if (sigInWindow(wtWindowLen) > appQuant) && sigInWindow(wtWindowLen) > 5
    contractions(winEnd) = 1;
else
    contractions(winEnd) = 0;
end

detQuant = quantile(detInWindow, ma_percentile);
if detInWindow(wtWindowLen) > detQuant%quantile(sigInWindow,ma_percentile)%median(sigInWindow)%
   detEventLog(winEnd) = 1;
else
    detEventLog(winEnd) = 0;
end

% this gives us our classification of bladder events 
% (contractions and artifacts). Instead of logging in a buffer
% you will record a time stamp (the teensy will have a fairly accurate RTC)
% and later, we will go through the data and see if it matches up with
% sensations the subject was having.


