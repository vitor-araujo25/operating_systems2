#ifndef WALK_DIR_H
#define WALK_DIR_H
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int walk_dir(const char *path, void (*func) (const char *));
#endif
