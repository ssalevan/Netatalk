/*
  $Id: test.c,v 1.1.2.1 2010-02-01 10:56:08 franklahm Exp $
  Copyright (c) 2010 Frank Lahm <franklahm@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <atalk/util.h>
#include <atalk/cnid.h>
#include <atalk/logger.h>
#include <atalk/volume.h>
#include <atalk/directory.h>
#include <atalk/queue.h>
#include <atalk/bstrlib.h>

#include "file.h"
#include "filedir.h"
#include "directory.h"
#include "dircache.h"
#include "hash.h"
#include "globals.h"
#include "afp_config.h"
#include "volume.h"

#include "test.h"
#include "subtests.h"
#include "afpfunc_helpers.h"

/* Stuff from main.c which of cource can't be added as source to testbin */
unsigned char nologin = 0;
struct afp_options default_options;
static AFPConfig *configs;

/* Static variables */

int main(int argc, char **argv)
{
    #define ARGNUM 7
    char *args[ARGNUM] = {"test", "-F", "test.conf", "-f", "test.default", "-s" ,"test.system"};
    int reti;
    uint16_t vid;
    struct vol *vol;
    struct dir *dir;
    struct dir *retdir;
    struct path *path;

    /* initialize */
    afp_version = 32;
    printf("Initializing\n============\n");
    TEST(setuplog("default log_note /dev/tty"));
    TEST(afp_options_init(&default_options));
    TEST_int(afp_options_parse( ARGNUM, args, &default_options), 1);
    TEST_expr(configs = configinit(&default_options), configs != NULL);
    TEST(cnid_init());
    TEST(load_volumes(&configs->obj));
    TEST_int(dircache_init(8192), 0);
 
    printf("\n");

    /* now run tests */
    printf("Running tests\n=============\n");

    TEST_expr(vid = openvol(&configs->obj, "test"), vid != 0);
    TEST_expr(vol = getvolbyvid(vid), vol != NULL);

    /* test dircache.c stuff*/
    TEST_expr(dir = dir_new("dir", "dir", vol, DIRDID_ROOT, htonl(20), bfromcstr(vol->v_path)),
              dir != NULL);
    TEST_int(dircache_add(dir), 0);
    TEST_expr(retdir = dircache_search_by_did(vol, dir->d_did ),
              retdir != NULL && retdir == dir && bstrcmp(retdir->d_u_name, dir->d_u_name) == 0);
    TEST_expr(retdir = dircache_search_by_name(vol, DIRDID_ROOT, "dir", strlen("dir")),
              retdir != NULL && retdir == dir && bstrcmp(retdir->d_u_name, dir->d_u_name) == 0);
    TEST_int(dir_remove(vol, dir), 0);
    TEST_int(test001_add_x_dirs(vol, 100, 100000), 0);
    TEST_int(test002_rem_x_dirs(vol, 100, 100000), 0);

    /* test directory.c stuff */
    TEST_expr(retdir = dirlookup(vol, DIRDID_ROOT_PARENT), retdir != NULL);
    TEST_expr(retdir = dirlookup(vol, DIRDID_ROOT), retdir != NULL);
    TEST_expr(path = cname(vol, retdir, cnamewrap("Network Trash Folder")), path != NULL);

    TEST_expr(retdir = dirlookup(vol, DIRDID_ROOT), retdir != NULL);
    TEST_int(getfiledirparms(&configs->obj, vid, DIRDID_ROOT_PARENT, "test"), 0);
    TEST_int(getfiledirparms(&configs->obj, vid, DIRDID_ROOT, ""), 0);

    TEST_expr(reti = createdir(&configs->obj, vid, DIRDID_ROOT, "dir1"),
              reti == 0 || reti == AFPERR_EXIST);
    TEST_expr(retdir = dircache_search_by_name(vol, DIRDID_ROOT, "dir1", strlen("dir1")),
              retdir != NULL);

    TEST_int(getfiledirparms(&configs->obj, vid, DIRDID_ROOT, "dir1"), 0);
    TEST_int(getfiledirparms(&configs->obj, vid, retdir->d_did, "//"), 0);


    TEST_int(createfile(&configs->obj, vid, DIRDID_ROOT, "dir1/file1"), 0);
    TEST_int(delete(&configs->obj, vid, DIRDID_ROOT, "dir1/file1"), 0);
    TEST_int(delete(&configs->obj, vid, DIRDID_ROOT, "dir1"), 0);

    TEST_int(createfile(&configs->obj, vid, DIRDID_ROOT, "file1"), 0);
    TEST_int(getfiledirparms(&configs->obj, vid, DIRDID_ROOT, "file1"), 0);
    TEST_int(delete(&configs->obj, vid, DIRDID_ROOT, "file1"), 0);


    /* test enumerate.c stuff */
    TEST_int(enumerate(&configs->obj, vid, DIRDID_ROOT), 0);
}