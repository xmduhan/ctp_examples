// 标准C库文件
#include <stdio.h>
#include <string.h>
#include <cstdlib>

// CTP头文件
#include <ThostFtdcTraderApi.h>
#include <ThostFtdcMdApi.h>
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>

// 线程控制相关
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// 定时器相关
#include <signal.h>
#include <sys/time.h>


// 字符串编码转化
#include <code_convert.h>




// 登录请求结构体
CThostFtdcReqUserLoginField userLoginField;
// 用户请求结构体
CThostFtdcUserLogoutField userLogoutField;
// 线程同步标志
sem_t sem;
// requestID
int requestID = 0;


class CTraderHandler : public CThostFtdcTraderSpi {

public:

    CTraderHandler() {
        printf("CTraderHandler():被执行...\n");
    }

    // 允许登录事件
    virtual void OnFrontConnected() {
        static int i = 0;
        printf("OnFrontConnected():被执行...\n");
        // 在登出后系统会重新调用OnFrontConnected，这里简单判断并忽略第1次之后的所有调用。
        if (i++==0) {
            sem_post(&sem);
        }
    }

    // 登录结果响应
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                                CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
        printf("OnRspUserLogin():被执行...\n");
        if (pRspInfo->ErrorID == 0) {
            printf("登录成功!\n");
            sem_post(&sem);
        } else {
            printf("登录失败!\n");
        }
    }

    // 登出结果响应
    virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                                 CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
        printf("OnReqUserLogout():被执行...\n");
        if (pRspInfo->ErrorID == 0) {
            printf("登出成功!\n");
            sem_post(&sem);
        } else {
            printf("登出失败!\n");
        }
    }

    // 错误信息响应方法
    virtual void OnRspError
    (CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
        printf("OnRspError():被执行...\n");
    }

    ///请求查询投资者持仓响应
    virtual void OnRspQryInvestorPosition(
        CThostFtdcInvestorPositionField * pInvestorPosition,
        CThostFtdcRspInfoField * pRspInfo,
        int nRequestID,
        bool bIsLast
    ) {
        printf("OnRspQryInvestorPosition():被执行...\n");

        // 进程返回结果检查
        if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
            // typedef int TThostFtdcErrorIDType;
            // typedef char TThostFtdcErrorMsgType[81];
            char ErrorMsg[243];
            gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
            printf("OnRspQryInvestorPosition():出错:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        }

        // 如果有返回结果读取返回信息
        if ( pInvestorPosition != NULL ) {
            // 读取返回信息,并做编码转化
            ///合约代码 TThostFtdcInstrumentIDType char[31]
            char InstrumentID[93];
            gbk2utf8(pInvestorPosition->InstrumentID,InstrumentID,sizeof(InstrumentID));
            ///经纪公司代码 TThostFtdcBrokerIDType char[11]
            char BrokerID[33];
            gbk2utf8(pInvestorPosition->BrokerID,BrokerID,sizeof(BrokerID));
            ///投资者代码 TThostFtdcInvestorIDType char[13]
            char InvestorID[39];
            gbk2utf8(pInvestorPosition->InvestorID,InvestorID,sizeof(InvestorID));
            ///持仓多空方向 TThostFtdcPosiDirectionType char
            //// THOST_FTDC_PD_Net '1' 净
            //// THOST_FTDC_PD_Long '2' 多头
            //// THOST_FTDC_PD_Short '3' 空头
            char PosiDirection = pInvestorPosition->PosiDirection;
            ///投机套保标志 TThostFtdcHedgeFlagType char
            //// THOST_FTDC_HF_Speculation '1' 投机
            //// THOST_FTDC_HF_Arbitrage '2' 套利
            //// THOST_FTDC_HF_Hedge '3' 套保
            char HedgeFlag = pInvestorPosition->HedgeFlag;
            ///持仓日期 TThostFtdcPositionDateType char
            //// THOST_FTDC_PSD_Today '1' 今日持仓
            //// THOST_FTDC_PSD_History '2' 历史持仓
            char PositionDate = pInvestorPosition->PositionDate;
            ///上日持仓 TThostFtdcVolumeType int
            int YdPosition = pInvestorPosition->YdPosition;
            ///今日持仓 TThostFtdcVolumeType int
            int Position = pInvestorPosition->Position;
            ///多头冻结 TThostFtdcVolumeType int
            int LongFrozen = pInvestorPosition->LongFrozen;
            ///空头冻结 TThostFtdcVolumeType int
            int ShortFrozen = pInvestorPosition->ShortFrozen;
            ///开仓冻结金额 TThostFtdcMoneyType double
            double LongFrozenAmount = pInvestorPosition->LongFrozenAmount;
            ///开仓冻结金额 TThostFtdcMoneyType double
            double ShortFrozenAmount = pInvestorPosition->ShortFrozenAmount;
            ///开仓量 TThostFtdcVolumeType int
            int OpenVolume = pInvestorPosition->OpenVolume;
            ///平仓量 TThostFtdcVolumeType int
            int CloseVolume = pInvestorPosition->CloseVolume;
            ///开仓金额 TThostFtdcMoneyType double
            double OpenAmount = pInvestorPosition->OpenAmount;
            ///平仓金额 TThostFtdcMoneyType double
            double CloseAmount = pInvestorPosition->CloseAmount;
            ///持仓成本 TThostFtdcMoneyType double
            double PositionCost = pInvestorPosition->PositionCost;
            ///上次占用的保证金 TThostFtdcMoneyType double
            double PreMargin = pInvestorPosition->PreMargin;
            ///占用的保证金 TThostFtdcMoneyType double
            double UseMargin = pInvestorPosition->UseMargin;
            ///冻结的保证金 TThostFtdcMoneyType double
            double FrozenMargin = pInvestorPosition->FrozenMargin;
            ///冻结的资金 TThostFtdcMoneyType double
            double FrozenCash = pInvestorPosition->FrozenCash;
            ///冻结的手续费 TThostFtdcMoneyType double
            double FrozenCommission = pInvestorPosition->FrozenCommission;
            ///资金差额 TThostFtdcMoneyType double
            double CashIn = pInvestorPosition->CashIn;
            ///手续费 TThostFtdcMoneyType double
            double Commission = pInvestorPosition->Commission;
            ///平仓盈亏 TThostFtdcMoneyType double
            double CloseProfit = pInvestorPosition->CloseProfit;
            ///持仓盈亏 TThostFtdcMoneyType double
            double PositionProfit = pInvestorPosition->PositionProfit;
            ///上次结算价 TThostFtdcPriceType double
            double PreSettlementPrice = pInvestorPosition->PreSettlementPrice;
            ///本次结算价 TThostFtdcPriceType double
            double SettlementPrice = pInvestorPosition->SettlementPrice;
            ///交易日 TThostFtdcDateType char[9]
            char TradingDay[27];
            gbk2utf8(pInvestorPosition->TradingDay,TradingDay,sizeof(TradingDay));
            ///结算编号 TThostFtdcSettlementIDType int
            int SettlementID = pInvestorPosition->SettlementID;
            ///开仓成本 TThostFtdcMoneyType double
            double OpenCost = pInvestorPosition->OpenCost;
            ///交易所保证金 TThostFtdcMoneyType double
            double ExchangeMargin = pInvestorPosition->ExchangeMargin;
            ///组合成交形成的持仓 TThostFtdcVolumeType int
            int CombPosition = pInvestorPosition->CombPosition;
            ///组合多头冻结 TThostFtdcVolumeType int
            int CombLongFrozen = pInvestorPosition->CombLongFrozen;
            ///组合空头冻结 TThostFtdcVolumeType int
            int CombShortFrozen = pInvestorPosition->CombShortFrozen;
            ///逐日盯市平仓盈亏 TThostFtdcMoneyType double
            double CloseProfitByDate = pInvestorPosition->CloseProfitByDate;
            ///逐笔对冲平仓盈亏 TThostFtdcMoneyType double
            double CloseProfitByTrade = pInvestorPosition->CloseProfitByTrade;
            ///今日持仓 TThostFtdcVolumeType int
            int TodayPosition = pInvestorPosition->TodayPosition;
            ///保证金率 TThostFtdcRatioType double
            double MarginRateByMoney = pInvestorPosition->MarginRateByMoney;
            ///保证金率(按手数) TThostFtdcRatioType double
            double MarginRateByVolume = pInvestorPosition->MarginRateByVolume;

			printf("InstrumentID=%s,PosiDirection=%c,Position=%d,OpenVolume=%d,PositionCost=%.2f,CloseProfitByDate=%.2f,CloseVolume=%d\n",InstrumentID,PosiDirection,Position,OpenVolume,PositionCost,CloseProfitByDate,CloseVolume);

        }

        // 如果响应函数已经返回最后一条信息
        if(bIsLast) {
            // 通知主过程，响应函数将结束
            sem_post(&sem);
        }
    }

};


// 定时器控制函数
void timeout_handler(int signalno) {
    // 定时控制步骤
}

// 初始化定时器
void init_timer(void)
{
    // init sigaction
    struct sigaction act;
    act.sa_handler = timeout_handler;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGPROF, &act, NULL);
    // set timer
    struct itimerval val;
    val.it_value.tv_sec = 1;
    val.it_value.tv_usec = 0;
    val.it_interval = val.it_value;
    setitimer(ITIMER_PROF, &val, NULL);
}


int main() {

    // 初始化线程同步变量
    sem_init(&sem,0,0);

    // 从环境变量中读取登录信息
    char * CTP_FrontAddress = getenv("CTP_FrontAddress");
    if ( CTP_FrontAddress == NULL ) {
        printf("环境变量CTP_FrontAddress没有设置\n");
        return(0);
    }

    char * CTP_BrokerId = getenv("CTP_BrokerId");
    if ( CTP_BrokerId == NULL ) {
        printf("环境变量CTP_BrokerId没有设置\n");
        return(0);
    }
    strcpy(userLoginField.BrokerID,CTP_BrokerId);

    char * CTP_UserId = getenv("CTP_UserId");
    if ( CTP_UserId == NULL ) {
        printf("环境变量CTP_UserId没有设置\n");
        return(0);
    }
    strcpy(userLoginField.UserID,CTP_UserId);

    char * CTP_Password = getenv("CTP_Password");
    if ( CTP_Password == NULL ) {
        printf("环境变量CTP_Password没有设置\n");
        return(0);
    }
    strcpy(userLoginField.Password,CTP_Password);

    // 创建TraderAPI和回调响应控制器的实例
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
    CTraderHandler traderHandler = CTraderHandler();
    CTraderHandler * pTraderHandler = &traderHandler;
    pTraderApi->RegisterSpi(pTraderHandler);

    // 订阅相关信息推送
    //// THOST_TERT_RESTART:从本交易日开始重传
    //// THOST_TERT_RESUME:从上次收到的续传
    //// THOST_TERT_QUICK:只传送登录后私有流的内容
    pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);
    // 订阅公共流
    //// THOST_TERT_RESTART:从本交易日开始重传
    //// THOST_TERT_RESUME:从上次收到的续传
    //// THOST_TERT_QUICK:只传送登录后公共流的内容
    pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);

    // 设置服务器地址
    pTraderApi->RegisterFront(CTP_FrontAddress);
    // 链接交易系统
    pTraderApi->Init();
    // 等待服务器发出登录消息
    sem_wait(&sem);
    // 发出登陆请求
    pTraderApi->ReqUserLogin(&userLoginField, requestID++);
    // 等待登录成功消息
    sem_wait(&sem);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///请求查询投资者持仓
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 定义调用API的数据结构
    CThostFtdcQryInvestorPositionField requestData;
    // 确保没有初始化的数据不会被访问
    memset(&requestData,0,sizeof(requestData));
    // 为调用结构题设置参数信息
    ///经纪公司代码 TThostFtdcBrokerIDType char[11]
    strcpy(requestData.BrokerID,CTP_BrokerId);
    ///投资者代码 TThostFtdcInvestorIDType char[13]
    strcpy(requestData.InvestorID,CTP_UserId);
    ///合约代码 TThostFtdcInstrumentIDType char[31]
    strcpy(requestData.InstrumentID,"");


    // 调用API,并等待响应函数返回
    int result = pTraderApi->ReqQryInvestorPosition(&requestData,requestID++);
    sem_wait(&sem);

    /////////////////////////////////////////////////////////////////////////////////////////////////


    // 拷贝用户登录信息到登出信息
    strcpy(userLogoutField.BrokerID,userLoginField.BrokerID);
    strcpy(userLogoutField.UserID, userLoginField.UserID);
    pTraderApi->ReqUserLogout(&userLogoutField, requestID++);

    // 等待登出成功
    sem_wait(&sem);

    printf("主线程执行完毕!\n");
    return(0);

}
