% Example 2, Corresponding points
% Load images
% I1=imread('E:\502\picture\161118/Sol011A_P2534_1_True_RAD.jpg');
%  I2=imread('E:\502\picture\161118/Sol011A_P2535_1_True_RAD.jpg');
%   I1=imread('E:\502\matlab\SURF\TestImages/testc1.png');
%   I2=imread('E:\502\matlab\SURF\TestImages/testc2.png');
 %   I1=imread('E:\502\matlab\SURF\TestImages/F256.png');
%    I2=imread('E:\502\matlab\SURF\TestImages/S256.png');
   
% I1=imread('E:\502\SAD_test_data\group1/l_img.png');
% I2=imread('E:\502\SAD_test_data\group1/r_img.png');
% I1=imread('E:\502\SAD_test_data\group2/img_L1024.png');
% I2=imread('E:\502\SAD_test_data\group2/img_R1024.png');
% I1=imread('E:\502\SAD_test_data\group3/img_L5_1024.png');
% I2=imread('E:\502\SAD_test_data\group3/img_R5_1024.png');
% I1=imread('E:\502\SAD_test_data\group4/img_L6_1024.png');
% I2=imread('E:\502\SAD_test_data\group4/img_R6_1024.png');

 I1=imread('I:\E\502\picture\SAD_test_data\group1/img1.png');
 I2=imread('I:\E\502\picture\SAD_test_data\group1/img2.png');
% I1=imread('E:\502\picture\SAD_test_data\group2/img1.png');
%   I2=imread('E:\502\picture\SAD_test_data\group2/img2.png');
%  I1=imread('E:\502\picture\SAD_test_data\group3/img1.png');
% I2=imread('E:\502\picture\SAD_test_data\group3/img2.png');
% I1=imread('E:\502\picture\SAD_test_data\group4/img1.png');
% I2=imread('E:\502\picture\SAD_test_data\group4/img2.png');

%   I1=imread('G:\Data\SLAM\TUM RGB-D\rgbd_dataset_freiburg1_desk\rgb\1305031452.791720.png');
%   I2=imread('G:\Data\SLAM\TUM RGB-D\rgbd_dataset_freiburg1_desk\rgb\1305031452.823674.png');
%保存图像文件数据
[m,n]=size(I1);
fid=fopen('d:\a1.txt','w+');
for i=1:m
    for j=1:n
        fprintf(fid,'%d  ',I1(i,j));
    end
    fprintf(fid,'\n');
end
fclose(fid);
[m,n]=size(I2);
fid=fopen('d:\a2.txt','w+');
for i=1:m
    for j=1:n
        fprintf(fid,'%d  ',I2(i,j));
    end
    fprintf(fid,'\n');
end
fclose(fid);
%%%%%%%%%%%%%%%%%%%%%%%图像下采样及保存%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% SI1 = zeros(256, 256);
% for i=1:256
%     for j=1:256
%    %     simg(i,j) = mean( mean( img(i*4-3:i*4,j*4-3:j*4) ) );
%          SI1(i,j) = mean( mean( I1(i*4-3,j*4-3) ) );
%     end
% end
% S = uint8(SI1);
% imwrite(S,'E:\img1.png','png');
% SI2 = zeros(256, 256);
% for i=1:256
%     for j=1:256
%    %     simg(i,j) = mean( mean( img(i*4-3:i*4,j*4-3:j*4) ) );
%          SI2(i,j) = mean( mean( I2(i*4-3,j*4-3) ) );
%     end
% end
% S = uint8(SI2);
% imwrite(S,'E:\img2.png','png');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 % tic;
% Get the Key Points
  Options.init_sample =1;
  Options.tresh=20;
  Options.octaves=5;
  Options.verbose = false;
  Options.upright = false;
  Options.harrisTH = 0.005;
  Options.Edge = 20;            %由于SURF特征计算使用20*20邻域，这里不考虑边界20像素的点
 % errTH=0.1;
 %  Ipts1=HSurf(I1,Options);
 %  Ipts2=HSurf(I2,Options);
  Ipts1=OpenSurf(I1,Options);
 % Options.tresh=50;
  Ipts2=OpenSurf(I2,Options);
 
 
         errTH=0.05;
         
  %  Ipts1=Harris_surfPP(I1,Options);
  % Ipts2=Harris_surfPP(I2,Options);
    tic;
  coorIpt1 = zeros(length(Ipts1),4);
   for i = 1:length(Ipts1),
      coorIpt1(i,:) = [Ipts1(i).x,Ipts1(i).y,Ipts1(i).scale,Ipts1(i).orientation];
   end
   coorIpt2 = zeros(length(Ipts2),4);
   for i = 1:length(Ipts2),
      coorIpt2(i,:) = [Ipts2(i).x,Ipts2(i).y,Ipts2(i).scale,Ipts2(i).orientation];
  end
% Put the landmark descriptors in a matrix
  D1 = reshape([Ipts1.descriptor],64,[]); 
  D2 = reshape([Ipts2.descriptor],64,[]); 
% Find the best matches
  err=zeros(1,length(Ipts1));
  cor1=1:length(Ipts1); 
  cor2=zeros(1,length(Ipts1));
  for i=1:length(Ipts1),
      distance=sum((D2-repmat(D1(:,i),[1 length(Ipts2)])).^2,1);
      [err(i),cor2(i)]=min(distance);
  end
% Sort matches on vector distance
  [err, ind]=sort(err); 
  cor1=cor1(ind); 
  cor2=cor2(ind);
% Show both images
  I = zeros([size(I1,1) size(I1,2)*2 size(I1,3)]);
  I(:,1:size(I1,2),:)=I1; I(:,size(I1,2)+1:size(I1,2)+size(I2,2),:)=I2;
  figure, imshow(I/255); hold on;
  nIpt1 = length(Ipts1)
  nIpt2 = length(Ipts2)
  Nmatched =  sum(err<errTH)
   matched = zeros(Nmatched,4);
  toc;
% Show the best matches
  for i=1:Nmatched,
      c=rand(1,3);  %用于画不同颜色图 [ 0.1933,    0.0263,    0.4688];
      plot([Ipts1(cor1(i)).x Ipts2(cor2(i)).x+size(I1,2)],[Ipts1(cor1(i)).y Ipts2(cor2(i)).y],'-','Color',c)
      plot([Ipts1(cor1(i)).x Ipts2(cor2(i)).x+size(I1,2)],[Ipts1(cor1(i)).y Ipts2(cor2(i)).y],'o','Color',c,'MarkerSize',4*Ipts1(cor1(i)).scale)
      text([Ipts1(cor1(i)).x Ipts2(cor2(i)).x+size(I1,2)],[Ipts1(cor1(i)).y Ipts2(cor2(i)).y],[num2str(i)],'color','b');
       matched(i,:) = [Ipts1(cor1(i)).x, Ipts1(cor1(i)).y, Ipts2(cor2(i)).x,Ipts2(cor2(i)).y]; 
  end
  %%%%%%%%%%%%%%%%%%%%%%%%%1024图像下采样以后显示要放大4倍%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    for i=1:30,
%       c=rand(1,3);  %用于画不同颜色图 [ 0.1933,    0.0263,    0.4688];
%       plot([Ipts1(cor1(i)).x*4 Ipts2(cor2(i)).x*4+size(I1,2)],[Ipts1(cor1(i)).y*4 Ipts2(cor2(i)).y*4],'-','Color',c)
%       plot([Ipts1(cor1(i)).x*4 Ipts2(cor2(i)).x*4+size(I1,2)],[Ipts1(cor1(i)).y*4 Ipts2(cor2(i)).y*4],'o','Color',c,'MarkerSize',4*Ipts1(cor1(i)).scale)
%       text([Ipts1(cor1(i)).x*4 Ipts2(cor2(i)).x*4+size(I1,2)],[Ipts1(cor1(i)).y*4 Ipts2(cor2(i)).y*4],[num2str(i)],'color','b');
%        matched(i,:) = [Ipts1(cor1(i)).x, Ipts1(cor1(i)).y, Ipts2(cor2(i)).x,Ipts2(cor2(i)).y]; 
%   end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    for i=1:30,
%       c=rand(1,3);  %用于画不同颜色图 [ 0.1933,    0.0263,    0.4688];
%       plot([Ipts1(cor1(i)).x./Ipts1(cor1(i)).scale Ipts2(cor2(i)).x./Ipts2(cor2(i)).scale+size(I1,2)],[Ipts1(cor1(i)).y./Ipts1(cor1(i)).scale Ipts2(cor2(i)).y./Ipts2(cor2(i)).scale],'-','Color',c)
%       plot([Ipts1(cor1(i)).x./Ipts1(cor1(i)).scale Ipts2(cor2(i)).x./Ipts2(cor2(i)).scale+size(I1,2)],[Ipts1(cor1(i)).y./Ipts1(cor1(i)).scale Ipts2(cor2(i)).y./Ipts2(cor2(i)).scale],'o','Color',c,'MarkerSize',4*Ipts1(cor1(i)).scale)
%       text([Ipts1(cor1(i)).x./Ipts1(cor1(i)).scale Ipts2(cor2(i)).x./Ipts2(cor2(i)).scale+size(I1,2)],[Ipts1(cor1(i)).y./Ipts1(cor1(i)).scale Ipts2(cor2(i)).y./Ipts2(cor2(i)).scale],[num2str(i)],'color','b');
%   end
%  