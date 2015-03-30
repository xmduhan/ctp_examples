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

    ///请求查询资金账户响应
    virtual void OnRspQryTradingAccount(
        CThostFtdcTradingAccountField * pTradingAccount,
        CThostFtdcRspInfoField * pRspInfo,
        int nRequestID,
        bool bIsLast
    ) {
        printf("OnRspQryTradingAccount():被执行...\n");

        // 进程返回结果检查
        if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
            // typedef int TThostFtdcErrorIDType;
            // typedef char TThostFtdcErrorMsgType[81];
            char ErrorMsg[243];
            gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
            printf("OnRspQryTradingAccount():出错:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        }

        // 如果有返回结果读取返回信息
        if ( pTradingAccount != NULL ) {
            // 读取返回信息,并做编码转化
            ///经纪公司代码 TThostFtdcBrokerIDType char[11]
            char BrokerID[33];
            gbk2utf8(pTradingAccount->BrokerID,BrokerID,sizeof(BrokerID));
            ///投资者帐号 TThostFtdcAccountIDType char[13]
            char AccountID[39];
            gbk2utf8(pTradingAccount->AccountID,AccountID,sizeof(AccountID));
			printf("len(pTradingAccount->AccountID)=%d\n",(int)strlen(pTradingAccount->AccountID));
			printf("len(AccountID)=%d\n",(int)strlen(AccountID));
            ///上次质押金额 TThostFtdcMoneyType double
            double PreMortgage = pTradingAccount->PreMortgage;
            ///上次信用额度 TThostFtdcMoneyType double
            double PreCredit = pTradingAccount->PreCredit;
            ///上次存款额 TThostFtdcMoneyType double
            double PreDeposit = pTradingAccount->PreDeposit;
            ///上次结算准备金 TThostFtdcMoneyType double
            double PreBalance = pTradingAccount->PreBalance;
            ///上次占用的保证金 TThostFtdcMoneyType double
            double PreMargin = pTradingAccount->PreMargin;
            ///利息基数 TThostFtdcMoneyType double
            double InterestBase = pTradingAccount->InterestBase;
            ///利息收入 TThostFtdcMoneyType double
            double Interest = pTradingAccount->Interest;
            ///入金金额 TThostFtdcMoneyType double
            double Deposit = pTradingAccount->Deposit;
            ///出金金额 TThostFtdcMoneyType double
            double Withdraw = pTradingAccount->Withdraw;
            ///冻结的保证金 TThostFtdcMoneyType double
            double FrozenMargin = pTradingAccount->FrozenMargin;
            ///冻结的资金 TThostFtdcMoneyType double
            double FrozenCash = pTradingAccount->FrozenCash;
            ///冻结的手续费 TThostFtdcMoneyType double
            double FrozenCommission = pTradingAccount->FrozenCommission;
            ///当前保证金总额 TThostFtdcMoneyType double
            double CurrMargin = pTradingAccount->CurrMargin;
            ///资金差额 TThostFtdcMoneyType double
            double CashIn = pTradingAccount->CashIn;
            ///手续费 TThostFtdcMoneyType double
            double Commission = pTradingAccount->Commission;
            ///平仓盈亏 TThostFtdcMoneyType double
            double CloseProfit = pTradingAccount->CloseProfit;
            ///持仓盈亏 TThostFtdcMoneyType double
            double PositionProfit = pTradingAccount->PositionProfit;
            ///期货结算准备金 TThostFtdcMoneyType double
            double Balance = pTradingAccount->Balance;
            ///可用资金 TThostFtdcMoneyType double
            double Available = pTradingAccount->Available;
            ///可取资金 TThostFtdcMoneyType double
            double WithdrawQuota = pTradingAccount->WithdrawQuota;
            ///基本准备金 TThostFtdcMoneyType double
            double Reserve = pTradingAccount->Reserve;
            ///交易日 TThostFtdcDateType char[9]
            char TradingDay[27];
            gbk2utf8(pTradingAccount->TradingDay,TradingDay,sizeof(TradingDay));
            ///结算编号 TThostFtdcSettlementIDType int
            int SettlementID = pTradingAccount->SettlementID;
            ///信用额度 TThostFtdcMoneyType double
            double Credit = pTradingAccount->Credit;
            ///质押金额 TThostFtdcMoneyType double
            double Mortgage = pTradingAccount->Mortgage;
            ///交易所保证金 TThostFtdcMoneyType double
            double ExchangeMargin = pTradingAccount->ExchangeMargin;
            ///投资者交割保证金 TThostFtdcMoneyType double
            double DeliveryMargin = pTradingAccount->DeliveryMargin;
            ///交易所交割保证金 TThostFtdcMoneyType double
            double ExchangeDeliveryMargin = pTradingAccount->ExchangeDeliveryMargin;
            ///保底期货结算准备金 TThostFtdcMoneyType double
            double ReserveBalance = pTradingAccount->ReserveBalance;
            ///币种代码 TThostFtdcCurrencyIDType char[4]
            char CurrencyID[12];
            gbk2utf8(pTradingAccount->CurrencyID,CurrencyID,sizeof(pTradingAccount->CurrencyID));
            ///上次货币质入金额 TThostFtdcMoneyType double
            double PreFundMortgageIn = pTradingAccount->PreFundMortgageIn;
            ///上次货币质出金额 TThostFtdcMoneyType double
            double PreFundMortgageOut = pTradingAccount->PreFundMortgageOut;
            ///货币质入金额 TThostFtdcMoneyType double
            double FundMortgageIn = pTradingAccount->FundMortgageIn;
            ///货币质出金额 TThostFtdcMoneyType double
            double FundMortgageOut = pTradingAccount->FundMortgageOut;
            ///货币质押余额 TThostFtdcMoneyType double
            double FundMortgageAvailable = pTradingAccount->FundMortgageAvailable;
            ///可质押货币金额 TThostFtdcMoneyType double
            double MortgageableFund = pTradingAccount->MortgageableFund;
            ///特殊产品占用保证金 TThostFtdcMoneyType double
            double SpecProductMargin = pTradingAccount->SpecProductMargin;
            ///特殊产品冻结保证金 TThostFtdcMoneyType double
            double SpecProductFrozenMargin = pTradingAccount->SpecProductFrozenMargin;
            ///特殊产品手续费 TThostFtdcMoneyType double
            double SpecProductCommission = pTradingAccount->SpecProductCommission;
            ///特殊产品冻结手续费 TThostFtdcMoneyType double
            double SpecProductFrozenCommission = pTradingAccount->SpecProductFrozenCommission;
            ///特殊产品持仓盈亏 TThostFtdcMoneyType double
            double SpecProductPositionProfit = pTradingAccount->SpecProductPositionProfit;
            ///特殊产品平仓盈亏 TThostFtdcMoneyType double
            double SpecProductCloseProfit = pTradingAccount->SpecProductCloseProfit;
            ///根据持仓盈亏算法计算的特殊产品持仓盈亏 TThostFtdcMoneyType double
            double SpecProductPositionProfitByAlg = pTradingAccount->SpecProductPositionProfitByAlg;
            ///特殊产品交易所保证金 TThostFtdcMoneyType double
            double SpecProductExchangeMargin = pTradingAccount->SpecProductExchangeMargin;

			// 打印出关键信息
			printf("经纪公司代码=%s,投资者帐号=%s,币种代码=%s,可用金额=%.2f\n",BrokerID,AccountID,CurrencyID,Available);

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
    ///请求查询资金账户
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 定义调用API的数据结构
    CThostFtdcQryTradingAccountField requestData;
    // 确保没有初始化的数据不会被访问
    memset(&requestData,0,sizeof(requestData));
    // 为调用结构题设置参数信息
    ///经纪公司代码 TThostFtdcBrokerIDType char[11]
    strcpy(requestData.BrokerID,"");
    ///投资者代码 TThostFtdcInvestorIDType char[13]
    strcpy(requestData.InvestorID,"");
    ///币种代码 TThostFtdcCurrencyIDType char[4]
    strcpy(requestData.CurrencyID,"");


    // 调用API,并等待响应函数返回
    int result = pTraderApi->ReqQryTradingAccount(&requestData,requestID++);
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
