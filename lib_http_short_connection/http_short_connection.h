#ifndef _HTTP_SHORT_CONNECTION_H_
#define _HTTP_SHORT_CONNECTION_H_

#include "http_req_common.h"

#define HTTP_SHORT_CONNECTION_DEBUG_ENABLE                  (0)     //0:disable 1:enable

#define HTTP_SHORT_CONNECTION_RETRY_MAX_TIMES               (2)

#define HTTP_SHORT_CONNECTION_SEND_TIME_OUT                 (5000)
#define HTTP_SHORT_CONNECTION_RECV_TIME_OUT                 (6000)

#define HTTPS_SHORT_CONNECTION_SEND_TIME_OUT                (5000)
#define HTTPS_SHORT_CONNECTION_RECV_TIME_OUT                (6000)

#ifndef HTTP_SHORT_CONNECTION_HEAD_MAX_LEN
#define HTTP_SHORT_CONNECTION_HEAD_MAX_LEN                  (1*1024)
#endif

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
merr_t http_short_connection_nossl(HTTP_REQ_S *http_nossl_req_p);

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
merr_t http_short_connection_ssl(HTTP_REQ_S *http_req_p);

#endif
