## 项目版本

### u1.2 (unfinished)
- `新增` 直接移植 carPro2 项目模板
- 本版本仅作引入暂存，未进行修改适配

### v1.1
- `优化` 修复编译问题

### v1.0
- `优化` 改变项目结构（有编译问题）   

v0.4   优化按键系统，解决代码汉字报错问题

v0.3   完成基础模板，包括：gpio，key，oled，pid，pwm，rotate，serial，timer

v0.2   在 `stm32f4xx.h` 中增加 BIT(x) 和 BITS(start, end) 宏定义 // 25.7.23

v0.1   删去重复引用 // 25.7.23

v0 更改晶振 25M -> 8M // 25.3.3
> [doc](https://blog.csdn.net/weixin_57941207/article/details/133945394)

## 项目结构
```shell
├─library：外设库源文件
├─DebugConfig：报错 // 已排除
├─Listings：中间文件 // 已排除
├─Objects：目标文件  // 已排除
├─Startup：单片机初始化文件
│  ├─core_cm4.h：内核寄存器的定义
│  ├─core_cmFunc.h----+
│  ├─core_cmInstr.h	  |----操作内核相关文件
│  ├─core_cmSimd.h----+
│  ├─startup_stm32f40_41xxx.s：启动文件
│  ├─stm32f4xx.h：外设寄存器的定义    
│  ├─system_stm32f4xx.c：系统初始化，配置系统时钟   
│  └─system_stm32f4xx.h
├─User：用户主函数源文件
│  ├─main.c：用户编写的主函数 
│  ├─main.h
│  ├─stm32f4xx_conf.h：具体板卡型号的配置
│  ├─stm32f4xx_it.c：用户编写的中断服务程序 // 已排除
│  └─stm32f4xx_it.h      
└─README.md：项目说明文档
```

