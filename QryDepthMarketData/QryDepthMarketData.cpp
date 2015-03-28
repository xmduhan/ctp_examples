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

    ///请求查询行情响应
    virtual void OnRspQryDepthMarketData(
        CThostFtdcDepthMarketDataField * pDepthMarketData,
        CThostFtdcRspInfoField * pRspInfo,
        int nRequestID,
        bool bIsLast
    ) {
        printf("OnRspQryDepthMarketData():被执行...\n");
        // 读取返回信息,并做编码转化
        ///交易日 char[9]
        char TradingDay[27];
        gbk2utf8(pDepthMarketData->TradingDay,TradingDay,sizeof(TradingDay));
        ///合约代码 char[31]
        char InstrumentID[93];
        gbk2utf8(pDepthMarketData->InstrumentID,InstrumentID,sizeof(InstrumentID));
        ///交易所代码 char[9]
        char ExchangeID[27];
        gbk2utf8(pDepthMarketData->ExchangeID,ExchangeID,sizeof(ExchangeID));
        ///合约在交易所的代码 char[31]
        char ExchangeInstID[93];
        gbk2utf8(pDepthMarketData->ExchangeInstID,ExchangeInstID,sizeof(ExchangeInstID));
        ///最新价 double
        double LastPrice = pDepthMarketData->LastPrice;
        ///上次结算价 double
        double PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
        ///昨收盘 double
        double PreClosePrice = pDepthMarketData->PreClosePrice;
        ///昨持仓量 double
        double PreOpenInterest = pDepthMarketData->PreOpenInterest;
        ///今开盘 double
        double OpenPrice = pDepthMarketData->OpenPrice;
        ///最高价 double
        double HighestPrice = pDepthMarketData->HighestPrice;
        ///最低价 double
        double LowestPrice = pDepthMarketData->LowestPrice;
        ///数量 int
        int Volume = pDepthMarketData->Volume;
        ///成交金额 double
        double Turnover = pDepthMarketData->Turnover;
        ///持仓量 double
        double OpenInterest = pDepthMarketData->OpenInterest;
        ///今收盘 double
        double ClosePrice = pDepthMarketData->ClosePrice;
        ///本次结算价 double
        double SettlementPrice = pDepthMarketData->SettlementPrice;
        ///涨停板价 double
        double UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
        ///跌停板价 double
        double LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
        ///昨虚实度 double
        double PreDelta = pDepthMarketData->PreDelta;
        ///今虚实度 double
        double CurrDelta = pDepthMarketData->CurrDelta;
        ///最后修改时间 char[9]
        char UpdateTime[27];
        gbk2utf8(pDepthMarketData->UpdateTime,UpdateTime,sizeof(UpdateTime));
        ///最后修改毫秒 int
        int UpdateMillisec = pDepthMarketData->UpdateMillisec;
        ///申买价一 double
        double BidPrice1 = pDepthMarketData->BidPrice1;
        ///申买量一 int
        int BidVolume1 = pDepthMarketData->BidVolume1;
        ///申卖价一 double
        double AskPrice1 = pDepthMarketData->AskPrice1;
        ///申卖量一 int
        int AskVolume1 = pDepthMarketData->AskVolume1;
        ///申买价二 double
        double BidPrice2 = pDepthMarketData->BidPrice2;
        ///申买量二 int
        int BidVolume2 = pDepthMarketData->BidVolume2;
        ///申卖价二 double
        double AskPrice2 = pDepthMarketData->AskPrice2;
        ///申卖量二 int
        int AskVolume2 = pDepthMarketData->AskVolume2;
        ///申买价三 double
        double BidPrice3 = pDepthMarketData->BidPrice3;
        ///申买量三 int
        int BidVolume3 = pDepthMarketData->BidVolume3;
        ///申卖价三 double
        double AskPrice3 = pDepthMarketData->AskPrice3;
        ///申卖量三 int
        int AskVolume3 = pDepthMarketData->AskVolume3;
        ///申买价四 double
        double BidPrice4 = pDepthMarketData->BidPrice4;
        ///申买量四 int
        int BidVolume4 = pDepthMarketData->BidVolume4;
        ///申卖价四 double
        double AskPrice4 = pDepthMarketData->AskPrice4;
        ///申卖量四 int
        int AskVolume4 = pDepthMarketData->AskVolume4;
        ///申买价五 double
        double BidPrice5 = pDepthMarketData->BidPrice5;
        ///申买量五 int
        int BidVolume5 = pDepthMarketData->BidVolume5;
        ///申卖价五 double
        double AskPrice5 = pDepthMarketData->AskPrice5;
        ///申卖量五 int
        int AskVolume5 = pDepthMarketData->AskVolume5;
        ///当日均价 double
        double AveragePrice = pDepthMarketData->AveragePrice;
        ///业务日期 char[9]
        char ActionDay[27];
        gbk2utf8(pDepthMarketData->ActionDay,ActionDay,sizeof(ActionDay));


        // 如果响应函数已经返回最后一个信息
        if(bIsLast) {
            // 通知主过程，响应函数将结束
            sem_post(&sem);
        }
    }

};


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

    // 设置服务器地址
    pTraderApi->RegisterFront(CTP_FrontAddress);
    printf("CTP_FrontAddress=%s",CTP_FrontAddress);
    // 链接交易系统
    pTraderApi->Init();
    // 等待服务器发出登录消息
    sem_wait(&sem);
    // 发出登陆请求
    pTraderApi->ReqUserLogin(&userLoginField, requestID++);
    // 等待登录成功消息
    sem_wait(&sem);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///请求查询行情
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 定义调用API的数据结构
    CThostFtdcQryDepthMarketDataField requestData;
    // 确保没有初始化的数据不会被访问
    memset(&requestData,0,sizeof(requestData));
    // 为调用结构题设置参数信息
    ///合约代码
    strcpy(requestData.InstrumentID,"");


    // 调用API,并等待响应函数返回
    int result = pTraderApi->ReqQryDepthMarketData(&requestData,requestID++);
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