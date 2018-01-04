function pic=IntegralImage_IntegralImage(I)
% This function IntegralImage_IntegralImage will ..
%
% J = IntegralImage_IntegralImage( I )
%  
%  inputs,
%    I : An 2D image color or greyscale
%  
%  outputs,
%    J : The integral image 
%  
% Function is written by D.Kroon University of Twente (July 2010)

% Convert Image to double
% switch(class(I));
%     case 'uint8'
%         I=double(I)/255;
%     case 'uint16'
%         I=double(I)/65535;
%     case 'int8'
%         I=(double(I)+128)/255;
%     case 'int16'
%         I=(double(I)+32768)/65535;
%     otherwise
%         I=double(I);
% end

% Convert Image to greyscale
%写矩阵I到文件
[m,n,k]=size(I); 

if(size(I,3)==3)        %RGB(.jpg格式)
    fid=fopen('d:\a1.txt','w+');
    for i=1:m
        for j=1:n
            fprintf(fid,'%d  ',I(i,j,1));
        end
        fprintf(fid,'\n');
    end
    fclose(fid);
    fid=fopen('d:\a2.txt','w+');
    for i=1:m
        for j=1:n
            fprintf(fid,'%d  ',I(i,j,2));
        end
        fprintf(fid,'\n');
    end
    fclose(fid);

    fid=fopen('d:\a3.txt','w+');
    for i=1:m
        for j=1:n
            fprintf(fid,'%d  ',I(i,j,3));
        end
        fprintf(fid,'\n');
    end
    fclose(fid);
	cR = .2990; cG = .5870; cB = .1140;
% 	K=I(:,:,1)*cR+I(:,:,2)*cG+I(:,:,3)*cB;
    K=double(I(:,:,1))*cR+double(I(:,:,2))*cG+double(I(:,:,3))*cB;
else                        %灰度图
    K = double(I);
end

%保存图像数据
fid=fopen('d:\a.txt','w+');
for i=1:m
    for j=1:n
        fprintf(fid,'%d  ',I(i,j));
    end
    fprintf(fid,'\n');
end
fclose(fid);

 I=round(K);


% Make the integral image
pic = cumsum(cumsum(I,1),2);
