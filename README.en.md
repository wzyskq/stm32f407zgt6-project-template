English | [简体中文](README.md)

# CH32V307VCT6 Project Template

## Overview
This repository is based on stm32f407zgt6 and has been heavily modified to be compatible with the VSCode [Embedded IDE](https://em-ide.com) extension.

## Usage

### Prerequisites
1. Configure the toolchain and corresponding OpenOCD path in VSCode EIDE.
2. Download the **repository archive** or obtain this project via **Git**.
```git
git clone https://github.com/wzyskq/stm32f407zgt6-project-template.git
```

### Keil
Open the 407.uvprojx file with Keil, then build and flash as usual.

### Embedded IDE
Open the stm32f407zgt6.code-workspace workspace file in VSCode, then build and flash directly.

## Project Structure

**Main Project Tree**
```shell
.
├─.keil
│  ├─lst      (Keil build list folder)
│  └─obj      (Keil build output folder)
├─.vscode
│  ├─.eide
│  ├─.vscode
│  └─obj      (EIDE build output folder)
├─device      (Driver files and other code)
├─sdk         (Software development kit)
│  ├─library
│  └─startup
└─user        (Main functions, interrupt handlers, etc.)
```

**Additional Notes**
- The .keil and .vscode folders in the root directory were created manually.
- The .vscode subdirectories are VSCode-generated configuration files.

## Feedback
If you have any questions or suggestions, feel free to submit an Issue or Pull Request on GitHub.

## Version History
Please refer to the VERSION file for project versions.

## License
This project is distributed under the MIT License. For details, see the LICENSE file.
