#ifdef _OPEN_WEB_CLIENT_MODULE_
#ifndef __HTTP_CURL_H__
#define __HTTP_CURL_H__

#include "Types.h"
#include "Define.h"

// @Brief http请求
class _KERNEL_EXPORTS CHttpClient
{
public:
	CHttpClient();
	~CHttpClient();

	//////////////////////////////////////////////////////////////////////////
	/// 同步请求

	/// @function HTTP POST请求
	/// 
	/// @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	/// @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	/// @param strResponse 输出参数,返回的内容
	/// @return 返回是否Post成功
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);

	/// @function HTTP GET请求
	///
	/// @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	/// @param strResponse 输出参数,返回的内容
	/// @return 返回是否Get成功
	int Get(const std::string & strUrl, std::string & strResponse);

	/// @function HTTPS POST请求,无证书版本
	/// 
	/// @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	/// @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	/// @param strResponse 输出参数,返回的内容
	/// @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	/// @return 返回是否Posts成功
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);

	/// @function HTTPS GET请求,无证书版本
	/// @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	/// @param strResponse 输出参数,返回的内容
	/// @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	/// @return 返回是否Gets成功
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);

	//////////////////////////////////////////////////////////////////////////
	/// 异步请求

	/// @function HTTP POST请求
	/// @note 只支持服务器模块调用，必须在引擎初始化后调用
	/// 
	/// @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	/// @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	/// @param call 回复回调函数
	/// @param timeout 超时时间 0：永不超时 ms为单位
	/// @param name 请求名称
	/// @return 返回是否Post成功
	bool AsyncPost(const string& strUrl, const string& strPost, Kernel::HttpReponseCall call, int timeout = 3 * 1000, const string& name = "http_async_post");

	/// @function HTTP GET请求
	/// @note 只支持服务器模块调用，必须在引擎初始化后调用
	///
	/// @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	/// @param call 回复回调函数
	/// @param timeout 超时时间 0：永不超时 ms为单位
	/// @param name 请求名称
	/// @return 返回是否Get成功
	bool AsyncGet(const std::string& strUrl, Kernel::HttpReponseCall call, int timeout = 3 * 1000, const string& name = "http_async_get");

public:
	void SetDebug(bool bDebug);

private:
	bool m_bDebug;
};

#endif //__HTTP_CURL_H__
#endif //_OPEN_WEB_CLIENT_MODULE_