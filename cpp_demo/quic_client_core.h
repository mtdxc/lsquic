////////////////////////////////////////////////////////////////////////
/// @file       quic_client_core.h
/// @brief      quic客户端通信核心声明
/// @details    quic客户端通信核心声明
/// @author     王超
/// @version    1.0
/// @date       2021/11/10
/// @copyright  (c) 2021-2031 。保留所有权利
////////////////////////////////////////////////////////////////////////
#pragma once
#include "quic_com.h"
#include <string>
#include <thread>
#include <memory>
#include <fcntl.h>


class quic_client_core
{
public:
    quic_client_core(const quic_infos &infos);

    ~quic_client_core();

    bool init();

private:

    int create_sock(char *ip, unsigned int port, struct sockaddr_storage *local_sas);
    struct sockaddr_in new_addr(char *ip, unsigned int port);

    /* quic客户端的回调函数 */
    static lsquic_conn_ctx_t *client_on_new_conn_cb(void *ea_stream_if_ctx, lsquic_conn_t *conn);
    static void client_on_conn_closed_cb(lsquic_conn_t *conn);
    static void client_on_hsk_done(lsquic_conn_t *conn, enum lsquic_hsk_status status);

    
    static lsquic_stream_ctx_t *client_on_new_stream_cb(void *ea_stream_if_ctx, lsquic_stream_t *stream);
    static void client_on_read_cb(lsquic_stream_t *stream, lsquic_stream_ctx_t *h);
    static void client_on_write_cb(lsquic_stream_t *stream, lsquic_stream_ctx_t *h);
    static void client_on_close_cb (lsquic_stream_t *stream, lsquic_stream_ctx_t *st_h);   /* stream close 是调用的回调函数 */
    /* quic客户端的回调函数结束 */

    void create_event_loop();

    int init_ssl_ctx ();

    static SSL_CTX * get_ssl_ctx (void * ctx, const struct sockaddr *);
    static struct ssl_ctx_st *no_cert (void *cert_lu_ctx, const struct sockaddr *sa_UNUSED, const char *sni);
    
    /* 客户端读取网络数据 */
    static void client_read_net_data(EV_P_ ev_io *w, int revents);

    /* 客户端读取本地数据 */
    static void client_read_local_data (EV_P_ ev_io *w, int revents);

    void client_process_conns(BasicState *state);
    static void client_process_conns_cb(EV_P_ ev_timer *conn_watcher, int revents);

    static int send_packets_out(void *ctx, const struct lsquic_out_spec *specs, unsigned n_specs);

    struct lsquic_engine_api m_engine_api;
    BasicState *m_State;
    struct lsquic_packout_mem_if m_pmi;

    struct lsquic_stream_if m_stream_if;

    LIVE_QUCI_TYPE m_type;/* client or server  */

    quic_infos m_quci_infos;

    std::shared_ptr<std::thread> m_event_thread = nullptr;   /* libev loop线程 */
    volatile bool m_event_thread_running; /* m_event_thread是否运行标志，true--运行 */

};
