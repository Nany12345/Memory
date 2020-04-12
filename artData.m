% for runInd = 1:21
%     DataSet = zeros(40000,2);
%     for genInd = 1:200
%         if genInd == 200
%             rawData = [normrnd(0,0.1,[400,1]),(rand(400,1)-0.5)*sqrt(2)];
%             rawData(rawData(:,1)<0,:) = [];
%             if size(rawData,1)>200
%                 rawData = rawData(1:200,:);
%             else
%                 s = size(rawData,1);
%                 for i = s+1:200
%                     while true
%                         newData = [normrnd(0,0.1),(rand-0.5)*sqrt(2)];
%                         if newData(1) > 0
%                             rawData = [rawData;newData];
%                             break;
%                         end
%                     end
%                 end
%             end
%             rotate = [cos(theta),-1*sin(theta);sin(theta),cos(theta)];
%             rawData = rawData*rotate;
%             Data = rawData+(200-genInd+0.5);
%             DataSet((genInd-1)*200+1:genInd*200,:) = Data;
%         else
%             rawData = [normrnd(0,0.1,[200,1]),(rand(200,1)-0.5)*sqrt(2)];
%             rotate = [cos(theta),-1*sin(theta);sin(theta),cos(theta)];
%             rawData = rawData*rotate;
%             Data = rawData+(200-genInd+0.5);
%             DataSet((genInd-1)*200+1:genInd*200,:) = Data;
%         end
%     end
%     outFile = ['./NDTree_Linux/DataArt/Art2_200000_',num2str(runInd),'_10000.txt'];
%     dlmwrite(outFile,DataSet,'delimiter',' ');
% end

DataSet = zeros(40000,2);
f = figure;
f.Position = [200,200,600,600];
c = {'r','g','b','k'};
theta = -pi/4;
count = 1;
sca = cell(1,4);
hold on;
plot(1:5,0:4,'--k');
plot(0:4,1:5,'--k');
plot(0:5,0:5,'--k');
hold on;
for genInd = 1:200
    if genInd == 200
        rawData = [normrnd(0,0.1,[400,1]),(rand(400,1)-0.5)*sqrt(2)];
        rawData(rawData(:,1)<0,:) = [];
        if size(rawData,1)>200
            rawData = rawData(1:200,:);
        else
            s = size(rawData,1);
            for i = s+1:200
                while true
                    newData = [normrnd(0,0.1),(rand-0.5)*sqrt(2)];
                    if newData(1) > 0
                        rawData = [rawData;newData];
                        break;
                    end
                end
            end
        end
        rotate = [cos(theta),-1*sin(theta);sin(theta),cos(theta)];
        rawData = rawData*rotate;
        Data = rawData+(200-genInd+0.5);
        DataSet((genInd-1)*200+1:genInd*200,:) = Data;
    else
        rawData = [normrnd(0,0.1,[200,1]),(rand(200,1)-0.5)*sqrt(2)];
        rotate = [cos(theta),-1*sin(theta);sin(theta),cos(theta)];
        rawData = rawData*rotate;
        Data = rawData+(200-genInd+0.5);
        DataSet((genInd-1)*200+1:genInd*200,:) = Data;
    end
    if genInd >= 199 || genInd == 197 || genInd == 196
        sca{count} = scatter(Data(:,1),Data(:,2),15,'fill',c{count});
        count = count+1;
    end
end
l = legend([sca{1} sca{2} sca{3} sca{4}],{'g=1','g=2','g=199','g=200'});
l.Location = 'northwest';
legend('boxoff');
axis([0,5,0,5]);
grid on;
ax = gca;
ax.FontSize = 20;
ax.XTick = [0:2,4,5];
ax.YTick = [0:2,4,5];
ax.XTickLabel = {'0','1','2','199','200'};
ax.YTickLabel = {'0','1','2','199','200'};
ax.XLabel.Interpreter = 'latex';
ax.XLabel.String = '$f_1$';
ax.YLabel.Interpreter = 'latex';
ax.YLabel.String = '$f_2$';
saveas(f,'./art_dis.emf');