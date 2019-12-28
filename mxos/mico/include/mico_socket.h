/**
 ******************************************************************************
 * @file    mico_socket.h
 * @author  William Xu
 * @version V1.0.0
 * @date    16-Sep-2014
 * @brief   This file provides all the headers of BSD socket APIs.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */
#ifndef __MICOSOCKET_H__
#define __MICOSOCKET_H__

#include "mxos_socket.h"
#include "mtls.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef mtls_t mico_ssl_t;

typedef mtls_ver_t ssl_version_type_t;

#define ssl_set_client_version mtls_set_ver
#define ssl_socket mtls_socket
#define ssl_set_cert mtls_set_cert
#define ssl_set_client_cert mtls_set_client_cert
#define ssl_connect mtls_connect
//#define ssl_connect_with_domain mtls_connect_with_domain
#define ssl_connect_sni mtls_connect_sni
#define ssl_accept mtls_accept
#define ssl_send mtls_send
#define ssl_recv mtls_recv
#define ssl_close mtls_close
#define ssl_pending mtls_pending
#define ssl_get_error mtls_get_error
//#define ssl_set_using_nonblock mtls_set_using_nonblock
//#define ssl_x509_get_text_by_nid mtls_x509_get_text_by_nid

#ifdef __cplusplus
}
#endif

#endif /*__MICO_SOCKET_H__*/



