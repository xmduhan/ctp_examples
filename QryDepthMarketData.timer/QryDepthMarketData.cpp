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


// 定义API信息
CThostFtdcTraderApi *pTraderApi;

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

    ///请求查询行情响应
    virtual void OnRspQryDepthMarketData(
        CThostFtdcDepthMarketDataField * pDepthMarketData,
        CThostFtdcRspInfoField * pRspInfo,
        int nRequestID,
        bool bIsLast
    ) {
        printf("OnRspQryDepthMarketData():被执行...\n");

        // 进程返回结果检查
        if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
            // typedef int TThostFtdcErrorIDType;
            // typedef char TThostFtdcErrorMsgType[81];
            char ErrorMsg[243];
            gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
            printf("OnRspQryDepthMarketData():出错:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        }

        // 如果有返回结果读取返回信息
        if ( pDepthMarketData != NULL ) {
            // 读取返回信息,并做编码转化
            ///交易日 TThostFtdcDateType char[9]
            char TradingDay[27];
            gbk2utf8(pDepthMarketData->TradingDay,TradingDay,sizeof(TradingDay));
            ///合约代码 TThostFtdcInstrumentIDType char[31]
            char InstrumentID[93];
            gbk2utf8(pDepthMarketData->InstrumentID,InstrumentID,sizeof(InstrumentID));
            ///交易所代码 TThostFtdcExchangeIDType char[9]
            char ExchangeID[27];
            gbk2utf8(pDepthMarketData->ExchangeID,ExchangeID,sizeof(ExchangeID));
            ///合约在交易所的代码 TThostFtdcExchangeInstIDType char[31]
            char ExchangeInstID[93];
            gbk2utf8(pDepthMarketData->ExchangeInstID,ExchangeInstID,sizeof(ExchangeInstID));
            ///最新价 TThostFtdcPriceType double
            double LastPrice = pDepthMarketData->LastPrice;
            ///上次结算价 TThostFtdcPriceType double
            double PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
            ///昨收盘 TThostFtdcPriceType double
            double PreClosePrice = pDepthMarketData->PreClosePrice;
            ///昨持仓量 TThostFtdcLargeVolumeType double
            double PreOpenInterest = pDepthMarketData->PreOpenInterest;
            ///今开盘 TThostFtdcPriceType double
            double OpenPrice = pDepthMarketData->OpenPrice;
            ///最高价 TThostFtdcPriceType double
            double HighestPrice = pDepthMarketData->HighestPrice;
            ///最低价 TThostFtdcPriceType double
            double LowestPrice = pDepthMarketData->LowestPrice;
            ///数量 TThostFtdcVolumeType int
            int Volume = pDepthMarketData->Volume;
            ///成交金额 TThostFtdcMoneyType double
            double Turnover = pDepthMarketData->Turnover;
            ///持仓量 TThostFtdcLargeVolumeType double
            double OpenInterest = pDepthMarketData->OpenInterest;
            ///今收盘 TThostFtdcPriceType double
            double ClosePrice = pDepthMarketData->ClosePrice;
            ///本次结算价 TThostFtdcPriceType double
            double SettlementPrice = pDepthMarketData->SettlementPrice;
            ///涨停板价 TThostFtdcPriceType double
            double UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
            ///跌停板价 TThostFtdcPriceType double
            double LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
            ///昨虚实度 TThostFtdcRatioType double
            double PreDelta = pDepthMarketData->PreDelta;
            ///今虚实度 TThostFtdcRatioType double
            double CurrDelta = pDepthMarketData->CurrDelta;
            ///最后修改时间 TThostFtdcTimeType char[9]
            char UpdateTime[27];
            gbk2utf8(pDepthMarketData->UpdateTime,UpdateTime,sizeof(UpdateTime));
            ///最后修改毫秒 TThostFtdcMillisecType int
            int UpdateMillisec = pDepthMarketData->UpdateMillisec;
            ///申买价一 TThostFtdcPriceType double
            double BidPrice1 = pDepthMarketData->BidPrice1;
            ///申买量一 TThostFtdcVolumeType int
            int BidVolume1 = pDepthMarketData->BidVolume1;
            ///申卖价一 TThostFtdcPriceType double
            double AskPrice1 = pDepthMarketData->AskPrice1;
            ///申卖量一 TThostFtdcVolumeType int
            int AskVolume1 = pDepthMarketData->AskVolume1;
            ///申买价二 TThostFtdcPriceType double
            double BidPrice2 = pDepthMarketData->BidPrice2;
            ///申买量二 TThostFtdcVolumeType int
            int BidVolume2 = pDepthMarketData->BidVolume2;
            ///申卖价二 TThostFtdcPriceType double
            double AskPrice2 = pDepthMarketData->AskPrice2;
            ///申卖量二 TThostFtdcVolumeType int
            int AskVolume2 = pDepthMarketData->AskVolume2;
            ///申买价三 TThostFtdcPriceType double
            double BidPrice3 = pDepthMarketData->BidPrice3;
            ///申买量三 TThostFtdcVolumeType int
            int BidVolume3 = pDepthMarketData->BidVolume3;
            ///申卖价三 TThostFtdcPriceType double
            double AskPrice3 = pDepthMarketData->AskPrice3;
            ///申卖量三 TThostFtdcVolumeType int
            int AskVolume3 = pDepthMarketData->AskVolume3;
            ///申买价四 TThostFtdcPriceType double
            double BidPrice4 = pDepthMarketData->BidPrice4;
            ///申买量四 TThostFtdcVolumeType int
            int BidVolume4 = pDepthMarketData->BidVolume4;
            ///申卖价四 TThostFtdcPriceType double
            double AskPrice4 = pDepthMarketData->AskPrice4;
            ///申卖量四 TThostFtdcVolumeType int
            int AskVolume4 = pDepthMarketData->AskVolume4;
            ///申买价五 TThostFtdcPriceType double
            double BidPrice5 = pDepthMarketData->BidPrice5;
            ///申买量五 TThostFtdcVolumeType int
            int BidVolume5 = pDepthMarketData->BidVolume5;
            ///申卖价五 TThostFtdcPriceType double
            double AskPrice5 = pDepthMarketData->AskPrice5;
            ///申卖量五 TThostFtdcVolumeType int
            int AskVolume5 = pDepthMarketData->AskVolume5;
            ///当日均价 TThostFtdcPriceType double
            double AveragePrice = pDepthMarketData->AveragePrice;
            ///业务日期 TThostFtdcDateType char[9]
            char ActionDay[27];
            gbk2utf8(pDepthMarketData->ActionDay,ActionDay,sizeof(ActionDay));

			printf("TradingDay=%s,",TradingDay);    // 交易日
            printf("InstrumentID=%s,",InstrumentID);  // 合约代码
            printf("LastPrice=%f,",LastPrice);  // 最新价
            printf("UpdateTime=%s,",UpdateTime);  // 最新价
            printf("Volume=%d\n",Volume);  // 最新价

        }

        // 如果响应函数已经返回最后一条信息
        if(bIsLast) {
            // 通知主过程，响应函数将结束
            // sem_post(&sem);
        }
    }

};


// 定时器控制函数
void timeout_handler(int signalno) {

    // 定时控制步骤

	////////////////////////////////////////////////////////////////////////////////////////////////
    ///请求查询行情
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 定义调用API的数据结构
    CThostFtdcQryDepthMarketDataField requestData;
    // 确保没有初始化的数据不会被访问
    memset(&requestData,0,sizeof(requestData));
    // 为调用结构题设置参数信息
    ///合约代码 TThostFtdcInstrumentIDType char[31]
    strcpy(requestData.InstrumentID,"IF1504");
    // 调用API
    int result = pTraderApi->ReqQryDepthMarketData(&requestData,requestID++);
    /////////////////////////////////////////////////////////////////////////////////////////////////
	
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
    pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
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
    ///  启动定时器，通过定时器发出行情查询请求
    ///////////////////////////////////////////////////////////////////////////////////////////////
	init_timer();	
	while(1);
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
