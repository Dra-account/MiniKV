#ifndef ENGINE_H
#define ENGINE_H

#include "parser.h"
#include "storage.h"

int engine_execute(Storage* storage, const KvCommand* cmd);

#endif
