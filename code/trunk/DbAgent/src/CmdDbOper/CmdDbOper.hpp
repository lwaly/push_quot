/*******************************************************************************
 * Project:  DbAgent
 * @file     CmdDbOper.hpp
 * @brief
 * @author   xxx
 * @date:    2016年3月28日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CMDDBOPER_CMDDBOPER_HPP_
#define SRC_CMDDBOPER_CMDDBOPER_HPP_

#include "dbi/MysqlDbi.hpp"
#include "util/json/CJsonObject.hpp"

#include "cmd/Cmd.hpp"
#include "storage/dataproxy.pb.h"

#ifdef __cplusplus
extern "C" {
#endif
oss::Cmd* create();
#ifdef __cplusplus
}
#endif

namespace oss {

const int gc_iMaxBeatTimeInterval = 30;
const int gc_iMaxColValueSize = 65535;

//数据库连接结构体定义
struct tagConnection {
    loss::CMysqlDbi* pDbi;
    time_t ullBeatTime;
    int iQueryPermit;
    int iTimeout;

    tagConnection() : pDbi(NULL), ullBeatTime(0), iQueryPermit(0), iTimeout(0) {
    }

    ~tagConnection() {
        if (pDbi != NULL) {
            delete pDbi;
            pDbi = NULL;
        }
    }
};

class CmdDbOper: public Cmd {
public:
    CmdDbOper();
    virtual ~CmdDbOper();

    virtual bool Init();

    virtual bool AnyMessage(
        const tagMsgShell& stMsgShell,
        const MsgHead& oInMsgHead,
        const MsgBody& oInMsgBody);

protected:
    bool GetDbConnection(const DataMem::MemOperate& oQuery, loss::CMysqlDbi** ppMasterDbi, loss::CMysqlDbi** ppSlaveDbi);
    bool FetchOrEstablishConnection(const std::string& strMasterIdentify, const std::string& strSlaveIdentify,
                                    const loss::CJsonObject& oInstanceConf, loss::CMysqlDbi** ppMasterDbi, loss::CMysqlDbi** ppSlaveDbi);
    std::string GetFullTableName(const std::string& strTableName, uint32 uiFactor);

    int ConnectDb(const loss::CJsonObject& oInstanceConf, loss::CMysqlDbi* pDbi, bool bIsMaster = true);
    int Query(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi);
    void CheckConnection(); //检查连接是否已超时
    void Response(int iErrno, const std::string& strErrMsg);
    bool Response(const DataMem::MemRsp& oRsp);

    bool CreateSql(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi, std::string& strSql);
    bool CreateSelect(const DataMem::MemOperate& oQuery, std::string& strSql);
    bool CreateInsert(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi, std::string& strSql);
    bool CreateUpdate(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi, std::string& strSql);
    bool CreateDelete(const DataMem::MemOperate& oQuery, std::string& strSql);
    bool CreateCondition(const DataMem::MemOperate::DbOperate::Condition& oCondition, loss::CMysqlDbi* pDbi, std::string& strCondition);
    bool CreateConditionGroup(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi, std::string& strCondition);
    bool CreateGroupBy(const DataMem::MemOperate& oQuery, std::string& strGroupBy);
    bool CreateOrderBy(const DataMem::MemOperate& oQuery, std::string& strOrderBy);
    bool CreateLimit(const DataMem::MemOperate& oQuery, std::string& strLimit);
    bool CheckColName(const std::string& strColName);

private:
    MsgHead m_oInMsgHead;
    MsgBody m_oInMsgBody;
    oss::tagMsgShell m_stMsgShell;
    int m_iConnectionTimeout;   //空闲连接超时（单位秒）
    char* m_szColValue;         //字段值
    loss::CJsonObject m_oDbConf;
    std::map<std::string, std::set<uint32> > m_mapFactorSection; //分段因子区间配置，key为因子类型
    std::map<std::string, loss::CJsonObject*> m_mapDbInstanceInfo;  //数据库配置信息key为("%u:%u:%u", uiDataType, uiFactor, uiFactorSection)
    std::map<std::string, tagConnection*> m_mapDbiPool;     //数据库连接池，key为identify（如：192.168.18.22:3306）
};

} /* namespace oss */

#endif /* SRC_CMDDBOPER_CMDDBOPER_HPP_ */
