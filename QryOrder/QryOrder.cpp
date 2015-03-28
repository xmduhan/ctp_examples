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

    ///请求查询报单响应
    virtual void OnRspQryOrder(
        CThostFtdcOrderField * pOrder,
        CThostFtdcRspInfoField * pRspInfo,
        int nRequestID,
        bool bIsLast
    ) {
        printf("OnRspQryOrder():被执行...\n");
        if ( pOrder != NULL ) {
            // 读取返回信息,并做编码转化
            ///经纪公司代码 char[11]
            char BrokerID[33];
            gbk2utf8(pOrder->BrokerID,BrokerID,sizeof(BrokerID));
            ///投资者代码 char[13]
            char InvestorID[39];
            gbk2utf8(pOrder->InvestorID,InvestorID,sizeof(InvestorID));
            ///合约代码 char[31]
            char InstrumentID[93];
            gbk2utf8(pOrder->InstrumentID,InstrumentID,sizeof(InstrumentID));
            ///报单引用 char[13]
            char OrderRef[39];
            gbk2utf8(pOrder->OrderRef,OrderRef,sizeof(OrderRef));
            ///用户代码 char[16]
            char UserID[48];
            gbk2utf8(pOrder->UserID,UserID,sizeof(UserID));
            ///报单价格条件 char
            char OrderPriceType = pOrder->OrderPriceType;
            ///买卖方向 char
            char Direction = pOrder->Direction;
            ///组合开平标志 char[5]
            char CombOffsetFlag[15];
            gbk2utf8(pOrder->CombOffsetFlag,CombOffsetFlag,sizeof(CombOffsetFlag));
            ///组合投机套保标志 char[5]
            char CombHedgeFlag[15];
            gbk2utf8(pOrder->CombHedgeFlag,CombHedgeFlag,sizeof(CombHedgeFlag));
            ///价格 double
            double LimitPrice = pOrder->LimitPrice;
            ///数量 int
            int VolumeTotalOriginal = pOrder->VolumeTotalOriginal;
            ///有效期类型 char
            char TimeCondition = pOrder->TimeCondition;
            ///GTD日期 char[9]
            char GTDDate[27];
            gbk2utf8(pOrder->GTDDate,GTDDate,sizeof(GTDDate));
            ///成交量类型 char
            char VolumeCondition = pOrder->VolumeCondition;
            ///最小成交量 int
            int MinVolume = pOrder->MinVolume;
            ///触发条件 char
            char ContingentCondition = pOrder->ContingentCondition;
            ///止损价 double
            double StopPrice = pOrder->StopPrice;
            ///强平原因 char
            char ForceCloseReason = pOrder->ForceCloseReason;
            ///自动挂起标志 int
            int IsAutoSuspend = pOrder->IsAutoSuspend;
            ///业务单元 char[21]
            char BusinessUnit[63];
            gbk2utf8(pOrder->BusinessUnit,BusinessUnit,sizeof(BusinessUnit));
            ///请求编号 int
            int RequestID = pOrder->RequestID;
            ///本地报单编号 char[13]
            char OrderLocalID[39];
            gbk2utf8(pOrder->OrderLocalID,OrderLocalID,sizeof(OrderLocalID));
            ///交易所代码 char[9]
            char ExchangeID[27];
            gbk2utf8(pOrder->ExchangeID,ExchangeID,sizeof(ExchangeID));
            ///会员代码 char[11]
            char ParticipantID[33];
            gbk2utf8(pOrder->ParticipantID,ParticipantID,sizeof(ParticipantID));
            ///客户代码 char[11]
            char ClientID[33];
            gbk2utf8(pOrder->ClientID,ClientID,sizeof(ClientID));
            ///合约在交易所的代码 char[31]
            char ExchangeInstID[93];
            gbk2utf8(pOrder->ExchangeInstID,ExchangeInstID,sizeof(ExchangeInstID));
            ///交易所交易员代码 char[21]
            char TraderID[63];
            gbk2utf8(pOrder->TraderID,TraderID,sizeof(TraderID));
            ///安装编号 int
            int InstallID = pOrder->InstallID;
            ///报单提交状态 char
            char OrderSubmitStatus = pOrder->OrderSubmitStatus;
            ///报单提示序号 int
            int NotifySequence = pOrder->NotifySequence;
            ///交易日 char[9]
            char TradingDay[27];
            gbk2utf8(pOrder->TradingDay,TradingDay,sizeof(TradingDay));
            ///结算编号 int
            int SettlementID = pOrder->SettlementID;
            ///报单编号 char[21]
            char OrderSysID[63];
            gbk2utf8(pOrder->OrderSysID,OrderSysID,sizeof(OrderSysID));
            ///报单来源 char
            char OrderSource = pOrder->OrderSource;
            ///报单状态 char
            char OrderStatus = pOrder->OrderStatus;
            ///报单类型 char
            char OrderType = pOrder->OrderType;
            ///今成交数量 int
            int VolumeTraded = pOrder->VolumeTraded;
            ///剩余数量 int
            int VolumeTotal = pOrder->VolumeTotal;
            ///报单日期 char[9]
            char InsertDate[27];
            gbk2utf8(pOrder->InsertDate,InsertDate,sizeof(InsertDate));
            ///委托时间 char[9]
            char InsertTime[27];
            gbk2utf8(pOrder->InsertTime,InsertTime,sizeof(InsertTime));
            ///激活时间 char[9]
            char ActiveTime[27];
            gbk2utf8(pOrder->ActiveTime,ActiveTime,sizeof(ActiveTime));
            ///挂起时间 char[9]
            char SuspendTime[27];
            gbk2utf8(pOrder->SuspendTime,SuspendTime,sizeof(SuspendTime));
            ///最后修改时间 char[9]
            char UpdateTime[27];
            gbk2utf8(pOrder->UpdateTime,UpdateTime,sizeof(UpdateTime));
            ///撤销时间 char[9]
            char CancelTime[27];
            gbk2utf8(pOrder->CancelTime,CancelTime,sizeof(CancelTime));
            ///最后修改交易所交易员代码 char[21]
            char ActiveTraderID[63];
            gbk2utf8(pOrder->ActiveTraderID,ActiveTraderID,sizeof(ActiveTraderID));
            ///结算会员编号 char[11]
            char ClearingPartID[33];
            gbk2utf8(pOrder->ClearingPartID,ClearingPartID,sizeof(ClearingPartID));
            ///序号 int
            int SequenceNo = pOrder->SequenceNo;
            ///前置编号 int
            int FrontID = pOrder->FrontID;
            ///会话编号 int
            int SessionID = pOrder->SessionID;
            ///用户端产品信息 char[11]
            char UserProductInfo[33];
            gbk2utf8(pOrder->UserProductInfo,UserProductInfo,sizeof(UserProductInfo));
            ///状态信息 char[81]
            char StatusMsg[243];
            gbk2utf8(pOrder->StatusMsg,StatusMsg,sizeof(StatusMsg));
            ///用户强评标志 int
            int UserForceClose = pOrder->UserForceClose;
            ///操作用户代码 char[16]
            char ActiveUserID[48];
            gbk2utf8(pOrder->ActiveUserID,ActiveUserID,sizeof(ActiveUserID));
            ///经纪公司报单编号 int
            int BrokerOrderSeq = pOrder->BrokerOrderSeq;
            ///相关报单 char[21]
            char RelativeOrderSysID[63];
            gbk2utf8(pOrder->RelativeOrderSysID,RelativeOrderSysID,sizeof(RelativeOrderSysID));
            ///郑商所成交数量 int
            int ZCETotalTradedVolume = pOrder->ZCETotalTradedVolume;
            ///互换单标志 int
            int IsSwapOrder = pOrder->IsSwapOrder;


        }
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
    ///请求查询报单
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 定义调用API的数据结构
    CThostFtdcQryOrderField requestData;
    // 确保没有初始化的数据不会被访问
    memset(&requestData,0,sizeof(requestData));
    // 为调用结构题设置参数信息
    ///经纪公司代码
    strcpy(requestData.BrokerID,"");
    ///投资者代码
    strcpy(requestData.InvestorID,"");
    ///合约代码
    strcpy(requestData.InstrumentID,"");
    ///交易所代码
    strcpy(requestData.ExchangeID,"");
    ///报单编号
    strcpy(requestData.OrderSysID,"");
    ///开始时间
    strcpy(requestData.InsertTimeStart,"");
    ///结束时间
    strcpy(requestData.InsertTimeEnd,"");


    // 调用API,并等待响应函数返回
    int result = pTraderApi->ReqQryOrder(&requestData,requestID++);
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