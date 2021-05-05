                               说        明 


    VmBasic是一款基于虚拟机的结构化的编程语言，用VmBasic编写的代码将被最终解
释成二进制的字节码由虚拟机运行，本压缩档包含VmBasic和虚拟机VmBeta还有开发环
境的IDE以及它们所有的源程序，和开发文档，技术资料，关于程序的申明请见
license.txt。

    VmBasic和VmBeta的所有源程序，文档以及可执行程序，示例文件由skywind(林伟)
skywindt@yeah.net独立设计完成，依照GNU的精神：你可以任意复制拷贝和在你的网站
上发布，可以任意更改本压缩档里面的源程序，也可以用在你的程序中，但请在程序或
说明中保留上述原始版权信息。如果你对于它有什么好的意见请写信给我，或者到
http://softnb.51.net或http://www.joynb.com留言，你的建议是对我的最大鼓励，我
将继续完善它。如果你修改了其中的源程序，希望你吧修改的作品也发一分给我。

    这个编译器用来干什么的？用于对软件/游戏提供一个可扩充的脚本机制，VmBeta指
令通过输出/输入的方法象用户自己的程序进行通讯，用户通过处理输出输入消息来达到
功能的扩充，使它符合你产品的需要。提供脚本机制的目的也在于让用户可以更灵活地
操作软件，比如3DMax里面类QBasic的MaxScript或者其他一些用途，因此，出于用户编
写程序的简单，这里选择了DOS自带的QBasic语言做蓝本，由于运行前解释成二紧制的虚
拟机字节码，因此速度是解释性语言不能比拟的，代码的设计在于与平台无关性，换句
话说，你可以将这些核心代码在Windows下或者Unix下毫无修改的编译，代码经过下面几
个编译器的测试：
                Borland C++ 3.1 for DOS
                DJGPP for DOS
                DEV-C++ 4.0 (with GNU C++ Compiler) for Windows
                Visual C++ 6.0
                Borland C++ Builder 5.0
                Watcom C++ 10.0
                GNU C++ under Unix and Linux
关于代码更详细的说明和程序规划见manual目录下的readme.txt文件，以及写在各文件
里面详细的注释。

下面是本压缩档的文件及目录说明：

    Manual目录下的东西：
          readme.txt          关于更多代码的说明
          vmbeta.txt          关于虚拟机的设计说明
          vmbasic.txt         VmBasic的语法及相关说明
          
    ImApp目录下面的东西：
          VmBeta/             虚拟机主程序
          VmBIDE/             VmBasic编译程序及IDE，CPPB作的
          Console/            虚拟机和编译器文本版可以无修改
                              在unix下编译
          

    
林伟 skywindt@yeah.net
2002年5月1日
