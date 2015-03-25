# -*- coding: utf8 -*-

import sys
from jinja2 import Environment, FileSystemLoader




def main():
	'''
	从命令行上获取一个API名称（不含Req和OnRsp前缀），生成相关的API组合的测试代码
	命令行参数1  API名称（如:QryTradingAccount，对应的请求函数为ReqQryTradingAccount，对应的相应函数为OnRspQryTradingAccount）  
	'''
	if len(sys.argv) != 2 :
		print '命令使用格式不正确,请参照以下例子:'
		print '$ python create_api_tester.py API名称'
		print '其中API名称形如QryTradingAccount，对应的请求函数为ReqQryTradingAccount，对应的相应函数为OnRspQryTradingAccount。'
		return(0)
	
	apiName = sys.argv[1]
	print '开始生成%s的接口测试代码:...' % apiName


	# 读取模板信息
	env = Environment(loader=FileSystemLoader('templates'))
	source = env.get_template('source.tpl.cpp')
	#print source.render()
		



if __name__ == '__main__':
    main()
