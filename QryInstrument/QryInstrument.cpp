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


// 服务器地址
char * serverAddress = (char *)"tcp://101.231.96.18:51205";
// 登录请求结构体
CThostFtdcReqUserLoginField userLoginField;
// 用户请求结构体
CThostFtdcUserLogoutField userLogoutField;
// 线程同步标志
sem_t sem;

// 合约查询结构

// requestID
int requestID = 0;


class CTraderHandler : public CThostFtdcTraderSpi{

	public:

    CTraderHandler(){
        printf("CTraderHandler:called.\n");
    }

	// 允许登录事件
    virtual void OnFrontConnected() {
		static int i = 0;
		// 在登出后系统会重新调用OnFrontConnected，这里简单判断并忽略第1次之后的所有调用。
		if (i++==0) {
			printf("OnFrontConnected:called.\n");
			sem_post(&sem);
		}
    }

	// 登录结果响应
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
    	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
        printf("OnRspUserLogin:called\n");
		if (pRspInfo->ErrorID == 0) {
			printf("登录成功!\n");
			sem_post(&sem);
		}else{
			printf("登录失败!\n");
		}
    }

	// 登出结果响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		printf("OnReqUserLogout:called\n");
		if (pRspInfo->ErrorID == 0) {
			printf("登出成功!\n");
			sem_post(&sem);
		}else{
			printf("登出失败!\n");
		}
	}

	// 查询合约结果响应
	//virtual void OnRspQryInstrument
	//	(ThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast) {
		//static int i=0;
		//char InstrumentName[100];
		//codeConvert((char *)"GBK",(char*)"UTF8",pInstrument->InstrumentName,InstrumentName,sizeof(InstrumentName));			
		//printf("InstrumentID=%s,ExchangeID=%s,InstrumentName=%s\n",
		//pInstrument->InstrumentID,pInstrument->ExchangeID,InstrumentName);
		//i++;
		//if(bIsLast){
		//	printf("一共有%d合约可供交易\n",i);
		//	sem_post(&sem);
		//}	
	//}

	virtual void OnRspQryInstrument(
		CThostFtdcInstrumentField * pInstrument,
		CThostFtdcRspInfoField * pRspInfo,
		int nRequestID,
		bool bIsLast){

	}

};


int main(){

	// 初始化线程同步变量
	sem_init(&sem,0,0);


	// 创建TraderAPI和回调响应控制器的实例
	CThostFtdcTraderApi *pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
	CTraderHandler traderHandler = CTraderHandler();
	CTraderHandler * pTraderHandler = &traderHandler;
	pTraderApi->RegisterSpi(pTraderHandler);

	// 设置服务器地址
	pTraderApi->RegisterFront(serverAddress);
	// 链接交易系统
	pTraderApi->Init();

	// 等待服务器发出登录消息
	sem_wait(&sem);

	// 读取登录信息
	printf("BrokerID:");
	scanf("%s", (char*) &userLoginField.BrokerID);

	printf("userid:");
	scanf("%s", (char*) &userLoginField.UserID);

	printf("password:");
	scanf("%s", (char*) &userLoginField.Password);

	// 发出登陆请求
	pTraderApi->ReqUserLogin(&userLoginField, requestID++);

	// 等待登录成功消息
	sem_wait(&sem);

	// 查询合约	
	//CThostFtdcQryInstrumentField qryInstrumentField;
	//memset(&qryInstrumentField,0,sizeof(qryInstrumentField));	
	//int result = pTraderApi->ReqQryInstrument(&qryInstrumentField,requestID++);
	//sem_wait(&sem);

	// 调用API请求函数
	pQryInstrument requestData;
	int result = pTraderApi->ReqQryInstrument(&requestData,requestID++);
	
	
	

	// 拷贝用户登录信息到登出信息
	strcpy(userLogoutField.BrokerID,userLoginField.BrokerID);
	strcpy(userLogoutField.UserID, userLoginField.UserID);
	pTraderApi->ReqUserLogout(&userLogoutField, requestID++);

	// 等待登出成功
	sem_wait(&sem);


	printf("主线程执行完毕!\n");
	return(0);


}