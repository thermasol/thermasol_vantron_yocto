/* GLES2 to GLES3 compatibility header */
#ifndef __gl3ext_h_
#define __gl3ext_h_

#include <GLES2/gl2ext.h>

/* Additional GLES3 types not in GLES2 */
typedef void (*GLDEBUGPROC)(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char *message, const void *userParam);

#endif
