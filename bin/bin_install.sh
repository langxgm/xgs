#! /bin/sh

#初始化数据
USER=`whoami`
START_DIR=`pwd`
DEST_DIR="/GameServer/server/bin"
RM_FILES="gate game world sdk"
TAR_FILE=$1

#打印函数
index=0
function echo_color()
{
    let "index++"
    str=$1
    echo -e "\033[36m$index.# ${str}\033[0m" #青色
}

#检查参数
if [ "$TAR_FILE" == "" ]; then
    echo_color "error TAR_FILE=$TAR_FILE"
    exit 1
fi

#开始
#打印用户名&启动目录&目标目录
echo_color "user="$USER
echo_color "start_dir="$START_DIR
echo_color "dest_dir="$DEST_DIR

#删除旧版本
echo_color "cd "$DEST_DIR
cd $DEST_DIR

echo_color "rm -fv "$RM_FILES
rm -fv $RM_FILES

#添加新版本
echo_color "cd "$START_DIR
cd $START_DIR

echo_color "tar xzvf $TAR_FILE to dir "$DEST_DIR
tar xzvf $TAR_FILE -C $DEST_DIR

#结束
echo_color "quit"
