# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

# One runner executable per language (see PLAN.md, "Architecture").

add_subdirectory( cpp )

if( CSB_WITH_TEASCRIPT )
    add_subdirectory( teascript )
else()
    message( STATUS "TeaScript runner: disabled (set CSB_WITH_TEASCRIPT=ON and CSB_TEASCRIPT_DIR)" )
endif()

if( CSB_WITH_CHAISCRIPT )
    add_subdirectory( chaiscript )
else()
    message( STATUS "ChaiScript runners: disabled (set CSB_WITH_CHAISCRIPT=ON and CSB_CHAISCRIPT_DIR)" )
endif()
