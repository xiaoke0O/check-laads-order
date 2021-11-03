# 检查LAADS下载订单的完整性

通过读取订单中的cksum文件，并计算本地下载好的订单文件的cksum值，进行比较，进而得出检验报告。
可以得出

- 下载错误的文件
- 未下载的文件（缺失文件）
- 多余文件（不应该有的文件）

目前只在 LAADS的MODIS订单和VIIRS订单上测试过过

## 依赖项

- [ ] : Qt == 5.15

## TODO

- 校验所选订单（当前默认校验全部订单）

## 注意事项

- 鉴于目前绝大多数电脑都是64位了，所以我在Release页只打包了64位windows的可执行文件，如果您确实在使用32位的windows操作系统，请提交Issues或者提交评论或者给我发邮件。

- 在MSVC编译器下编译会不通过，会报fopen已被弃用，并且其我又定义了报错视为错误。我不喜欢加太多`#ifdef _MSC_VER`，尤其是在`if(fopen())`这里，并且`fopen_s`与`fopen`的参数不同，这样代码程序会有撕裂感。建议大家使用mingw64编译，使用msys2安装qt5、g++、cmake都很方便。如果确有需要使用MSVC编译，可以删除`src/CMakeLists.txt`下`target_compile_options`中的`/WX`。
