#!/bin/bash
./start.sh CenterServer
./start.sh AccessServer
./start.sh LogicServer
./start.sh DataProxyServer
./start.sh DbAgentRead
./start.sh DbAgentWrite
./start.sh InterfaceServer
