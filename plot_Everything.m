addpath(genpath(pwd));

%% Run Time Art
% obj = [2,3,5,10];
% % axisLabel = {'NT','NTR5','NTR10','NTR20','NTR50','NTR100','NTR200'};
% axisLabel = {'ND-Tree','ND-Tree-Reverse'};
% proName = {'MOKP','MPDMP','PS','DTLZ','MinusDTLZ','WFG','MinusWFG','Art'};
% proNameSim = {'MOKP','MPDMP','PS','DTLZ','MDTLZ','WFG','MWFG','Art'};
% algName = 'NSGA-II';
% for proSer = 8
%     proNum = 1;
%     if proSer == 3
%         proNum = 2;
%     elseif proSer == 4 || proSer == 6 || proSer == 7
%         proNum = 9;
%     elseif proSer == 5
%         proNum = 4;
%     end
%     outDir = ['./DataFigure/RunTime_',proNameSim{proSer}];
%     if exist(outDir,'dir')==0
%         mkdir(outDir);
%     end
%     RunTime = zeros(21,4,proNum);
%     CompNum = zeros(21,4,proNum);
%     for objInd = 1
%         for proInd = 1:proNum
%             if proSer < 3 || proSer == 8
%                 proNameE = [proNameSim{proSer},'_M',num2str(obj(objInd))];
%                 proNameT = [proName{proSer},'\_M',num2str(obj(objInd))];
%                 pproName = proName{proSer};
%             else
%                 proNameE = [proNameSim{proSer},num2str(proInd),'_M',num2str(obj(objInd))];
%                 proNameT = [proName{proSer},num2str(proInd),'\_M',num2str(obj(objInd))];
%                 pproName = [proName{proSer},num2str(proInd)];
%             end
%             Data = dlmread(['./RunTime_Mem_C=20/RunTime_',proNameE,'.txt']);            
%             runtime = Data(:,[1,19]); 
%             f = figure;
%             f.Position = [200,200,600,600];
%             boxplot(runtime/1000);
%             ax = gca;
%             ax.YScale = 'linear';
%             ax.YLim = [0,15];
%             ax.YTick = 0:3:15;
%             ax.YLabel.String = 'Run Time in Milliseconds';
%             grid on;
%             ax.FontSize = 20;
%             ax.XTickLabel = {'ND-Tree','ND-Tree-Reverse'};
%             saveas(f,['./Figure_RunTime/RunTime_',proNameE,'.emf']);
%             close all;
%         end
%     end
% end

%% Relative RunTime
% obj = [2,3,5,10];
% % axisLabel = {'NT','NTR5','NTR10','NTR20','NTR50','NTR100','NTR200'};
% axisLabel = {'ND-Tree','ND-Tree-Reverse'};
% proName = {'MOKP','MPDMP','PS','DTLZ','MinusDTLZ','WFG','MinusWFG','Art'};
% proNameSim = {'MOKP','MPDMP','PS','DTLZ','MDTLZ','WFG','MWFG','Art'};
% algName = 'NSGA-II';
% num_node = [10,15,20,25];
% for proSer = 1:3
%     proNum = 1;
%     if proSer == 3
%         proNum = 2;
%     elseif proSer == 4 || proSer == 6 || proSer == 7
%         proNum = 9;
%     elseif proSer == 5
%         proNum = 4;
%     end
%     outDir = ['./DataFigure/RunTime_',proNameSim{proSer}];
%     if exist(outDir,'dir')==0
%         mkdir(outDir);
%     end
%     for proInd = 1:proNum
%         RunTime = zeros(4,4);
%         for objInd = 1:4
%             for num_ind = 1:4
%                 C = num_node(num_ind);
%                 if proSer < 3 || proSer == 8
%                     proNameE = [proNameSim{proSer},'_M',num2str(obj(objInd))];
%                     proNameT = [proName{proSer},'\_M',num2str(obj(objInd))];
%                     pproName = proName{proSer};
%                 else
%                     proNameE = [proNameSim{proSer},num2str(proInd),'_M',num2str(obj(objInd))];
%                     proNameT = [proName{proSer},num2str(proInd),'\_M',num2str(obj(objInd))];
%                     pproName = [proName{proSer},num2str(proInd)];
%                 end
%                 Data = dlmread(['./RunTime_Mem_C=',num2str(C),'/RunTime_',proNameE,'.txt']);            
%                 runtime = Data(:,[1,19]); 
%                 RunTime(objInd,num_ind) = mean(runtime(:,2)./runtime(:,1));
%             end
%         end
%         if proSer < 3 || proSer == 8
%             proNameE = proNameSim{proSer};
%         else
%             proNameE = [proNameSim{proSer},num2str(proInd)];
%         end
%         f = figure;
%         f.Position = [200,200,700,600];
% 
%         hold on;
%         plot(1:4,RunTime(1,:),'-rp','LineWidth',3,'MarkerSize',13);
%         plot(1:4,RunTime(2,:),'-.g*','LineWidth',3,'MarkerSize',13);
%         plot(1:4,RunTime(3,:),'--bo','LineWidth',3,'MarkerSize',13);
%         plot(1:4,RunTime(4,:),':ks','LineWidth',3,'MarkerSize',13);
% 
%         legend({'M=2','M=3','M=5','M=10'}, ...
%                 'Box',   'off',...
%                 'Location', 'northoutside',...
%                 'NumColumns', 4);
%         ax = gca;
%         ax.XLim = [0.5,4.5];
%         ax.YLim = [0.3,1];
%         ax.YScale = 'linear';
%         ax.YLabel.String = 'Relative RunTime';
%         grid on;
%         ax.FontSize = 20;
%         ax.XTick = 1:4;
%         ax.YTick = 0.3:0.1:1;
%         ax.XTickLabel = {'C=10','C=15','C=20','C=25'};
%         saveas(f,['./Figure_RunTime/RunTime_',proNameE,'.emf']);
%         close all;
%     end
% end

%% Run Time
% obj = [2,3,5,10];
% % axisLabel = {'NT','NTR5','NTR10','NTR20','NTR50','NTR100','NTR200'};
% axisLabel = {'ND-Tree','ND-Tree-Reverse'};
% proName = {'MOKP','MPDMP','PS','DTLZ','MinusDTLZ','WFG','MinusWFG','Art'};
% proNameSim = {'MOKP','MPDMP','PS','DTLZ','MDTLZ','WFG','MWFG','Art'};
% algName = 'NSGA-II';
% num_node = [10,15,20,25];
% for proSer = 1:3
%     proNum = 1;
%     if proSer == 3
%         proNum = 2;
%     elseif proSer == 4 || proSer == 6 || proSer == 7
%         proNum = 9;
%     elseif proSer == 5
%         proNum = 4;
%     end
%     outDir = ['./DataFigure/RunTime_',proNameSim{proSer}];
%     if exist(outDir,'dir')==0
%         mkdir(outDir);
%     end
%     RunTime = zeros(2,4,4,proNum);
%     for objInd = 1:4
%         for proInd = 1:proNum
%             for num_ind = 1:4
%                 C = num_node(num_ind);
%                 if proSer < 3 || proSer == 8
%                     proNameE = [proNameSim{proSer},'_M',num2str(obj(objInd))];
%                     proNameT = [proName{proSer},'\_M',num2str(obj(objInd))];
%                     pproName = proName{proSer};
%                 else
%                     proNameE = [proNameSim{proSer},num2str(proInd),'_M',num2str(obj(objInd))];
%                     proNameT = [proName{proSer},num2str(proInd),'\_M',num2str(obj(objInd))];
%                     pproName = [proName{proSer},num2str(proInd)];
%                 end
%                 Data = dlmread(['./RunTime_Mem_C=',num2str(C),'/RunTime_',proNameE,'.txt']);            
%                 runtime = Data(:,[1,19]); 
%                 RunTime(1,num_ind,objInd,proInd) = mean(runtime(:,1)); 
%                 RunTime(2,num_ind,objInd,proInd) = mean(runtime(:,2));
%             end
%         end
%     end
%     for proInd = 1:proNum
%         if proSer < 3 || proSer == 8
%             proNameE = proNameSim{proSer};
%         else
%             proNameE = [proNameSim{proSer},num2str(proInd)];
%         end
%         f = figure;
%         f.Position = [200,200,700,600];
%         color = {'r','g','b','k'};
%         for objInd = 1:4
%             data = RunTime(:,:,objInd,proInd);
%             hold on;
%             plot(1:4,data(1,:)/1000,['-',color{objInd},'*'],'LineWidth',3,'MarkerSize',13);
%             plot(1:4,data(2,:)/1000,['--',color{objInd},'p'],'LineWidth',3,'MarkerSize',13);
%         end
%         legend({'NT-M2','NTR-M2','NT-M3','NTR-M3','NT-M5','NTR-M5','NT-M10','NTR-M10',}, ...
%                 'Box',   'off',...
%                 'Location', 'northoutside',...
%                 'NumColumns', 4);
%         ax = gca;
%         ax.YScale = 'log';
%         ax.YLabel.String = 'RunTime in Millionseconds';
%         grid on;
%         ax.FontSize = 20;
%         ax.XTick = 1:4;
%         ax.XLim = [0.5,4.5];
%         ax.XTickLabel = {'C=10','C=15','C=20','C=25'};
%         saveas(f,['./Figure_RunTime/RunTime_',proNameE,'_two.emf']);
%         close all;
%     end
% end

%% Relative RunTime Mem M
% obj = [2,3,5,10];
% axisLabel = {'NTR5','NTR10','NTR20','NTR50','NTR100','NTR200'};
% proName = {'MOKP','MPDMP','PS','DTLZ','MinusDTLZ','WFG','MinusWFG','Art'};
% proNameSim = {'MOKP','MPDMP','PS','DTLZ','MDTLZ','WFG','MWFG','Art'};
% algName = 'NSGA-II';
% num_node = [10,15,20,25];
% for proSer = 1:3
%     proNum = 1;
%     if proSer == 3
%         proNum = 2;
%     elseif proSer == 4 || proSer == 6 || proSer == 7
%         proNum = 9;
%     elseif proSer == 5
%         proNum = 4;
%     end
%     outDir = ['./DataFigure/RunTime_',proNameSim{proSer}];
%     if exist(outDir,'dir')==0
%         mkdir(outDir);
%     end
%     for C = [10,15,20,25]
%         for proInd = 1:proNum
%             RunTime = zeros(4,6);
%             for objInd = 1:4
%                 if proSer < 3 || proSer == 8
%                     proNameE = [proNameSim{proSer},'_M',num2str(obj(objInd))];
%                     proNameT = [proName{proSer},'\_M',num2str(obj(objInd))];
%                     pproName = proName{proSer};
%                 else
%                     proNameE = [proNameSim{proSer},num2str(proInd),'_M',num2str(obj(objInd))];
%                     proNameT = [proName{proSer},num2str(proInd),'\_M',num2str(obj(objInd))];
%                     pproName = [proName{proSer},num2str(proInd)];
%                 end
%                 Data = dlmread(['./RunTime_Mem_C=',num2str(C),'/RunTime_',proNameE,'.txt']);            
%                 runtime = Data(:,1:3:19); 
%                 RunTime(objInd,:) = mean(runtime(:,2:end)./runtime(:,1));
%             end
%             if proSer < 3 || proSer == 8
%                 proNameE = proNameSim{proSer};
%             else
%                 proNameE = [proNameSim{proSer},num2str(proInd)];
%             end
%             f = figure;
%             f.Position = [200,200,1000,600];
% 
%             hold on;
%             plot(1:6,RunTime(1,:),'-rp','LineWidth',3,'MarkerSize',13);
%             plot(1:6,RunTime(2,:),'-.g*','LineWidth',3,'MarkerSize',13);
%             plot(1:6,RunTime(3,:),'--bo','LineWidth',3,'MarkerSize',13);
%             plot(1:6,RunTime(4,:),':ks','LineWidth',3,'MarkerSize',13);
% 
%             legend({'M=2','M=3','M=5','M=10'}, ...
%                     'Box',   'off',...
%                     'Location', 'northoutside',...
%                     'NumColumns', 4);
%             ax = gca;
%             ax.XLim = [0.5,6.5];
%             ax.YLim = [0.2,1.2];
%             ax.YScale = 'linear';
%             ax.YLabel.String = 'Relative RunTime';
%             grid on;
%             ax.FontSize = 20;
%             ax.XTick = 1:6;
%             ax.YTick = 0.2:0.2:1.2;
%             ax.XTickLabel = axisLabel;
%             saveas(f,['./Figure_RunTime_Mem/RunTime_',proNameE,'C=',num2str(C),'_Mem.emf']);
%             close all;
%         end
%     end
% end

%% Relative RunTime Mem C
obj = [2,3,5,10];
axisLabel = {'NTR5','NTR10','NTR20','NTR50','NTR100','NTR200'};
proName = {'MOKP','MPDMP','PS','DTLZ','MinusDTLZ','WFG','MinusWFG','Art'};
proNameSim = {'MOKP','MPDMP','PS','DTLZ','MDTLZ','WFG','MWFG','Art'};
algName = 'NSGA-II';
num_node = [10,15,20,25];
for proSer = 1:3
    proNum = 1;
    if proSer == 3
        proNum = 2;
    elseif proSer == 4 || proSer == 6 || proSer == 7
        proNum = 9;
    elseif proSer == 5
        proNum = 4;
    end
    outDir = ['./DataFigure/RunTime_',proNameSim{proSer}];
    if exist(outDir,'dir')==0
        mkdir(outDir);
    end
    for proInd = 1:proNum
        for objInd = 1:4
            RunTime = zeros(4,6);
            C_Array = [10,15,20,25];
            for C_Ind = 1:4
                C = C_Array(C_Ind);
                if proSer < 3 || proSer == 8
                    proNameE = [proNameSim{proSer},'_M',num2str(obj(objInd))];
                    proNameT = [proName{proSer},'\_M',num2str(obj(objInd))];
                    pproName = proName{proSer};
                else
                    proNameE = [proNameSim{proSer},num2str(proInd),'_M',num2str(obj(objInd))];
                    proNameT = [proName{proSer},num2str(proInd),'\_M',num2str(obj(objInd))];
                    pproName = [proName{proSer},num2str(proInd)];
                end
                Data = dlmread(['./RunTime_Mem_C=',num2str(C),'/RunTime_',proNameE,'.txt']);            
                runtime = Data(:,1:3:19); 
                RunTime(C_Ind,:) = mean(runtime(:,2:end)./runtime(:,1));
            end
            if proSer < 3 || proSer == 8
                proNameE = proNameSim{proSer};
            else
                proNameE = [proNameSim{proSer},num2str(proInd)];
            end
            f = figure;
            f.Position = [200,200,1000,600];

            hold on;
            plot(1:6,RunTime(1,:),'-rp','LineWidth',3,'MarkerSize',13);
            plot(1:6,RunTime(2,:),'-.g*','LineWidth',3,'MarkerSize',13);
            plot(1:6,RunTime(3,:),'--bo','LineWidth',3,'MarkerSize',13);
            plot(1:6,RunTime(4,:),':ks','LineWidth',3,'MarkerSize',13);

            legend({'C=10','C=15','C=20','C=25'}, ...
                    'Box',   'off',...
                    'Location', 'northoutside',...
                    'NumColumns', 4);
            ax = gca;
            ax.XLim = [0.5,6.5];
            ax.YLim = [0.2,1.2];
            ax.YScale = 'linear';
            ax.YLabel.String = 'Relative RunTime';
            grid on;
            ax.FontSize = 20;
            ax.XTick = 1:6;
            ax.YTick = 0.2:0.2:1.2;
            ax.XTickLabel = axisLabel;
            saveas(f,['./Figure_RunTime_Mem/RunTime_',proNameE,'M=',num2str(obj(objInd)),'_Mem.emf']);
            close all;
        end
    end
end