#define throw(...)
#include <jsonrpccpp/client.h>
#undef throw
#define ERR Errors::ERROR_CLIENT_INVALID_RESPONSE

class RPC_client : public Client
{
    public:
        RPC_client(IClientConnector &conn, clientVersion_t type = JSONRPC_CLIENT_V2) : Client(conn, type) {}

        Value get_node_list()
        {
            Value p;
            p = nullValue;
            Value result = this->CallMethod("get_node_list",p);
            if (result.isArray())
                return result;
            else
                throw JsonRpcException(ERR, result.toStyledString());
        }
        string handshake(const string& rev_ip_port)
        {
            Value p;
            p["rev_ip_port"] = rev_ip_port;
            Value result = this->CallMethod("handshake",p);
            if (result.isString())
                return result.asString();
            else
                throw JsonRpcException(ERR, result.toStyledString());
        }
        int bc_height()
        {
            Value p;
            p = nullValue;
            Value result = this->CallMethod("bc_height",p);
            if (result.isIntegral())
                return result.asInt();
            else
                throw JsonRpcException(ERR, result.toStyledString());
        }
        Value get_block_number(int id)
        {
            Value p;
            p["id"] = id;
            Value result = this->CallMethod("get_block_number",p);
            if (result.isObject())
                return result;
            else
                throw JsonRpcException(ERR, result.toStyledString());
        }
        Value get_block_by_hash(const string& hash)
        {
            Value p;
            p["hash"] = hash;
            Value result = this->CallMethod("get_block_by_hash",p);
            if (result.isObject())
                return result;
            else
                throw JsonRpcException(ERR, result.toStyledString());
        }
        Value get_block_range(int from, int to)
        {
            Value p;
            p["from"] = from;
            p["to"] = to;
            Value result = this->CallMethod("get_block_range",p);
            if (result.isArray())
                return result;
            else
                throw JsonRpcException(ERR, result.toStyledString());
        }
        string block_push(const Value& header)
        {
            Value p;
            p["header"] = header;
            Value result = this->CallMethod("block_push",p);
            if (result.isString())
                return result.asString();
            else
                throw JsonRpcException(ERR, result.toStyledString());
        }
        string tx_push(int amount, const string& bind_pub_key, const string& hash, int nonce, const string& recv_addr, const string& send_addr, const string& sign, int tx_epoch, int type)
        {
            Value p;
            p["amount"] = amount;
            p["bind_pub_key"] = bind_pub_key;
            p["hash"] = hash;
            p["nonce"] = nonce;
            p["recv_addr"] = recv_addr;
            p["send_addr"] = send_addr;
            p["sign"] = sign;
            p["tx_epoch"] = tx_epoch;
            p["type"] = type;
            Value result = this->CallMethod("tx_push",p);
            if (result.isString())
                return result.asString();
            else
                throw JsonRpcException(ERR, result.toStyledString());
        }
};
