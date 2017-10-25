# Marvell WiFi
**Marvell WiFi** 是一款运行在RT-Thread实时操作系统上的sdio wifi驱动软件。

# Hardware Requirements
ROM: 512KB或以上  
RAM: 128KB或以上  
带SDIO 2.0 Host控制器  

# WiFi Chips Support
88w8782  
88w8801  
![88w8782.png][1]

# Features
1. STA, UAP模式(可共存，但无路由)
2. 认证方式: OPEN/WPA2-PSK
3. 自动重连
4. 低功耗模式
5. 速率高(stm32f407驱动可达2MB/s)

# Compiler
MDK5(注意: 由于本驱动含有大量gcc特性，需要在编译器C/C++选项下Misc Controls中添加 --gnu):  
![mdk5(--gnu).png][2]

# Components Dependence
1. sdio驱动框架(RT_USING_SDIO)
2. Lwip协议栈(RT_USING_LWIP)
3. dfs虚拟文件系统(RT_USING_DFS)
4. libc库(RT_USING_LIBC)     
5. rt_hw_us_delay(请在bsp中自行实现)

# Adding Method
利用RT-Thread官方提供的env工具获取pakage并生成工程  
或手动下载pakage添加到现有工程目录下，在rt_config.h中开启宏并用scons工具重新生成:

    #define PKG_USING_MARVELLWIFI
    #define MARVELLWIFI_USING_STA

# Initialize
第一步：硬复位wifi芯片，可通过复位电路或GPIO控制实现。  
第二步：若使用组件初始化

    #define RT_USING_COMPONENTS_INIT

则只需在组件初始化之前初始化好sdio框架和sdio主机端驱动

    rt_mmcsd_core_init();
    stm32f4xx_sdio_init();
    mmcsd_delay_ms(200);

    rt_components_init();

否则请先手动初始化本驱动所依赖的其他组件，再调用

    mwifi_system_init();

注意，在第一次使用前，请在文件系统中新建目录：/mrvl并将本驱动FwImage文件夹下的固件放到此目录下。
![firmware.png][3]

驱动加载时需要为芯片烧写固件，若加载成功可以在终端命令行中看到如下信息：
![initialize.png][4]

# Usage
提供msh下命令，键入

    mwifi

可查看用法：  
![usage.png][5]

## Example

    mwifi mlan0 connect SSID -k PASSWORD    //连接SSID，密码PASSWORD
    mwifi mlan0 disconnect                  //断开连接
    mwifi mlan0 pwrsave -e                  //进入低功耗模式
    mwifi mlan0 scan                        //扫描附近的热点

更多用法，请参考mwifi.c

# Attention
1. 使用时注意调整任务优先级：tcpip > MOAL_WORKQ > sdio_irq = etx
2. 若STA需要自动重连功能：请开启宏(MARVELLWIFI_STA_REASSOCIATION)

***

如果在使用过程中有任何疑问，欢迎发起issues或email到这个地址：`jianb1995@hotmail.com`


  [1]: image/88w8782.png "88w8782.png"
  [2]: image/mdk5(--gnu).png "mdk5(--gnu).png"
  [3]: image/firmware.png "firmware.png"
  [4]: image/initialize.png "initialize.png"
  [5]: image/usage.png "usage.png"
