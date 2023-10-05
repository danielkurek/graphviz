/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

/*
 * Glenn Fowler
 * AT&T Research
 *
 * include style search support
 */

#include <ast/ast.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef struct Dir_s {		/* directory list element */
    struct Dir_s *next;		/* next in list                 */
    char dir[1];		/* directory path               */
} Dir_t;

static struct {			/* directory list state           */
    Dir_t *head;		/* directory list head          */
    Dir_t *tail;		/* directory list tail          */
} state;

/*
 * return path to name using pathinclude() list
 * if lib!=0 then pathpath() attempted after include search
 * if type!=0 and name has no '.' then file.type also attempted
 * any *: prefix in lib is ignored (discipline library dictionary support)
 */

char *pathfind(const char *name, const char *lib, const char *type)
{
    Dir_t *dp;
    char *s;
    char tmp[PATH_MAX];
    char *buf;

    if (access(name, R_OK) >= 0)
	return strdup(name);
    if (type) {
	snprintf(tmp, sizeof(tmp), "%s.%s", name, type);
	if (access(tmp, R_OK) >= 0)
	    return strdup(tmp);
    }
    if (*name != '/') {
	if (strchr(name, '.'))
	    type = 0;
	for (dp = state.head; dp; dp = dp->next) {
	    snprintf(tmp, sizeof(tmp), "%s/%s", dp->dir, name);
	    if ((buf = pathpath(tmp)))
		return buf;
	    if (type) {
		snprintf(tmp, sizeof(tmp), "%s/%s.%s", dp->dir, name,
			  type);
		if ((buf = pathpath(tmp)))
		    return buf;
	    }
	}
	if (lib) {
	    if ((s = strrchr(lib, ':')))
		lib = s + 1;
	    snprintf(tmp, sizeof(tmp), "lib/%s/%s", lib, name);
	    if ((buf = pathpath(tmp)))
		return buf;
	    if (type) {
		snprintf(tmp, sizeof(tmp), "lib/%s/%s.%s", lib, name,
			  type);
		if ((buf = pathpath(tmp)))
		    return buf;
	    }
	}
    }
    return 0;
}
