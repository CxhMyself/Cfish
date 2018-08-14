#ifndef _HTTP_STATUE_CODE_
#define _HTTP_STATUE_CODE_


/*
回应头未完全设置完 肯定写不完是真的 总共六十余种  这里只设置最常用的14种
*/

/*
	成功类响应
*/

//成功 啥问题都没有
#define _200_STR			"OK"

//只包含响应的头部元信息 岂不是和head相似一般在 浏览器不发生刷新 这里可以通过判定URL是否完全一致来判断
#define _204_STR		"no Content"

//响应报文中含有Content-Range 指定范围的实体内容
#define _206_STR	"Partial Content"

/*
	3XX
	重定向状态吗
	这类状态码代表需要客户端采取进一步的操作才能完成请求。通常，这些状态码用来重定向，后续的请求地址（重定向目标）在本次响应的 Location 域中指明。
*/

//这个表示请求的资源已经被重新分配了URL 此时应当按照Location中的URL进行重新访问
#define _301_STR	"Moved Permanently"

//与301相似只是此时不会更新书签中的URL(如果用户保存为书签的话)
#define _302_STR				"Found"

//与302状态码有着相同的功能 但是此状态码表示客户端必须采用GET的方法请求重定向资源
#define _303_STR			"See Other"

//和重定向没有关系 如果客户端发送带条件的请求时 如果不满足条件直接返回304
#define _304_STR 			"Not Modified"

//与302相似
#define _307_STR				"Temporary Redirect"

/*
	4XX
	这类的状态码代表了客户端看起来可能发生了错误，妨碍了服务器的处理。除非响应的是一个 HEAD 请求，否则服务器就应该返回一个解释当前错误状况的实体
*/

//请求头有错误 请求的参数有误 请求错误
#define _400_STR		"Bad Request"

//这都是啥啥啥 这个表示请求报文需通过HTTP认证  回复中应该含有 WWW-Authenticate 字段 用来让客户端弹出认证用的对话窗口
#define _401_STR		"Unauthorized"

//无权限访问
#define _403_STR			"Forbidden"

//找不到资源
#define _404_STR			"Not Found"

/*
	5XX
	服务器错误 Emmm暂时不实现这俩了 没办法
*/

//服务器内部故障 这TM怎么实现 Epoll 中的EPOLLERR属于服务器内部错误
#define _500_STR					"Internal Server Error"


//服务器正在忙的话 会返回这个 用户上线设置S_CLOSE
#define _503_STR					"Server Unavailable"

//获取回复头的宏函数

#endif










