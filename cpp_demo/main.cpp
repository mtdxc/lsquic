#include "quic_client_core.h"
#include "quic_server_core.h"
#include "public/log.h"

int main()
{
    quic_infos infos;

    /* 服务端用 */
    infos.local_port = 12345;
    infos.local_ip = "127.0.0.1";

    /* 客户端用 */
    infos.peer_port = 12345;
    infos.peer_ip = "127.0.0.1";

    infos.cert_file = "../ssh_key/cacert.pem";
    infos.key_file  = "../ssh_key/privkey.pem";
#if 1
    infos.type = LIVE_QUCI_TYPE_CLIENT;
    quic_client_core client_core(infos);
    client_core.init();
#else
    infos.type = LIVE_QUCI_TYPE_SERVER;
    quic_server_core server_core(infos);
    server_core.init();
#endif
    getchar();
    return 0;
}