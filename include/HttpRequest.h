#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <map>
#include "Types.h"

namespace Kernel
{
// web请求
class CHttpRequest
{
public:
	CHttpRequest(uint64_t sessionID, void* pSession)
		:m_SessionID(sessionID)
		,m_SeessionObj(pSession)
	{

	}

	~CHttpRequest() { }

	/// @function 获取会话id
	/// 
	/// @return 返回会话id
	uint64_t GetSessionID() const { return m_SessionID; }

	/// @function 获取会话对象
	/// 
	/// @return 返回会话对象
	void* GetSessionObj() { return m_SeessionObj; }

private:
	uint64_t		m_SessionID;		// 会话Id
	void*			m_SeessionObj;		// 会话对象

public:
	string			m_AcceptType;				// 获取客户端支持的 MIME 接受类型			
	string			m_AnonymousID;
	string			m_ApplicationPath;			// 获取服务器上应用程序的虚拟应用程序根路径。
	string			m_AppRelativeCurrentExecutionFilePath;
	string			m_Browser;
	string			m_ClientCertificate;		// 获取当前请求的客户端安全证书
	string			m_ContentEncoding;			// 获取或设置实体主体的字符集
	string			m_ContentLength;			// 指定客户端发送的内容长度（以字节计）
	string			m_ContentType;				// 获取或设置传入请求的 MIME 内容类型
	string			m_Cookies;					// 获取客户端发送的 Cookie 的集合
	string			m_CurrentExecutionFilePath;
	string			m_FilePath;
	string			m_Files;
	string			m_Filter;
	string			m_Form;
	string			m_HttpMethod;				// 获取客户端使用的 HTTP 数据传输方法（如 GET、POST 或 HEAD）
	string			m_InputStream;				// 获取传入的 HTTP 实体主体的内容
	string			m_IsAuthenticated;
	string			m_IsLocal;
	string			m_IsSecureConnection;
	string			m_Item;
	string			m_LogonUserIdentity;
	string			m_Params;
	string			m_Path;
	string			m_PathInfo;
	string			m_PhysicalApplicationPath;
	string			m_PhysicalPath;
	string			m_QueryString;
	string			m_RawUrl;
	string			m_RequestType;				// 获取或设置客户端使用的 HTTP 数据传输方法（GET 或 POST）
	string			m_ServerVariables;
	string			m_TotalBytes;				// 获取当前输入流中的字节数
	string			m_Url;						// 获取有关当前请求的 URL 的信息
	string			m_UrlReferrer;
	string			m_UserAgent;				// 获取客户端浏览器的原始用户代理信息
	string			m_UserHostAddress;			// 获取远程客户端的 IP 主机地址
	string			m_UserHostName;				// 获取远程客户端的 DNS 名称
	string			m_UserLanguages;			// 获取客户端语言首选项

	typedef std::map<string, string> WebHeaderMap;
	typedef WebHeaderMap::iterator HeadIt;
	WebHeaderMap	m_Headers;					// 获取 HTTP 头集合
	string			m_TmpHeadKey;
	string			m_TmpHeadValue;

}; // end class CHttpRequest

} // end namespace Kernel


#endif //__HTTP_REQUEST_H__