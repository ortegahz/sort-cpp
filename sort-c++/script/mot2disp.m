%%
% author: manu

%%
close all; clear;

%%
opts.dir_seq = '/media/manu/intem/sort/2DMOT2015/train/TUD-Campus';

%%
path_det = fullfile(opts.dir_seq, 'det', 'det.txt');

%%
% [fid tid x y w h score 3dx 3dy sdz]
fileID = fopen(path_det);
C = textscan(fileID, '%d,%d,%f,%f,%f,%f,%f,%f,%f,%f');
fclose(fileID);

%%
dir_imgs = fullfile(opts.dir_seq, 'img1');
list_img  = struct2cell(dir(fullfile(dir_imgs, '*.jpg')))';
paths_img = fullfile(dir_imgs, list_img(:, 1));

num_frame = length(paths_img);

for i = 1 : num_frame
    name_img = sprintf('%06d.jpg', i);
    
    path_img = fullfile(dir_imgs, name_img);
    
    fprintf('processing %dth img %s [total %d]\n', ...
        i, path_img, num_frame);
    
    img = imread(path_img);

    imshow(img);
    title(path_img)
    
    hold on;
    
    idxs = find(C{1} == i);
    for j = 1 : length(idxs)
        idx = idxs(j);
        tid = C{2}(idx);
        x = C{3}(idx);
        y = C{4}(idx);
        w = C{5}(idx);
        h = C{6}(idx);
%         fprintf('%d,%d,%f,%f,%f,%f\n', i, tid, x, y, w, h);
        rectangle('Position', [x, y, w, h], 'Edgecolor', 'g', 'LineWidth', 3);
    end
    
    hold off;
    
    pause(0.01);

end

close all;

%%