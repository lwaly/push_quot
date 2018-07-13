/*******************************************************************************
 * Project:  DbAgent
 * @file     CmdDbOper.cpp
 * @brief
 * @author   xxx
 * @date:    2016年3月28日
 * @note
 * Modify history:
 ******************************************************************************/
#include "CmdDbOper.hpp"

#ifdef __cplusplus
extern "C" {
#endif
oss::Cmd* create() {
    oss::Cmd* pCmd = new oss::CmdDbOper();
    return (pCmd);
}
#ifdef __cplusplus
}
#endif

namespace oss {

CmdDbOper::CmdDbOper()
    : m_iConnectionTimeout(gc_iMaxBeatTimeInterval), m_szColValue(NULL) {
    m_szColValue = new char[gc_iMaxColValueSize];
}

CmdDbOper::~CmdDbOper() {
    if (m_szColValue != NULL) {
        delete[] m_szColValue;
        m_szColValue = NULL;
    }

    for (std::map<std::string, tagConnection*>::iterator iter = m_mapDbiPool.begin();
            iter != m_mapDbiPool.end(); ++iter) {
        if (iter->second != NULL) {
            delete iter->second;
            iter->second = NULL;
        }
    }
    m_mapDbiPool.clear();
}

bool CmdDbOper::Init() {
    //配置文件路径查找
    std::string strConfFile = GetConfigPath() + std::string("Agent/CmdDbOper.json");
    LOG4_DEBUG("CONF FILE = %s.", strConfFile.c_str());

    std::ifstream fin(strConfFile.c_str());
    if (fin.good()) {
        std::stringstream ssContent;
        ssContent << fin.rdbuf();
        fin.close();
        if (m_oDbConf.Parse(ssContent.str())) {
            LOG4_TRACE("m_oDbConf pasre OK");
            char szInstanceGroup[64] = {0};
            char szFactorSectionKey[32] = {0};
            uint32 uiDataType = 0;
            uint32 uiFactor = 0;
            uint32 uiFactorSection = 0;
            if (m_oDbConf["table"].IsEmpty()) {
                LOG4_ERROR("m_oDbConf[\"table\"] is empty!");
                return (false);
            }
            if (m_oDbConf["database"].IsEmpty()) {
                LOG4_ERROR("m_oDbConf[\"database\"] is empty!");
                return (false);
            }
            if (m_oDbConf["cluster"].IsEmpty()) {
                LOG4_ERROR("m_oDbConf[\"cluster\"] is empty!");
                return (false);
            }
            if (m_oDbConf["db_group"].IsEmpty()) {
                LOG4_ERROR("m_oDbConf[\"db_group\"] is empty!");
                return (false);
            }

            for (int i = 0; i < m_oDbConf["data_type"].GetArraySize(); ++i) {
                if (m_oDbConf["data_type_enum"].Get(m_oDbConf["data_type"](i), uiDataType)) {
                    for (int j = 0; j < m_oDbConf["section_factor"].GetArraySize(); ++j) {
                        if (m_oDbConf["section_factor_enum"].Get(m_oDbConf["section_factor"](j), uiFactor)) {
                            if (m_oDbConf["factor_section"][m_oDbConf["section_factor"](j)].IsArray()) {
                                std::set<uint32> setFactorSection;
                                for (int k = 0; k < m_oDbConf["factor_section"][m_oDbConf["section_factor"](j)].GetArraySize(); ++k) {
                                    if (m_oDbConf["factor_section"][m_oDbConf["section_factor"](j)].Get(k, uiFactorSection)) {
                                        snprintf(szInstanceGroup, sizeof(szInstanceGroup), "%u:%u:%u", uiDataType, uiFactor, uiFactorSection);
                                        snprintf(szFactorSectionKey, sizeof(szFactorSectionKey), "LE_%u", uiFactorSection);
                                        setFactorSection.insert(uiFactorSection);
                                        loss::CJsonObject* pInstanceGroup
                                            = new loss::CJsonObject(m_oDbConf["cluster"][m_oDbConf["data_type"](i)][m_oDbConf["section_factor"](j)][szFactorSectionKey]);
                                        LOG4_TRACE("%s : %s", szInstanceGroup, pInstanceGroup->ToString().c_str());
                                        m_mapDbInstanceInfo.insert(std::pair<std::string, loss::CJsonObject*>(szInstanceGroup, pInstanceGroup));
                                    } else {
                                        LOG4_ERROR("m_oDbConf[\"factor_section\"][%s](%d) is not exist!",
                                                   m_oDbConf["section_factor"](j).c_str(), k);
                                        continue;
                                    }
                                }
                                snprintf(szInstanceGroup, sizeof(szInstanceGroup), "%u:%u", uiDataType, uiFactor);
                                LOG4_TRACE("%s factor size %u", szInstanceGroup, setFactorSection.size());
                                m_mapFactorSection.insert(std::pair<std::string, std::set<uint32> >(szInstanceGroup, setFactorSection));
                            } else {
                                LOG4_ERROR("m_oDbConf[\"factor_section\"][%s] is not a json array!",
                                           m_oDbConf["section_factor"](j).c_str());
                                continue;
                            }
                        } else {
                            LOG4_ERROR("missing %s in m_oDbConf[\"section_factor_enum\"]", m_oDbConf["section_factor"](j).c_str());
                            continue;
                        }
                    }
                } else {
                    LOG4_ERROR("missing %s in m_oDbConf[\"data_type_enum\"]", m_oDbConf["data_type"](i).c_str());
                    continue;
                }
            }
        } else {
            LOG4_ERROR("m_oDbConf pasre error");
            return (false);
        }
    } else {
        //配置信息流读取失败
        LOG4_ERROR("Open conf \"%s\" error!", strConfFile.c_str());
        return (false);
    }

    return (true);
}

bool CmdDbOper::AnyMessage(const oss::tagMsgShell& stMsgShell, const MsgHead& oInMsgHead, const MsgBody& oInMsgBody) {
    LOG4_TRACE("%s()", __FUNCTION__);
    m_stMsgShell = stMsgShell;
    m_oInMsgHead = oInMsgHead;
    m_oInMsgBody = oInMsgBody;

    DataMem::MemOperate oMemOperate;
    loss::CMysqlDbi* pMasterDbi = NULL;
    loss::CMysqlDbi* pSlaveDbi = NULL;

    if (!oMemOperate.ParseFromString(oInMsgBody.body())) {
        LOG4_ERROR("Parse protobuf msg error!");
        Response(ERR_PARASE_PROTOBUF, "DataMem::MemOperate ParseFromString() failed!");
        return (false);
    }

    bool bConnected = GetDbConnection(oMemOperate, &pMasterDbi, &pSlaveDbi);
    if (bConnected) {
        LOG4_TRACE("succeed in getting db connection");
        int iResult = 0;
        if (DataMem::MemOperate::DbOperate::SELECT == oMemOperate.db_operate().query_type()) {
            iResult = Query(oMemOperate, pSlaveDbi);
            if (0 == iResult) {
                return (true);
            } else {
                iResult = Query(oMemOperate, pMasterDbi);
                if (0 == iResult) {
                    return (true);
                } else {
                    Response(pMasterDbi->GetErrno(), pMasterDbi->GetError());
                    return (false);
                }
            }
        } else {
            if (NULL == pMasterDbi) {
                Response(pMasterDbi->GetErrno(), pMasterDbi->GetError());
                return (false);
            }
            iResult = Query(oMemOperate, pMasterDbi);
            if (0 == iResult) {
                return (true);
            } else {
                Response(pMasterDbi->GetErrno(), pMasterDbi->GetError());
                return (false);
            }
        }
    }
    return (false);
}

bool CmdDbOper::GetDbConnection(const DataMem::MemOperate& oQuery,
                                loss::CMysqlDbi** ppMasterDbi, loss::CMysqlDbi** ppSlaveDbi) {
    LOG4_TRACE("%s()", __FUNCTION__);
    char szFactor[32] = {0};
    int32 iDataType = 0;
    int32 iSectionFactorType = 0;
    m_oDbConf["table"][oQuery.db_operate().table_name()].Get("data_type", iDataType);
    m_oDbConf["table"][oQuery.db_operate().table_name()].Get("section_factor", iSectionFactorType);
    snprintf(szFactor, 32, "%d:%d", iDataType, iSectionFactorType);
    std::map<std::string, std::set<uint32> >::const_iterator c_factor_iter =  m_mapFactorSection.find(szFactor);
    if (c_factor_iter == m_mapFactorSection.end()) {
        LOG4_ERROR("no db config found for data_type %d section_factor_type %d",
                   iDataType, iSectionFactorType);
        Response(ERR_LACK_CLUSTER_INFO, "no db config found for oMemOperate.cluster_info()!");
        return (false);
    } else {
        std::set<uint32>::const_iterator c_section_iter = c_factor_iter->second.lower_bound(oQuery.section_factor());
        if (c_section_iter == c_factor_iter->second.end()) {
            LOG4_ERROR("no factor_section config found for data_type %u section_factor_type %u section_factor %u",
                       iDataType, iSectionFactorType, oQuery.section_factor());
            Response(ERR_LACK_CLUSTER_INFO, "no db config for the cluster info!");
            return (false);
        } else {
            snprintf(szFactor, 32, "%u:%u:%u", iDataType, iSectionFactorType, *c_section_iter);
            std::map<std::string, loss::CJsonObject*>::iterator conf_iter = m_mapDbInstanceInfo.find(szFactor);
            if (conf_iter == m_mapDbInstanceInfo.end()) {
                LOG4_ERROR("no db config found for %s which consist of data_type %u section_factor_type %u section_factor %u",
                           szFactor, iDataType, iSectionFactorType, oQuery.section_factor());
                Response(ERR_LACK_CLUSTER_INFO, "no db config for the cluster info!");
                return (false);
            }
            std::string strDbName = m_oDbConf["table"][oQuery.db_operate().table_name()]("db_name");
            std::string strDbGroup = m_oDbConf["database"][strDbName]("db_group");
            std::string strInstance;
            if (!conf_iter->second->Get(strDbGroup, strInstance)) {
                Response(ERR_LACK_CLUSTER_INFO, "no db instance config for db group!");
                LOG4_ERROR("no db instance config for db group \"%s\"!", strDbGroup.c_str());
                return (false);
            }

            if (oQuery.db_operate().query_type() > atoi(m_oDbConf["db_group"][strInstance]("query_permit").c_str())) {
                Response(ERR_NO_RIGHT, "no right to excute oMemOperate.db_operate().query_type()!");
                LOG4_ERROR("no right to excute oMemOperate.db_operate().query_type() %d!",
                           oQuery.db_operate().query_type());
                return (false);
            }
            std::string strMasterHost = m_oDbConf["db_group"][strInstance]("master_host");
            std::string strSlaveHost = m_oDbConf["db_group"][strInstance]("slave_host");
            std::string strPort = m_oDbConf["db_group"][strInstance]("port");
            std::string strMasterIdentify = strMasterHost + std::string(":") + strPort;
            std::string strSlaveIdentify = strSlaveHost + std::string(":") + strPort;
            bool bEstablishConnection = FetchOrEstablishConnection(strMasterIdentify, strSlaveIdentify,
                                        m_oDbConf["db_group"][strInstance], ppMasterDbi, ppSlaveDbi);
            return (bEstablishConnection);
        }
    }
}

bool CmdDbOper::FetchOrEstablishConnection(const std::string& strMasterIdentify, const std::string& strSlaveIdentify,
        const loss::CJsonObject& oInstanceConf, loss::CMysqlDbi** ppMasterDbi, loss::CMysqlDbi** ppSlaveDbi) {
    LOG4_TRACE("%s(%s, %s, %s)", __FUNCTION__, strMasterIdentify.c_str(), strSlaveIdentify.c_str(), oInstanceConf.ToString().c_str());
    *ppMasterDbi = NULL;
    *ppSlaveDbi = NULL;
    std::map<std::string, tagConnection*>::iterator dbi_iter = m_mapDbiPool.find(strMasterIdentify);
    if (dbi_iter == m_mapDbiPool.end()) {
        tagConnection* pConnection = new tagConnection();
        if (NULL == pConnection) {
            Response(ERR_NEW, "malloc space for db connection failed!");
            return (false);
        }
        loss::CMysqlDbi* pDbi = new loss::CMysqlDbi();
        if (NULL == pDbi) {
            Response(ERR_NEW, "malloc space for db connection failed!");
            delete pConnection;
            pConnection = NULL;
            return (false);
        }
        pConnection->pDbi = pDbi;
        if (0 == ConnectDb(oInstanceConf, pDbi, true)) {
            LOG4_TRACE("succeed in connecting %s.", strMasterIdentify.c_str());
            *ppMasterDbi = pDbi;
            pConnection->iQueryPermit = atoi(oInstanceConf("query_permit").c_str());
            pConnection->iTimeout = atoi(oInstanceConf("timeout").c_str());
            pConnection->ullBeatTime = time(NULL);
            m_mapDbiPool.insert(std::pair<std::string, tagConnection*>(strMasterIdentify, pConnection));
        } else {
            delete pConnection;
            pConnection = NULL;
        }
    } else {
        dbi_iter->second->ullBeatTime = time(NULL);
        *ppMasterDbi = dbi_iter->second->pDbi;
    }

    LOG4_TRACE("find slave %s.", strSlaveIdentify.c_str());
    dbi_iter = m_mapDbiPool.find(strSlaveIdentify);
    if (dbi_iter == m_mapDbiPool.end()) {
        tagConnection* pConnection = new tagConnection();
        if (NULL == pConnection) {
            Response(ERR_NEW, "malloc space for db connection failed!");
            return (false);
        }
        loss::CMysqlDbi* pDbi = new loss::CMysqlDbi();
        if (NULL == pDbi) {
            Response(ERR_NEW, "malloc space for db connection failed!");
            delete pConnection;
            pConnection = NULL;
            return (false);
        }
        pConnection->pDbi = pDbi;
        if (0 == ConnectDb(oInstanceConf, pDbi, true)) {
            LOG4_TRACE("succeed in connecting %s.", strSlaveIdentify.c_str());
            *ppSlaveDbi = pDbi;
            pConnection->iQueryPermit = atoi(oInstanceConf("query_permit").c_str());
            pConnection->iTimeout = atoi(oInstanceConf("timeout").c_str());
            pConnection->ullBeatTime = time(NULL);
            m_mapDbiPool.insert(std::pair<std::string, tagConnection*>(strSlaveIdentify, pConnection));
        } else {
            delete pConnection;
            pConnection = NULL;
        }
    } else {
        dbi_iter->second->ullBeatTime = time(NULL);
        *ppSlaveDbi = dbi_iter->second->pDbi;
    }

    LOG4_TRACE("pMasterDbi = 0x%x, pSlaveDbi = 0x%x.", *ppMasterDbi, *ppSlaveDbi);
    if (*ppMasterDbi || *ppSlaveDbi) {
        return (true);
    } else {
        return (false);
    }
}

int CmdDbOper::ConnectDb(const loss::CJsonObject& oInstanceConf, loss::CMysqlDbi* pDbi, bool bIsMaster) {
    LOG4_DEBUG("%s()", __FUNCTION__);
    int iResult = 0;
    char szIdentify[32] = {0};
    loss::tagDbConfDetail stDbConfDetail;
    if (bIsMaster) {
        strncpy(stDbConfDetail.m_stDbConnInfo.m_szDbHost,
                oInstanceConf("master_host").c_str(), sizeof(stDbConfDetail.m_stDbConnInfo.m_szDbHost));
        snprintf(szIdentify, sizeof(szIdentify), "%s:%s", oInstanceConf("master_host").c_str(), oInstanceConf("port").c_str());
    } else {
        strncpy(stDbConfDetail.m_stDbConnInfo.m_szDbHost,
                oInstanceConf("slave_host").c_str(), sizeof(stDbConfDetail.m_stDbConnInfo.m_szDbHost));
        snprintf(szIdentify, sizeof(szIdentify), "%s:%s", oInstanceConf("slave_host").c_str(), oInstanceConf("port").c_str());
    }
    strncpy(stDbConfDetail.m_stDbConnInfo.m_szDbUser, oInstanceConf("user").c_str(), sizeof(stDbConfDetail.m_stDbConnInfo.m_szDbUser));
    strncpy(stDbConfDetail.m_stDbConnInfo.m_szDbPwd, oInstanceConf("password").c_str(), sizeof(stDbConfDetail.m_stDbConnInfo.m_szDbPwd));
    strncpy(stDbConfDetail.m_stDbConnInfo.m_szDbName, "test", sizeof(stDbConfDetail.m_stDbConnInfo.m_szDbName));
    strncpy(stDbConfDetail.m_stDbConnInfo.m_szDbCharSet, oInstanceConf("charset").c_str(), sizeof(stDbConfDetail.m_stDbConnInfo.m_szDbCharSet));
    stDbConfDetail.m_stDbConnInfo.m_uiDbPort = atoi(oInstanceConf("port").c_str());
    stDbConfDetail.m_ucDbType = loss::MYSQL_DB;
    stDbConfDetail.m_ucAccess = 1; //直连

    LOG4_DEBUG("InitDbConn(%s, %s, %s, %s, %u)", stDbConfDetail.m_stDbConnInfo.m_szDbHost,
               stDbConfDetail.m_stDbConnInfo.m_szDbUser, stDbConfDetail.m_stDbConnInfo.m_szDbPwd,
               stDbConfDetail.m_stDbConnInfo.m_szDbName, stDbConfDetail.m_stDbConnInfo.m_uiDbPort);
    iResult = pDbi->InitDbConn(stDbConfDetail);
    if (0 != iResult) {
        LOG4_ERROR("error %d: %s", pDbi->GetErrno(), pDbi->GetError().c_str());
    }
    return (iResult);
}

int CmdDbOper::Query(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi) {
    LOG4_TRACE("%s()", __FUNCTION__);
    int iResult = 0;
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    DataMem::MemRsp oRsp;
    oRsp.set_from(DataMem::MemRsp::FROM_DB);
    DataMem::Record* pRecord = NULL;
    DataMem::Field* pField = NULL;

    if (NULL == pDbi) {
        LOG4_ERROR("pDbi is null!");
        Response(ERR_QUERY, "pDbi is null");
        return (ERR_QUERY);
    }

    std::string strSql;
    if (!CreateSql(oQuery, pDbi, strSql)) {
        LOG4_ERROR("Scrabble up sql error!");
        Response(ERR_QUERY, strSql);
        return (ERR_QUERY);
    }
    LOG4_DEBUG("%s", strSql.c_str());
    iResult = pDbi->ExecSql(strSql);
    if (iResult == 0) {
        if (DataMem::MemOperate::DbOperate::SELECT == oQuery.db_operate().query_type()) {
            if (NULL != pDbi->UseResult()) {
                uint32 uiDataLen = 0;
                uint32 uiRecordSize = 0;
                int32 iRecordNum = 0;
                unsigned int uiFieldNum = pDbi->FetchFieldNum();

                //字段值进行赋值
                MYSQL_ROW stRow;
                unsigned long* lengths;

                oRsp.set_err_no(ERR_OK);
                oRsp.set_err_msg("success");
                while (NULL != (stRow = pDbi->GetRow())) {
                    uiRecordSize = 0;
                    ++iRecordNum;
                    lengths = pDbi->FetchLengths();
                    pRecord = oRsp.add_record_data();
                    for (unsigned int i = 0; i < uiFieldNum; ++i) {
                        uiRecordSize += lengths[i];
                    }
                    if (uiRecordSize > 1000000) {
                        LOG4_ERROR("error %d: %s", ERR_RESULTSET_EXCEED, "result set(one record) exceed 1 MB!");
                        Response(ERR_RESULTSET_EXCEED, "result set(one record) exceed 1 MB!");
                        return (ERR_RESULTSET_EXCEED);
                    }
                    if (uiDataLen + uiRecordSize > 1000000) {
                        oRsp.set_curcount(iRecordNum);
                        oRsp.set_totalcount(iRecordNum + 1);    // 表示未完
                        if (Response(oRsp)) {
                            oRsp.clear_record_data();
                            uiDataLen = 0;
                        } else {
                            return (ERR_RESULTSET_EXCEED);
                        }
                    }

                    for (unsigned int i = 0; i < uiFieldNum; ++i) {
                        pField = pRecord->add_field_info();
                        if (0 == lengths[i]) {
                            pField->set_col_value("");
                        } else {
                            pField->set_col_value(stRow[i], lengths[i]);
                            uiDataLen += lengths[i];
                        }
                    }
                }
                oRsp.set_curcount(iRecordNum);
                oRsp.set_totalcount(iRecordNum);

                Response(oRsp);
                return (pDbi->GetErrno());
            } else {
                Response(pDbi->GetErrno(), pDbi->GetError().c_str());
                LOG4_ERROR("%d: %s", pDbi->GetErrno(), pDbi->GetError().c_str());
                return (pDbi->GetErrno());
            }
        } else {
            Response(pDbi->GetErrno(), pDbi->GetError().c_str());
            LOG4_DEBUG("%d: %s", pDbi->GetErrno(), pDbi->GetError().c_str());
            return (pDbi->GetErrno());
        }
    } else {
        LOG4_ERROR("%s\t%d: %s", strSql.c_str(), pDbi->GetErrno(), pDbi->GetError().c_str());
        Response(pDbi->GetErrno(), pDbi->GetError().c_str());
    }

    return (iResult);
}

void CmdDbOper::CheckConnection() {
    LOG4_TRACE("%s()", __FUNCTION__);
    //    time_t lNowTime = time(NULL);

    //长连接。检查mapdbi中的连接实例有无超时的，超时的连接删除
    //    std::map<std::string,tagConnection*>::iterator conn_iter;
    //    for (conn_iter = m_mapDbiPool.begin(); conn_iter != m_mapDbiPool.end(); )
    //    {
    //        int iTimeOut = m_iConnectionTimeout;
    //        if (conn_iter->second->iTimeout > 0)
    //        {
    //            iTimeOut = conn_iter->second->iTimeout;
    //        }
    //
    //        int iDiffTime = lNowTime - conn_iter->second->ullBeatTime;  //求时差
    //        if (iDiffTime > iTimeOut)
    //        {
    //            if (NULL != conn_iter->second)
    //            {
    //                delete conn_iter->second;
    //                conn_iter->second = NULL;
    //            }
    //            m_mapDbiPool.erase(conn_iter);
    //        }
    //        else
    //        {
    //            ++conn_iter;
    //        }
    //    }
}

void CmdDbOper::Response(int iErrno, const std::string& strErrMsg) {
    LOG4_TRACE("error %d: %s", iErrno, strErrMsg.c_str());
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    DataMem::MemRsp oRsp;
    oRsp.set_from(DataMem::MemRsp::FROM_DB);
    oRsp.set_err_no(iErrno);
    oRsp.set_err_msg(strErrMsg);
    oOutMsgBody.set_body(oRsp.SerializeAsString());
    oOutMsgHead.set_cmd(m_oInMsgHead.cmd() + 1);
    oOutMsgHead.set_seq(m_oInMsgHead.seq());
    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
    if (!GetLabor()->SendTo(m_stMsgShell, oOutMsgHead, oOutMsgBody)) {
        LOG4_ERROR("send to MsgShell(fd %d, seq %u) error!", m_stMsgShell.iFd, m_stMsgShell.ulSeq);
    }
}

bool CmdDbOper::Response(const DataMem::MemRsp& oRsp) {
    LOG4_TRACE("error %d: %s", oRsp.err_no(), oRsp.err_msg().c_str());
    MsgHead oOutMsgHead = m_oInMsgHead;
    MsgBody oOutMsgBody;
    oOutMsgBody.set_body(oRsp.SerializeAsString());
    oOutMsgHead.set_cmd(m_oInMsgHead.cmd() + 1);
    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
    if (!GetLabor()->SendTo(m_stMsgShell, oOutMsgHead, oOutMsgBody)) {
        LOG4_ERROR("send to MsgShell(fd %d, seq %u) error!", m_stMsgShell.iFd, m_stMsgShell.ulSeq);
        return (false);
    }
    return (true);
}

std::string CmdDbOper::GetFullTableName(const std::string& strTableName, uint32 uiFactor) {
    char szFullTableName[128] = {0};
    std::string strDbName = m_oDbConf["table"][strTableName]("db_name");
    int iTableNum = atoi(m_oDbConf["table"][strTableName]("table_num").c_str());
    if (1 == iTableNum) {
        snprintf(szFullTableName, sizeof(szFullTableName), "%s.%s", strDbName.c_str(), strTableName.c_str());
    } else {
        uint32 uiTableIndex = uiFactor % iTableNum;
        snprintf(szFullTableName, sizeof(szFullTableName), "%s.%s_%02d", strDbName.c_str(), strTableName.c_str(), uiTableIndex);
    }
    return (szFullTableName);
}

bool CmdDbOper::CreateSql(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi, std::string& strSql) {
    LOG4_TRACE("%s()", __FUNCTION__);

    strSql.clear();
    bool bResult = false;
    for (unsigned int i = 0; i < oQuery.db_operate().table_name().size(); ++i) {
        if ((oQuery.db_operate().table_name()[i] >= 'A' && oQuery.db_operate().table_name()[i] <= 'Z')
                || (oQuery.db_operate().table_name()[i] >= 'a' && oQuery.db_operate().table_name()[i] <= 'z')
                || (oQuery.db_operate().table_name()[i] >= '0' && oQuery.db_operate().table_name()[i] <= '9')
                || oQuery.db_operate().table_name()[i] == '_' || oQuery.db_operate().table_name()[i] == '.') {
            ;
        } else {
            return (false);
        }
    }

    switch (oQuery.db_operate().query_type()) {
    case DataMem::MemOperate::DbOperate::SELECT: {
            bResult = CreateSelect(oQuery, strSql);
            break;
        }
    case DataMem::MemOperate::DbOperate::INSERT:
    case DataMem::MemOperate::DbOperate::INSERT_IGNORE:
    case DataMem::MemOperate::DbOperate::REPLACE: {
            bResult = CreateInsert(oQuery, pDbi, strSql);
            break;
        }
    case DataMem::MemOperate::DbOperate::UPDATE: {
            bResult = CreateUpdate(oQuery, pDbi, strSql);
            break;
        }
    case DataMem::MemOperate::DbOperate::DELETE: {
            bResult = CreateDelete(oQuery, strSql);
            break;
        }
    }

    if (oQuery.db_operate().conditions_size() > 0) {
        std::string strCondition;
        bResult = CreateConditionGroup(oQuery, pDbi, strCondition);
        if (bResult) {
            strSql += std::string(" WHERE ") + strCondition;
        }
    }

    if (oQuery.db_operate().groupby_col_size() > 0) {
        std::string strGroupBy;
        bResult = CreateGroupBy(oQuery, strGroupBy);
        if (bResult) {
            strSql += std::string(" GROUP BY ") + strGroupBy;
        }
    }

    if (oQuery.db_operate().orderby_col_size() > 0) {
        std::string strOrderBy;
        bResult = CreateOrderBy(oQuery, strOrderBy);
        if (bResult) {
            strSql += std::string(" ORDER BY ") + strOrderBy;
        }
    }

    if (oQuery.db_operate().has_limit()) {
        std::string strLimit;
        bResult = CreateLimit(oQuery, strLimit);
        if (bResult) {
            strSql += std::string(" LIMIT ") + strLimit;
        }
    }

    return (bResult);
}

bool CmdDbOper::CreateSelect(const DataMem::MemOperate& oQuery, std::string& strSql) {
    strSql = "SELECT ";
    for (int i = 0; i < oQuery.db_operate().fields_size(); ++i) {
        if (!CheckColName(oQuery.db_operate().fields(i).col_name())) {
            LOG4_ERROR("invalidcol_name \"%s\".", oQuery.db_operate().fields(i).col_name().c_str());
            return (false);
        }
        if (i == 0) {
            if (oQuery.db_operate().fields(i).has_col_as()) {
                strSql += oQuery.db_operate().fields(i).col_name() + std::string(" AS ") + oQuery.db_operate().fields(i).col_as();
            } else {
                strSql += oQuery.db_operate().fields(i).col_name();
            }
        } else {
            if (oQuery.db_operate().fields(i).has_col_as()) {
                strSql += std::string(",") + oQuery.db_operate().fields(i).col_name() + std::string(" AS ") + oQuery.db_operate().fields(i).col_as();
            } else {
                strSql += std::string(",") + oQuery.db_operate().fields(i).col_name();
            }
        }
    }

    std::string strTableName;
    if (oQuery.db_operate().has_mod_factor() && 0 != oQuery.db_operate().mod_factor()) {
        strTableName = GetFullTableName(oQuery.db_operate().table_name(), oQuery.db_operate().mod_factor());
    } else {
        strTableName = GetFullTableName(oQuery.db_operate().table_name(), oQuery.section_factor());
    }
    if (strTableName.empty()) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "dbname_table is NULL");
        return false;
    }

    strSql += std::string(" FROM ") + strTableName;

    return true;
}

bool CmdDbOper::CreateInsert(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi, std::string& strSql) {
    LOG4_TRACE("%s()", __FUNCTION__);

    strSql = "";
    if (DataMem::MemOperate::DbOperate::INSERT == oQuery.db_operate().query_type()) {
        strSql = "INSERT INTO ";
    } else if (DataMem::MemOperate::DbOperate::INSERT_IGNORE == oQuery.db_operate().query_type()) {
        strSql = "INSERT IGNORE INTO ";
    } else if (DataMem::MemOperate::DbOperate::REPLACE == oQuery.db_operate().query_type()) {
        strSql = "REPLACE INTO ";
    }

    std::string strTableName;
    if (oQuery.db_operate().has_mod_factor() && 0 != oQuery.db_operate().mod_factor()) {
        strTableName = GetFullTableName(oQuery.db_operate().table_name(), oQuery.db_operate().mod_factor());
    } else {
        strTableName = GetFullTableName(oQuery.db_operate().table_name(), oQuery.section_factor());
    }
    if (strTableName.empty()) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "dbname_tbname is null");
        return false;
    }

    strSql += strTableName;

    for (int i = 0; i < oQuery.db_operate().fields_size(); ++i) {
        if (!CheckColName(oQuery.db_operate().fields(i).col_name())) {
            return (false);
        }
        if (i == 0) {
            strSql += std::string("(") + oQuery.db_operate().fields(i).col_name();
        } else {
            strSql += std::string(",") + oQuery.db_operate().fields(i).col_name();
        }
    }
    strSql += std::string(") ");

    for (int i = 0; i < oQuery.db_operate().fields_size(); ++i) {
        if (i == 0) {
            if (DataMem::STRING == oQuery.db_operate().fields(i).col_type()) {
                pDbi->EscapeString(m_szColValue, oQuery.db_operate().fields(i).col_value().c_str(), oQuery.db_operate().fields(i).col_value().size());
                strSql += std::string("VALUES('") + std::string(m_szColValue) + std::string("'");
            } else {
                for (unsigned int j = 0; j < oQuery.db_operate().fields(i).col_value().size(); ++j) {
                    if (oQuery.db_operate().fields(i).col_value()[j] >= '0' || oQuery.db_operate().fields(i).col_value()[j] <= '9'
                            || oQuery.db_operate().fields(i).col_value()[j] == '.') {
                        ;
                    } else {
                        return (false);
                    }
                }
                strSql += std::string("VALUES(") + oQuery.db_operate().fields(i).col_value();
            }
        } else {
            if (DataMem::STRING == oQuery.db_operate().fields(i).col_type()) {
                pDbi->EscapeString(m_szColValue, oQuery.db_operate().fields(i).col_value().c_str(), oQuery.db_operate().fields(i).col_value().size());
                strSql += std::string(",'") + std::string(m_szColValue) + std::string("'");
            } else {
                for (unsigned int j = 0; j < oQuery.db_operate().fields(i).col_value().size(); ++j) {
                    if (oQuery.db_operate().fields(i).col_value()[j] >= '0' || oQuery.db_operate().fields(i).col_value()[j] <= '9'
                            || oQuery.db_operate().fields(i).col_value()[j] == '.') {
                        ;
                    } else {
                        return (false);
                    }
                }
                strSql += std::string(",") + oQuery.db_operate().fields(i).col_value();
            }
        }
    }
    strSql += std::string(")");

    return true;
}

bool CmdDbOper::CreateUpdate(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi, std::string& strSql) {
    LOG4_TRACE("%s()", __FUNCTION__);

    strSql = "UPDATE ";
    std::string strTableName;
    if (oQuery.db_operate().has_mod_factor() && 0 != oQuery.db_operate().mod_factor()) {
        strTableName = GetFullTableName(oQuery.db_operate().table_name(), oQuery.db_operate().mod_factor());
    } else {
        strTableName = GetFullTableName(oQuery.db_operate().table_name(), oQuery.section_factor());
    }
    if (strTableName.empty()) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "dbname_tbname is null");
        return false;
    }

    strSql += strTableName;

    for (int i = 0; i < oQuery.db_operate().fields_size(); ++i) {
        if (!CheckColName(oQuery.db_operate().fields(i).col_name())) {
            return (false);
        }
        if (i == 0) {
            if (DataMem::STRING == oQuery.db_operate().fields(i).col_type()) {
                pDbi->EscapeString(m_szColValue, oQuery.db_operate().fields(i).col_value().c_str(), oQuery.db_operate().fields(i).col_value().size());
                strSql += std::string(" SET ") + oQuery.db_operate().fields(i).col_name() + std::string("=");
                strSql += std::string("'") + std::string(m_szColValue) + std::string("'");
            } else {
                for (unsigned int j = 0; j < oQuery.db_operate().fields(i).col_value().size(); ++j) {
                    if (oQuery.db_operate().fields(i).col_value()[j] >= '0' || oQuery.db_operate().fields(i).col_value()[j] <= '9'
                            || oQuery.db_operate().fields(i).col_value()[j] == '.') {
                        ;
                    } else {
                        return (false);
                    }
                }
                strSql += std::string(" SET ") + oQuery.db_operate().fields(i).col_name()
                          + std::string("=") + oQuery.db_operate().fields(i).col_value();
            }
        } else {
            if (DataMem::STRING == oQuery.db_operate().fields(i).col_type()) {
                pDbi->EscapeString(m_szColValue, oQuery.db_operate().fields(i).col_value().c_str(), oQuery.db_operate().fields(i).col_value().size());
                strSql += std::string(", ") + oQuery.db_operate().fields(i).col_name() + std::string("=");
                strSql += std::string("'") + std::string(m_szColValue) + std::string("'");
            } else {
                for (unsigned int j = 0; j < oQuery.db_operate().fields(i).col_value().size(); ++j) {
                    if (oQuery.db_operate().fields(i).col_value()[j] >= '0' || oQuery.db_operate().fields(i).col_value()[j] <= '9'
                            || oQuery.db_operate().fields(i).col_value()[j] == '.') {
                        ;
                    } else {
                        return (false);
                    }
                }
                strSql += std::string(", ") + oQuery.db_operate().fields(i).col_name()
                          + std::string("=") + oQuery.db_operate().fields(i).col_value();
            }
        }
    }

    return true;
}

bool CmdDbOper::CreateDelete(const DataMem::MemOperate& oQuery, std::string& strSql) {
    LOG4_TRACE("%s()", __FUNCTION__);

    strSql = "DELETE FROM ";
    std::string strTableName;
    if (oQuery.db_operate().has_mod_factor() && 0 != oQuery.db_operate().mod_factor()) {
        strTableName = GetFullTableName(oQuery.db_operate().table_name(), oQuery.db_operate().mod_factor());
    } else {
        strTableName = GetFullTableName(oQuery.db_operate().table_name(), oQuery.section_factor());
    }
    if (strTableName.empty()) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "dbname_tbname is null");
        return false;
    }

    strSql += strTableName;

    return true;
}

bool CmdDbOper::CreateCondition(const DataMem::MemOperate::DbOperate::Condition& oCondition, loss::CMysqlDbi* pDbi, std::string& strCondition) {
    LOG4_TRACE("%s()", __FUNCTION__);

    if (!CheckColName(oCondition.col_name())) {
        return (false);
    }
    strCondition = oCondition.col_name();
    switch (oCondition.relation()) {
    case DataMem::MemOperate::DbOperate::Condition::EQ:
        strCondition += std::string("=");
        break;
    case DataMem::MemOperate::DbOperate::Condition::NE:
        strCondition += std::string("<>");
        break;
    case DataMem::MemOperate::DbOperate::Condition::GT:
        strCondition += std::string(">");
        break;
    case DataMem::MemOperate::DbOperate::Condition::LT:
        strCondition += std::string("<");
        break;
    case DataMem::MemOperate::DbOperate::Condition::GE:
        strCondition += std::string(">=");
        break;
    case DataMem::MemOperate::DbOperate::Condition::LE:
        strCondition += std::string("<=");
        break;
    case DataMem::MemOperate::DbOperate::Condition::LIKE:
        strCondition += std::string(" LIKE ");
        break;
    case DataMem::MemOperate::DbOperate::Condition::IN:
        strCondition += std::string(" IN (");
        break;
    default:
        break;
    }

    if (oCondition.has_col_name_right()) {
        if (!CheckColName(oCondition.col_name_right())) {
            return (false);
        }
        strCondition += oCondition.col_name_right();
    } else {
        for (int i = 0; i < oCondition.col_values_size(); ++i) {
            if (i > 0) {
                strCondition += std::string(",");
            }

            if (DataMem::STRING == oCondition.col_type()) {
                pDbi->EscapeString(m_szColValue, oCondition.col_values(i).c_str(), oCondition.col_values(i).size());
                strCondition += std::string("'") + std::string(m_szColValue) + std::string("'");
            } else {
                for (unsigned int j = 0; j < oCondition.col_values(i).size(); ++j) {
                    if (oCondition.col_values(i)[j] >= '0' || oCondition.col_values(i)[j] <= '9'
                            || oCondition.col_values(i)[j] == '.') {
                        ;
                    } else {
                        return (false);
                    }
                }
                strCondition += oCondition.col_values(i);
            }
        }

        if (DataMem::MemOperate::DbOperate::Condition::IN == oCondition.relation()) {
            strCondition += std::string(")");
        }
    }

    return true;
}

bool CmdDbOper::CreateConditionGroup(const DataMem::MemOperate& oQuery, loss::CMysqlDbi* pDbi, std::string& strCondition) {
    LOG4_TRACE("%s()", __FUNCTION__);

    bool bResult = false;
    for (int i = 0; i < oQuery.db_operate().conditions_size(); ++i) {
        if (i > 0) {
            if (oQuery.db_operate().has_group_relation()) {
                if (DataMem::MemOperate::DbOperate::ConditionGroup::OR == oQuery.db_operate().group_relation()) {
                    strCondition += std::string(" OR ");
                } else {
                    strCondition += std::string(" AND ");
                }
            } else {
                strCondition += std::string(" AND ");
            }
        }
        if (oQuery.db_operate().conditions_size() > 1) {
            strCondition += std::string("(");
        }
        for (int j = 0; j < oQuery.db_operate().conditions(i).condition_size(); ++j) {
            std::string strRelation;
            std::string strOneCondition;
            if (DataMem::MemOperate::DbOperate::ConditionGroup::OR == oQuery.db_operate().conditions(i).relation()) {
                strRelation = " OR ";
            } else {
                strRelation = " AND ";
            }
            bResult = CreateCondition(oQuery.db_operate().conditions(i).condition(j), pDbi, strOneCondition);
            if (bResult) {
                if (j > 0) {
                    strCondition += strRelation;
                }
                strCondition += strOneCondition;
            } else {
                return (bResult);
            }
        }
        if (oQuery.db_operate().conditions_size() > 1) {
            strCondition += std::string(")");
        }
    }

    return true;
}

bool CmdDbOper::CreateGroupBy(const DataMem::MemOperate& oQuery, std::string& strGroupBy) {
    LOG4_TRACE("%s()", __FUNCTION__);

    strGroupBy = "";
    for (int i = 0; i < oQuery.db_operate().groupby_col_size(); ++i) {
        if (!CheckColName(oQuery.db_operate().groupby_col(i))) {
            return (false);
        }
        if (i == 0) {
            strGroupBy += oQuery.db_operate().groupby_col(i);
        } else {
            strGroupBy += std::string(",") + oQuery.db_operate().groupby_col(i);
        }
    }

    return true;
}

bool CmdDbOper::CreateOrderBy(const DataMem::MemOperate& oQuery, std::string& strOrderBy) {
    LOG4_TRACE("%s()", __FUNCTION__);

    strOrderBy = "";
    for (int i = 0; i < oQuery.db_operate().orderby_col_size(); ++i) {
        if (!CheckColName(oQuery.db_operate().orderby_col(i).col_name())) {
            return (false);
        }
        if (i == 0) {
            if (DataMem::MemOperate::DbOperate::OrderBy::DESC == oQuery.db_operate().orderby_col(i).relation()) {
                strOrderBy += oQuery.db_operate().orderby_col(i).col_name() + std::string(" DESC");
            } else {
                strOrderBy += oQuery.db_operate().orderby_col(i).col_name() + std::string(" ASC");
            }
        } else {
            if (DataMem::MemOperate::DbOperate::OrderBy::DESC == oQuery.db_operate().orderby_col(i).relation()) {
                strOrderBy += std::string(",") + oQuery.db_operate().orderby_col(i).col_name() + std::string(" DESC");
            } else {
                strOrderBy += std::string(",") + oQuery.db_operate().orderby_col(i).col_name() + std::string(" ASC");
            }
        }
    }

    return true;
}

bool CmdDbOper::CreateLimit(const DataMem::MemOperate& oQuery, std::string& strLimit) {
    LOG4_TRACE("%s()", __FUNCTION__);

    char szLimit[16] = {0};
    if (oQuery.db_operate().has_limit_from()) {
        snprintf(szLimit, sizeof(szLimit), "%u,%u", oQuery.db_operate().limit_from(), oQuery.db_operate().limit());
        strLimit = szLimit;
    } else {
        snprintf(szLimit, sizeof(szLimit), "%u", oQuery.db_operate().limit());
        strLimit = szLimit;
    }
    return true;
}

bool CmdDbOper::CheckColName(const std::string& strColName) {
    std::string strUpperColName;
    for (unsigned int i = 0; i < strColName.size(); ++i) {
        if (strColName[i] == '\'' || strColName[i] == '"' || strColName[i] == '\\'
                || strColName[i] == ';' || strColName[i] == '*' || strColName[i] == ' ') {
            return (false);
        }

        if (strColName[i] >= 'a' && strColName[i] <= 'z') {
            strUpperColName[i] = strColName[i] - 32;
        } else {
            strUpperColName[i] = strColName[i];
        }
    }

    if (strUpperColName == std::string("AND") || strUpperColName == std::string("OR")
            || strUpperColName == std::string("UPDATE") || strUpperColName == std::string("DELETE")
            || strUpperColName == std::string("UNION") || strUpperColName == std::string("AS")
            || strUpperColName == std::string("CHANGE") || strUpperColName == std::string("SET")
            || strUpperColName == std::string("TRUNCATE") || strUpperColName == std::string("DESC")) {
        return (false);
    }
    return true;
}


} /* namespace oss */
