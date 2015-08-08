function [ Time, Pabd, Pves, Artifacts, Sense0, Sense1, Sense2, filename ] = read_data_A( datnum )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

% Find .lvm files
%lab computer OR laptop
%pathName = 'C:\Users\Robert\Dropbox\WIMM\Acute UDS CTRL Data WIMM Algorithm\Dataset_A';
%home computer:
pathName = 'E:\User Files\Dropbox\Dropbox\WIMM\Acute UDS CTRL Data WIMM Algorithm\Dataset_A';
%cd(pathName);

fileList = dir('Dataset_A\*.lvm');
fileName = fileList(datnum).name;

% Load data
Data = dlmread(fullfile(pathName, fileName),'\t',23,0);

% Column headers from .lvm files
ColNames = {'Time','Pabd','C','Pves','E','F','Stim','H','Strong Urge',...
    'FlowON','FlowOFF','First Sensation','Moderate Urge','Artifact'};
% Example data vector assignments
Time = Data(:,strcmp(ColNames,'Time'));
Pabd = Data(:,strcmp(ColNames,'Pabd'));
Pves = Data(:,strcmp(ColNames,'Pves'));
Artifacts = Data(:,strcmp(ColNames,'Artifact'));

Sense0 = Data(:,strcmp(ColNames,'First Sensation'));
Sense1 = Data(:,strcmp(ColNames,'Moderate Urge'));
Sense2 = Data(:,strcmp(ColNames,'Strong Urge'));
filename = fileName;

end

