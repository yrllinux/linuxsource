1、本单元测试分为两个部分，一是MCU-MT，另一是MCU-MCU。
2、对MCU-MT部分，直接在一台PC上即可实现测试。
3、对MCU-MCU部分，需将SMCU模块的配置文件maunitsmcu.ini配置为某公共GK，如172.16.191.79，	
   同时将maunitcom.h头文件中的宏LOCAL_ADDR改为另一台即将放置本模块的PC的地址，如      172.16.5.55。然后对SMCU模块进行编译，将编译出来的smcu.exe文件放在上述另一台PC上，并运    行。
4、回到本地，进行正常测试