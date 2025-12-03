[English](README.en.md) | 简体中文

# CH32V307VCT6 工程模板

## 概述

本仓库基于 stm32f407zgt6，经魔改后兼容 VSCode [Embedded IDE](https://em-ide.com) 插件

## 使用方法

### 前置条件
1. 配置 VSCode EIDE 的工具链，按 EIDE 提示下载 OpenOCD
2. 下载**仓库压缩包**或者通过**Git**获取本项目
```git
git clone https://github.com/wzyskq/stm32f407zgt6-project-template.git
```

### Keil
用 keil 打开 `.keil/407.uvprojx` 文件，正常编译烧录即可

### Embedded IDE
VSCode 打开 `.vscode/stm32f407zgt6.code-workspace` 工作区文件，编译烧录即可

**补充说明**
- 若用有线 DAP Link 烧录，建议将接口配置换为 `./tools/cmsis-dap-v1.cfg`
- 若用无线 DAP Link 烧录，建议将接口配置换为 `./tools/cmsis-dap.cfg`

## 项目结构

**主项目树**
```shell
.
├─.keil
│  ├─lst      （keil 编译列表文件夹）
│  └─obj      （keil 编译程序文件夹）
├─.vscode
│  ├─.eide
│  ├─.vscode     
│  └─obj      （EIDE 编译程序文件夹）
├─device      （驱动文件、其他代码）
├─sdk         （软件开发工具包）
│  ├─library
│  └─startup
└─user        （主函数/中断函数等文件）
```

**补充说明**
- 根目录下的 `.keil` 和 `.vscode` 是笔者自己创建的
- 子目录中 `.vscode` 才是由 VSCode 生成的配置文件

## 问题反馈
如有任何问题或建议，欢迎通过 GitHub 提交 Issue 或 Pull Request。

## 版本历史
本项目版本请参阅 [VERSION](VERSION.md) 文件。

## 许可证
本项目基于 MIT 许可证进行分发。有关详细信息，请参阅 [LICENSE](LICENSE) 文件。
