# ProxySwitch - 系统代理快速切换工具

![Windows](https://img.shields.io/badge/Windows-7+-0078D6?logo=windows)

一个轻量级的Windows系统代理切换工具，通过简单的GUI界面实现一键代理开关功能。

## 功能特性

- **可视化按钮切换代理状态**：通过按钮即可轻松启用或禁用代理。
- **代理服务器地址硬编码为 `10.88.202.71:10001`**：默认配置了代理服务器地址。
- **自动刷新网络设置**：修改代理设置后自动刷新系统网络配置。

## 编译指南

### 环境要求
- **Windows SDK**：需要Windows SDK支持。
- **编译环境**：支持C++11标准的编译器，如Visual Studio或MinGW。
- **依赖库**：需要链接`wininet`、`user32`和`uxtheme`库。

### 编译步骤
1. 创建Win32项目。
2. 添加`proxy.cpp`到项目源文件。
3. 配置项目链接库：
   ```bash
   -lwininet -luser32 -luxtheme
   ```
4. 编译生成可执行文件。

## 使用说明
1. **运行程序**：直接运行生成的`ProxySwitch.exe`。
2. **主界面**：显示一个切换按钮和状态文本。
3. **切换代理状态**：
   - 点击按钮切换代理状态。
   - **启用代理**：按钮文字变为"禁用代理"。
   - **禁用代理**：按钮恢复"启用代理"。

## 注意事项
- **管理员权限**：需要管理员权限运行，以修改系统代理设置。
- **代理地址**：代理地址硬编码在源代码中，修改需重新编译。
- **字体依赖**：系统需安装`Segoe UI`字体（Windows默认包含）。
- **兼容性**：兼容Windows 7及以上系统。

## 实现原理
通过Windows API实现代理配置：
- `InternetSetOption`：用于修改代理设置。
- `INTERNET_PER_CONN_OPTION`：配置连接参数。
- `PROXY_TYPE_PROXY`：标志位控制代理状态。
- 强制刷新系统设置：确保修改立即生效。

## 授权协议
[MIT License](LICENSE) | Copyright © yuebi
