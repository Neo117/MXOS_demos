#include "mxos.h"
#include "http_short_connection.h"
#include "HTTPUtils.h"
#include "SocketUtils.h"

#if (HTTP_SHORT_CONNECTION_DEBUG_ENABLE == 1)
#define app_log(M, ...)             custom_log("HTTP_SHORT_CONNECTION", M, ##__VA_ARGS__)
#elif (HTTP_SHORT_CONNECTION_DEBUG_ENABLE == 0)
#define app_log(M, ...)            {;}
#else
#error "HTTP_SHORT_CONNECTION_DEBUG_ENABLE not define!"
#endif

static merr_t onReceivedData( struct _HTTPHeader_t * inHeader, uint32_t inPos, uint8_t * inData, size_t inLen, void * inUserContext );
static void http_short_connection_thread( void *arg );

extern int lwip_gethostbyname_r(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop);

//get wifi status
static bool http_get_wifi_status(void)
{
    mwifi_link_info_t link_status;

    memset(&link_status, 0, sizeof(link_status));

    mwifi_get_link_info(&link_status);

    return (bool)(link_status.is_connected);
}



merr_t http_short_connection(HTTP_REQ_S *http_req_p)
{
    merr_t err = kGeneralErr;
    mos_thread_id_t thread_id = NULL;

    require_string(http_req_p != NULL, exit, "param error!");

    if(http_get_wifi_status() == false)
    {
        err = kGeneralErr;
        app_log("[ERROR]wifi is not connect!");
        goto exit;
    }

    if ( http_req_p->is_ssl == false )
    {
        thread_id = mos_thread_new( MOS_APPLICATION_PRIORITY, "http_thread", http_short_connection_thread, 0x800, http_req_p );
    } else
    {
        thread_id = mos_thread_new( MOS_APPLICATION_PRIORITY, "https_thread", http_short_connection_thread, 0x2800, http_req_p );
    }
    require_string( thread_id != NULL, exit, "ERROR: Unable to start http_thread thread" );

    mos_thread_join( thread_id );

    err = kNoErr;
 exit:
     if(err != kNoErr)
     {
         app_log("http short connection failed");
     }

    return err;
}

/**
 * @brief  http request nossl, http redirection is not supported
 * @param  http_nossl_req_p[in/out]: http short connection conetxt
 * @param  http_nossl_req_p.is_success[out]: http request is success
 * @param  http_nossl_req_p.domain_name[in]: domain name
 * @param  http_nossl_req_p.port[in]: http request port
 * @param  http_nossl_req_p.timeout_ms[in]: select timeout after http request is send
 * @param  http_nossl_req_p.http_req[in]: the http request string, users need to combine data themselves
 * @param  http_nossl_req_p.req_len[in]: the length of http request string
 * @param  http_nossl_req_p.http_res[in/out]: server response buff, memset the buff before you call this func
 * @param  http_nossl_req_p.res_len[in]: server response buff len
 * @param  http_nossl_req_p.res_len[in]: server response real string len
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned
 */
merr_t http_short_connection_nossl(HTTP_REQ_S *http_req_p)
{
    http_req_p->is_ssl = false;
    return http_short_connection(http_req_p);
}

/**
 * @brief  http request ssl, http redirection is not supported
 * @param  http_nossl_req_p[in/out]: http short connection conetxt
 * @param  http_nossl_req_p.is_success[out]: http request is success
 * @param  http_nossl_req_p.domain_name[in]: domain name
 * @param  http_nossl_req_p.port[in]: http request port
 * @param  http_nossl_req_p.timeout_ms[in]: select timeout after http request is send
 * @param  http_nossl_req_p.http_req[in]: the http request string, users need to combine data themselves
 * @param  http_nossl_req_p.req_len[in]: the length of http request string
 * @param  http_nossl_req_p.http_res[in/out]: server response buff, memset the buff before you call this func
 * @param  http_nossl_req_p.res_len[in]: server response buff len
 * @param  http_nossl_req_p.res_len[in]: server response real string len
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned
 */
merr_t http_short_connection_ssl(HTTP_REQ_S *http_req_p)
{
    http_req_p->is_ssl = true;
    return http_short_connection(http_req_p);
}

//set tcp keep_alive param
static int user_set_tcp_option(int socket, int send_timeout, int recv_timeout )
{
    int retVal = 0;
    struct timeval t;

    t.tv_sec = send_timeout / 1000;
    t.tv_usec = (send_timeout % 1000)*1000;

    retVal = setsockopt( socket, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *) &t, sizeof(t) );
    require_string( retVal >= 0, exit, "setsockopt error!" );

    t.tv_sec = recv_timeout / 1000;
    t.tv_usec = (recv_timeout % 1000)*1000;
    retVal = setsockopt( socket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &t, sizeof(t) );
    require_string( retVal >= 0, exit, "setsockopt error!" );

    exit:
    return retVal;
}

static merr_t usergethostbyname( const char * domain, uint8_t * addr, uint8_t addrLen )
{
    merr_t err = kGeneralErr;
    uint8_t retry = 0;
    struct hostent hostinfo, *phost = NULL;
    char buf[256] = {0};
    int ret = 0, api_ret = 0;

    require_string( domain != NULL && addr != NULL && addrLen >= 16, exit, "param error" );

    start:
    api_ret = lwip_gethostbyname_r( domain, &hostinfo, buf, sizeof(buf), &phost, &ret );
    require( api_ret == 0 && phost != NULL, exit_with_retry );

    memset( addr, 0, addrLen );
    strncpy( (char *) addr, inet_ntoa( *(struct in_addr*) hostinfo.h_addr_list[0] ), addrLen );

    app_log( "host addr is:%s", addr );

    err = kNoErr;

    exit_with_retry:
    retry++;
    if ( retry < HTTP_SHORT_CONNECTION_RETRY_MAX_TIMES && err != kNoErr )
    {
        app_log("dns error, retry = %d", retry);
        goto start;
    }

    exit:
    return err;
}

static void http_short_connection_thread( void *arg )
{
    merr_t err = kGeneralErr;
    int http_fd = -1;
    int ret = 0;
    fd_set readfds;
    char ipstr[20] = { 0 };
    struct sockaddr_in addr;
    HTTPHeader_t *httpHeader = NULL;
    struct timeval t;
    HTTP_REQ_S *http_req_p = (HTTP_REQ_S *)(arg);
    mtls_t client_ssl = NULL;
    int ssl_errno = 0;
    bool is_ssl;
    bool is_sni = false;

    require_string(http_req_p != NULL, exit, "param error!");

    if ((http_req_p->domain_name == NULL) ||
        (http_req_p->http_req == NULL) ||
        (http_req_p->http_res == NULL) ||
        (http_req_p->req_len == 0) ||
        (http_req_p->res_len == 0)) {
        app_log("param error!");
        goto exit;
    }
    is_ssl = http_req_p->is_ssl;
    is_sni = http_req_p->is_sni;
    http_req_p->is_success = false;

    t.tv_sec = 0;
    t.tv_usec = (http_req_p->timeout_ms) * 1000;

    err = usergethostbyname( http_req_p->domain_name, (uint8_t *) ipstr, sizeof(ipstr) );
    if ( err != kNoErr )
    {
        app_log("dns error!!! doamin:%s", http_req_p->domain_name);
        goto exit;
    }

    app_log("HTTP server address: host:%s, ip: %s", http_req_p->domain_name, ipstr);

    http_req_p->real_res_len = 0;

    /*HTTPHeaderCreateWithCallback set some callback functions */
    httpHeader = HTTPHeaderCreateWithCallback( HTTP_SHORT_CONNECTION_HEAD_MAX_LEN, onReceivedData, NULL, http_req_p );
    if ( httpHeader == NULL )
    {
        app_log("HTTPHeaderCreateWithCallback() error");
        goto exit;
    }

    SocketClose( &http_fd );
    http_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr( ipstr );
    addr.sin_port = htons( http_req_p->port );

    ret = user_set_tcp_option( http_fd, HTTPS_SHORT_CONNECTION_SEND_TIME_OUT, HTTPS_SHORT_CONNECTION_RECV_TIME_OUT );
    if ( ret < 0 )
    {
        app_log("user_set_tcp_keepalive() error");
        goto exit;
    }

    app_log("start connect");

    err = connect( http_fd, (struct sockaddr *)&addr, sizeof(addr) );
    require_noerr_action( err, exit, app_log("connect http server failed"));

    app_log("start ssl connect");

    if ( is_ssl )
    {
        if ( is_sni )
        {
            client_ssl = mtls_connect_sni( http_fd, 0, NULL, http_req_p->domain_name, &ssl_errno );
        } else
        {
            client_ssl = mtls_connect( http_fd, 0, NULL, &ssl_errno );
        }
        require_action( client_ssl != NULL, exit, {err = kGeneralErr; app_log("ssl_connnect error, errno = %d", ssl_errno);} );

        mtls_send( client_ssl, http_req_p->http_req, http_req_p->req_len );
    } else
    {
        send( http_fd, http_req_p->http_req, http_req_p->req_len, 0 );
    }
    FD_ZERO( &readfds );
    FD_SET( http_fd, &readfds );

    ret = select( http_fd + 1, &readfds, NULL, NULL, &t );
    if ( ret == -1 || ret == 0 )
    {
        app_log("-----------------select error, ret = %d", ret);
        err = kGeneralErr;
        goto exit;
    }

    if ( FD_ISSET( http_fd, &readfds ) )
    {
        /*parse header*/
        if (is_ssl)
            err = SocketReadHTTPSHeader( client_ssl, httpHeader );
        else
            err = SocketReadHTTPHeader( http_fd, httpHeader );
        switch ( err )
        {
            case kNoErr:
                http_req_p->status_code = httpHeader->statusCode;

                if (is_ssl)
                    err = SocketReadHTTPSBody( client_ssl, httpHeader ); /*get body data*/
                else
                    err = SocketReadHTTPBody( http_fd, httpHeader );
                require_noerr( err, exit );

                app_log( "Content Data:[%ld]%s", http_req_p->real_res_len, http_req_p->http_res ); /*get data and print*/
                http_req_p->is_success = true;


                break;

            case EWOULDBLOCK:
            {
                break;
            }
            case kNoSpaceErr:
            {
                app_log("kNoSpaceErr");
                break;
            }
            case kConnectionErr:
            {
                app_log("kConnectionErr");
                break;
            }
            default:
            {
                app_log("ERROR: HTTP Header parse error: %d", err);
                break;
            }
        }
    }

    exit:
    if ( client_ssl != NULL)
    {
        mtls_close( client_ssl );
        client_ssl = NULL;
    }
    SocketClose( &http_fd );

    HTTPHeaderDestory( &httpHeader );

    //app_log( "Exit: Client exit with err = %d, fd: %d", err, http_fd );

    mos_thread_delete( NULL );
    return;
}

/*one request may receive multi reply*/
static merr_t onReceivedData( struct _HTTPHeader_t * inHeader, uint32_t inPos, uint8_t * inData, size_t inLen, void * inUserContext )
{
    merr_t err = kGeneralErr;
    HTTP_REQ_S *http_req_p = (HTTP_REQ_S *)inUserContext;

    if(inLen == 0)
    {
        app_log("in_len = 0");
        return kNoErr;
    }

    if( inHeader->chunkedData == false)
    {
        //Extra data with a content length value
        require_action(inHeader->contentLength < http_req_p->res_len, exit, err = kGeneralErr);

        memcpy( http_req_p->http_res + inPos, inData, inLen );

        http_req_p->real_res_len += inLen;
    }else
    {
        //extra data use a chunked data protocol
        app_log("This is a chunked data, %d", inLen);

        require_action( (http_req_p->real_res_len + inLen) < http_req_p->res_len, exit, err = kGeneralErr );
        http_req_p->real_res_len += inLen;

        memcpy( http_req_p->http_res + inPos, inData, inLen );
    }
    err = kNoErr;

 exit:
    return err;
}

