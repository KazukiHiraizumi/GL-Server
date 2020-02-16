#ifndef CPARSEH
#define CPARSEH

#include <queue>
#include <string>

extern int cparser_busy;
extern std::queue<std::string> cparser_queue;
extern void cparser(char *);

#endif
