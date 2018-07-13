/*******************************************************************************
 * Project:  CenterServer
 * @file    Comm.hpp
 * @brief
 * @author   xxx
 * @date:    2015年9月16日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_NODEREG_COMM_HPP_
#define SRC_NODEREG_COMM_HPP_

#include "util/json/CJsonObject.hpp"
#include "OssDefine.hpp"
#include "util/UnixTime.hpp"

namespace oss {
//节点信息信息
typedef struct st_NodeInfo {
    int node_id; //节点ID
    std::string node_type; //节点类型
    int node_port; //节点内网端口
    std::string node_ip; //节点内网IP
    int node_access_port; //节点外部访问端口
    std::string node_access_ip; //节点外部访问IP
    int worker_num; //工作进程数量
    oss::uint64 active_time; //激活时间
    int load;
    int connect;
    int client;
    int recvNum;
    int recvByte;
    int sendNum;
    int sendByte;
    std::string worker;
    st_NodeInfo() {
        clean();
    }
    void clean() {
        node_id = 0;
        node_type = "";
        node_port = 0;
        node_ip = "";
        node_access_port = 0;
        node_access_ip = "";
        worker_num = 0;
        active_time = 0; //激活时间
        load = 0;
        connect = 0;
        client = 0;
        recvNum = 0;
        recvByte = 0;
        sendNum = 0;
        sendByte = 0;
        worker = "";
    }
    //解析json数据
    bool pareJsonData(loss::CJsonObject& jObj) {
        loss::CJsonObject nodeobj, workerobj;
        if (jObj.Get("node_type", node_type) && jObj.Get("node_ip", node_ip)
                && jObj.Get("node_port", node_port)
                && jObj.Get("worker_num", worker_num)
                && jObj.Get("node_id", node_id)
                && jObj.Get("node", nodeobj)
                && jObj.Get("worker", workerobj)) {
            //数据作检查
            if (node_type == "" || node_port == 0 || worker_num == 0
                    || node_ip == "") {
                return false;
            }
            active_time = ::time(NULL); //使用中心服务器的本地时间（发送来的时间精度有问题，且不合适统一管理）
            worker = workerobj.ToString();
            jObj.Get("access_port", node_access_port);
            jObj.Get("access_ip", node_access_ip);
            nodeobj.Get("load", load);
            nodeobj.Get("connect", connect);
            nodeobj.Get("client", client);
            nodeobj.Get("recv_num", recvNum);
            nodeobj.Get("recv_byte", recvByte);
            nodeobj.Get("send_num", sendNum);
            nodeobj.Get("send_byte", sendByte);
            return true;
        }
        return false;
    }
    //获取节点的key
    const std::string getNodeKey() const {
        char strNodeKey[100] = { 0 };
        sprintf(strNodeKey, "%s:%d", node_ip.c_str(), node_port);
        return std::string(strNodeKey);
    }
    st_NodeInfo& operator =(const st_NodeInfo& info) {
        node_id = info.node_id;
        node_type = info.node_type;
        node_port = info.node_port;
        node_ip = info.node_ip;
        node_access_port = info.node_access_port;
        node_access_ip = info.node_access_ip;
        active_time = info.active_time;
        worker_num = info.worker_num;
        load = info.load;
        connect = info.connect;
        client = info.client;
        recvNum = info.recvNum;
        recvByte = info.recvByte;
        sendNum = info.sendNum;
        sendByte = info.sendByte;
        worker = info.worker;
        return *this;
    }
    bool update(const st_NodeInfo& info) { //更新只更新负载部分
        if ((node_ip == info.node_ip) && (node_port == info.node_port)) {
            active_time = info.active_time;
            worker_num = info.worker_num;
            load = info.load;
            connect = info.connect;
            client = info.client;
            recvNum = info.recvNum;
            recvByte = info.recvByte;
            sendNum = info.sendNum;
            sendByte = info.sendByte;
            worker = info.worker;
            return true;
        }
        return false;
    }
} st_NodeInfo;
//节点信息信息
typedef struct st_NodeStaisticsInfo {
    int node_id; //节点ID
    std::string node_type; //节点类型
    int node_port; //节点内网端口
    std::string node_ip; //节点内网IP
    int node_access_port; //节点外部访问端口
    std::string node_access_ip; //节点外部访问IP
    int worker_num; //工作进和数量
    int load;
    int connect;
    int client;
    int recvNum;
    int recvByte;
    int sendNum;
    int sendByte;
    st_NodeStaisticsInfo() {
        clear();
    }
    void clear() {
        node_id = 0;
        node_type = "";
        node_port = 0;
        node_ip = "";
        node_access_port = 0;
        node_access_ip = "";
        worker_num = 0;
        load = 0;
        connect = 0;
        client = 0;
        recvNum = 0;
        recvByte = 0;
        sendNum = 0;
        sendByte = 0;
    }
    void clearLoad() {
        recvNum = 0;
        recvByte = 0;
        sendNum = 0;
        sendByte = 0;
    }
    st_NodeStaisticsInfo(const st_NodeStaisticsInfo& info) {
        node_id = info.node_id;
        node_type = info.node_type;
        node_port = info.node_port;
        node_ip = info.node_ip;
        node_access_port = info.node_access_port;
        node_access_ip = info.node_access_ip;
        worker_num = info.worker_num;
        load = info.load;
        connect = info.connect;
        client = info.client;
        recvNum = info.recvNum;
        recvByte = info.recvByte;
        sendNum = info.sendNum;
        sendByte = info.sendByte;
    }
    st_NodeStaisticsInfo& operator =(const st_NodeStaisticsInfo& info) {
        node_id = info.node_id;
        node_type = info.node_type;
        node_port = info.node_port;
        node_ip = info.node_ip;
        node_access_port = info.node_access_port;
        node_access_ip = info.node_access_ip;
        worker_num = info.worker_num;
        load = info.load;
        connect = info.connect;
        client = info.client;
        recvNum = info.recvNum;
        recvByte = info.recvByte;
        sendNum = info.sendNum;
        sendByte = info.sendByte;
        return *this;
    }
    st_NodeStaisticsInfo& operator +=(const st_NodeStaisticsInfo& info) {
        if ((node_type == info.node_type) && (node_ip == info.node_ip)
                && (node_port == info.node_port)) {
            recvNum += info.recvNum;
            recvByte += info.recvByte;
            sendNum += info.sendNum;
            sendByte += info.sendByte;
        }
        return *this;
    }
    st_NodeStaisticsInfo(const st_NodeInfo& info) {
        node_id = info.node_id;
        node_type = info.node_type;
        node_port = info.node_port;
        node_ip = info.node_ip;
        node_access_port = info.node_access_port;
        node_access_ip = info.node_access_ip;
        worker_num = info.worker_num;
        load = info.load;
        connect = info.connect;
        client = info.client;
        recvNum = info.recvNum;
        recvByte = info.recvByte;
        sendNum = info.sendNum;
        sendByte = info.sendByte;
    }
    st_NodeStaisticsInfo& operator =(const st_NodeInfo& info) {
        node_id = info.node_id;
        node_type = info.node_type;
        node_port = info.node_port;
        node_ip = info.node_ip;
        node_access_port = info.node_access_port;
        node_access_ip = info.node_access_ip;
        worker_num = info.worker_num;
        load = info.load;
        connect = info.connect;
        client = info.client;
        recvNum = info.recvNum;
        recvByte = info.recvByte;
        sendNum = info.sendNum;
        sendByte = info.sendByte;
        return *this;
    }
    st_NodeStaisticsInfo& operator +=(const st_NodeInfo& info) {
        if ((node_ip == info.node_ip) && (node_port == info.node_port)) {
            //统计只统计负载部分
            if (worker_num != info.worker_num) {
                worker_num = info.worker_num;
            }
            if (load != info.load) {
                load = info.load;
            }
            if (connect != info.connect) {
                connect = info.connect;
            }
            if (client != info.client) {
                client = info.client;
            }
            recvNum += info.recvNum;
            recvByte += info.recvByte;
            sendNum += info.sendNum;
            sendByte += info.sendByte;
        }
        return *this;
    }
} st_NodeStaisticsInfo;

//节点负载状态
struct NodeLoadStatus {
    NodeLoadStatus() {
        clear();
    }
    NodeLoadStatus(const NodeLoadStatus& nodeLoadStatus) {
        nodetype = nodeLoadStatus.nodetype;
        serverload = nodeLoadStatus.serverload;
        innerport = nodeLoadStatus.innerport;
        innerip = nodeLoadStatus.innerip;
        outerport = nodeLoadStatus.outerport;
        outerip = nodeLoadStatus.outerip;
    }
    NodeLoadStatus& operator =(const NodeLoadStatus& nodeLoadStatus) {
        nodetype = nodeLoadStatus.nodetype;
        serverload = nodeLoadStatus.serverload;
        innerport = nodeLoadStatus.innerport;
        innerip = nodeLoadStatus.innerip;
        outerport = nodeLoadStatus.outerport;
        outerip = nodeLoadStatus.outerip;
        return *this;
    }
    NodeLoadStatus(const st_NodeInfo& nodeInfo) {
        nodetype = nodeInfo.node_type;
        serverload = nodeInfo.load;
        innerport = nodeInfo.node_port;
        innerip = nodeInfo.node_ip;
        outerport = nodeInfo.node_access_port;
        outerip = nodeInfo.node_access_ip;
    }
    NodeLoadStatus& operator =(const st_NodeInfo& nodeInfo) {
        nodetype = nodeInfo.node_type;
        serverload = nodeInfo.load;
        innerport = nodeInfo.node_port;
        innerip = nodeInfo.node_ip;
        outerport = nodeInfo.node_access_port;
        outerip = nodeInfo.node_access_ip;
        return *this;
    }

    std::string nodetype;
    oss::uint16 serverload;
    oss::uint16 innerport;
    std::string innerip;
    oss::uint16 outerport;
    std::string outerip;
    void clear() {
        nodetype.clear();
        serverload = 0;
        innerport = 0;
        innerip.clear();
        outerport = 0;
        outerip.clear();
    }
};

} /* namespace oss */

#endif /* SRC_NODEREG_COMM_HPP_ */
