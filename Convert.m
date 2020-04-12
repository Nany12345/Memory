cd(fileparts(mfilename('fullpath')));
addpath(genpath(cd));
alg = {'NT','NTR5','NTR10','NTR20','NTR50','NTR100','NTR'};
obj = [2,3,5,10];
inDir = './NDTree_Linux';
for C = [10,15,20,30]
    outDir = ['./Data_C=',num2str(C)];
    if exist(outDir,'dir')==0
       mkdir(outDir);
    end
    for objInd = 1:4
        for algInd = 1:2
            for proInd = 1:4
                decNum = obj(objInd)+9;
                if proInd == 1
                    decNum = 10;
                    proName = 'MPDMP';
                    proNameO = 'MPDMP';
                elseif proInd == 2
                    decNum = 250;
                    proName = 'MOKP';
                    proNameO = 'MOKP';
                elseif proInd == 3
                    decNum = 30;
                    proName = 'PS1';
                    proNameO = 'PS1';
                else
                    decNum = 30;
                    proName = 'PS2';
                    proNameO = 'PS2';
                end
                inFileName = [inDir,'/RunTime_Mem_C=',num2str(C),'/RunTime_',proName,'_M',num2str(obj(objInd)),'.txt'];
                disp(inFileName);
                Data = dlmread(inFileName);
                runTime = Data(:,1:3:19);
                UEASize = Data(:,2:3:20);
                CompNum = Data(:,3:3:21);
                for algInd = 1:7
                    outAlgDir = [outDir,'/',alg{algInd}];
                    if exist(outAlgDir,'dir')==0
                        mkdir(outAlgDir);
                    end
                    for runInd = 1:21
                        outFileName = [outAlgDir,'/',alg{algInd},'_',proNameO, ...
                            '_M',num2str(obj(objInd)),'_D',num2str(decNum), ...
                            '_',num2str(runInd),'.mat'];
                        runtime = runTime(runInd,algInd);
                        ueasize = UEASize(runInd,algInd);
                        compnum = CompNum(runInd,algInd);
                        metric = struct('runtime',runtime,'ueasize',ueasize,'compnum',compnum);
                        save(outFileName,'metric');
                    end
                end
            end
        end
    end
end
