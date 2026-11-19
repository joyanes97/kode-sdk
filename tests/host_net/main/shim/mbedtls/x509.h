/* SPDX-License-Identifier: GPL-3.0-or-later */
/**
 * @file x509.h
 * @brief The two verification flags kdrv_net.c reads, with mbedTLS's values.
 *
 * This binary has no mbedTLS: the HTTP client is a mock, and no certificate
 * is ever verified here. The values are the ones in mbedTLS's own x509.h.
 */
#ifndef KODE_HOST_SHIM_MBEDTLS_X509_H_
#define KODE_HOST_SHIM_MBEDTLS_X509_H_

#define MBEDTLS_X509_BADCERT_EXPIRED  0x01    /**< The certificate's validity has ended. */
#define MBEDTLS_X509_BADCERT_FUTURE   0x0200  /**< The certificate's validity has not begun. */

#endif /* KODE_HOST_SHIM_MBEDTLS_X509_H_ */
