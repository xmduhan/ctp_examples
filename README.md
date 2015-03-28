# ctp_examples
CTP API 使用范例

# 使用前需要设置环境变量
export CTP_FrontAddress="tcp://x.x.x.x:51205"
export CTP_BrokerId=xxxxxx
export CTP_UserId=xxxxxxxx
export CTP_Password=xxxxxx

# 基本测试例子
- hello 测试CTP API环境的安装和工作是否正常
- tradeapitest 官方的API参考文档中的例子
- login 一个登录登出的例子
- thread linux线程同步例子

# 使用命令创建API调用框架
$ python create_api_tester.py API名称
其中API名称形如QryTradingAccount，对应的请求函数为ReqQryTradingAccount，对应的相应函数为OnRspQryTradingAccount。
create_api_tester.py执行成功会创建以API名称命名的目录（如:QryTradingAccount,OrderInsert,QryDepthMarketData等)，目录中有一个c++源文件和一个makefile。使用以下命令进行编译和执行：
$ make 
./API名称

