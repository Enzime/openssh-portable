/* 	$OpenBSD: test_parse.c,v 1.3 2025/06/12 10:09:39 dtucker Exp $ */
/*
 * Regress test for misc user/host/URI parsing functions.
 *
 * Placed in the public domain.
 */

#include "includes.h"

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../test_helper/test_helper.h"

#include "log.h"
#include "misc.h"

void test_parse(void);

void
test_parse(void)
{
	int port;
	char *user, *host, *path;

	TEST_START("misc_parse_user_host_path");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_user_host_path("someuser@some.host:some/path",
	    &user, &host, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "some.host");
	ASSERT_STRING_EQ(path, "some/path");
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_user_ipv4_path");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_user_host_path("someuser@1.22.33.144:some/path",
	    &user, &host, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "1.22.33.144");
	ASSERT_STRING_EQ(path, "some/path");
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_user_[ipv4]_path");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_user_host_path("someuser@[1.22.33.144]:some/path",
	    &user, &host, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "1.22.33.144");
	ASSERT_STRING_EQ(path, "some/path");
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_user_[ipv4]_nopath");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_user_host_path("someuser@[1.22.33.144]:",
	    &user, &host, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "1.22.33.144");
	ASSERT_STRING_EQ(path, ".");
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_user_ipv6_path");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_user_host_path("someuser@[::1]:some/path",
	    &user, &host, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "::1");
	ASSERT_STRING_EQ(path, "some/path");
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_uri");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://someuser@some.host:22/some/path",
	    &user, &host, &port, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "some.host");
	ASSERT_INT_EQ(port, 22);
	ASSERT_STRING_EQ(path, "some/path");
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_uri_ipv4");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://someuser@192.168.1.1:22/some/path",
	    &user, &host, &port, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "192.168.1.1");
	ASSERT_INT_EQ(port, 22);
	ASSERT_STRING_EQ(path, "some/path");
	free(user); free(host); free(path);
	TEST_DONE();

	/* Bracketed hostnames/IPv4 should be rejected (only IPv6 allowed) */
	TEST_START("misc_parse_uri_[ipv4]_rejected");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://someuser@[192.168.1.1]:22/some/path",
	    &user, &host, &port, &path), -1);
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_uri_[hostname]_rejected");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://someuser@[example.com]:22/some/path",
	    &user, &host, &port, &path), -1);
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_uri_[1]_rejected");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://[1]:22",
	    &user, &host, &port, &path), -1);
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_uri_ipv6");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://someuser@[::1]:22/some/path",
	    &user, &host, &port, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "::1");
	ASSERT_INT_EQ(port, 22);
	ASSERT_STRING_EQ(path, "some/path");
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_uri_ipv6_full");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://someuser@[2001:db8::1]:2222/some/path",
	    &user, &host, &port, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "2001:db8::1");
	ASSERT_INT_EQ(port, 2222);
	ASSERT_STRING_EQ(path, "some/path");
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_uri_ipv6_noport");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://someuser@[::1]/some/path",
	    &user, &host, &port, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "::1");
	ASSERT_INT_EQ(port, -1);
	ASSERT_STRING_EQ(path, "some/path");
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_uri_ipv6_nopath");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://someuser@[::1]:22",
	    &user, &host, &port, &path), 0);
	ASSERT_STRING_EQ(user, "someuser");
	ASSERT_STRING_EQ(host, "::1");
	ASSERT_INT_EQ(port, 22);
	ASSERT_PTR_EQ(path, NULL);
	free(user); free(host); free(path);
	TEST_DONE();

	TEST_START("misc_parse_uri_ipv6_nouser");
	user = host = path = NULL;
	ASSERT_INT_EQ(parse_uri("ssh", "ssh://[::1]:22",
	    &user, &host, &port, &path), 0);
	ASSERT_PTR_EQ(user, NULL);
	ASSERT_STRING_EQ(host, "::1");
	ASSERT_INT_EQ(port, 22);
	ASSERT_PTR_EQ(path, NULL);
	free(user); free(host); free(path);
	TEST_DONE();
}
