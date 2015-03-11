// tradeapitest.cpp :
// 一个简单的例子，介绍CThostFtdcTraderApi和CThostFtdcTraderSpi接口的使用。
// 本例将演示一个报单录入操作的过程



// 报单录入操作是否完成的标志
// Create a manual reset event with no signal
// 这段需要注释掉，这是windows下的代码
//HANDLE g_hEvent = CreateEvent(NULL, true, false, NULL);

// 会员代码
//TThostFtdcBrokerIDType g_chBrokerID;
// 交易用户代码
//TThostFtdcUserIDType g_chUserID;

using namespace std;



class QuoteMdSpi:CThostFtdcMdSpi{

};


//class CSimpleHandler : CThostFtdcTraderSpi{
class CSimpleHandler{

    public:
    // 构造函数，需要一个有效的指向CThostFtdcMduserApi实例的指针
    //CSimpleHandler(CThostFtdcTraderApi *pUserApi) : m_pUserApi(pUserApi) {}
    //~CSimpleHandler() {}

    // 当客户端与交易托管系统建立起通信连接，客户端需要进行登录
    /*
	virtual void OnFrontConnected()
    {
        CThostFtdcReqUserLoginField reqUserLogin;
        // get BrokerID
        printf("BrokerID:");
        scanf("%s", &g_chBrokerID);
        strcpy(reqUserLogin. BrokerID, g_chBrokerID);
        // get userid
        printf("userid:");
        scanf("%s", &g_chUserID);
        strcpy(reqUserLogin.UserID, g_chUserID);
        // get password
        printf("password:");
        scanf("%s", &reqUserLogin.Password);
        // 发出登陆请求
        m_pUserApi->ReqUserLogin(&reqUserLogin, 0);
    }
	//*/

    // 当客户端与交易托管系统通信连接断开时，该方法被调用
    /*
	virtual void OnFrontDisconnected(int nReason)
    {
      // 当发生这个情况后，API会自动重新连接，客户端可不做处理
      printf("OnFrontDisconnected.\n");
    }
	//*/

    // 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
    /*
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        printf("OnRspUserLogin:\n");
        printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,
            pRspInfo->ErrorMsg);
        printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
        if (pRspInfo->ErrorID != 0) {
            // 端登失败，客户端需进行错误处理
            printf("Failed to login, errorcode=%d errormsg=%s requestid=%d
            chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
            exit(-1);
        }
        // 端登成功,发出报单录入请求
        CThostFtdcInputOrderField ord;
        memset(&ord, 0, sizeof(ord));
        //经纪公司代码
        strcpy(ord.BrokerID, g_chBrokerID);
        //投资者代码
        strcpy(ord.InvestorID, "12345");
        // 合约代码
        strcpy(ord.InstrumentID, "cn0601");
        ///报单引用
        strcpy(ord.OrderRef, "000000000001");
        // 用户代码
        strcpy(ord.UserID, g_chUserID);
        // 报单价格条件
        ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
        // 买卖方向
        ord.Direction = THOST_FTDC_D_Buy;
        // 组合开平标志
        strcpy(ord.CombOffsetFlag, "0");
        // 组合投机套保标志
        strcpy(ord.CombHedgeFlag, "1");
        // 价格
        ord.LimitPrice = 50000;
        // 数量
        ord.VolumeTotalOriginal = 10;
        // 有效期类型
        ord.TimeCondition = THOST_FTDC_TC_GFD;
        // GTD日期
        strcpy(ord.GTDDate, "");
        // 成交量类型
        ord.VolumeCondition = THOST_FTDC_VC_AV;
        // 最小成交量
        ord.MinVolume = 0;
        // 触发条件
        ord.ContingentCondition = THOST_FTDC_CC_Immediately;
        // 止损价
        ord.StopPrice = 0;
        // 强平原因
        ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
        // 自动挂起标志
        ord.IsAutoSuspend = 0;
        m_pUserApi->ReqOrderInsert(&ord, 1);
    }
    //*/
	
	// 报单录入应答
    /*
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        // 输出报单录入结果
        printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,
        pRspInfo->ErrorMsg);
        // 通知报单录入完成
        SetEvent(g_hEvent);
    };
	//*/


    ///报单回报
    /*
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder)
    {
        printf("OnRtnOrder:\n");
        printf("OrderSysID=[%s]\n", pOrder->OrderSysID);
    }
	//*/

    // 针对用户请求的出错通知
    /*
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
        bool bIsLast)
    {
        printf("OnRspError:\n");
        printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,
        pRspInfo->ErrorMsg);
        printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
        // 客户端需进行错误处理
        //{客户端的错误处理}
    }
	//*/

    private:
    // 指向CThostFtdcMduserApi实例的指针
    //CThostFtdcTraderApi *m_pUserApi;
};



int main()
{
    // 产生一个CThostFtdcTraderApi实例
    //CThostFtdcTraderApi *pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
    // 产生一个事件处理的实例
    //CSimpleHandler sh(pUserApi);
    // 注册一事件处理的实例
    //pUserApi->RegisterSpi(&sh);
    // 订阅私有流
    // TERT_RESTART:从本交易日开始重传
    // TERT_RESUME:从上次收到的续传
    // TERT_QUICK:只传送登录后私有流的内容
    //pUserApi->SubscribePrivateTopic(TERT_RESUME);
    // 订阅公共流
    // TERT_RESTART:从本交易日开始重传
    // TERT_RESUME:从上次收到的续传
    // TERT_QUICK:只传送登录后公共流的内容
    //pUserApi->SubscribePublicTopic(TERT_RESUME);
    // 设置交易托管系统服务的地址，可以注册多个地址备用
    //pUserApi->RegisterFront("tcp://172.16.0.31:57205");
    // 使客户端开始与后台服务建立连接
    //pUserApi->Init();
    // 客户端等待报单操作完成
    // 这段必须注释掉，因为这是windows下的API
    //WaitForSingleObject(g_hEvent, INFINITE);
    // 释放API实例
    //pUserApi->Release();
    
	return 0;
}
