/*******************************************************************************
 * Project:  DataProxyServer
 * @file     StepWriteBackToRedis.cpp
 * @brief
 * @author   xxx
 * @date:    2016年3月19日
 * @note
 * Modify history:
 ******************************************************************************/
#include "StepWriteBackToRedis.hpp"

namespace oss {

StepWriteBackToRedis::StepWriteBackToRedis(const tagMsgShell& stMsgShell, const MsgHead& oInMsgHead,
        const DataMem::MemOperate& oMemOperate, SessionRedisNode* pNodeSession,
        int iRelative, const std::string& strKeyField, const loss::CJsonObject* pJoinField)
    : m_stMsgShell(stMsgShell), m_oReqMsgHead(oInMsgHead), m_oMemOperate(oMemOperate),
      m_iRelative(iRelative), m_strKeyField(strKeyField), m_oJoinField(pJoinField),
      m_pNodeSession(pNodeSession), pStepSetTtl(NULL) {
}

StepWriteBackToRedis::~StepWriteBackToRedis() {
}

E_CMD_STATUS StepWriteBackToRedis::Emit(const DataMem::MemRsp& oRsp) {
    LOG4_TRACE("%s()", __FUNCTION__);
    if (!m_pNodeSession) {
        LOG4_ERROR("m_pNodeSession is null!");
        return (STATUS_CMD_FAULT);
    }
    loss::RedisCmd* pRedisCmd = RedisCmd();
    if (pRedisCmd) {
        bool bGetRedisNode;
        if (m_oMemOperate.redis_operate().has_hash_key()) {
            bGetRedisNode = m_pNodeSession->GetRedisNode(m_oMemOperate.redis_operate().hash_key(), m_strMasterNode, m_strSlaveNode);
        } else {
            bGetRedisNode = m_pNodeSession->GetRedisNode(m_oMemOperate.redis_operate().key_name(), m_strMasterNode, m_strSlaveNode);
        }
        if (!bGetRedisNode) {
            LOG4_ERROR("%d: redis node not found!", ERR_REDIS_NODE_NOT_FOUND);
            return (STATUS_CMD_FAULT);
        }

        pRedisCmd->SetCmd(m_oMemOperate.redis_operate().redis_cmd_write());
        pRedisCmd->Append(m_oMemOperate.redis_operate().key_name());
        if (RELATIVE_JOIN == m_iRelative) {
            if (!MakeCmdWithJoin(oRsp, pRedisCmd)) {
                return (STATUS_CMD_FAULT);
            }
        } else if (RELATIVE_DATASET == m_iRelative) {
            if (oss::REDIS_T_HASH == m_oMemOperate.redis_operate().redis_structure()) {
                if (!MakeCmdForHashWithDataSet(oRsp, pRedisCmd)) {
                    return (STATUS_CMD_FAULT);
                }
            } else {
                if (!MakeCmdWithDataSet(oRsp, pRedisCmd)) {
                    return (STATUS_CMD_FAULT);
                }
            }
        } else {
            if (oss::REDIS_T_HASH == m_oMemOperate.redis_operate().redis_structure()) {
                if (!MakeCmdForHashWithoutDataSet(oRsp, pRedisCmd)) {
                    return (STATUS_CMD_FAULT);
                }
            } else {
                if (!MakeCmdWithoutDataSet(oRsp, pRedisCmd)) {
                    return (STATUS_CMD_FAULT);
                }
            }
        }
        if (RegisterCallback(m_strMasterNode, this)) {
            return (STATUS_CMD_RUNNING);
        }
        LOG4_ERROR("RegisterCallback(%s, StepWriteToRedis) error!", m_strMasterNode.c_str());
    }
    return (STATUS_CMD_FAULT);
}

E_CMD_STATUS StepWriteBackToRedis::Callback(const redisAsyncContext* c, int status, redisReply* pReply) {
    LOG4_TRACE("%s()", __FUNCTION__);
    if (REDIS_OK != status) {
        LOG4_ERROR("redis %s cmd status %d!", m_strMasterNode.c_str(), status);
        return (STATUS_CMD_FAULT);
    }
    if (NULL == pReply) {
        LOG4_ERROR("redis %s error %d: %s!", m_strMasterNode.c_str(), c->err, c->errstr);
        return (STATUS_CMD_FAULT);
    }
    LOG4_TRACE("redis reply->type = %d", pReply->type);
    if (REDIS_REPLY_ERROR == pReply->type) {
        LOG4_ERROR("redis %s error %d: %s!", m_strMasterNode.c_str(), pReply->type, pReply->str);
        return (STATUS_CMD_FAULT);
    }
    if (REDIS_REPLY_STATUS == pReply->type) {
        LOG4_DEBUG("redis %s status %d: %s!", m_strMasterNode.c_str(), pReply->type, pReply->str);
    }

    // 设置过期时间
    if (m_oMemOperate.redis_operate().has_key_ttl() && m_oMemOperate.redis_operate().key_ttl() != 0) {
        pStepSetTtl = new StepSetTtl(m_strMasterNode,
                                     m_oMemOperate.redis_operate().key_name(), m_oMemOperate.redis_operate().key_ttl());
    } else {
        // 未设置过期时间的读取操作，从DB中读取回写redis只保留3天
        pStepSetTtl = new StepSetTtl(m_strMasterNode, m_oMemOperate.redis_operate().key_name(), 259200);
    }
    if (NULL == pStepSetTtl) {
        LOG4_ERROR("malloc space for StepSetTtl error!");
        return (STATUS_CMD_FAULT);
    }
    Pretreat(pStepSetTtl);
    pStepSetTtl->Emit(ERR_OK);
    return (STATUS_CMD_COMPLETED);
}

bool StepWriteBackToRedis::MakeCmdWithJoin(const DataMem::MemRsp& oRsp, loss::RedisCmd* pRedisCmd) {
    LOG4_TRACE("%s()", __FUNCTION__);
    if (0 == oRsp.record_data_size()) {
        return (false);
    }
    int iKeyFieldIndex = -1;
    std::vector<int> vecJoinFieldIndex;
    std::vector<int>::iterator join_field_index_iter;
    std::map<int, std::string> mapJoinFieldValues;
    std::map<int, std::string>::iterator join_field_value_iter;
    std::set<std::string> setJoinFields;
    std::set<std::string>::iterator join_field_iter;
    for (int j = 0; j < m_oJoinField.GetArraySize(); ++j) {
        setJoinFields.insert(m_oJoinField(j));
    }
    for (int i = 0; i < m_oMemOperate.db_operate().fields_size(); ++i) {
        if (m_oMemOperate.db_operate().fields(i).has_col_as()) {
            if (m_strKeyField.size() > 0 && m_strKeyField == m_oMemOperate.db_operate().fields(i).col_as()) {
                iKeyFieldIndex = i;
            }
            join_field_iter = setJoinFields.find(m_oMemOperate.db_operate().fields(i).col_as());
            if (join_field_iter != setJoinFields.end()) {
                vecJoinFieldIndex.push_back(i);
                mapJoinFieldValues.insert(std::make_pair(i, std::string("")));
            }
        } else if (m_oMemOperate.db_operate().fields(i).has_col_name()) {
            if (m_strKeyField.size() > 0 && m_strKeyField == m_oMemOperate.db_operate().fields(i).col_name()) {
                iKeyFieldIndex = i;
            }
            join_field_iter = setJoinFields.find(m_oMemOperate.db_operate().fields(i).col_name());
            if (join_field_iter != setJoinFields.end()) {
                vecJoinFieldIndex.push_back(i);
                mapJoinFieldValues.insert(std::make_pair(i, std::string("")));
            }
        } else { // CmdDataProxy中有检查，所以不会走到这个分支
            LOG4_ERROR("col_name and col_as were empty in db operate!");
            return (false);
        }
    }
    if (m_strKeyField.size() > 0 && -1 == iKeyFieldIndex) {
        LOG4_ERROR("read join key field \"%s\" not found in db operate!", m_strKeyField.c_str());
        return (false);
    }
    std::string strKeyFieldValue;
    std::string strJoinFieldValue;
    for (int rec = 0; rec < oRsp.record_data_size(); ++rec) {
        strKeyFieldValue.clear();
        strJoinFieldValue.clear();
        for (join_field_value_iter = mapJoinFieldValues.begin();
                join_field_value_iter != mapJoinFieldValues.end(); ++join_field_value_iter) {
            join_field_value_iter->second = "";
        }
        for (int col = 0; col < oRsp.record_data(rec).field_info_size(); ++col) {
            if (iKeyFieldIndex == col) {
                if (!oRsp.record_data(rec).field_info(col).has_col_value() || oRsp.record_data(rec).field_info(col).col_value().size() == 0) {
                    LOG4_ERROR("error %d: %s", ERR_KEY_FIELD_VALUE, "the value of key field \"%s\" can not be empty, and record: %s!",
                               m_strKeyField.c_str(), oRsp.record_data(rec).DebugString().c_str());
                    strKeyFieldValue.clear();
                    break;  // 处理下一条记录
                }
                strKeyFieldValue = oRsp.record_data(rec).field_info(col).col_value();
            }
            join_field_value_iter = mapJoinFieldValues.find(col);
            if (join_field_value_iter != mapJoinFieldValues.end()) {
                if (!oRsp.record_data(rec).field_info(col).has_col_value() || oRsp.record_data(rec).field_info(col).col_value().size() == 0) {
                    LOG4_ERROR("error %d: %s", ERR_KEY_FIELD_VALUE, "the value of join field can not be empty, and record: %s!",
                               m_strKeyField.c_str(), oRsp.record_data(rec).DebugString().c_str());
                    strKeyFieldValue.clear();
                    break;  // 处理下一条记录
                }
                join_field_value_iter->second = oRsp.record_data(rec).field_info(col).col_value();
            }
        }
        if (mapJoinFieldValues.size() == 0) {
            continue;
        }
        for (join_field_index_iter = vecJoinFieldIndex.begin();
                join_field_index_iter != vecJoinFieldIndex.end(); ++join_field_index_iter) {
            join_field_value_iter = mapJoinFieldValues.find(*join_field_index_iter);
            if (join_field_value_iter != mapJoinFieldValues.end()) {
                if (strJoinFieldValue.size() == 0) {
                    strJoinFieldValue = join_field_value_iter->second;
                } else {
                    strJoinFieldValue += std::string(":") + join_field_value_iter->second;
                }
            }
        }
        if (strKeyFieldValue.size() > 0) {
            pRedisCmd->Append(strKeyFieldValue);
        }
        pRedisCmd->Append(strJoinFieldValue);
    }
    return (true);
}

bool StepWriteBackToRedis::MakeCmdWithDataSet(const DataMem::MemRsp& oRsp, loss::RedisCmd* pRedisCmd) {
    LOG4_TRACE("%s()", __FUNCTION__);
    if (0 == oRsp.record_data_size()) {
        return (false);
    }
    DataMem::Record oRecord;
    for (int i = 0; i < oRsp.record_data_size(); ++i) {
        oRecord.Clear();
        for (int j = 0; j < oRsp.record_data(i).field_info_size(); ++j) {
            DataMem::Field* pField = oRecord.add_field_info();
            if (oRsp.record_data(i).field_info(j).has_col_value()) {
                pField->set_col_value(oRsp.record_data(i).field_info(j).col_value());
            } else {
                pField->set_col_value("");
            }
        }
        pRedisCmd->Append(oRecord.SerializeAsString());
    }
    return (true);
}

bool StepWriteBackToRedis::MakeCmdWithoutDataSet(const DataMem::MemRsp& oRsp, loss::RedisCmd* pRedisCmd) {
    LOG4_TRACE("%s()", __FUNCTION__);
    if (0 == oRsp.record_data_size()) {
        return (false);
    }
    for (int i = 0; i < oRsp.record_data_size(); ++i) {
        for (int j = 0; j < oRsp.record_data(i).field_info_size(); ++j) {
            if (oRsp.record_data(i).field_info(j).has_col_value()) {
                pRedisCmd->Append(oRsp.record_data(i).field_info(j).col_value());
            } else {
                pRedisCmd->Append("");
            }
        }
    }
    return (true);
}

bool StepWriteBackToRedis::MakeCmdForHashWithDataSet(const DataMem::MemRsp& oRsp, loss::RedisCmd* pRedisCmd) {
    LOG4_TRACE("%s()", __FUNCTION__);
    if (0 == oRsp.record_data_size()) {
        return (false);
    }
    int iHashKeyFieldIndex = -1;
    for (int i = 0; i < m_oMemOperate.db_operate().fields_size(); ++i) {
        if (m_strKeyField.size() > 0 && m_oMemOperate.db_operate().fields(i).has_col_name()) {
            if (m_strKeyField == m_oMemOperate.db_operate().fields(i).col_name()) {
                iHashKeyFieldIndex = i;
                break;
            } else if (m_oMemOperate.db_operate().fields(i).has_col_as()) {
                if (m_strKeyField == m_oMemOperate.db_operate().fields(i).col_as()) {
                    iHashKeyFieldIndex = i;
                    break;
                }
            }
        }
    }
    if (-1 == iHashKeyFieldIndex) {
        LOG4_ERROR("read dataset key field \"%s\" not found in db operate!", m_strKeyField.c_str());
        return (false);
    }
    std::string strHashFieldName;
    DataMem::Record oRecord;
    for (int rec = 0; rec < oRsp.record_data_size(); ++rec) {
        strHashFieldName.clear();
        oRecord.Clear();
        for (int col = 0; col < oRsp.record_data(rec).field_info_size(); ++col) {
            DataMem::Field* pField = oRecord.add_field_info();
            if (oRsp.record_data(rec).field_info(col).has_col_value()) {
                pField->set_col_value(oRsp.record_data(rec).field_info(col).col_value());
                if (iHashKeyFieldIndex == col && oRsp.record_data(rec).field_info(col).col_value().length() > 0) {
                    strHashFieldName = oRsp.record_data(rec).field_info(col).col_value();
                }
            } else {
                pField->set_col_value("");
            }
        }
        if (0 == strHashFieldName.length()) {
            LOG4_ERROR("the hash file name can not be empty, but the db result set row[%d] \"%s\" is empty!",
                       rec, m_strKeyField.c_str());
            return (false);
        }
        pRedisCmd->Append(strHashFieldName);
        pRedisCmd->Append(oRecord.SerializeAsString());
    }
    return (true);
}

bool StepWriteBackToRedis::MakeCmdForHashWithoutDataSet(const DataMem::MemRsp& oRsp, loss::RedisCmd* pRedisCmd) {
    LOG4_TRACE("%s()", __FUNCTION__);
    if (0 == oRsp.record_data_size()) {
        return (false);
    }
    if (RELATIVE_TABLE == m_iRelative && 1 < oRsp.record_data_size()) {
        LOG4_ERROR("redis hash without dataset is not supported more than one record write back,"
                   "there are %d records from DB!", oRsp.record_data_size());
        return (false);
    }
    if ("HGETALL" == m_oMemOperate.redis_operate().redis_cmd_read()
            || "HAVLS" == m_oMemOperate.redis_operate().redis_cmd_read()) {
        return (MakeCmdForHashWithoutDataSetWithoutField(oRsp, pRedisCmd));
    } else {
        return (MakeCmdForHashWithoutDataSetWithField(oRsp, pRedisCmd));
    }
}

bool StepWriteBackToRedis::MakeCmdForHashWithoutDataSetWithField(const DataMem::MemRsp& oRsp, loss::RedisCmd* pRedisCmd) {
    if (m_oMemOperate.redis_operate().fields_size() != oRsp.record_data(0).field_info_size()) {
        LOG4_ERROR("redis hash(hmget or hget) field num %d not match the field num %d from DB result row!",
                   m_oMemOperate.redis_operate().fields_size(), oRsp.record_data(0).field_info_size());
        return (false);
    }
    if (m_oMemOperate.db_operate().fields_size() != oRsp.record_data(0).field_info_size()) {
        LOG4_ERROR("dbagent request field num %d not match the dbagent response field num %d!",
                   m_oMemOperate.db_operate().fields_size(), oRsp.record_data(0).field_info_size());
        return (false);
    }

    for (int i = 0; i < oRsp.record_data(0).field_info_size(); ++i) {
        if (oRsp.record_data(0).field_info(i).has_col_as()) {
            pRedisCmd->Append(oRsp.record_data(0).field_info(i).col_as());
        } else if (m_oMemOperate.db_operate().fields(i).has_col_as()) {
            pRedisCmd->Append(m_oMemOperate.db_operate().fields(i).col_as());
        } else if (oRsp.record_data(0).field_info(i).has_col_name()) {
            pRedisCmd->Append(oRsp.record_data(0).field_info(i).col_name());
        } else if (m_oMemOperate.db_operate().fields(i).has_col_name()) {
            pRedisCmd->Append(m_oMemOperate.db_operate().fields(i).col_name());
        } else {
            LOG4_ERROR("no field name found for row[%d]!", i);
            return (false);
        }

        if (oRsp.record_data(0).field_info(i).has_col_value()) {
            pRedisCmd->Append(oRsp.record_data(0).field_info(i).col_value());
        } else {
            pRedisCmd->Append("");
        }
    }
    return (true);
}

bool StepWriteBackToRedis::MakeCmdForHashWithoutDataSetWithoutField(const DataMem::MemRsp& oRsp, loss::RedisCmd* pRedisCmd) {
    if (m_oMemOperate.db_operate().fields_size() != oRsp.record_data(0).field_info_size()) {
        LOG4_ERROR("dbagent request field num %d not match the dbagent response field num %d!",
                   m_oMemOperate.db_operate().fields_size(), oRsp.record_data(0).field_info_size());
        return (false);
    }

    for (int i = 0; i < oRsp.record_data(0).field_info_size(); ++i) {
        if (oRsp.record_data(0).field_info(i).has_col_as()) {
            pRedisCmd->Append(oRsp.record_data(0).field_info(i).col_as());
        } else if (m_oMemOperate.db_operate().fields(i).has_col_as()) {
            pRedisCmd->Append(m_oMemOperate.db_operate().fields(i).col_as());
        } else if (oRsp.record_data(0).field_info(i).has_col_name()) {
            pRedisCmd->Append(oRsp.record_data(0).field_info(i).col_name());
        } else if (m_oMemOperate.db_operate().fields(i).has_col_name()) {
            pRedisCmd->Append(m_oMemOperate.db_operate().fields(i).col_name());
        } else {
            LOG4_ERROR("no field name found for row[%d]!", i);
            return (false);
        }

        if (oRsp.record_data(0).field_info(i).has_col_value()) {
            pRedisCmd->Append(oRsp.record_data(0).field_info(i).col_value());
        } else {
            pRedisCmd->Append("");
        }
    }
    return (true);
}

} /* namespace oss */
