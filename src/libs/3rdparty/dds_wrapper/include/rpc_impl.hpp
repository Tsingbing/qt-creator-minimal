#pragma once
#include <rpcdds/strategies/ThreadPoolStrategy.h>
#include <rpcdds/transports/dds/RTPSServerTransport.h>
#include <rpcdds/transports/dds/RTPSProxyTransport.h>
#include <rpcdds/exceptions/Exceptions.h>
#include <rpcdds/utils/Utilities.h>

#include <memory>

template <typename ProtocolType, typename ServerType, typename ImplType>
class RpcServerImpl {

public:
    RpcServerImpl(void){};
    ~RpcServerImpl(void){};

    auto CreateRpcServer(std::string service_name, ImplType &servant, std::string sub_service_name, unsigned int pool_size)
        -> std::shared_ptr<ServerType> {
        pool.reset(new ThreadPoolStrategy(pool_size));
        protocol.reset(new ProtocolType());
        transport.reset(new RTPSServerTransport(service_name.c_str(), sub_service_name.c_str()));
        std::shared_ptr<ServerType> server(new ServerType(*pool, *transport, *protocol, servant));
        return server;
    }

private:
    std::unique_ptr<ThreadPoolStrategy> pool;
    std::unique_ptr<ProtocolType> protocol;
    std::unique_ptr<RTPSServerTransport> transport;
};

template <typename ProtocolType, typename ProxyType>
class RpcClientImpl {

public:
    RpcClientImpl(void){};
    ~RpcClientImpl(void){};

    auto CreateRpcClient(std::string service_name, std::string sub_service_name)
        -> std::shared_ptr<ProxyType> {
        protocol.reset(new ProtocolType());
        transport.reset(new RTPSProxyTransport(service_name.c_str(), sub_service_name.c_str()));
        std::shared_ptr<ProxyType> client(new ProxyType(*transport, *protocol));
        return client;
    }

private:
    std::unique_ptr<ProtocolType> protocol;
    std::unique_ptr<RTPSProxyTransport> transport;
};
