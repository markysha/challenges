#!/bin/bash

function usage()
{
    echo 'Usage: run.sh FILENAME [-h] [-l L_TEST] [-r R_TEST] [-d]'
}


if [[ -z $CHALLENGE_PATH ]]; then
    echo -e '$CHALLENGE_PATH not found.\nWrite something like "export CHALLENGE_PATH=~/bubblecup2020/challenge" in your ~/.bashrc or ~/.zshrc'
    exit 1
fi

if [[ -z $CXX ]]; then
    echo -e '$CXX not found.\nWrite something like "export CXX=g++" in your ~/.bashrc or ~/.zshrc'
    exit 1
fi

SOURCE=$1; shift

LAST=$CHALLENGE_PATH/last/$(basename $SOURCE)

DATE=$(date | grep -o '..:..:..')

mkdir -p $CHALLENGE_PATH/input
mkdir -p $CHALLENGE_PATH/outputs

OUTPUT=$CHALLENGE_PATH/outputs/output_$DATE
mkdir -p $LAST
mkdir -p $OUTPUT

L_TEST=0
R_TEST=228
DEBUG=false
SCORE=false
TIME=false

while [ "$1" != "" ]; do
    case $1 in
        -h | --help)
            usage
            exit
            ;;
        -l|--ltest)
            shift
            L_TEST=$1
            ;;
        -r|--rtest)
            shift
            R_TEST=$1
            ;;
        -d|--debug)
            DEBUG=true
            ;;
        -s|--score)
            SCORE=true
            ;;
        -t|--time)
            TIME=true
            ;;
        *)
            echo "ERROR: unknown parameter \"$1\""
            usage
            exit 1
            ;;
    esac
    shift
done

echo "Date = $DATE"

if ! $DEBUG; then
    COMPILE_CMD="$CXX $SOURCE --std=c++17 -O3 -g -D LOCAL -o $OUTPUT/a.out"
else
    COMPILE_CMD="$CXX $SOURCE --std=c++17 -fsanitize=address,undefined -g -D LOCAL -o $OUTPUT/a.out"
fi

###### BUBBLE_CUP ONLY ######
if grep 'INSERT_COMMON_HERE' $SOURCE > /dev/null; then
    cp $CHALLENGE_PATH/src/common.hpp $LAST/main.cpp
    grep -v 'INSERT_COMMON_HERE' $SOURCE >> $LAST/main.cpp
else
    cp $SOURCE $LAST/main.cpp
fi
#############################


echo 'Compiling...'

if ! $COMPILE_CMD; then
    echo 'Compilation failed. Exit.'
    exit 1
fi

if $SCORE; then
    if [[ -f $CHALLENGE_PATH/lib/score.cpp ]]; then
        $CXX $CHALLENGE_PATH/lib/score.cpp --std=c++17 -O3 -D LOCAL -o $OUTPUT/score.out
    else
        echo 'Score compilation failed. Exit.'
        exit 1
    fi
fi

echo -e 'Finish compiling!\n'

if [[ ! -d $CHALLENGE_PATH/input ]]; then
    echo "$CHALLENGE_PATH/input directory not found. Exit."
    exit
fi

tar cfz "$OUTPUT/source.tgz" $SOURCE

for input in $CHALLENGE_PATH/input/*
do
    let "i += 1"
    if [[ $R_TEST -lt $i ]]; then
        break
    fi

    if [[ $i -lt $L_TEST ]]; then
        continue
    fi


    echo "TEST '$(basename $input)'"

    if ! $TIME; then
        if ! $OUTPUT/a.out $i < $input > $OUTPUT/ans_$i; then
            echo -e "\nFailed while processing test #$i. Exit."
            exit
        fi
    else
        if ! time $OUTPUT/a.out $i < $input > $OUTPUT/ans_$i; then
            echo -e "\nFailed while processing test #$i. Exit."
            exit
        fi
    fi

    if $SCORE; then
        # if [[ -f $OUTPUT/score.out ]]; then
        #     $OUTPUT/score.out $input $OUTPUT/ans_$i
        # else
        #     echo "Score not found."
        # fi

        ### BUBBLE_CUP CHALLENGE2 COSTYL ###
        # cp $CHALLENGE_PATH/useful/judge/judge.out $OUTPUT/score.out

        $OUTPUT/score.out $input $OUTPUT/ans_$i

        # echo -n "SCORE = "
        # cat $OUTPUT/score.txt
        ####################################
    else
        echo "Finished!"
    fi

    echo ""

done

cp -r $OUTPUT/* $LAST
