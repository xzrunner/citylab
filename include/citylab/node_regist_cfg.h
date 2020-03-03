#ifndef EXE_FILEPATH
#error "You must define EXE_FILEPATH macro before include this file"
#endif

#define PARM_NODE_CLASS ShortestPath
#define PARM_NODE_NAME shortest_path
#include EXE_FILEPATH

#define PARM_NODE_CLASS HeightField
#define PARM_NODE_NAME height_field
#include EXE_FILEPATH