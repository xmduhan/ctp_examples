# -*- coding: utf-8 -*-


import CppHeaderParser


def getCppHeader(filename):
	'''
	通过一个文件名获得一个CppHeader结构数据
	filename 文件名称(含路径)
	返回 CppHeader结构
	'''
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
	TODO:这里存在一个问题，就是在c++类中方法可以重载，方法名称不唯一
		 在CTP接口函数中不存在这种情况，暂时忽略
	'''
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






