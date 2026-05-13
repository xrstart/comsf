# comsf - 串口上位机助手

基于 Qt 6 的 Windows 桌面串口调试工具，支持动态仪表盘、自定义控件、数据解析、逻辑判断，以及 **AI 智能助手**。

## 下载使用

前往 [Releases](https://github.com/xrstart/comsf/releases) 下载最新版 zip，解压后双击 `comsf.exe` 即可运行，无需安装任何依赖。

## 功能特性

### 串口通信
- 自动扫描可用串口
- 支持波特率、数据位、停止位、校验位、流控制配置
- ASCII / HEX 双模式收发
- 实时数据日志，支持暂停、清空、时间戳

### 动态仪表盘
用户可在仪表盘区域自由添加以下三种控件：

- **自定义发送按钮** - 一键发送预设报文（ASCII 或 HEX）
- **数据解析显示框** - 通过格式化字符串（如 `Torque:%d Nm`）从原始数据中提取并显示数值
- **逻辑信号灯** - 根据逻辑表达式（如 `Torque > 2.8 AND PWM > 45`）判断条件，通过颜色指示状态

### AI 智能助手（v1.2 新增）

软件右侧常驻 AI 面板，包含两个功能标签页：

#### 数据诊断
在接收区选中串口报文，右键选择 **AI 解析选中数据**，即可调用大模型分析报文格式、查找异常或猜测数据规律。

#### 仪表盘副驾
通过自然语言对话，让 AI 自动在仪表盘上创建控件，无需手动配置：

| 输入示例 | 生成结果 |
|---------|---------|
| `帮我加一个解析框，格式是 Torque:%d Nm` | 数据解析显示框 |
| `加一个红色的逻辑灯，条件是 PWM > 45` | 逻辑信号灯 |
| `加三个按钮，分别发 s、hello、dsfa` | 三个 ASCII 发送按钮 |
| `发一个十六进制 01FF 的按钮` | HEX 模式发送按钮 |

#### AI 配置
菜单栏 **AI → AI 设置**，支持配置 API Key、Base URL、模型名称，并内置 DeepSeek、OpenAI、Moonshot 快捷预设。

兼容所有 OpenAI 格式的 API 接口。

### 编辑与布局
- 编辑模式下支持控件拖拽、缩放
- 自动对齐吸附 + 辅助线
- 右键菜单添加/编辑/删除控件

### 配置管理
- 一键导出所有控件配置为 JSON 文件
- 导入配置瞬间恢复工作环境
- 保存时自动备份

### 界面
- 支持 4 种主题（默认浅色、深色、蓝色、绿色）
- 左右分屏布局：左侧串口与仪表盘，右侧 AI 助手面板

## 从源码构建

### 环境要求
- Qt 6.8.0（或兼容版本）
- MinGW 13.1 / GCC 13+
- Qt Creator（可选）

### 编译
```bash
cd comsf
qmake comsf.pro "QMAKE_CXX=D:/Qt/Tools/mingw1310_64/bin/g++.exe" "QMAKE_CC=D:/Qt/Tools/mingw1310_64/bin/gcc.exe" "QMAKE_LINK=D:/Qt/Tools/mingw1310_64/bin/g++.exe"
mingw32-make -f Makefile.Release -j8
```

> 注意：如果系统中存在多个 MinGW 版本，qmake 时必须显式指定编译器路径，避免版本冲突。

### 打包为独立程序
```bash
mkdir deploy
copy release\comsf.exe deploy\
windeployqt deploy\comsf.exe
```

打包完成后 `deploy` 文件夹即为独立可运行程序。

## 技术栈
- C++17
- Qt 6 (Widgets + SerialPort + Network)
- qmake 构建系统
