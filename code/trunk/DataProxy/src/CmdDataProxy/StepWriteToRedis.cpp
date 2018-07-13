/*******************************************************************************
 * Project:  DataProxyServer
 * @file     StepWriteToRedis.cpp
 * @brief
 * @author   xxx
 * @date:    2016年3月19日
 * @note
 * Modify history:
 ******************************************************************************/
#include "StepWriteToRedis.hpp"

namespace oss {

StepWriteToRedis::StepWriteToRedis(const tagMsgShell& stMsgShell, const MsgHead& oInMsgHead,
                                   const DataMem::MemOperate::RedisOperate& oRedisOperate,
                                   SessionRedisNode* pNodeSession, Step* pNextStep)
    : RedisStorageStep(pNextStep),
      m_stMsgShell(stMsgShell), m_oReqMsgHead(oInMsgHead), m_oRedisOperate(oRedisOperate),
      m_pNodeSession(pNodeSession), pStepSetTtl(NULL) {
}

StepWriteToRedis::~StepWriteToRedis() {
}

E_CMD_STATUS StepWriteToRedis::Emit(int iErrno, const std::string& strErrMsg, const std::string& strErrShow) {
    LOG4_TRACE("%s()", __FUNCTION__);
    if (!m_pNodeSession) {
        return (STATUS_CMD_FAULT);
    }
    loss::RedisCmd* pRedisCmd = RedisCmd();
    if (pRedisCmd) {
        bool bGetRedisNode;
        if (m_oRedisOperate.has_hash_key()) {
            bGetRedisNode = m_pNodeSession->GetRedisNode(m_oRedisOperate.hash_key(), m_strMasterNode, m_strSlaveNode);
        } else {
            bGetRedisNode = m_pNodeSession->GetRedisNode(m_oRedisOperate.key_name(), m_strMasterNode, m_strSlaveNode);
        }
        if (!bGetRedisNode) {
            Response(m_stMsgShell, m_oReqMsgHead, ERR_REDIS_NODE_NOT_FOUND, "redis node not found!");
            return (STATUS_CMD_FAULT);
        }

        LOG4_DEBUG("%s", m_oRedisOperate.DebugString().c_str());
        pRedisCmd->SetCmd(m_oRedisOperate.redis_cmd_write());
        pRedisCmd->Append(m_oRedisOperate.key_name());
        for (int i = 0; i < m_oRedisOperate.fields_size(); ++i) {
            if (m_oRedisOperate.fields(i).has_col_name() && m_oRedisOperate.fields(i).col_name().size() > 0) {
                pRedisCmd->Append(m_oRedisOperate.fields(i).col_name());
            }
            if (m_oRedisOperate.fields(i).has_col_value()) { // && m_oRedisOperate.fields(i).col_value().size() > 0)
                pRedisCmd->Append(m_oRedisOperate.fields(i).col_value());
            }
        }
        if (RegisterCallback(m_strMasterNode, this)) {
            return (STATUS_CMD_RUNNING);
        }
        Response(m_stMsgShell, m_oReqMsgHead, ERR_REGISTERCALLBACK_REDIS, "RegisterCallback(RedisStep) error!");
        LOG4_ERROR("RegisterCallback(%s, StepWriteToRedis) error!", m_strMasterNode.c_str());
    }
    return (STATUS_CMD_FAULT);
}

E_CMD_STATUS StepWriteToRedis::Callback(const redisAsyncContext* c, int status, redisReply* pReply) {
    LOG4_TRACE("%s()", __FUNCTION__);
    char szErrMsg[256] = {0};
    if (REDIS_OK != status) {
        snprintf(szErrMsg, sizeof(szErrMsg), "redis %s cmd status %d!", m_strMasterNode.c_str(), status);
        Response(m_stMsgShell, m_oReqMsgHead, ERR_REDIS_CMD, szErrMsg);
        return (STATUS_CMD_FAULT);
    }
    if (NULL == pReply) {
        snprintf(szErrMsg, sizeof(szErrMsg), "redis %s error %d: %s!", m_strMasterNode.c_str(), c->err, c->errstr);
        Response(m_stMsgShell, m_oReqMsgHead, ERR_REDIS_CMD, szErrMsg);
        return (STATUS_CMD_FAULT);
    }
    LOG4_TRACE("redis reply->type = %d", pReply->type);
    if (REDIS_REPLY_ERROR == pReply->type) {
        snprintf(szErrMsg, sizeof(szErrMsg), "redis %s error %d: %s!", m_strMasterNode.c_str(), pReply->type, pReply->str);
        Response(m_stMsgShell, m_oReqMsgHead, ERR_REDIS_CMD, szErrMsg);
        return (STATUS_CMD_FAULT);
    }

    // 从redis中读到数据
    DataMem::MemRsp oRsp;
    oRsp.set_err_no(ERR_OK);
    oRsp.set_err_msg("OK");
    oRsp.set_from(DataMem::MemRsp::FROM_REDIS);
    // 字符串类型的set命令的返回值的类型是REDIS_REPLY_STATUS，然后只有当返回信息是"OK"时，才表示该命令执行成功。
    if (pReply->type == REDIS_REPLY_STATUS) {
        if (strcasecmp(pReply->str, "OK") != 0) {
            oRsp.set_err_no(pReply->type);
            oRsp.set_err_msg(pReply->str, pReply->len);
        }
    } else if (REDIS_REPLY_STRING == pReply->type) {
        DataMem::Record* pRecord = oRsp.add_record_data();
        DataMem::Field* pField = pRecord->add_field_info();
        pField->set_col_value(pReply->str, pReply->len);
    } else if (REDIS_REPLY_INTEGER == pReply->type) {
        DataMem::Record* pRecord = oRsp.add_record_data();
        DataMem::Field* pField = pRecord->add_field_info();
        char szValue[32] = {0};
        snprintf(szValue, 32, "%lld", pReply->integer);
        pField->set_col_value(szValue);
    } else if (REDIS_REPLY_ARRAY == pReply->type) {
        LOG4_TRACE("pReply->type = %d, pReply->elements = %u", pReply->type, pReply->elements);
        if (0 == pReply->elements) {
            Response(m_stMsgShell, m_oReqMsgHead, ERR_OK, "OK");        // 操作是正常的，但结果集为空
        }
        if (oss::REDIS_T_HASH == m_oRedisOperate.redis_structure()) {
            ReadReplyArrayForHashWithoutDataSet(pReply);
        } else {
            ReadReplyArrayWithoutDataSet(pReply);
        }
    } else {
        snprintf(szErrMsg, sizeof(szErrMsg), "unexprected redis reply type %d: %s!", pReply->type, pReply->str);
        LOG4_WARN("unexprected redis reply type %d: %s!", pReply->type, pReply->str);
        //        Response(m_stMsgShell, m_oReqMsgHead, ERR_UNEXPECTED_REDIS_REPLY, szErrMsg);
        //        return(STATUS_CMD_FAULT);
    }

    Response(m_stMsgShell, m_oReqMsgHead, oRsp);
    NextStep(oRsp.err_no(), oRsp.err_msg());

    // 设置过期时间
    if (m_oRedisOperate.has_key_ttl() && m_oRedisOperate.key_ttl() != 0) {
        pStepSetTtl = new StepSetTtl(m_strMasterNode, m_oRedisOperate.key_name(), m_oRedisOperate.key_ttl());
        if (NULL == pStepSetTtl) {
            LOG4_ERROR("malloc space for StepSetTtl error!");
            return (STATUS_CMD_FAULT);
        }
        Pretreat(pStepSetTtl);
        pStepSetTtl->Emit(ERR_OK);
    }
    return (STATUS_CMD_COMPLETED);
}

bool StepWriteToRedis::ReadReplyArrayForHashWithoutDataSet(redisReply* pReply) {
    LOG4_TRACE("%s()", __FUNCTION__);
    char szErrMsg[256] = {0};
    char szValue[32] = {0};
    DataMem::MemRsp oRsp;
    oRsp.set_err_no(ERR_OK);
    oRsp.set_err_msg("OK");
    oRsp.set_from(DataMem::MemRsp::FROM_REDIS);
    oRsp.set_totalcount(1);
    int iDataLen = oRsp.ByteSize();
    DataMem::Record* pRecord = oRsp.add_record_data();
    for (size_t i = 0; i < pReply->elements; ++i) {
        DataMem::Field* pField = pRecord->add_field_info();
        if (REDIS_REPLY_STRING == pReply->element[i]->type) {
            pField->set_col_value(pReply->element[i]->str, pReply->element[i]->len);
            iDataLen += pReply->element[i]->len;
        } else if (REDIS_REPLY_INTEGER == pReply->element[i]->type) {
            snprintf(szValue, 32, "%lld", pReply->element[i]->integer);
            pField->set_col_value(szValue);
            iDataLen += 20;
        } else if (REDIS_REPLY_NIL == pReply->element[i]->type) {
            pField->set_col_value("");
            LOG4_WARN("pReply->element[%d]->type == REDIS_REPLY_NIL", i);
        } else {
            LOG4_ERROR("pReply->element[%d]->type = %d", i, pReply->element[i]->type);
            snprintf(szErrMsg, sizeof(szErrMsg), "unexprected redis reply type %d and element[%d] type %d: %s!",
                     pReply->type, i, pReply->element[i]->type, pReply->element[i]->str);
            Response(m_stMsgShell, m_oReqMsgHead, ERR_UNEXPECTED_REDIS_REPLY, szErrMsg);
            return (false);
        }

        if (iDataLen > 1000000) { // pb 最大限制
            Response(m_stMsgShell, m_oReqMsgHead, ERR_RESULTSET_EXCEED, "hash result set exceed 1 MB!");
            return (false);
        }
    }

    oRsp.set_curcount(1);
    if (Response(m_stMsgShell, m_oReqMsgHead, oRsp)) {
        return (true);
    }
    return (false);
}

bool StepWriteToRedis::ReadReplyArrayWithoutDataSet(redisReply* pReply) {
    LOG4_TRACE("%s()", __FUNCTION__);
    char szErrMsg[256] = {0};
    char szValue[32] = {0};
    DataMem::MemRsp oRsp;
    oRsp.set_err_no(ERR_OK);
    oRsp.set_err_msg("OK");
    oRsp.set_from(DataMem::MemRsp::FROM_REDIS);
    oRsp.set_totalcount(pReply->elements);
    int iDataLen = oRsp.ByteSize();
    for (size_t i = 0; i < pReply->elements; ++i) {
        if (pReply->element[i]->len > 1000000) { // pb 最大限制
            Response(m_stMsgShell, m_oReqMsgHead, ERR_RESULTSET_EXCEED, "hgetall result set exceed 1 MB!");
            return (false);
        }
        if (iDataLen + pReply->element[i]->len > 1000000) { // pb 最大限制
            oRsp.set_curcount(i + 1);
            if (Response(m_stMsgShell, m_oReqMsgHead, oRsp)) {
                oRsp.clear_record_data();
                iDataLen = 0;
            } else {
                return (false);
            }
        }
        DataMem::Record* pRecord = oRsp.add_record_data();
        DataMem::Field* pField = pRecord->add_field_info();
        if (REDIS_REPLY_STRING == pReply->element[i]->type) {
            pField->set_col_value(pReply->element[i]->str, pReply->element[i]->len);
            iDataLen += pReply->element[i]->len;
        } else if (REDIS_REPLY_INTEGER == pReply->element[i]->type) {
            snprintf(szValue, 32, "%lld", pReply->element[i]->integer);
            pField->set_col_value(szValue);
            iDataLen += 20;
        } else if (REDIS_REPLY_NIL == pReply->element[i]->type) {
            pField->set_col_value("");
            LOG4_WARN("pReply->element[%d]->type == REDIS_REPLY_NIL", i);
        } else {
            LOG4_ERROR("pReply->element[%d]->type = %d", i, pReply->element[i]->type);
            snprintf(szErrMsg, sizeof(szErrMsg), "unexprected redis reply type %d and element[%d] type %d: %s!",
                     pReply->type, i, pReply->element[i]->type, pReply->element[i]->str);
            Response(m_stMsgShell, m_oReqMsgHead, ERR_UNEXPECTED_REDIS_REPLY, szErrMsg);
            return (false);
        }
    }

    oRsp.set_curcount(pReply->elements);
    if (Response(m_stMsgShell, m_oReqMsgHead, oRsp)) {
        return (true);
    }
    return (false);
}

} /* namespace oss */
