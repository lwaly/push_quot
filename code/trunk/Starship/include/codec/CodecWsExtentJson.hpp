/*******************************************************************************
 * Project:  Starship
 * @file     CodecWsExtentJson.hpp
 * @brief    与手机客户端通信协议编解码器
 * @author   cjy
 * @date:    2016年9月3日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CODEC_CODECWSEXTENTJSON_HPP_
#define SRC_CODEC_CODECWSEXTENTJSON_HPP_
#include <string>
#include <map>
#include "util/http/http_parser.h"
#include "protocol/http.pb.h"
#include "StarshipCodec.hpp"
#include "ClientMsgHead.hpp"

namespace oss
{

/*
 0               1               2               3
 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 +-+-+-+-+-------+-+-------------+-------------------------------+
 |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
 |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
 |N|V|V|V|       |S|             |   (if payload len==126/127)   |
 | |1|2|3|       |K|             |                               |
 +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
 |     Extended payload length continued, if payload len == 127  |
 + - - - - - - - - - - - - - - - +-------------------------------+
 |                               |Masking-key, if MASK set to 1  |
 +-------------------------------+-------------------------------+
 | Masking-key (continued)       |          Payload Data         |
 +-------------------------------- - - - - - - - - - - - - - - - +
 :                     Payload Data continued ...                :
 + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 |                     Payload Data continued ...                |
 +---------------------------------------------------------------+
 FIN：1位
                 表示这是消息的最后一帧（结束帧），一个消息由一个或多个数据帧构成。若消息由一帧构成，起始帧即结束帧。
 * */
/*
 RSV1，RSV2，RSV3：3位
                 如果未定义扩展，各位是0；如果定义了扩展，即为非0值。如果接收的帧此处非0，扩展中却没有该值的定义，那么关闭连接。
 * */
/*
 OPCODE：4位
                 解释PayloadData，如果接收到未知的opcode，接收端必须关闭连接。
 0x0表示附加数据帧
 0x1表示文本数据帧
 0x2表示二进制数据帧
 0x3-7暂时无定义，为以后的非控制帧保留
 0x8表示连接关闭
 0x9表示ping
 0xA表示pong
 0xB-F暂时无定义，为以后的控制帧保留
 * */

const uint8 WEBSOCKET_FIN                   = 0x80;
const uint8 WEBSOCKET_RSV1                  = 0x40;
const uint8 WEBSOCKET_RSV2                  = 0x20;
const uint8 WEBSOCKET_RSV3                  = 0x10;

/*
 OPCODE：4位
 解释PayloadData，如果接收到未知的opcode，接收端必须关闭连接。
 0x0表示附加数据帧
 0x1表示文本数据帧
 0x2表示二进制数据帧
 0x3-7暂时无定义，为以后的非控制帧保留
 0x8表示连接关闭
 0x9表示ping
 0xA表示pong
 0xB-F暂时无定义，为以后的控制帧保留
  |Opcode  | Meaning                             | Reference |
-+--------+-------------------------------------+-----------|
 | 0      | Continuation Frame                  | RFC 6455  |
-+--------+-------------------------------------+-----------|
 | 1      | Text Frame                          | RFC 6455  |
-+--------+-------------------------------------+-----------|
 | 2      | Binary Frame                        | RFC 6455  |
-+--------+-------------------------------------+-----------|
 | 8      | Connection Close Frame              | RFC 6455  |
-+--------+-------------------------------------+-----------|
 | 9      | Ping Frame                          | RFC 6455  |
-+--------+-------------------------------------+-----------|
 | 10     | Pong Frame                          | RFC 6455  |
-+--------+-------------------------------------+-----------|
 * */
const uint8 WEBSOCKET_OPCODE                = (0x0F);
const uint8 WEBSOCKET_FRAME_CONTINUE        = (0x0);
const uint8 WEBSOCKET_FRAME_TEXT            = (0x1);
const uint8 WEBSOCKET_FRAME_BINARY          = (0x2);
const uint8 WEBSOCKET_FRAME_CLOSE           = (0x8);
const uint8 WEBSOCKET_FRAME_PING            = (0x9);
const uint8 WEBSOCKET_FRAME_PONG            = (0xA);


/*
 * MASK & PAYLOAD_LEN
 * */
const uint8 WEBSOCKET_MASK                  = 0x80;
const uint8 WEBSOCKET_PAYLOAD_LEN           = 0x7F;
const uint8 WEBSOCKET_PAYLOAD_LEN_UINT16    = 126;
const uint8 WEBSOCKET_PAYLOAD_LEN_UINT64    = 127;

inline uint64 htonll(uint64 val) {
    return (((uint64) htonl(val)) << 32) + htonl(val >> 32);
}

inline uint64 ntohll(uint64 val) {
    return (((uint64) ntohl(val)) << 32) + ntohl(val >> 32);
}

class CodecWsExtentJson: public StarshipCodec
{
public:
    CodecWsExtentJson(loss::E_CODEC_TYPE eCodecType, const std::string& strKey = "That's a lizard.");
    virtual ~CodecWsExtentJson();
    //解码编码websocket请求
    //Decode解码时判别是http请求还是websocket请求来处理,因为是框架固定调用该函数
    virtual E_CODEC_STATUS Encode(const MsgHead& oMsgHead, const MsgBody& oMsgBody, loss::CBuffer* pBuff);
    virtual E_CODEC_STATUS Decode(loss::CBuffer* pBuff, MsgHead& oMsgHead, MsgBody& oMsgBody);
    //解码编码http请求
    virtual E_CODEC_STATUS Encode(const HttpMsg& oHttpMsg, loss::CBuffer* pBuff);
    virtual E_CODEC_STATUS Decode(loss::CBuffer* pBuff, HttpMsg& oHttpMsg);
private:
    E_CODEC_STATUS EncodeHandShake(const HttpMsg& oHttpMsg,
                    loss::CBuffer* pBuff);
    E_CODEC_STATUS EncodeHttp(const HttpMsg& oHttpMsg, loss::CBuffer* pBuff);
private:
    //http parse
    static int OnMessageBegin(http_parser *parser);
    static int OnUrl(http_parser *parser, const char *at, size_t len);
    static int OnStatus(http_parser *parser, const char *at, size_t len);
    static int OnHeaderField(http_parser *parser, const char *at, size_t len);
    static int OnHeaderValue(http_parser *parser, const char *at, size_t len);
    static int OnHeadersComplete(http_parser *parser);
    static int OnBody(http_parser *parser, const char *at, size_t len);
    static int OnMessageComplete(http_parser *parser);
    static int OnChunkHeader(http_parser *parser);
    static int OnChunkComplete(http_parser *parser);
    
	/*
     * oHttpMsg序列化为字符串
     * */
    const std::string& ToString(const HttpMsg& oHttpMsg);
    http_parser_settings m_parser_setting;
    http_parser m_parser;
    std::string m_strHttpString;
    std::map<std::string, std::string> m_mapAddingHttpHeader;       ///< encode前添加的http头，encode之后要清空
};

} /* namespace neb */

#endif /* SRC_CODEC_CODECWSEXTENTJSON_HPP_ */
