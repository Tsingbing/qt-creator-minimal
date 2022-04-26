#pragma once

#include <QtGlobal>

#if defined(CANTOOL_LIBRARY)
#  define CANTOOLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CANTOOLSHARED_EXPORT Q_DECL_IMPORT
#endif

