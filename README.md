#2048

![2048](2048.png)

##2018.07.07

> 1.新增重新开始按钮

##2018.06.07
> 1. 修改游戏结束判断bug

##2018.06.03
> 1. 增加关闭程序时进行数据保存
> 2. 增加打开程序时进行数据读取
> 3. 增加游戏结束的判断
> 4. 修改了若干bug

##2018.6.1
> 1. 完成界面模块
> 2. 完成主要游戏主逻辑模块，还有bug
> 3. 完成分数模块

##游戏主逻辑说明
> 1. 初始生成两个值，要么2，要么4
> 2. 移动（上下左右四个方向）
>    1. 在行/列上找到当前行第一个为空的值,记录下该位置
>    2. 再往后找到第一个不为空的值
>    3. 将这两个位置交换。
> 3. 合并
>    1. 在 **2.移动** 中，边界值为空
>        当交换后的位置与交换后的位置的前一个位置（简称前一个位置）的值相等，前一个位置值*2，删除要移动的值。
>    2. 在 **2.移动** 中，边界值不为空
>        判断边界值是否与后面第一个不为空的值相等
>      1. 相等，边界值*2，删除第一个不为空的值
>      2. 不相等，不做任何操作
> 4. 游戏结束
>    1. 如果出现2048，赢，游戏结束
>    2. 当方格填满，没有合并项，失败，游戏结束

> 注：要记录下该位置在同一回合中是否合并过，避免同一回合多次合并
