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

    ///报单录入请求响应
    virtual void OnRspOrderInsert(
        CThostFtdcInputOrderField * pInputOrder,
        CThostFtdcRspInfoField * pRspInfo,
        int nRequestID,
        bool bIsLast
    ) {
        printf("OnRspOrderInsert():被执行...\n");

        // 进程返回结果检查
        if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
            // typedef int TThostFtdcErrorIDType;
            // typedef char TThostFtdcErrorMsgType[81];
            char ErrorMsg[243];
            gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
            printf("OnRspOrderInsert():出错:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        }

        // 如果有返回结果读取返回信息
        if ( pInputOrder != NULL ) {
            // 读取返回信息,并做编码转化
            ///经纪公司代码 TThostFtdcBrokerIDType char[11]
            char BrokerID[33];
            gbk2utf8(pInputOrder->BrokerID,BrokerID,sizeof(BrokerID));
            ///投资者代码 TThostFtdcInvestorIDType char[13]
            char InvestorID[39];
            gbk2utf8(pInputOrder->InvestorID,InvestorID,sizeof(InvestorID));
            ///合约代码 TThostFtdcInstrumentIDType char[31]
            char InstrumentID[93];
            gbk2utf8(pInputOrder->InstrumentID,InstrumentID,sizeof(InstrumentID));
            ///报单引用 TThostFtdcOrderRefType char[13]
            char OrderRef[39];
            gbk2utf8(pInputOrder->OrderRef,OrderRef,sizeof(OrderRef));
            ///用户代码 TThostFtdcUserIDType char[16]
            char UserID[48];
            gbk2utf8(pInputOrder->UserID,UserID,sizeof(UserID));
            ///报单价格条件 TThostFtdcOrderPriceTypeType char
            //// THOST_FTDC_OPT_AnyPrice '1' 任意价
            //// THOST_FTDC_OPT_LimitPrice '2' 限价
            //// THOST_FTDC_OPT_BestPrice '3' 最优价
            //// THOST_FTDC_OPT_LastPrice '4' 最新价
            //// THOST_FTDC_OPT_LastPricePlusOneTicks '5' 最新价浮动上浮1个ticks
            //// THOST_FTDC_OPT_LastPricePlusTwoTicks '6' 最新价浮动上浮2个ticks
            //// THOST_FTDC_OPT_LastPricePlusThreeTicks '7' 最新价浮动上浮3个ticks
            //// THOST_FTDC_OPT_AskPrice1 '8' 卖一价
            //// THOST_FTDC_OPT_AskPrice1PlusOneTicks '9' 卖一价浮动上浮1个ticks
            //// THOST_FTDC_OPT_AskPrice1PlusTwoTicks 'A' 卖一价浮动上浮2个ticks
            //// THOST_FTDC_OPT_AskPrice1PlusThreeTicks 'B' 卖一价浮动上浮3个ticks
            //// THOST_FTDC_OPT_BidPrice1 'C' 买一价
            //// THOST_FTDC_OPT_BidPrice1PlusOneTicks 'D' 买一价浮动上浮1个ticks
            //// THOST_FTDC_OPT_BidPrice1PlusTwoTicks 'E' 买一价浮动上浮2个ticks
            //// THOST_FTDC_OPT_BidPrice1PlusThreeTicks 'F' 买一价浮动上浮3个ticks
            char OrderPriceType = pInputOrder->OrderPriceType;
            ///买卖方向 TThostFtdcDirectionType char
            //// THOST_FTDC_D_Buy '0' 买
            //// THOST_FTDC_D_Sell '1' 卖
            char Direction = pInputOrder->Direction;
            ///组合开平标志 TThostFtdcCombOffsetFlagType char[5]
            //// THOST_FTDC_OF_Open '0' 开仓
            //// THOST_FTDC_OF_Close '1' 平仓
            //// THOST_FTDC_OF_ForceClose '2' 强平
            //// THOST_FTDC_OF_CloseToday '3' 平今
            //// THOST_FTDC_OF_CloseYesterday '4' 平昨
            //// THOST_FTDC_OF_ForceOff '5' 强减
            //// THOST_FTDC_OF_LocalForceClose '6' 本地强平
            char CombOffsetFlag[15];
            gbk2utf8(pInputOrder->CombOffsetFlag,CombOffsetFlag,sizeof(CombOffsetFlag));
            ///组合投机套保标志 TThostFtdcCombHedgeFlagType char[5]
            //// THOST_FTDC_HF_Speculation '1' 投机
            //// THOST_FTDC_HF_Arbitrage '2' 套利
            //// THOST_FTDC_HF_Hedge '3' 套保
            char CombHedgeFlag[15];
            gbk2utf8(pInputOrder->CombHedgeFlag,CombHedgeFlag,sizeof(CombHedgeFlag));
            ///价格 TThostFtdcPriceType double
            double LimitPrice = pInputOrder->LimitPrice;
            ///数量 TThostFtdcVolumeType int
            int VolumeTotalOriginal = pInputOrder->VolumeTotalOriginal;
            ///有效期类型 TThostFtdcTimeConditionType char
            //// THOST_FTDC_TC_IOC '1' 立即完成，否则撤销
            //// THOST_FTDC_TC_GFS '2' 本节有效
            //// THOST_FTDC_TC_GFD '3' 当日有效
            //// THOST_FTDC_TC_GTD '4' 指定日期前有效
            //// THOST_FTDC_TC_GTC '5' 撤销前有效
            //// THOST_FTDC_TC_GFA '6' 集合竞价有效
            char TimeCondition = pInputOrder->TimeCondition;
            ///GTD日期 TThostFtdcDateType char[9]
            char GTDDate[27];
            gbk2utf8(pInputOrder->GTDDate,GTDDate,sizeof(GTDDate));
            ///成交量类型 TThostFtdcVolumeConditionType char
            //// THOST_FTDC_VC_AV '1' 任何数量
            //// THOST_FTDC_VC_MV '2' 最小数量
            //// THOST_FTDC_VC_CV '3' 全部数量
            char VolumeCondition = pInputOrder->VolumeCondition;
            ///最小成交量 TThostFtdcVolumeType int
            int MinVolume = pInputOrder->MinVolume;
            ///触发条件 TThostFtdcContingentConditionType char
            //// THOST_FTDC_CC_Immediately '1' 立即
            //// THOST_FTDC_CC_Touch '2' 止损
            //// THOST_FTDC_CC_TouchProfit '3' 止赢
            //// THOST_FTDC_CC_ParkedOrder '4' 预埋单
            //// THOST_FTDC_CC_LastPriceGreaterThanStopPrice '5' 最新价大于条件价
            //// THOST_FTDC_CC_LastPriceGreaterEqualStopPrice '6' 最新价大于等于条件价
            //// THOST_FTDC_CC_LastPriceLesserThanStopPrice '7' 最新价小于条件价
            //// THOST_FTDC_CC_LastPriceLesserEqualStopPrice '8' 最新价小于等于条件价
            //// THOST_FTDC_CC_AskPriceGreaterThanStopPrice '9' 卖一价大于条件价
            //// THOST_FTDC_CC_AskPriceGreaterEqualStopPrice 'A' 卖一价大于等于条件价
            //// THOST_FTDC_CC_AskPriceLesserThanStopPrice 'B' 卖一价小于条件价
            //// THOST_FTDC_CC_AskPriceLesserEqualStopPrice 'C' 卖一价小于等于条件价
            //// THOST_FTDC_CC_BidPriceGreaterThanStopPrice 'D' 买一价大于条件价
            //// THOST_FTDC_CC_BidPriceGreaterEqualStopPrice 'E' 买一价大于等于条件价
            //// THOST_FTDC_CC_BidPriceLesserThanStopPrice 'F' 买一价小于条件价
            //// THOST_FTDC_CC_BidPriceLesserEqualStopPrice 'H' 买一价小于等于条件价
            char ContingentCondition = pInputOrder->ContingentCondition;
            ///止损价 TThostFtdcPriceType double
            double StopPrice = pInputOrder->StopPrice;
            ///强平原因 TThostFtdcForceCloseReasonType char
            //// THOST_FTDC_FCC_NotForceClose '0' 非强平
            //// THOST_FTDC_FCC_LackDeposit '1' 资金不足
            //// THOST_FTDC_FCC_ClientOverPositionLimit '2' 客户超仓
            //// THOST_FTDC_FCC_MemberOverPositionLimit '3' 会员超仓
            //// THOST_FTDC_FCC_NotMultiple '4' 持仓非整数倍
            //// THOST_FTDC_FCC_Violation '5' 违规
            //// THOST_FTDC_FCC_Other '6' 其它
            //// THOST_FTDC_FCC_PersonDeliv '7' 自然人临近交割
            char ForceCloseReason = pInputOrder->ForceCloseReason;
            ///自动挂起标志 TThostFtdcBoolType int
            int IsAutoSuspend = pInputOrder->IsAutoSuspend;
            ///业务单元 TThostFtdcBusinessUnitType char[21]
            char BusinessUnit[63];
            gbk2utf8(pInputOrder->BusinessUnit,BusinessUnit,sizeof(BusinessUnit));
            ///请求编号 TThostFtdcRequestIDType int
            int RequestID = pInputOrder->RequestID;
            ///用户强评标志 TThostFtdcBoolType int
            int UserForceClose = pInputOrder->UserForceClose;
            ///互换单标志 TThostFtdcBoolType int
            int IsSwapOrder = pInputOrder->IsSwapOrder;

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
    ///报单录入请求
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 定义调用API的数据结构
    CThostFtdcInputOrderField requestData;
    // 确保没有初始化的数据不会被访问
    memset(&requestData,0,sizeof(requestData));
    // 为调用结构题设置参数信息
    ///经纪公司代码 TThostFtdcBrokerIDType char[11]
    strcpy(requestData.BrokerID,"");
    ///投资者代码 TThostFtdcInvestorIDType char[13]
    strcpy(requestData.InvestorID,"");
    ///合约代码 TThostFtdcInstrumentIDType char[31]
    strcpy(requestData.InstrumentID,"");
    ///报单引用 TThostFtdcOrderRefType char[13]
    strcpy(requestData.OrderRef,"");
    ///用户代码 TThostFtdcUserIDType char[16]
    strcpy(requestData.UserID,"");
    ///报单价格条件 TThostFtdcOrderPriceTypeType char
    //// THOST_FTDC_OPT_AnyPrice '1' 任意价
    //// THOST_FTDC_OPT_LimitPrice '2' 限价
    //// THOST_FTDC_OPT_BestPrice '3' 最优价
    //// THOST_FTDC_OPT_LastPrice '4' 最新价
    //// THOST_FTDC_OPT_LastPricePlusOneTicks '5' 最新价浮动上浮1个ticks
    //// THOST_FTDC_OPT_LastPricePlusTwoTicks '6' 最新价浮动上浮2个ticks
    //// THOST_FTDC_OPT_LastPricePlusThreeTicks '7' 最新价浮动上浮3个ticks
    //// THOST_FTDC_OPT_AskPrice1 '8' 卖一价
    //// THOST_FTDC_OPT_AskPrice1PlusOneTicks '9' 卖一价浮动上浮1个ticks
    //// THOST_FTDC_OPT_AskPrice1PlusTwoTicks 'A' 卖一价浮动上浮2个ticks
    //// THOST_FTDC_OPT_AskPrice1PlusThreeTicks 'B' 卖一价浮动上浮3个ticks
    //// THOST_FTDC_OPT_BidPrice1 'C' 买一价
    //// THOST_FTDC_OPT_BidPrice1PlusOneTicks 'D' 买一价浮动上浮1个ticks
    //// THOST_FTDC_OPT_BidPrice1PlusTwoTicks 'E' 买一价浮动上浮2个ticks
    //// THOST_FTDC_OPT_BidPrice1PlusThreeTicks 'F' 买一价浮动上浮3个ticks
    requestData.OrderPriceType = '0';
    ///买卖方向 TThostFtdcDirectionType char
    //// THOST_FTDC_D_Buy '0' 买
    //// THOST_FTDC_D_Sell '1' 卖
    requestData.Direction = '0';
    ///组合开平标志 TThostFtdcCombOffsetFlagType char[5]
    //// THOST_FTDC_OF_Open '0' 开仓
    //// THOST_FTDC_OF_Close '1' 平仓
    //// THOST_FTDC_OF_ForceClose '2' 强平
    //// THOST_FTDC_OF_CloseToday '3' 平今
    //// THOST_FTDC_OF_CloseYesterday '4' 平昨
    //// THOST_FTDC_OF_ForceOff '5' 强减
    //// THOST_FTDC_OF_LocalForceClose '6' 本地强平
    strcpy(requestData.CombOffsetFlag,"");
    ///组合投机套保标志 TThostFtdcCombHedgeFlagType char[5]
    //// THOST_FTDC_HF_Speculation '1' 投机
    //// THOST_FTDC_HF_Arbitrage '2' 套利
    //// THOST_FTDC_HF_Hedge '3' 套保
    strcpy(requestData.CombHedgeFlag,"");
    ///价格 TThostFtdcPriceType double
    requestData.LimitPrice = 0;
    ///数量 TThostFtdcVolumeType int
    requestData.VolumeTotalOriginal = 0;
    ///有效期类型 TThostFtdcTimeConditionType char
    //// THOST_FTDC_TC_IOC '1' 立即完成，否则撤销
    //// THOST_FTDC_TC_GFS '2' 本节有效
    //// THOST_FTDC_TC_GFD '3' 当日有效
    //// THOST_FTDC_TC_GTD '4' 指定日期前有效
    //// THOST_FTDC_TC_GTC '5' 撤销前有效
    //// THOST_FTDC_TC_GFA '6' 集合竞价有效
    requestData.TimeCondition = '0';
    ///GTD日期 TThostFtdcDateType char[9]
    strcpy(requestData.GTDDate,"");
    ///成交量类型 TThostFtdcVolumeConditionType char
    //// THOST_FTDC_VC_AV '1' 任何数量
    //// THOST_FTDC_VC_MV '2' 最小数量
    //// THOST_FTDC_VC_CV '3' 全部数量
    requestData.VolumeCondition = '0';
    ///最小成交量 TThostFtdcVolumeType int
    requestData.MinVolume = 0;
    ///触发条件 TThostFtdcContingentConditionType char
    //// THOST_FTDC_CC_Immediately '1' 立即
    //// THOST_FTDC_CC_Touch '2' 止损
    //// THOST_FTDC_CC_TouchProfit '3' 止赢
    //// THOST_FTDC_CC_ParkedOrder '4' 预埋单
    //// THOST_FTDC_CC_LastPriceGreaterThanStopPrice '5' 最新价大于条件价
    //// THOST_FTDC_CC_LastPriceGreaterEqualStopPrice '6' 最新价大于等于条件价
    //// THOST_FTDC_CC_LastPriceLesserThanStopPrice '7' 最新价小于条件价
    //// THOST_FTDC_CC_LastPriceLesserEqualStopPrice '8' 最新价小于等于条件价
    //// THOST_FTDC_CC_AskPriceGreaterThanStopPrice '9' 卖一价大于条件价
    //// THOST_FTDC_CC_AskPriceGreaterEqualStopPrice 'A' 卖一价大于等于条件价
    //// THOST_FTDC_CC_AskPriceLesserThanStopPrice 'B' 卖一价小于条件价
    //// THOST_FTDC_CC_AskPriceLesserEqualStopPrice 'C' 卖一价小于等于条件价
    //// THOST_FTDC_CC_BidPriceGreaterThanStopPrice 'D' 买一价大于条件价
    //// THOST_FTDC_CC_BidPriceGreaterEqualStopPrice 'E' 买一价大于等于条件价
    //// THOST_FTDC_CC_BidPriceLesserThanStopPrice 'F' 买一价小于条件价
    //// THOST_FTDC_CC_BidPriceLesserEqualStopPrice 'H' 买一价小于等于条件价
    requestData.ContingentCondition = '0';
    ///止损价 TThostFtdcPriceType double
    requestData.StopPrice = 0;
    ///强平原因 TThostFtdcForceCloseReasonType char
    //// THOST_FTDC_FCC_NotForceClose '0' 非强平
    //// THOST_FTDC_FCC_LackDeposit '1' 资金不足
    //// THOST_FTDC_FCC_ClientOverPositionLimit '2' 客户超仓
    //// THOST_FTDC_FCC_MemberOverPositionLimit '3' 会员超仓
    //// THOST_FTDC_FCC_NotMultiple '4' 持仓非整数倍
    //// THOST_FTDC_FCC_Violation '5' 违规
    //// THOST_FTDC_FCC_Other '6' 其它
    //// THOST_FTDC_FCC_PersonDeliv '7' 自然人临近交割
    requestData.ForceCloseReason = '0';
    ///自动挂起标志 TThostFtdcBoolType int
    requestData.IsAutoSuspend = 0;
    ///业务单元 TThostFtdcBusinessUnitType char[21]
    strcpy(requestData.BusinessUnit,"");
    ///请求编号 TThostFtdcRequestIDType int
    requestData.RequestID = 0;
    ///用户强评标志 TThostFtdcBoolType int
    requestData.UserForceClose = 0;
    ///互换单标志 TThostFtdcBoolType int
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