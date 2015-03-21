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

char * serverAddress = "tcp://101.231.96.18:51205";


class CTraderHandler : public CThostFtdcTraderSpi{

    CTraderHandler(){
        printf("CTraderHandler:called.\n");
    }

    virtual void OnFrontConnected() {

        printf("OnFrontConnected:called.\n");
    }

    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
          CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){

        printf("OnRspUserLogin:called\n");
    }


};


int main(){

    // 创建TraderAPI和回调响应控制器的实例
    CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
    CTraderHandler traderHandler = CTraderHandler();
    CTraderHandler * pTraderHandler = &traderHandler;
    pTraderApi->RegisterSpi(pTraderHandler);

    // 设置服务器地址
    pTraderApi->RegisterFront(serverAddress);
    // 链接交易系统
    pUserApi->Init();

    return(0);
}
