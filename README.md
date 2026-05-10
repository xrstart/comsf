# comsf - 串口上位机助手

基于 Qt 6 的 Windows 桌面串口调试工具，支持动态仪表盘、自定义控件、数据解析与逻辑判断。

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

## 从源码构建

### 环境要求
- Qt 6.8.0（或兼容版本）
- MinGW 13.1 / GCC 13+
- Qt Creator（可选）

### 编译
```bash
cd comsf
qmake comsf.pro
mingw32-make -f Makefile.Release -j8
```

### 打包为独立程序
```bash
mkdir deploy
copy release\comsf.exe deploy\
windeployqt deploy\comsf.exe
```

打包完成后 `deploy` 文件夹即为独立可运行程序。

## 技术栈
- C++17
- Qt 6 (Widgets + SerialPort)
- qmake 构建系统
