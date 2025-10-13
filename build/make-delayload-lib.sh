#!/bin/sh

library_name=$1
importlib_name=$2
def_name=$3

echo Building $def_name from $importlib_name...

cat <<end_header > $def_name
LIBRARY $library_name
EXPORTS
end_header

nm -U $importlib_name | grep ' T [^.?]' | sed 's/^0\+ T _/  /' >> $def_name

delaylib_name=${library_name}_delayed.lib

echo Building $delaylib_name from $def_name...

dlltool --input-def $def_name --output-delaylib $delaylib_name
