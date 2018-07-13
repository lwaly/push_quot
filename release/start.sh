#!/bin/bash

function usage()
{
   echo "$0 \${server_bin}"
}

if [ $# -ne 1 ]
then
    usage
    exit 1
fi

ASYNC_SERVER_PATH=`dirname $0`
cd ${ASYNC_SERVER_PATH}
ASYNC_SERVER_PATH=`pwd`
ASYNC_BIN=${ASYNC_SERVER_PATH}/bin
ASYNC_CONF=${ASYNC_SERVER_PATH}/conf
ASYNC_LIB=${ASYNC_SERVER_PATH}/lib
ASYNC_LOG=${ASYNC_SERVER_PATH}/log
ASYNC_TEMP=${ASYNC_SERVER_PATH}/temp
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ASYNC_LIB}



${ASYNC_BIN}/$1 ${ASYNC_CONF}/$1.json

