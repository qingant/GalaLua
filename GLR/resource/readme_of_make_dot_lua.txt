1. 静态版本的glr所依赖的各种静态库，在编译完成后的安装目录，C语言的静态库在/opt/agree/gdk/share/static下，Lua的脚本库在/opt/agree/gdk/share/static/lua下。

编译静态版本的glr所依赖头文件resxloader.h，安装目录为/opt/agree/gdk/include/resource. 

2. make.lua和Makefile.static在git版本库中的目录为GalaLua/GLR/resource.

make.lua的默认安装路径为/opt/agree/gdk/bin，Makefile.static的默认安装路径为/opt/agree/gdk/share。如果Makefile.static的安装路径发生变化，请务必修改make.lua中的MAKEFILE_STATIC_PATH变量为当前的安装路径。

用户在编译自定义的agent版本时，只需将自定义的所有Lua的C语言静态库和Lua的脚本库列在一个文本文件中，每个文件一行即可。

例如：有如下三个C语言静态库，
    /a/b/libab.a
    /a/c/libbc.a
    /d/e/f/g/libdefg.a
有如下三个Lua脚本库
    /h/i/j/k/hijk.lua
    /h/l/m/hlmn.lua
    /o/p/q
    其中为/o/p/q为目录，在打包时会将整个目录中的所有内容打到资源包中。
那么，该文件的内容为（不包括括号所在行）：
（文件开始）
/h/i/j/k/hijk.lua
/h/l/m/hlmn.lua
/o/p/q

/a/b/libab.a
/a/c/libbc.a
/d/e/f/g/libdefg.a
（文件结束）

3. 用户自定义glr的编译
    在编译之前，用户需编辑/opt/agree/gdk/include/resource/resxloader.h文件，在其中声明Lua的C语言静态库的入口函数，同时在数组lualibray中添加Lua库名和库的入口函数。

    生成的目标文件名为glr_sl

    执行make.lua即可，参数为自定义库文件的列表。例如lua make.lua librarys.txt

