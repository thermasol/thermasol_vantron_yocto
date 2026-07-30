/* GLES2 to GLES3 compatibility header */
#ifndef __gl3_h_
#define __gl3_h_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

/* Additional GLES3 types not in GLES2 */
typedef void (GL_APIENTRYP GLDEBUGPROC)(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char *message, const void *userParam);

/* GLES3 constants that might be missing from GLES2 */
#ifndef GL_DEBUG_OUTPUT
#define GL_DEBUG_OUTPUT 0x92E0
#endif

#endif
