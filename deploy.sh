#!/bin/bash

declare -r EXCLUDES=$(dirname $BASH_SOURCE)/exclude.txt
declare -r REPO_ROOT=$(dirname $BASH_SOURCE)
declare -r PAYLOAD=$(dirname $BASH_SOURCE)/payload

if [ "$1" = "sandbox" ]; then
	declare -r TARGET_DIR=sandbox@mywadapi.com:
elif [ "$1" = "prod" ]; then
	declare -r TARGET_DIR=wadapi@mywadapi.com:
else
	echo "Please specify one of [sandbox/prod] as deploy target"
	exit
fi

mkdir $PAYLOAD
cp -r $REPO_ROOT/../framework/css $PAYLOAD
cp -r $REPO_ROOT/../framework/img $PAYLOAD
cp -r $REPO_ROOT/../framework/js $PAYLOAD
cp -r $REPO_ROOT/../framework/lib $PAYLOAD
cp -r $REPO_ROOT/../framework/third_party $PAYLOAD

mkdir -p $PAYLOAD/modules/wadapi
cp -r controller model worker $PAYLOAD/modules/wadapi
mv $PAYLOAD/modules/wadapi/model/XMLGateway.inc $PAYLOAD/lib/model/gateway

if [ "$2" = "go" ];then
	rsync -rltzuv --itemize-changes --delete -O --exclude-from $EXCLUDES $PAYLOAD/ $TARGET_DIR
else
	rsync -rltzuv --itemize-changes --delete -O --dry-run --exclude-from $EXCLUDES $PAYLOAD/ $TARGET_DIR
fi
rm -rf $PAYLOAD
