#pragma once
#include "rpc_impl.hpp"

class Rpc {

public:
    Rpc(void){};
    ~Rpc(void){};

    template <typename ProtocolType, typename ServerType, typename ImplType>
    static auto CreateRpcServer(std::string service_name, ImplType &servant, std::string sub_service_name = "default", unsigned int pool_size = 1)
        -> std::shared_ptr<ServerType>{
        auto m_rpc_impl(new RpcServerImpl<ProtocolType, ServerType, ImplType>());
        return m_rpc_impl->CreateRpcServer(service_name, servant, sub_service_name, pool_size);
    }

    template <typename ProtocolType, typename ProxyType>
    static auto CreateRpcClient(std::string service_name, std::string sub_service_name = "default")
        -> std::shared_ptr<ProxyType>{
        auto m_rpc_impl(new RpcClientImpl<ProtocolType, ProxyType>());
        return m_rpc_impl->CreateRpcClient(service_name, sub_service_name);
    }
};
