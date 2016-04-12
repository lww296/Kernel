#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include "Types.h"

namespace Kernel
{
// web回复
class CHttpResponse
{
public:
	CHttpResponse(uint64_t sessionID)
		:m_SessionID(sessionID)
	{

	}

	~CHttpResponse() { }

	/// @function 获取会话id
	/// 
	/// @return 返回会话id
	uint64_t GetSessionID() const { return m_SessionID; }

private:
	uint64_t		m_SessionID;					// 会话id

public:
	string			m_Buffer;						// 获取或设置一个值，该值指示是否缓冲输出并在处理完整个响应之后发送它
	string			m_BufferOutput;					// 获取或设置一个值，该值指示是否缓冲输出并在处理完整个页之后发送它
	string			m_Cache;						// 获取网页的缓存策略(例如，过期时间、保密性设置和变化条款)
	string			m_CacheControl;
	string			m_Charset;						// 获取或设置输出流的 HTTP 字符集
	string			m_ClientDisconnectedToken;
	string			m_ContentEncoding;				// 获取或设置输出流的 HTTP 字符集
	string			m_ContentType;					// 获取或设置输出流的 HTTP MIME 类型
	string			m_Cookies;						// 获取响应 cookie 集合
	string			m_Expires;
	string			m_Filter;
	string			m_HeaderEncoding;				// 获取或设置表示当前标头输出流的编码的 Encoding对象
	string			m_Headers;						// 获取响应标头的集合
	string			m_HeadersWritten;
	string			m_IsClientConnected;
	string			m_IsRequestBeingRedirected;
	string			m_Output;						// 实现到传出 HTTP 响应流的文本输出
	string			m_OutputStream;					// 实现到传出 HTTP 内容主体的二进制输出
	string			m_RedirectLocation;
	string			m_Status;						// 设置返回到客户端的 Status 行
	string			m_StatusCode;					// 获取或设置返回给客户端的输出的 HTTP 状态代码
	string			m_StatusDescription;			// 获取或设置返回给客户端的输出的 HTTP 状态字符串
	string			m_SubStatusCode;
	string			m_SupportsAsyncFlush;
	string			m_SuppressContent;				// 获取或设置指示是否将 HTTP 内容发送到客户端的值
	string			m_SuppressFormsAuthenticationRedirect;

}; // end class CHttpResponse

} // end namespace Kernel


#endif //__HTTP_RESPONSE_H__