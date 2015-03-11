
#include "hello.h"

int main(){
		// 测试调用CTP的API
		// 这里访问了CTP的库函数，如果程序能过正常编译并执行，说明头文件引用及库连接正常
		CThostFtdcMdApi *pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
		printf("Hello CTP!\n");
    return 0;
}
