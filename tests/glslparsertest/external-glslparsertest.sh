#!/bin/sh

if test "x$2" != "xpass" -a "x$2" != "xfail"; then
    echo "usage: external-glslparsertest.sh filename [pass|fail]"
    exit 1;
fi

if test "x$GLSL_PARSER" = x ; then
    echo 'Set GLSL_PARSER env var to point to parser program.'
    echo PIGLIT: {\'result\': \'fail\' }
    exit
fi

if test ! -x $GLSL_PARSER ; then
    echo 'Program specified by GLSL_PARSER env var does not exist or is not executable.'
    echo PIGLIT: {\'result\': \'fail\' }
    exit
fi


result=$($GLSL_PARSER $1)
status=$?

if test "x$2" = xpass; then
    if test "$status" = "0"; then
	echo PIGLIT: {\'result\': \'pass\' }
    else
	echo "shader source:"
	cat $1
	echo "compiler result:"
	echo "$result"
	echo PIGLIT: {\'result\': \'fail\' }
    fi
else
    if test "$status" = "0" -o "$status" = 139; then
	echo "shader should have failed:"
	cat $1
	echo PIGLIT: {\'result\': \'fail\' }
    else
	echo PIGLIT: {\'result\': \'pass\' }
    fi
fi
