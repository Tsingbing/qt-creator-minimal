#pragma once

#include <QtGlobal>

#if defined(DDSMONITOR_LIBRARY)
#  define DDSMONITORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DDSMONITORSHARED_EXPORT Q_DECL_IMPORT
#endif

