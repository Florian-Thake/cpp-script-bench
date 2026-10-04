# SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.
# SPDX-License-Identifier: MIT

add_executable( csb_cpp main.cpp )
target_link_libraries( csb_cpp PRIVATE csb_benchmarks )
csb_apply_compiler_options( csb_cpp )
