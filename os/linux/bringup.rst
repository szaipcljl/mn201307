
bringup 流程：
=============
提前3~5天准备环境，熟悉原理图等,具体：
(1) 准备新板bringup的环境，
(2) 根据gpio table 将gpio 理一遍。包括bootloader 和dts 中的gpio
(3) 重点看 原理图有变更的地方。
     soc bringup 注意： poc(硬件控制), sdram(参数、容量、分区，bst
     里面初始化sdram和emmc), emmc, pmic(上电时序)，

