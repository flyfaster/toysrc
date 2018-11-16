#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#eclipse -data $DIR 2>/dev/null &


if [[ "$OSTYPE" == "linux-gnu" ]]; then
        # ...
        echo "$OSTYPE"
elif [[ "$OSTYPE" == "darwin"* ]]; then
        # Mac OSX
        /Applications/Eclipse.app/Contents/MacOS/eclipse -data $DIR 2>/dev/null &
elif [[ "$OSTYPE" == "cygwin" ]]; then
        # POSIX compatibility layer and Linux environment emulation for Windows
        echo "$OSTYPE"
elif [[ "$OSTYPE" == "msys" ]]; then
        # Lightweight shell and GNU utilities compiled for Windows (part of MinGW)
        echo "$OSTYPE"
elif [[ "$OSTYPE" == "win32" ]]; then
        # I'm not sure this can happen.
        echo "$OSTYPE"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
        # ...
        echo "$OSTYPE"
else
        # Unknown.
        echo "$OSTYPE"
fi


