#!/usr/bin/env sh
find examples include src \( -name "*.h" -o -name "*.cpp" \) -exec clang-format -i {} \;