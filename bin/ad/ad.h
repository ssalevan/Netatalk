/* 
   Copyright (c) 2009 Frank Lahm <franklahm@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*/

#ifndef AD_H
#define AD_H

#define _XOPEN_SOURCE 600

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include <atalk/ftw.h>
#include <atalk/volinfo.h>
#include <atalk/cnid.h>

#define DIR_DOT_OR_DOTDOT(a) \
        ((strcmp(a, ".") == 0) || (strcmp(a, "..") == 0))

enum logtype {STD, DBG};

#define SLOG(...)                             \
    _log(STD, __VA_ARGS__)

#define ERROR(...)                              \
    do {                                        \
        _log(STD, __VA_ARGS__);                 \
        exit(1);                                \
    } while (0)

typedef struct {
    struct volinfo volinfo;
    struct vol     volume;
    char           db_stamp[ADEDLEN_PRIVSYN];
} afpvol_t;

extern int log_verbose;             /* Logging flag */
extern void _log(enum logtype lt, char *fmt, ...);

extern int ad_ls(int argc, char **argv);
extern int ad_cp(int argc, char **argv);
extern int ad_rm(int argc, char **argv);

/* ad_util.c */
extern int openvol(const char *path, afpvol_t *vol);
extern void closevol(afpvol_t *vol);
extern cnid_t cnid_for_path(const struct volinfo *vi,
                            const struct vol *vol,
                            const char *path,
                            cnid_t *did);
extern char *utompath(const struct volinfo *volinfo, const char *upath);
extern int convert_dots_encoding(const afpvol_t *svol, const afpvol_t *dvol, char *path, size_t buflen);

typedef struct {
    char *p_end;/* pointer to NULL at end of path */
    char *target_end;/* pointer to end of target base */
    char p_path[MAXPATHLEN + 2];/* pointer to the start of a path */
} PATH_T;

extern PATH_T to;
extern int fflag, iflag, lflag, nflag, pflag, vflag;

#endif /* AD_H */