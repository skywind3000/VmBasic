代码说明

1.初试说明：
这个目录下的代码，都依存于istream.h和ifstream.h里面所定义的流：基类Istream和它的派生：
扩展流类型，文件流类型，内存流类型具体见上面两个头文件里面的说明。

为什么要重新定义流？现在许多C++都提供流的支持，比如微软，波兰公司，还有C++最基本的
iostream.h都有流的定义，但是我仍然不能肯定，这些代码最终将会用到什么地方去，也许是VC++
/BC++宜或者Unix下面的GNU C++。平台变换的太快，因此有必要写下一些不变的东西，对操作系统
或者开发平台能够做到不偏不倚。所以既然定义了流，文件流，内存流那么其他的代码也就是基于
这些流的了，所以不会看见直接操作文件的代码。

2.模板说明：
在ivector.h里面实现了一个重要模板IVector动态数组模板，模仿STL里面的vector，其他的一些常
用模板在itemplat.h里面，实现了两个模板类：IDataArray还有ILister这两个模板也是经常被其他
文件调用的

3.虚拟机的说明：虚拟机被定义为一个纯虚类IVMInstance在ivms.h里

4.具体虚拟机的实现：IVMInstance的派生类IVMBeta在ivmsbeta.h里
  (更多的虚拟机技术说明见manual目录下面的vmbeta.txt)

5.词法分析器和表达式分析器说明
为了提供通用的词法/表达式的分析功能而创建了文件iacciden.h，使它不拘泥于所分析的语言，而
是将一些词法/语法分析的通用功能独立出来了，后面Basic脚本的编译也就是依据这个分析器提供
的基础功能词法分析器定义于iacciden.h里面具体见文件头的注释，提供纯虚类IAccToken用于在输
入流中提取脚本的识别符或者关键词等。在iaccident.h里面实现了一个Basic脚本的词法分析器
IBasicToken还有一个表达式解析器：ISExpress用于将中缀表达式转化成后缀表达式支持逻辑表达
式具体的说明见iaccident.h

6.虚拟机汇编读取类说明
在ivmsca.h里面有一个IVMBetaAsm类，可以从输入流中直接读取虚拟机汇编，并用简单的顺序分析
方法将它翻译成虚拟机的二进制代码，并且读入到一个IVMBeta类里面。

7.VmBasic编译器的说明：
依靠iacciden.h里面定义的词法分析器IBasicToken来从输入流中提取源程序的识别符或者关键词等
，然后调用相应模块生成代码。表达式分析也是靠iacciden.h里面的ISExpress类实现中缀转后缀的
操作然后用一个栈来生成代码，具体见ivmscc.h和ivmscexp.cpp里面的实现。


关于各类功能的说明及定义在各个头文件里面有详细注释，根据情况请自己查阅相应的注释，下面
是文件索引：

  istream.h		流定义文件，定义了纯虚类Istream
  ifstream.h		扩展流定义，定义了扩展流文件流内存流和
  ivector.h             动态数组容器模板
  itemplat.h            模板定义文件，自动分配表：ILister等
  iacciden.h            通用词法/表达式分析器
  ivms.h                虚拟机的基类，定义了一个虚拟机的纯虚类
  ivmsbeta.h            虚拟机beta目前实现的唯一一个虚拟机
  ivmsca.h              虚拟机汇编读取程序定义
  ivmscc.h              虚拟机VmBasic编译程序定义

林伟 skywindt@yeah.net
2004/4/24
