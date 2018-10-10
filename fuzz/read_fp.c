/*
 * Copyright (C) 2017 Alex Helfet
 * Copyright (C) 2018 Angelo Haller
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */


#include <capnp_c.h>
#include <string.h>
#include "addressbook.capnp.h"

int
main(int argc, char **argv)
{
	const char *name = "Firstname Lastname";
	const char *email = "username@domain.com";
	const char *school = "of life";

	int rv = 1;
	struct capn rc;
	if (argc != 2)
		goto err;

	FILE *fp = fopen(argv[1], "rb");
	if (fp == NULL)
		goto err;

	int init_mem_ret = capn_init_fp(&rc, fp, 0 /* packed */);
	if (init_mem_ret != 0)
		goto err_close;

	Person_ptr rroot;
	struct Person rp;
	rroot.p = capn_getp(capn_root(&rc), 0 /* off */, 1 /* resolve */);
	read_Person(&rp, rroot);

	// Assert deserialized values in `rp`
	if (rp.id != 17)
		++rv;
	if (strcmp(name, rp.name.str) != 0)
		++rv;
	if (strcmp(email, rp.email.str) != 0)
		++rv;

	if (rp.employment_which != Person_employment_school)
		++rv;
	if (strcmp(school, rp.employment.school.str) != 0)
		++rv;

	if (capn_len(rp.phones) != 2)
		++rv;

	size_t i;
	for (i = 0; i < capn_len(rp.phones); ++i) {
		struct Person_PhoneNumber rpn;
		get_Person_PhoneNumber(&rpn, rp.phones, i);
		if (i == 0) {
			if (strcmp("123", rpn.number.str) != 0)
				++rv;
			if (rpn.type != Person_PhoneNumber_Type_work)
				++rv;
		}
		if (i == 1) {
			if (strcmp("234", rpn.number.str) != 0)
				++rv;
			if (rpn.type != Person_PhoneNumber_Type_home)
				++rv;
		}
	}

	--rv;

err_free:
	capn_free(&rc);
err_close:
	fclose(fp);
err:
	return rv;
}
