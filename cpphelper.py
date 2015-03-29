# -*- coding: utf-8 -*-

import re
import CppHeaderParser


def getCppHeader(filename,ignoreSymbols=[]):
	'''
	通过一个文件名获得一个CppHeader结构数据
	filename 文件名称(含路径)
	ignoreSymbols  需要忽略的符号
	返回 CppHeader结构
	'''
	#TODO : 这里存在一个问题，如果使用了ignoreSymbols由于使用CppHeaderParser.ignoreSymbols.extend(ignoreSymbols)
	#       将影响到后续的调用，这个影响无法清楚，一直要等到重启解释器。
	
	if ignoreSymbols and type(ignoreSymbols) == list:
		CppHeaderParser.ignoreSymbols.extend(ignoreSymbols)	
	return CppHeaderParser.CppHeader(filename)


def getClass(header,className):
	'''
	通过一个类名称在一个CppHeader中查找一个Class结构(含struct)
	header  CppHeader结构
	className 要查找的类名称
	返回 Class结构
	'''
	return header.classes[className]


def getClassMethod(aclass,methodType,methodName):
	'''
	通过方法的类型和名称查找方法在一个class结构中查找一个结构
	acalss Class结构
	methodType 方法类型(指public,protected,private)
	methodName 方法名称
	返回 Method结构
	'''
	# TODO : 这里存在一个问题，就是在c++类中方法可以重载，方法名称不唯一
	#	     在CTP接口函数中不存在这种情况，暂时忽略
	
	return {method['name']:method for method in aclass['methods'][methodType]}[methodName]

def getStructFields(struct):
	'''
	获取一个结构体的字段列表
	struct  结构体数据
	返回 字段列表
	'''
	return [field for field in struct['properties']['public']]


def getMethodParameters(method):
	'''
	给定一个method结构返回其所有参数
	method Method结构
	返回 参数列表
	'''
	return [parameter for parameter in method['parameters'] ]


def getTypedefDict(filename):
	'''
	从一个C++头文件中获取所有的typedef数据
	filename 要读取的文件名称
	返回  一个typedef信息字典，其格式为:
	'''
	pattern = r'\s*typedef\s+(?P<type>\w+(\s+\*\s+)*)\s+(?P<name>\w+)\s*(\[\s*(?P<len>\w+)\s*\]){0,1}\s*;'
	regex = re.compile(pattern)
	with open(filename) as f : 
		header = f.read()
	typedefDict = {m.groupdict()['name']:m.groupdict() for m in regex.finditer(header)}
	return typedefDict

def getEnumDict(filename):
	'''
	从一个c++头文件中获取所有的枚举类型的值
	filename 要读取的文件名称
	返回  一个枚举数据字段，
		  其格式为{'类型名称':[{'name':'枚举名称1','value':'枚举对应值','remark':'说明'},...],...}

	'''
	# TODO 注意该函数功能并不通用，其基于CTP头文件的特性结构 
	# 先读取所有的typedef定义
	typedefs = getTypedefDict(filename)
	# 定义正则表达式
	definePattern  = r'///(?P<remark>\S+)\s+#define\s+(?P<name>\S+)\s+(?P<value>\S+)\s+'
	defineRegex = re.compile(definePattern)
	
	# 读取对应的头文件信息
	with open(filename) as f:
		header =  f.read()
	
	# 为每个类型尝试寻找其对应枚举值(如果存在的话)
	enumDict = {}
	for tdname in typedefs:
		enumPattern = r'T%s[\S\s]*?typedef\s*\S+\s+%s;' % (tdname[6:],tdname)
		enumDefineStringList = re.findall(enumPattern ,header)
		if enumDefineStringList :
			enumDefineString= enumDefineStringList [0]
			enumDict[tdname ] = [m.groupdict()for m in defineRegex .finditer(enumDefineString)]
				 
	return enumDict



#def getMethodReturn(method):
#	'''
#	给定一个method结构获取其返回值类型
#	method Method结构
#	返回 返回值类型名称
#	'''
#	return method['returns']


#def getParameterType(parameter):
#	'''
#	给定一个方法的参数结构，返回其类型
#	parameter 方法的参数结构
#	返回 参数的类型名称
#	'''
#	return parameter['type']






