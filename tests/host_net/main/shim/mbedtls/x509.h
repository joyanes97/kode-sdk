/* SPDX-License-Identifier: GPL-3.0-or-later */
/**
 * @file x509.h
 * @brief The one mbedTLS error kdrv_net.c reads, with mbedTLS's value.
 *
 * This binary has no mbedTLS: the HTTP client is a mock, and no certificate
 * is ever verified here. The value is the one in mbedTLS's own x509.h.
 */
#ifndef KODE_HOST_SHIM_MBEDTLS_X509_H_
#define KODE_HOST_SHIM_MBEDTLS_X509_H_

#define MBEDTLS_ERR_X509_CERT_VERIFY_FAILED  -0x2700  /**< The peer's certificate was refused. */

#endif /* KODE_HOST_SHIM_MBEDTLS_X509_H_ */
