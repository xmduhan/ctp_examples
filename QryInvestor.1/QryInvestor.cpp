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

    ///请求查询投资者响应
    virtual void OnRspQryInvestor(
        CThostFtdcInvestorField * pInvestor,
        CThostFtdcRspInfoField * pRspInfo,
        int nRequestID,
        bool bIsLast
    ) {
        printf("OnRspQryInvestor():被执行...\n");

        // 进程返回结果检查
        if ( (pRspInfo) && (pRspInfo->ErrorID != 0) )  {
            // typedef int TThostFtdcErrorIDType;
            // typedef char TThostFtdcErrorMsgType[81];
            char ErrorMsg[243];
            gbk2utf8(pRspInfo->ErrorMsg,ErrorMsg,sizeof(ErrorMsg));
            printf("OnRspQryInvestor():出错:ErrorId=%d,ErrorMsg=%s\n",pRspInfo->ErrorID,ErrorMsg);
        }

        // 如果有返回结果读取返回信息
        if ( pInvestor != NULL ) {
            printf("pInvestor != NULL\n");
			// 读取返回信息,并做编码转化
            ///投资者代码 TThostFtdcInvestorIDType char[13]
            char InvestorID[39];
            gbk2utf8(pInvestor->InvestorID,InvestorID,sizeof(InvestorID));
            ///经纪公司代码 TThostFtdcBrokerIDType char[11]
            char BrokerID[33];
            gbk2utf8(pInvestor->BrokerID,BrokerID,sizeof(BrokerID));
            ///投资者分组代码 TThostFtdcInvestorIDType char[13]
            char InvestorGroupID[39];
            gbk2utf8(pInvestor->InvestorGroupID,InvestorGroupID,sizeof(InvestorGroupID));
            ///投资者名称 TThostFtdcPartyNameType char[81]
            char InvestorName[243];
            gbk2utf8(pInvestor->InvestorName,InvestorName,sizeof(InvestorName));
            ///证件类型 TThostFtdcIdCardTypeType char
            //// THOST_FTDC_ICT_EID '0' 组织机构代码
            //// THOST_FTDC_ICT_IDCard '1' 中国公民身份证
            //// THOST_FTDC_ICT_OfficerIDCard '2' 军官证
            //// THOST_FTDC_ICT_PoliceIDCard '3' 警官证
            //// THOST_FTDC_ICT_SoldierIDCard '4' 士兵证
            //// THOST_FTDC_ICT_HouseholdRegister '5' 户口簿
            //// THOST_FTDC_ICT_Passport '6' 护照
            //// THOST_FTDC_ICT_TaiwanCompatriotIDCard '7' 台胞证
            //// THOST_FTDC_ICT_HomeComingCard '8' 回乡证
            //// THOST_FTDC_ICT_LicenseNo '9' 营业执照号
            //// THOST_FTDC_ICT_TaxNo 'A' 税务登记号/当地纳税ID
            //// THOST_FTDC_ICT_HMMainlandTravelPermit 'B' 港澳居民来往内地通行证
            //// THOST_FTDC_ICT_TwMainlandTravelPermit 'C' 台湾居民来往大陆通行证
            //// THOST_FTDC_ICT_DrivingLicense 'D' 驾照
            //// THOST_FTDC_ICT_SocialID 'F' 当地社保ID
            //// THOST_FTDC_ICT_LocalID 'G' 当地身份证
            //// THOST_FTDC_ICT_BusinessRegistration 'H' 商业登记证
            //// THOST_FTDC_ICT_HKMCIDCard 'I' 港澳永久性居民身份证
            //// THOST_FTDC_ICT_OtherCard 'x' 其他证件
            char IdentifiedCardType = pInvestor->IdentifiedCardType;
            ///证件号码 TThostFtdcIdentifiedCardNoType char[51]
            char IdentifiedCardNo[153];
            gbk2utf8(pInvestor->IdentifiedCardNo,IdentifiedCardNo,sizeof(IdentifiedCardNo));
            ///是否活跃 TThostFtdcBoolType int
            int IsActive = pInvestor->IsActive;
            ///联系电话 TThostFtdcTelephoneType char[41]
            char Telephone[123];
            gbk2utf8(pInvestor->Telephone,Telephone,sizeof(Telephone));
            ///通讯地址 TThostFtdcAddressType char[101]
            char Address[303];
            gbk2utf8(pInvestor->Address,Address,sizeof(Address));
            ///开户日期 TThostFtdcDateType char[9]
            char OpenDate[27];
            gbk2utf8(pInvestor->OpenDate,OpenDate,sizeof(OpenDate));
            ///手机 TThostFtdcMobileType char[41]
            char Mobile[123];
            gbk2utf8(pInvestor->Mobile,Mobile,sizeof(Mobile));
            ///手续费率模板代码 TThostFtdcInvestorIDType char[13]
            char CommModelID[39];
            gbk2utf8(pInvestor->CommModelID,CommModelID,sizeof(CommModelID));
            ///保证金率模板代码 TThostFtdcInvestorIDType char[13]
            char MarginModelID[39];
            gbk2utf8(pInvestor->MarginModelID,MarginModelID,sizeof(MarginModelID));

			printf("InvestorID=%s\n",InvestorID);  //投资者代码
			printf("BrokerID=%s\n",BrokerID);  // 经纪公司代码
			printf("InvestorName=%s\n",InvestorName);  // 投资者名称
            printf("IdentifiedCardType=%c\n",IdentifiedCardType); // 证件类型
			printf("IdentifiedCardNo=%s\n",IdentifiedCardNo);  // 证件号码
            printf("IsActive=%d\n",IsActive); // 是否活跃
            printf("OpenDate=%s\n",OpenDate);  // 开户日期	

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
    ///请求查询投资者
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 定义调用API的数据结构
    CThostFtdcQryInvestorField requestData;
    // 确保没有初始化的数据不会被访问
    memset(&requestData,0,sizeof(requestData));
    // 为调用结构题设置参数信息
    ///经纪公司代码 TThostFtdcBrokerIDType char[11]
    strcpy(requestData.BrokerID,"");
    ///投资者代码 TThostFtdcInvestorIDType char[13]
    strcpy(requestData.InvestorID,"");


    // 调用API,并等待响应函数返回
    int result = pTraderApi->ReqQryInvestor(&requestData,requestID++);
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
