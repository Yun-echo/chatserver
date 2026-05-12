#由所有的源码生成一个可执行文件


#定义了一个SRC_LIST变量，包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)#这个 . 是当前目录下的，它不会搜索子目录
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./redis MODEL_LIST)

#指定生成可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})

#指定可执行文件链接时需要依赖的库文件
#编译muduo_net muduo_base pthread 这三个库
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)