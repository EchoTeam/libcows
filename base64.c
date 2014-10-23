/*-
 * Copyright (c) 1999, 2000, 2001, 2002, 2003 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: base64.c,v 1.9 2006/12/11 07:11:52 vlm Exp $
 */

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <stdio.h>	/* for NULL */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define	B64_UNUSED	100

static unsigned char
_sf_uc_bi[255] = {
	255, /* '\0' */
100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 
	62, /* + */
	100, 100, 100, 
	63, /* / */
	/* 0-9 */
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 
	100, 100, 100, 
	255, /* = */
	100, 100, 100, 
	/* A-Z */
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 
	100, 100, 100, 100, 100, 100, 
	/* a-z */
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
};

static char *_sf_b64_buf = NULL;
static size_t _sf_b64_len = 0;
static unsigned char _sf_uc_ib[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/==";


/*
 * Get the previous result and detach it from the internal static storage.
 */
void *
base64_detach(size_t *size) {
	void *buf;

	buf = _sf_b64_buf;

	_sf_b64_buf = NULL;
	if(size)
		*size = _sf_b64_len;

	return (void *)buf;
}

/*
 * Encode the given data using Base64 encoding.
 */
char *
base64_encode(const void *data, size_t *size) {
	char *output;
	char *ou;
	const unsigned char *p=(const unsigned char *)data;
	int original_size = size?*size:strlen((const char *)data);
	const void *dte= (const char *)data + original_size;
	int malloc_len;
	int nc = 0;

	if(data == NULL) {
		errno = EINVAL;
		return NULL;
	}

	malloc_len = + (4 * (original_size + 2) / 3) + 1;
	malloc_len += malloc_len / 76;	/* For \n's */
	ou=output=(char *)malloc(malloc_len);
	if(output == NULL) {
		/* ENOMEM? */
		return NULL;
	}

	while((char *)dte - (char *)p >= 3) {
		*ou = _sf_uc_ib[ *p >> 2 ];
		ou[1] = _sf_uc_ib[ ((*p & 0x03) << 4) | (p[1] >> 4) ];
		ou[2] = _sf_uc_ib[ ((p[1] & 0x0F) << 2) | (p[2] >> 6) ];
		ou[3] = _sf_uc_ib[ p[2] & 0x3F ];

		p+=3;
		ou+=4;

		nc+=4;
		if((nc % 76) == 0) *ou++='\n';
	}
	if((char *)dte - (char *)p == 2) {
		*ou++ = _sf_uc_ib[ *p >> 2 ];
		*ou++ = _sf_uc_ib[ ((*p & 0x03) << 4) | (p[1] >> 4) ];
		*ou++ = _sf_uc_ib[ ((p[1] & 0x0F) << 2) ];
		*ou++ = '=';
	} else if((char *)dte - (char *)p == 1) {
		*ou++ = _sf_uc_ib[ *p >> 2 ];
		*ou++ = _sf_uc_ib[ ((*p & 0x03) << 4) ];
		*ou++ = '=';
		*ou++ = '=';
	}

	*ou = '\0';

	_sf_b64_len = (ou - output);
	assert(_sf_b64_len < malloc_len);
	if(size) *size = _sf_b64_len;

	if(_sf_b64_buf)
		free(_sf_b64_buf);
	return (_sf_b64_buf = output);
}

/*
 * Decode the given string using the base64 decoder.
 * If size is not specified, strlen(str) will be used.
 * The resulting data size will be placed into *size.
 */
void *
base64_decode(const char *str, size_t *size) {
	void *output, *ou;

	unsigned int pr[6];
	int n;
	int doit;

	if(str == NULL)
		str = "";

	ou = output = (void *)malloc((size?*size:strlen(str)) + 1);
	if(output == NULL)
		/* ENOMEM? */
		return NULL;

	if(size)
		*size = 0;

	doit = 1;

	do {
		n = 0;
		while(n < 4) {
			unsigned char ch;
			ch = _sf_uc_bi[*(unsigned char *)str];
			if(ch < B64_UNUSED) {
				pr[n] = ch;
				n++;
				str++;
			} else if(ch == B64_UNUSED) {
				str++;
			} else {
				doit = 0;
				break;
			}
		}

		if(doit == 0 && n < 4) {
			pr[n+2] = pr[n+1] = pr[n]='\0';
		}

		((char *)ou)[0] = (pr[0] << 2) | (pr[1] >> 4);
		((char *)ou)[1] = (pr[1] << 4) | (pr[2] >> 2);
		((char *)ou)[2] = (pr[2] << 6) | (pr[3] >> 0);

		ou = (char *)ou + ((n * 3) >> 2);

	} while(doit);

	*(char *)ou = '\0';	/* NUL-terminate */

	_sf_b64_len = ((char *)ou - (char *)output);

	if(size)
		*size = _sf_b64_len;

	/*
	 * Replace the old buffer.
	 */
	if(_sf_b64_buf)
		free(_sf_b64_buf);
	return (_sf_b64_buf=output);
}

