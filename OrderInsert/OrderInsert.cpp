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

    ///报单录入请求响应
    virtual void OnRspOrderInsert(
        CThostFtdcInputOrderField * pInputOrder,
        CThostFtdcRspInfoField * pRspInfo,
        int nRequestID,
        bool bIsLast
    ) {
        printf("OnRspOrderInsert():被执行...\n");
        // 读取返回信息,并做编码转化
        ///经纪公司代码 char[11]
        char BrokerID[33];
        gbk2utf8(pInputOrder->BrokerID,BrokerID,sizeof(BrokerID));
        ///投资者代码 char[13]
        char InvestorID[39];
        gbk2utf8(pInputOrder->InvestorID,InvestorID,sizeof(InvestorID));
        ///合约代码 char[31]
        char InstrumentID[93];
        gbk2utf8(pInputOrder->InstrumentID,InstrumentID,sizeof(InstrumentID));
        ///报单引用 char[13]
        char OrderRef[39];
        gbk2utf8(pInputOrder->OrderRef,OrderRef,sizeof(OrderRef));
        ///用户代码 char[16]
        char UserID[48];
        gbk2utf8(pInputOrder->UserID,UserID,sizeof(UserID));
        ///报单价格条件 char
        char OrderPriceType = pInputOrder->OrderPriceType;
        ///买卖方向 char
        char Direction = pInputOrder->Direction;
        ///组合开平标志 char[5]
        char CombOffsetFlag[15];
        gbk2utf8(pInputOrder->CombOffsetFlag,CombOffsetFlag,sizeof(CombOffsetFlag));
        ///组合投机套保标志 char[5]
        char CombHedgeFlag[15];
        gbk2utf8(pInputOrder->CombHedgeFlag,CombHedgeFlag,sizeof(CombHedgeFlag));
        ///价格 double
        double LimitPrice = pInputOrder->LimitPrice;
        ///数量 int
        int VolumeTotalOriginal = pInputOrder->VolumeTotalOriginal;
        ///有效期类型 char
        char TimeCondition = pInputOrder->TimeCondition;
        ///GTD日期 char[9]
        char GTDDate[27];
        gbk2utf8(pInputOrder->GTDDate,GTDDate,sizeof(GTDDate));
        ///成交量类型 char
        char VolumeCondition = pInputOrder->VolumeCondition;
        ///最小成交量 int
        int MinVolume = pInputOrder->MinVolume;
        ///触发条件 char
        char ContingentCondition = pInputOrder->ContingentCondition;
        ///止损价 double
        double StopPrice = pInputOrder->StopPrice;
        ///强平原因 char
        char ForceCloseReason = pInputOrder->ForceCloseReason;
        ///自动挂起标志 int
        int IsAutoSuspend = pInputOrder->IsAutoSuspend;
        ///业务单元 char[21]
        char BusinessUnit[63];
        gbk2utf8(pInputOrder->BusinessUnit,BusinessUnit,sizeof(BusinessUnit));
        ///请求编号 int
        int RequestID = pInputOrder->RequestID;
        ///用户强评标志 int
        int UserForceClose = pInputOrder->UserForceClose;
        ///互换单标志 int
        int IsSwapOrder = pInputOrder->IsSwapOrder;


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
    ///报单录入请求
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 定义调用API的数据结构
    CThostFtdcInputOrderField requestData;
    // 确保没有初始化的数据不会被访问
    memset(&requestData,0,sizeof(requestData));
    // 为调用结构题设置参数信息
    ///经纪公司代码
    strcpy(requestData.BrokerID,"");
    ///投资者代码
    strcpy(requestData.InvestorID,"");
    ///合约代码
    strcpy(requestData.InstrumentID,"");
    ///报单引用
    strcpy(requestData.OrderRef,"");
    ///用户代码
    strcpy(requestData.UserID,"");
    ///报单价格条件
    requestData.OrderPriceType = '0';
    ///买卖方向
    requestData.Direction = '0';
    ///组合开平标志
    strcpy(requestData.CombOffsetFlag,"");
    ///组合投机套保标志
    strcpy(requestData.CombHedgeFlag,"");
    ///价格
    requestData.LimitPrice = 0;
    ///数量
    requestData.VolumeTotalOriginal = 0;
    ///有效期类型
    requestData.TimeCondition = '0';
    ///GTD日期
    strcpy(requestData.GTDDate,"");
    ///成交量类型
    requestData.VolumeCondition = '0';
    ///最小成交量
    requestData.MinVolume = 0;
    ///触发条件
    requestData.ContingentCondition = '0';
    ///止损价
    requestData.StopPrice = 0;
    ///强平原因
    requestData.ForceCloseReason = '0';
    ///自动挂起标志
    requestData.IsAutoSuspend = 0;
    ///业务单元
    strcpy(requestData.BusinessUnit,"");
    ///请求编号
    requestData.RequestID = 0;
    ///用户强评标志
    requestData.UserForceClose = 0;
    ///互换单标志
    requestData.IsSwapOrder = 0;


    // 调用API,并等待响应函数返回
    int result = pTraderApi->ReqOrderInsert(&requestData,requestID++);
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