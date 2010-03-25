#!/bin/sh

if echo $1 | grep \\.frag > /dev/null; then
    type="f"
else
    type="v"
fi

~/src/glsl2/glsl $type $1 > /dev/null
status=$?

if test "x$2" = xpass; then
    if test "$status" = "0"; then
	echo PIGLIT: {\'result\': \'pass\' }
    else
	echo PIGLIT: {\'result\': \'fail\' }
    fi
else
    if test "$status" = "0"; then
	echo PIGLIT: {\'result\': \'fail\' }
    else
	echo PIGLIT: {\'result\': \'pass\' }
    fi
fi
