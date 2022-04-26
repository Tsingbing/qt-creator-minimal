#ifndef WIRE_GLOBAL_H
#define WIRE_GLOBAL_H

#include <QtGlobal>

#if defined(WIRE_LIBRARY)
#  define WIRESHARED_EXPORT Q_DECL_EXPORT
#else
#  define WIRESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // WIRE_GLOBAL_H
