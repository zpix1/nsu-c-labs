#! /bin/bash
# set -ex;

# Configuration
# $1 - lab dir (ex. lab0, lab8-0, ...)
# Please edit these lines
# Wine path
WINE="/Applications/Wine Stable.app/Contents/Resources/wine/bin/wine"
# Compile N Run script, you can download it here (TODO: add link)
COMPILE_N_RUN="/Users/john/saves/tester/compile_n_run.py"
# Default test directory (/lab0/$DEFAULT_CNR_TESTS_DIR/*.test_file)
DEFAULT_CNR_TESTS_DIR="my_test"
# A server to compile code, also edit curl if you want to change compilation method
WINE_COMPILE_URL="http://188.225.38.69:7777/api.php" 

RESULT=0

cd $1;

# CPP Check
echo "cppcheck"
cppcheck --enable=warning,style,performance,portability,unusedFunction \
    --std=c99 \
    --verbose \
    --error-exitcode=1 \
    --language=c \
    -DMAIN=main \
    -I include \
    include/*.h \
    src/*.c \
    test/*.c

let "RESULT += $?"

# My tests (compile_n_run)
echo "gcc tests:"
gcc -Werror \
    -Wall \
    -Wextra \
    -Waddress \
    -Warray-bounds \
    -Wbuiltin-macro-redefined \
    -Wswitch \
    -Wunreachable-code \
    -pedantic \
    -pedantic-errors src/main.c
let "RESULT += $?"
# for test_file in $DEFAULT_CNR_TESTS_DIR/*.test_file 
# do
#     $COMPILE_N_RUN src/main.c -t $test_file;
#     let "RESULT += $?"
# done;

# Wine testing (Petrov's tester)
echo "Wine tests:"
echo "compilation..."
CODE=`printf "%s" | cat src/main.c`
# Compile and get exe url
URL=`curl -s $WINE_COMPILE_URL --data-urlencode "code=$CODE" --data "comp_command=c"`
# Download exe
wget $URL -q -O test/main.exe
if [[ $? != 0 ]]; then echo "Problem downloading exe file" && exit $?; fi

cd test
"$WINE" test*.exe main.exe 2> /dev/null
let "RESULT += $?"
cd ..

if [[ $RESULT == 0 ]];
then 
    echo "SUCCESS. ALL TESTS PASSED!";
else
    echo "ERROR! SOME TESTS FAILED: RESULT_CODE=$RESULT"
fi;
