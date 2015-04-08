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

    ///请求查询合约响应
    virtual void OnRspQryInstrument(
        CThostFtdcInstrumentField * pInstrument,
        CThostFtdcRspInfoField * pRspInfo,
        int nRequestID,
        bool bIsLast
    ) {
        printf("OnRspQryInstrument():被执行...\n");

        // 进程返回结果检查
        if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
            // typedef int TThostFtdcErrorIDType;
            // typedef char TThostFtdcErrorMsgType[81];
            char ErrorMsg[243];
            gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
            printf("OnRspQryInstrument():出错:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        }

        // 如果有返回结果读取返回信息
        if ( pInstrument != NULL ) {
            // 读取返回信息,并做编码转化
            ///合约代码 TThostFtdcInstrumentIDType char[31]
            char InstrumentID[93];
            gbk2utf8(pInstrument->InstrumentID,InstrumentID,sizeof(InstrumentID));
            ///交易所代码 TThostFtdcExchangeIDType char[9]
            char ExchangeID[27];
            gbk2utf8(pInstrument->ExchangeID,ExchangeID,sizeof(ExchangeID));
            ///合约名称 TThostFtdcInstrumentNameType char[21]
            char InstrumentName[63];
            gbk2utf8(pInstrument->InstrumentName,InstrumentName,sizeof(InstrumentName));
            ///合约在交易所的代码 TThostFtdcExchangeInstIDType char[31]
            char ExchangeInstID[93];
            gbk2utf8(pInstrument->ExchangeInstID,ExchangeInstID,sizeof(ExchangeInstID));
            ///产品代码 TThostFtdcInstrumentIDType char[31]
            char ProductID[93];
            gbk2utf8(pInstrument->ProductID,ProductID,sizeof(ProductID));
            ///产品类型 TThostFtdcProductClassType char
            //// THOST_FTDC_PC_Futures '1' 期货
            //// THOST_FTDC_PC_Options '2' 期权
            //// THOST_FTDC_PC_Combination '3' 组合
            //// THOST_FTDC_PC_Spot '4' 即期
            //// THOST_FTDC_PC_EFP '5' 期转现
            char ProductClass = pInstrument->ProductClass;
            ///交割年份 TThostFtdcYearType int
            int DeliveryYear = pInstrument->DeliveryYear;
            ///交割月 TThostFtdcMonthType int
            int DeliveryMonth = pInstrument->DeliveryMonth;
            ///市价单最大下单量 TThostFtdcVolumeType int
            int MaxMarketOrderVolume = pInstrument->MaxMarketOrderVolume;
            ///市价单最小下单量 TThostFtdcVolumeType int
            int MinMarketOrderVolume = pInstrument->MinMarketOrderVolume;
            ///限价单最大下单量 TThostFtdcVolumeType int
            int MaxLimitOrderVolume = pInstrument->MaxLimitOrderVolume;
            ///限价单最小下单量 TThostFtdcVolumeType int
            int MinLimitOrderVolume = pInstrument->MinLimitOrderVolume;
            ///合约数量乘数 TThostFtdcVolumeMultipleType int
            int VolumeMultiple = pInstrument->VolumeMultiple;
            ///最小变动价位 TThostFtdcPriceType double
            double PriceTick = pInstrument->PriceTick;
            ///创建日 TThostFtdcDateType char[9]
            char CreateDate[27];
            gbk2utf8(pInstrument->CreateDate,CreateDate,sizeof(CreateDate));
            ///上市日 TThostFtdcDateType char[9]
            char OpenDate[27];
            gbk2utf8(pInstrument->OpenDate,OpenDate,sizeof(OpenDate));
            ///到期日 TThostFtdcDateType char[9]
            char ExpireDate[27];
            gbk2utf8(pInstrument->ExpireDate,ExpireDate,sizeof(ExpireDate));
            ///开始交割日 TThostFtdcDateType char[9]
            char StartDelivDate[27];
            gbk2utf8(pInstrument->StartDelivDate,StartDelivDate,sizeof(StartDelivDate));
            ///结束交割日 TThostFtdcDateType char[9]
            char EndDelivDate[27];
            gbk2utf8(pInstrument->EndDelivDate,EndDelivDate,sizeof(EndDelivDate));
            ///合约生命周期状态 TThostFtdcInstLifePhaseType char
            //// THOST_FTDC_IP_NotStart '0' 未上市
            //// THOST_FTDC_IP_Started '1' 上市
            //// THOST_FTDC_IP_Pause '2' 停牌
            //// THOST_FTDC_IP_Expired '3' 到期
            char InstLifePhase = pInstrument->InstLifePhase;
            ///当前是否交易 TThostFtdcBoolType int
            int IsTrading = pInstrument->IsTrading;
            ///持仓类型 TThostFtdcPositionTypeType char
            //// THOST_FTDC_PT_Net '1' 净持仓
            //// THOST_FTDC_PT_Gross '2' 综合持仓
            char PositionType = pInstrument->PositionType;
            ///持仓日期类型 TThostFtdcPositionDateTypeType char
            //// THOST_FTDC_PDT_UseHistory '1' 使用历史持仓
            //// THOST_FTDC_PDT_NoUseHistory '2' 不使用历史持仓
            char PositionDateType = pInstrument->PositionDateType;
            ///多头保证金率 TThostFtdcRatioType double
            double LongMarginRatio = pInstrument->LongMarginRatio;
            ///空头保证金率 TThostFtdcRatioType double
            double ShortMarginRatio = pInstrument->ShortMarginRatio;
            ///是否使用大额单边保证金算法 TThostFtdcMaxMarginSideAlgorithmType char
            //// THOST_FTDC_MMSA_NO '0' 不使用大额单边保证金算法
            //// THOST_FTDC_MMSA_YES '1' 使用大额单边保证金算法
            char MaxMarginSideAlgorithm = pInstrument->MaxMarginSideAlgorithm;

			printf("InstrumentID=%s,ExchangeID=%s,InstrumentName=%s\n",InstrumentID,ExchangeID,InstrumentName);

        }

        // 如果响应函数已经返回最后一条信息
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
    // 链接交易系统
    pTraderApi->Init();
    // 等待服务器发出登录消息
    sem_wait(&sem);
    // 发出登陆请求
    pTraderApi->ReqUserLogin(&userLoginField, requestID++);
    // 等待登录成功消息
    sem_wait(&sem);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///请求查询合约
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 定义调用API的数据结构
    CThostFtdcQryInstrumentField requestData;
    // 确保没有初始化的数据不会被访问
    memset(&requestData,0,sizeof(requestData));
    // 为调用结构题设置参数信息
    ///合约代码 TThostFtdcInstrumentIDType char[31]
    strcpy(requestData.InstrumentID,"IF1504");
    ///交易所代码 TThostFtdcExchangeIDType char[9]
    strcpy(requestData.ExchangeID,"");
    ///合约在交易所的代码 TThostFtdcExchangeInstIDType char[31]
    strcpy(requestData.ExchangeInstID,"");
    ///产品代码 TThostFtdcInstrumentIDType char[31]
    strcpy(requestData.ProductID,"");


    // 调用API,并等待响应函数返回
    int result = pTraderApi->ReqQryInstrument(&requestData,requestID++);
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
