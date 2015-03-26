# ctp_examples
CTP API 使用范例

- hello 测试CTP API环境的安装和工作是否正常
- tradeapitest 官方的API参考文档中的例子


- 使用前需要设置环境变量
export CTP_BrokerId=xxxxxx
export CTP_UserId=xxxxxxxx
export CTP_Password=xxxxxx

- 使用命令创建API调用框架
python create_api_tester.py API名称


其中API名称形如QryTradingAccount，对应的请求函数为ReqQryTradingAccount，对应的相应函数为OnRspQryTradingAccount。

