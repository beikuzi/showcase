% 创建图形窗口
figure(1);
grid on; 
title('traffic');

xlabel('X');
ylabel('Y');
zlabel('Z');

global x y z
x = [];
y = [];
z = [];
addP(0,0,0);
addP(10,10,10);

addP(4,-1,3);
addP(4,-1,5);
addP(-1,2,5);
addP(-1,2,7);
addP(4,-1,8);
addP(7,-1,8);
addP(-1,5,7);

scatter3(x, y, z, 'filled', 'Marker', 'o', 'MarkerEdgeColor', 'b', 'MarkerFaceColor', 'r');

linkP(4,5,'r');
linkP(3,4,'g');
linkP(5,6,'g');
linkP(4,7,'g');
linkP(7,8,'b');
linkP(6,9,'b');

function linkP(i,j,color)
    global x y z
    line([x(i),x(j)],[y(i),y(j)],[z(i),z(j)],'color',color);
    
    if y(i)==10
        text(x(i)-1.5,y(i),z(i)+1,"(站点:"+num2str(z(i))+",汽车:"+num2str(x(i))+")");
    else 
        text(x(i),y(i),z(i)+0.5,"(站点:"+num2str(z(i))+",地铁口"+num2str(y(i))+")");
    end
    
    i=j;
    if y(i)==10
        text(x(i)-1.5,y(i),z(i)+1,"(站点:"+num2str(z(i))+",汽车:"+num2str(x(i))+")");
    else 
        text(x(i),y(i),z(i)+0.5,"(站点:"+num2str(z(i))+",地铁口"+num2str(y(i))+")");
    end
end

function addP(i,j,k)
    if i==-1,i=10;end
    if j==-1,j=10;end
    global x y z
    x=[x i];
    y=[y j];
    z=[z k];
end