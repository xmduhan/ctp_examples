

#include <stdio.h>
#include "login.h"
#include <ThostFtdcMdApi.h>
using namespace std;



class QuoteMdSpi:CThostFtdcMdSpi{
	private :
		int nReq;
		CThostFtdcMdApi * md;
	
	// 定义构造函数和析构函数
	public:
		QuoteMdSpi(int reqid){
			nReq=reqid;
		}
		~QuoteMdSpi(void){};
	
	// 重载父类的虚拟函数
	virtual void Init(){};
	
	virtual void OnFrontConnected(){};

	virtual void OnFrontDisconnected(int nReason){};
	
	virtual void OnHeartBeatWarning(int nTimeLapse){};

	virtual void OnRspUserLogin(
		CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};

	virtual void OnRspUserLogout(
		CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast) {};
 
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	virtual void OnRspSubMarketData(
		CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	virtual void OnRspUnSubMarketData(
		CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {};

};


QuoteMdSpi spi = QuoteMdSpi(0);

CThostFtdcMdApi *pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();

int main(){

    printf("hello ctp!\n");
    return 0;
}


