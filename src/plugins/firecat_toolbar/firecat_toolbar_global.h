#pragma once

#include <QtGlobal>

#if defined(FIRECAT_TOOLBAR_LIBRARY)
#  define FIRECAT_TOOLBARSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FIRECAT_TOOLBARSHARED_EXPORT Q_DECL_IMPORT
#endif
