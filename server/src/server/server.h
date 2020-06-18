#pragma once
#include "../client/client.h"
#include "../util/events.h"
#include "ssl.h"

namespace tcp {
    constexpr uint8_t server_version = 0;

    enum select_status : int { error = 0, standby, ready };

    class server {
        int              m_socket;
        std::string_view m_port;

        fd_set   m_server_set;
        SSL_CTX* m_ctx;

        std::atomic<bool> m_status = false;

    public:
        std::vector<tcp::client> client_stack;


        event<client&>            connect_event;
        event<packet_t&, client&> receive_event;
        event<client&>            disconnect_event;

        server(const std::string_view port) : m_port{ port } {}
        ~server() = default;

        bool          start();
        select_status peek();
        void          accept_client();
        void          receive();
        void          stop();

        bool running() { return m_status; }

        static void monitor(server& srv)
        {
            while(srv.running()) {
                auto ret = srv.peek();
                if(ret == select_status::ready) {
                    srv.accept_client();
                    srv.receive();
                }
                else if(ret == select_status::standby) {
                    // check for timeout
                }
                else {
                    break;
                }
            }
        }
    };

}; // namespace tcp
