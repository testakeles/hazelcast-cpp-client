/*
 * Copyright (c) 2008-2020, Hazelcast, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef HAZELCAST_CLIENT_PROXY_PARTITIONSPECIFICPROXY_H_
#define HAZELCAST_CLIENT_PROXY_PARTITIONSPECIFICPROXY_H_

#include "hazelcast/client/proxy/ProxyImpl.h"
#include "hazelcast/util/ExceptionUtil.h"

namespace hazelcast {
    namespace client {
        namespace proxy {
            /**
             * Base class for proxies of distributed objects that lives in on partition.
             */
            class HAZELCAST_API PartitionSpecificClientProxy : public proxy::ProxyImpl {
            protected:
                PartitionSpecificClientProxy(const std::string &serviceName, const std::string &objectName,
                                             spi::ClientContext *context);

                virtual void onInitialize();

                template<typename V>
                boost::future<std::shared_ptr<V>>
                invokeOnPartitionAsync(std::unique_ptr<protocol::ClientMessage> &request,
                                       const std::shared_ptr<impl::ClientMessageDecoder<V> > &clientMessageDecoder) {
                    try {
                        auto future = invokeAndGetFuture(request, partitionId);
                        return future.then(boost::launch::sync, [=](boost::future<protocol::ClientMessage> f) {
                            return clientMessageDecoder->decodeClientMessage(f.get(), getSerializationService());
                        });
                    } catch (exception::IException &e) {
                        util::ExceptionUtil::rethrow(std::current_exception());
                    }
                    return boost::future<std::shared_ptr<V>>();
                }

                int partitionId;
            };
        }
    }
}

#endif //HAZELCAST_CLIENT_PROXY_PARTITIONSPECIFICPROXY_H_
