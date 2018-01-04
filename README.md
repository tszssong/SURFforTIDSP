# SURFforTIDSP
C代码实现SURF
针对TI DSP的，ccs版本5.4，没有专门针对DSP做优化
cmd文件针对tms320c6727,确定的是改成6748也能跑，用Simulation.ccxml可以仿真，打印匹配到的角点
把1.ccsproject 1.cproject 1.project前面的1去掉，然后ccs5里直接project->Import existing ccs project然后打开目录是可以直接编译运行的，注意仿真的话要保证activate的是simulation.ccxml

调试经验：
ccs安装路径最好别有空格和特殊字符，会导致编译无法生成.lib
很多问题可以通过修改堆栈大小解决：properties->build->c6000 linker->basic options

--------------
SURF里是网上找到的一份matlab代码，流传的基本是这个版本，自己根据实验改了一部分
如果参数设置相同，c程序运行结果会和matlab程序SURF/example2.m完全一致
