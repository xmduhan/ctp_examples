# -*- coding: utf8 -*-

import sys
import os
import cpphelper
from jinja2 import Environment, FileSystemLoader
from os import path



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

	# 测试使用
	apiName = 'QryInstrument'


	
	# 读取相关cpp头文件	
	ThostFtdcTraderApi_h = cpphelper.getCppHeader('api/ThostFtdcTraderApi.h',['TRADER_API_EXPORT'])
	ThostFtdcUserApiStruct_h = cpphelper.getCppHeader('api/ThostFtdcUserApiStruct.h')
	typedefDict = cpphelper.getTypedefDict('api/ThostFtdcUserApiDataType.h')

	
	# 获取响应函数相关信息
	responseMethodName = 'OnRsp%s' % apiName
	CThostFtdcTraderSpi = cpphelper.getClass(ThostFtdcTraderApi_h,'CThostFtdcTraderSpi')
	responseMethod = cpphelper.getClassMethod(CThostFtdcTraderSpi,'public',responseMethodName)
	respParameters = cpphelper.getMethodParameters(responseMethod)

	# 获取调用函数相关信息
	requestMethodName = 'Req%s' % apiName
	CThostFtdcTraderApi = cpphelper.getClass(ThostFtdcTraderApi_h,'CThostFtdcTraderApi')
	requestMethod = cpphelper.getClassMethod(CThostFtdcTraderApi,'public',requestMethodName)
	reqParameters = cpphelper.getMethodParameters(requestMethod)	
	
	# 检查请求函数的参数格式是否符合预期
	methedDeclare = '%s(%s)' % (requestMethod['name'],
		','.join('%s %s' % (parameter['type'],parameter['name']) for parameter in reqParameters))
	if len(reqParameters) != 2 : 
		print '无法处理的请求API结构:%s' % methedDeclare
		return(0)
	if reqParameters[1]['name'] != 'nRequestID' :
		print '无法处理的请求API结构:%s' % methedDeclare
		return(0) 
	if reqParameters[0]['pointer'] == False :
		print '无法处理的请求API结构:%s' % methedDeclare
		return(0)
	
	# 读取请求类型的所有字段列表和原始类型
	requestDataStruct = cpphelper.getClass(ThostFtdcUserApiStruct_h,reqParameters[0]['raw_type'])
	requestFields = cpphelper.getStructFields(requestDataStruct)
	for field in requestFields:
		typedef = typedefDict[ field['type'] ]
		field['original'] = typedef['type']
		field['len'] = typedef['len']
		#print '%s %s %s %s %s' % (field['name'],field['type'],field['doxygen'],field['original'],field['len'])		
		
	
	# 生成模板所需的信息集
	data = {
		'apiName' : apiName,
		'responseMethod' : responseMethod,
		'respParameters' : respParameters,
		'requestMethod' : requestMethod,
		'reqParameters' : reqParameters,
		'requestFields' : requestFields,
	}


	# 读取模板信息
	env = Environment(loader=FileSystemLoader('templates'))
	
	# 生成代码文件
	source = env.get_template('source.tpl.cpp')
	sourcePath = path.join(apiName,'%s.cpp' % apiName)
	with open(sourcePath, 'w') as f :
		# 间模板生成到文件
		f.write(source.render(**data).encode('utf-8'))
	# 使用astyle对文件进行格式化	
	os.system('astyle %s' % sourcePath)	
	
	# 生成make文件
	makefile = env.get_template('Makefile.tpl')
	makefilePath = path.join(apiName,'Makefile')
	with open(makefilePath, 'w') as f :
		f.write(makefile.render(**data).encode('utf-8'))



if __name__ == '__main__':
    main()


