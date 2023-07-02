rng(0)

obsInfo = rlNumericSpec([4 1]);
obsInfo.Name = "observations";

actInfo = rlFiniteSetSpec([0,1]);
actInfo.Name = "action";

env = rlSimulinkEnv("study","study/RL Agent",...
    obsInfo,actInfo);

criticNet = [
    featureInputLayer(obsInfo.Dimension(1))
    fullyConnectedLayer(20)
    reluLayer
    fullyConnectedLayer(10)
    sigmoidLayer
    fullyConnectedLayer(length(actInfo.Elements))
];

%深度网络
net = dlnetwork(criticNet);
summary(net);
%plot(net);
critic = rlVectorQValueFunction(net,obsInfo,actInfo);
getValue(critic,{rand(obsInfo.Dimension)});

%设置学习率
criticOpts = rlOptimizerOptions(LearnRate=0.01);
%设置agent参数
Ts = 0.02;
Tf = 30;
agentOptions = rlDQNAgentOptions(...
    SampleTime=Ts,...
    CriticOptimizerOptions=criticOpts);
%设置agent
myAgent=rlDQNAgent(critic,agentOptions);
%未知
getAction(myAgent,{rand(obsInfo.Dimension)});

%设置训练参数
trainOpts = rlTrainingOptions;
trainOpts.MaxStepsPerEpisode = 200;
trainOpts.MaxEpisodes= 50;

learning=false;
if learning
    trainingStats = train(myAgent,env,trainOpts);%强化学习
    save('train.mat',"myAgent");
else
    load('train.mat',"myAgent");
end


%进行评估
sim(myAgent,env)