function ipts=HSurf(img,Options)

functionname='HSurf.m';
functiondir=which(functionname);
functiondir=functiondir(1:end-length(functionname));
addpath([functiondir '/SubFunctions'])
       
% Process inputs
defaultoptions=struct('tresh',0.0002,'octaves',5,'init_sample',8,'upright',false,'extended',false,'verbose',false);
if(~exist('Options','var')), 
    Options=defaultoptions; 
else
    tags = fieldnames(defaultoptions);
    for i=1:length(tags)
         if(~isfield(Options,tags{i})),  Options.(tags{i})=defaultoptions.(tags{i}); end
    end
    if(length(tags)~=length(fieldnames(Options))), 
        warning('register_volumes:unknownoption','unknown options found');
    end
end

% Create Integral Image RGB图转为灰度图并计算积分图像
iimg=IntegralImage_IntegralImage(img);
iimg = iimg.*255.29;
[m,n]=size(iimg); 
fid=fopen('d:\iimg.txt','w+');
for i=1:m
    for j=1:n
        fprintf(fid,'%d  ',int32( iimg(i,j) ) );
    end
    fprintf(fid,'\n');
end
fclose(fid);
% Extract the interest points
FastHessianData.thresh = Options.tresh;
FastHessianData.octaves = Options.octaves;
FastHessianData.init_sample = Options.init_sample;
FastHessianData.img = iimg;
ipts = FastHessian_getIpoints(FastHessianData,Options.verbose);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if length(size(img))==3 
    h_img=rgb2gray(img);
else
    h_img = img;
end
[m,n]=size(h_img); 
% fid=fopen('d:\a.txt','w+');
% for i=1:m
%     for j=1:n
%         fprintf(fid,'%d  ',img(i,j));
%     end
%     fprintf(fid,'\n');
% end
% 
% fclose(fid);
 tic;

tmp=zeros(m+2,n+2); 
tmp(2:m+1,2:n+1)=h_img; 

Ix=zeros(m+2,n+2); 
Iy=zeros(m+2,n+2); 

Ix(:,2:n+1)=tmp(:,3:n+2)-tmp(:,1:n);  
Iy(2:m+1,:)=tmp(3:m+2,:)-tmp(1:m,:); 
% figure(1);imshow(Ix); %画Ix图形
% figure(2);imshow(Iy);
Ix2=Ix(2:m+1,2:n+1).^2; 
Iy2=Iy(2:m+1,2:n+1).^2; 
Ixy=Ix(2:m+1,2:n+1).*Iy(2:m+1,2:n+1);  

h=fspecial('gaussian',[7 7],2);  

Ix2=filter2(h,Ix2); 
Iy2=filter2(h,Iy2); 
Ixy=filter2(h,Ixy);  

R=zeros(m,n); 

for i=1:m 
    for j=1:n
        M=[Ix2(i,j) Ixy(i,j);Ixy(i,j) Iy2(i,j)]; 
        R(i,j)=det(M)-0.06*(trace(M))^2; 
    end
end

Rmax=max(max(R))+1; 
loc=[];
tmp(2:m+1,2:n+1)=R; 

for i=2:m+1 
    for j=2:n+1 
        if tmp(i,j)>0.02*Rmax
            sq=tmp(i-1:i+1,j-1:j+1); 
            sq=reshape(sq,1,9); 
            sq=[sq(1:4),sq(6:9)]; 
            if tmp(i,j)>sq 
                loc=[loc;[j-1,i-1]];
            end
        end
    end
end 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%取Harris角点和Surf特征点的交集
%Surf特征点中的x,y是对应原图像大小的像素坐标，不随尺度变化
%若Harris检测出的角点
%%%%%%%%%%%%%%%%
th_hsurf = 23.5;
nipt = 0;
for i = 1:length(ipts)
    keep_flag = 0;
    for j = 1:length(loc)   
        if( ((ipts(i).x-loc(j,1))^2 + (ipts(i).y-loc(j,2))^2) < th_hsurf )
           keep_flag = 1;
           break;
        end            
    end
    if(keep_flag == 1)   %存在与之相近的角点，保留该特征点
       nipt = nipt+1;
       hsipt(nipt) = ipts(i);
    end
end
% Describe the interest points
if(~isempty(hsipt))
    ipts = SurfDescriptor_DecribeInterestPoints(hsipt,Options.upright, Options.extended, iimg, Options.verbose);
end

