/*******************************************************************************
 * Project:  DataProxyServer
 * @file     StepReadFromDb.cpp
 * @brief
 * @author   xxx
 * @date:    2016年3月19日
 * @note
 * Modify history:
 ******************************************************************************/
#include "StepSendToDbAgent.hpp"

namespace oss {

StepSendToDbAgent::StepSendToDbAgent(const tagMsgShell& stMsgShell, const MsgHead& oInMsgHead,
                                     const DataMem::MemOperate& oMemOperate,
                                     SessionRedisNode* pNodeSession,
                                     int iRelative,
                                     const loss::CJsonObject* pTableField,
                                     const std::string& strKeyField,
                                     const loss::CJsonObject* pJoinField)
    : m_stMsgShell(stMsgShell), m_oReqMsgHead(oInMsgHead), m_oMemOperate(oMemOperate),
      m_iRelative(iRelative), m_strKeyField(strKeyField), m_oTableField(pTableField), m_oJoinField(pJoinField),
      m_bFieldFilter(false), m_bNeedResponse(true),
      m_pNodeSession(pNodeSession), pStepWriteBackToRedis(NULL) {
}

StepSendToDbAgent::~StepSendToDbAgent() {
}

E_CMD_STATUS StepSendToDbAgent::Emit(int iErrno, const std::string& strErrMsg, const std::string& strErrShow) {
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    if (DataMem::MemOperate::DbOperate::SELECT != m_oMemOperate.db_operate().query_type()
            && m_oMemOperate.has_redis_operate()) {
        m_bNeedResponse = false;
    }
    LOG4_TRACE("%s(m_bNeedResponse = %d)", __FUNCTION__, m_bNeedResponse);
    if (RELATIVE_DATASET == m_iRelative
            && DataMem::MemOperate::DbOperate::SELECT == m_oMemOperate.db_operate().query_type()) {
        for (int i = 0; i < m_oMemOperate.db_operate().fields_size(); ++i) {
            if ((m_oTableField(i) != m_oMemOperate.db_operate().fields(i).col_name())
                    && (m_oMemOperate.db_operate().fields(i).has_col_as()
                        && m_oTableField(i) != m_oMemOperate.db_operate().fields(i).col_as())) {
                m_bFieldFilter = true;
                break;
            }
        }
        // 客户端请求表的部分字段，但redis需要所有字段来回写，故修改向数据库的请求数据，待返回结果集再筛选部分字段回复客户端，所有字段用来回写redis
        if (m_bFieldFilter) {
            DataMem::MemOperate oMemOperate = m_oMemOperate;
            DataMem::MemOperate::DbOperate* oDbOper = oMemOperate.mutable_db_operate();
            oDbOper->clear_fields();
            for (int i = 0; i < m_oTableField.GetArraySize(); ++i) {
                DataMem::Field* pField = oDbOper->add_fields();
                pField->set_col_name(m_oTableField(i));
            }
            oOutMsgBody.set_body(oMemOperate.SerializeAsString());
        } else {
            oOutMsgBody.set_body(m_oMemOperate.SerializeAsString());
        }
    } else {
        oOutMsgBody.set_body(m_oMemOperate.SerializeAsString());
    }
    oOutMsgHead.set_cmd(CMD_REQ_STORATE);
    oOutMsgHead.set_seq(GetSequence());
    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());

    if (m_oMemOperate.db_operate().SELECT == m_oMemOperate.db_operate().query_type()) {
        if (!SendToNext("DBAGENT_R", oOutMsgHead, oOutMsgBody)) {
            LOG4_ERROR("SendToNext(\"DBAGENT_R\") error!");
            Response(m_stMsgShell, m_oReqMsgHead, ERR_DATA_TRANSFER, "SendToNext(\"DBAGENT_R\") error!");
            return (STATUS_CMD_FAULT);
        }
    } else {
        if (!SendToNext("DBAGENT_W", oOutMsgHead, oOutMsgBody)) {
            LOG4_ERROR("SendToNext(\"DBAGENT_W\") error!");
            Response(m_stMsgShell, m_oReqMsgHead, ERR_DATA_TRANSFER, "SendToNext(\"DBAGENT_W\") error!");
            return (STATUS_CMD_FAULT);
        }
    }
    return (STATUS_CMD_RUNNING);
}

E_CMD_STATUS StepSendToDbAgent::Callback(const tagMsgShell& stMsgShell,
        const MsgHead& oInMsgHead, const MsgBody& oInMsgBody, void* data) {
    LOG4_TRACE("%s()", __FUNCTION__);
    MsgHead oOutMsgHead = m_oReqMsgHead;
    MsgBody oOutMsgBody;
    DataMem::MemRsp oRsp;
    oRsp.set_from(DataMem::MemRsp::FROM_DB);
    if (!oRsp.ParseFromString(oInMsgBody.body())) {
        LOG4_ERROR("DataMem::MemRsp oRsp.ParseFromString() failed!");
        if (m_bNeedResponse) {
            oOutMsgBody.set_body(oInMsgBody.body());
            oOutMsgHead.set_cmd(m_oReqMsgHead.cmd() + 1);
            oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
            if (!SendTo(m_stMsgShell, oOutMsgHead, oOutMsgBody)) {
                LOG4_ERROR("send to MsgShell(fd %d, seq %u) error!", m_stMsgShell.iFd, m_stMsgShell.ulSeq);
                return (STATUS_CMD_FAULT);
            }
        }
        return (STATUS_CMD_FAULT);
    }

    WriteBackToRedis(m_stMsgShell, m_oReqMsgHead, oRsp);

    if (m_bNeedResponse) {
        if (m_bFieldFilter) {   // 需筛选回复字段
            DataMem::MemRsp oRspToClient;
            oRspToClient.set_err_no(oRsp.err_no());
            oRspToClient.set_err_msg(oRsp.err_msg());
            std::vector<int> vecColForClient;
            std::vector<int>::iterator iter;
            for (int i = 0; i < m_oMemOperate.db_operate().fields_size(); ++i) {
                for (int j = 0; j < m_oTableField.GetArraySize(); ++j) {
                    if ((m_oTableField(j) == m_oMemOperate.db_operate().fields(i).col_name())
                            || (m_oTableField(j) == m_oMemOperate.db_operate().fields(i).col_as())) {
                        vecColForClient.push_back(j);
                        break;
                    }
                }
            }
            for (int i = 0; i < oRsp.record_data_size(); i++) {
                DataMem::Record* pRecord = oRspToClient.add_record_data();
                for (iter = vecColForClient.begin(); iter != vecColForClient.end(); ++iter) {
                    DataMem::Field* pField = pRecord->add_field_info();
                    pField->set_col_value(oRsp.record_data(i).field_info(*iter).col_value());
                }
            }
            oOutMsgBody.set_body(oRspToClient.SerializeAsString());
        } else {
            oOutMsgBody.set_body(oInMsgBody.body());
        }
        oOutMsgHead.set_cmd(m_oReqMsgHead.cmd() + 1);
        oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
        if (!SendTo(m_stMsgShell, oOutMsgHead, oOutMsgBody)) {
            LOG4_ERROR("send to MsgShell(fd %d, seq %u) error!", m_stMsgShell.iFd, m_stMsgShell.ulSeq);
            return (STATUS_CMD_FAULT);
        }
    } else { // 无需回复请求方
        if (ERR_OK != oRsp.err_no()) {
            LOG4_ERROR("%d: %s", oRsp.err_no(), oRsp.err_msg().c_str());
        }
    }

    if (oRsp.totalcount() == oRsp.curcount()) {
        return (STATUS_CMD_COMPLETED);
    }
    return (STATUS_CMD_RUNNING);
}

E_CMD_STATUS StepSendToDbAgent::Timeout() {
    Response(m_stMsgShell, m_oReqMsgHead, ERR_TIMEOUT, "read from db or write to db timeout!");
    return (STATUS_CMD_FAULT);
}

void StepSendToDbAgent::WriteBackToRedis(const tagMsgShell& stMsgShell, const MsgHead& oInMsgHead, const DataMem::MemRsp& oRsp) {
    if (m_oMemOperate.has_redis_operate()
            && (DataMem::MemOperate::RedisOperate::T_READ == m_oMemOperate.redis_operate().op_type())
            && (ERR_OK == oRsp.err_no())) {
        pStepWriteBackToRedis = new StepWriteBackToRedis(stMsgShell, oInMsgHead, m_oMemOperate,
                m_pNodeSession, m_iRelative, m_strKeyField, &m_oJoinField);
        if (NULL == pStepWriteBackToRedis) {
            LOG4_ERROR("malloc space for pStepWriteBackToRedis error!");
            return;
        }
        Pretreat(pStepWriteBackToRedis);
        pStepWriteBackToRedis->Emit(oRsp);
    }
}

} /* namespace oss */
