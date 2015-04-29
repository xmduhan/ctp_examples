# CTP API 使用范例
- 该项目主要是为帮助生成CTP API测试片段，降低CTP接口的学习难度。
- 目前仅支持在linux下用gcc编译使用。

# 安装CppHeaderParser
sudo pip install ply
sudo pip install CppHeaderParser

# 安装模板生成工 jinja2
sudo pip install jinja2

# 安装astyle代码美化工具
sudo apt-get install astyle


# 安装接口api
- 解压
- unzip 6.2.5_20140811_apitraderapi_linux64.zip

- 创建安装目录
- sudo mkdir -p /etc/ctp/include
- sudo mkdir -p /etc/ctp/lib

- 拷贝文件
- sudo cp *.h /etc/ctp/include
- sudo cp *.so *.xml *.dtd /etc/ctp/lib


# 使用前需要设置环境变量
- export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/etc/ctp/include
- export CTP_FrontAddress="tcp://x.x.x.x:51205"
- export CTP_BrokerId=xxxxxx
- export CTP_UserId=xxxxxxxx
- export CTP_Password=xxxxxx

# 创建API调用框架
- create_api_tester.py这个命令可以帮我们生成一个特定CTP接口API的调用测试框架，其具体使用方法如下：
- $ python create_api_tester.py API名称
- 其中API名称形如QryTradingAccount，对应的请求函数为ReqQryTradingAccount，对应的相应函数为OnRspQryTradingAccount。
- create_api_tester.py执行成功会创建以API名称命名的目录（如:QryTradingAccount,OrderInsert,QryDepthMarketData等)，目录中有一个c++源文件和一个makefile。用编辑器打开c++源文件，设置调用参数，已经对响应函数编写相应的处理逻辑（比如：在屏幕上打印返回的参数信息）。
- 然后可以使用以下命令进行编译和执行：
- $ make 
- ./execute

# 基本测试例子
- 除了API调用生成框架外，项目还提供了几个例子程序
- hello 测试CTP API环境的安装和工作是否正常
- tradeapitest 官方的API参考文档中的例子(程序只能运行，但订单无法提交成功，需要提交逻辑，使用创建API调用框架生成OrderInsert目录。
- login 一个登录登出的例子
- thread linux线程同步例子
