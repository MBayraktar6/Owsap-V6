#ifndef __INC_BASE64_USING_OPENSSL_H__
#define __INC_BASE64_USING_OPENSSL_H__

bool base64_ssl(const unsigned char* input, int length, unsigned char* output, int outlen);
bool unbase64_ssl(unsigned char* input, int length, unsigned char* output, int outlen);

#endif // __INC_BASE64_USING_OPENSSL_H__
