%%
% author: manu

%%
close all; clear;

%%
opts.dir_seq = '/media/manu/intem/sort/2DMOT2015/train/ETH-Pedcross2';
% opts.dir_seq = '/media/manu/kingstop/workspace/sort/data/train/ETH-Pedcross2';

opts.n_colors = 50;

%%
% path_det = fullfile('/media/manu/kingstop/workspace/sort/data/train/ETH-Pedcross2', 'det', 'det.txt');
path_det = '/home/manu/nfs/swift/ETH-Pedcross2.txt';

%%
% [fid tid x y w h score 3dx 3dy sdz]
fileID = fopen(path_det);
C = textscan(fileID, '%d,%d,%f,%f,%f,%f,%f,%f,%f,%f');
fclose(fileID);

%%
colors = colormap(hsv(opts.n_colors));

dir_imgs = fullfile(opts.dir_seq, 'img1_pg');
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
        idx_color = mod(tid, opts.n_colors) + 1;
        rectangle('Position', [x, y, w, h], ...
            'Edgecolor', colors(idx_color, :), ...
            'LineWidth', 1);
        text(x, y, num2str(tid),'color', colors(idx_color, :), 'FontSize', 25)
    end
    
    hold off;
    
    pause(0.01);

end

close all;

%%