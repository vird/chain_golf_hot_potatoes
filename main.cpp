#define C const
#define S string
#define AS AbstractServer
#define ASC AbstractServerConnector
#define bM bindAndAddMethod
#define sVt serverVersion_t
#define JS JSON_STRING
#define V virtual
#define UN using namespace
#include<cstdio>
#include<filesystem>
#include<iostream>
#include<thread>
#include<jsonrpccpp/server.h>
#include<jsonrpccpp/server/connectors/httpserver.h>
UN std;UN jsonrpc;UN Json;class TS:public AS<TS>{public:TS(ASC &conn,sVt type=JSONRPC_SERVER_V2):AS<TS>(conn,type){bM(Procedure("balance",PARAMS_BY_NAME,JSON_INTEGER,"address",JS,0),&TS::balanceI);bM(Procedure("transaction",PARAMS_BY_NAME,JS,"amount",JSON_INTEGER,"from_address",JS,"to_address",JS,0),&TS::transactionI);}V void balanceI(C Value &rq,Value &rs){rs=B(rq["address"].asString());}V void transactionI(C Value &rq,Value &rs){rs=tx(rq["amount"].asInt(),rq["from_address"].asString(),rq["to_address"].asString());}V int B(C S &A)=0;V S tx(int amount,C S &fA,C S &tA)=0;};class MS:public TS{public:MS(ASC &c,sVt type);int B(C S &A)override;S tx(int amount,C S &fA,C S &tA)override;};MS::MS(ASC &c,sVt type):TS(c,type){}int MS::B(C S &A){cout<<"SERVER | Received in balance: address["<<A<<"]"<<endl;return 1;}S MS::tx(int amount,C S &fA,C S &tA){cout<<"SERVER | Received in transaction: from_address["<<fA<<"], to_address["<<tA<<"], amount["<<amount<<"]"<<endl;return "transaction block hash";}int main(){HttpServer hs(10001);MS s(hs,JSONRPC_SERVER_V1V2);s.StartListening();cout<<"welcome to UTON HACK!"<<endl;this_thread::sleep_for(chrono::seconds(45));s.StopListening();return 0;}