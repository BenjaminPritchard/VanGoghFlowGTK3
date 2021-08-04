
#pragma once

// code adapted from: https://github.com/anko/xkbcat/blob/master/xkbcat.c

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool shouldStop;         

bool initKB();
void *CheckForHotKeys();