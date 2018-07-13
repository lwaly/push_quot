#!/bin/bash

../../../../tools/bin/protoc -I=.. --cpp_out=. ../protocol.proto ../common.proto ../enumeration.proto ../user_base_info.proto ../msg.proto ../data_transfer.proto
