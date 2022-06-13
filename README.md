一款极简化本地类 Polygon 的自动化生成数据、检验、打包的脚本。

### 功能

基于 ```testlib.h``` 实现，可以自动生成数据（最多 10 组）并检验数据合法性，两份 std 输出是否一致，数据大小是否超过一定大小（默认 1M），并自动打包成如下目录结构：

```
problems
	| 程序
		gen-chart
		gen.cpp
		val.cpp
		std.cpp/std.py
		std2.cpp/std2.py
	| 输入
		1.in
		2.in
		3.in
		4.in
		5.in
		6.in
		7.in
		8.in
		9.in
		10.in
	| 输出
		1.out
		2.out
		3.out
		4.out
		5.out
		6.out
		7.out
		8.out
		9.out
		10.out
	题面.*
	题解.*
	数据分布.*
```

### 适用平台

Windows
Linux

### 使用方法

#### 目录结构

目录结构如下：

```
problems
    gen.cpp           // 数据生成器，基于 testlib.h
    val.cpp           // 数据校验器，基于 testlib.h
    gen-chart         // 存放用于生成数据的命令
    std.cpp/std.py    // 第一份std，优先检查是否存在std.cpp
    std2.cpp/std2.py  // 第二份std，优先检查是否存在std2.cpp
    题面.*            // 题面文件，最好是 doc/docx
    题解.*            // 题解文件
    数据分布.*        // 数据分布表，最好是 xlsx 文件
    autowork         // 该脚本对应的可执行文件
    testlib.h
```

之后在该目录下执行 autowork 就行。

#### gen.cpp

基于 ```testlib.h``` 实现 generator。

#### val.cpp

基于 ```testlib.h``` 实现 validator。

#### gen-chart

文件内每一行是生成数据的一行命令，如下格式：

```
./gen 5 example > 1.in
./gen 10 90 > 2.in
./gen 10 50 > 3.in
./gen 1000 97 > 4.in
./gen 1000 1 > 5.in
./gen 1000 50 > 6.in
./gen 1000 87 > 7.in
./gen 1000 100 > 8.in
./gen 500000 0 > 9.in
./gen 500000 56 > 10.in
```

具体可以参考 ```testlib.h``` 实现 ```generator``` 的方法。

