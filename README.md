# ECC-Encryptor

### •&emsp;一个用C++实现的简易ECC加密器，帮助你更进一步学习基于椭圆曲线的非对称加密原理
&emsp;&emsp;  
&emsp;&emsp;在网络上搜到的所谓“ECC加密实现”，多数都基于教学目的，使用阶和基点坐标值极小的椭圆曲线，这种所谓的实现仅能用于原理展示，
实际在加密中的应用是十分危险的（如当阶小于2^8时，使用酷睿i7-7700 CPU，在无相关加速的情况下也可以做到数秒内推出私钥），不能
直接用于生产环境中。同时基于其修改也是相对困难的：多数原理展示用代码都使用了int类型，其上限仅为2^31-1，而安全的椭圆曲线基本
都要求阶数高于2^128（如本加密器，阶为115792089237316195423570985008687907852837564279074904382605163141518161494337，
其数量级在2^256，C++中任何基本类型都无法对其进行储存与运算），因此我从头基于ECC加密原理，实现了这个加密器.

&emsp;&emsp;加密器使用与比特币相同的椭圆曲线参数（参数下方给出），安全的随机数由x86 CPU提供的Rdrand指令（不可用时，会自动切换至
libsodium提供随机数）生成安全的随机数，基本保证可以投入实际应用中，保证通信的信息安全。Rdrand生成的随机数安全性完全由您的
x86 CPU供应商保证（通常是Intel或AMD），如您不信任您的供应商，请自行切换随机数生成方法，相关方法于***RdRand.h***中可以修改！

&emsp;&emsp;  

# 使用方法
### 1. 引导式操作（开箱即用）
&emsp;&emsp;**- 下载Release版本，后直接双击ECC.exe运行即可，提供保姆般贴心的引导！**~~（这都不会用的话建议回炉重造了）~~  
&emsp;&emsp;  
&emsp;&emsp;***+ 请注意！！！引导式操作为保证安全，密钥对仅会保留在内存中，并在程序结束运行后自动永远清除！！！***  
&emsp;&emsp;***+ 如果您还需要保留密钥对进行后续的解密操作，请自行将屏幕上显示的密钥对复制粘贴至一个安全的区域！***  

&emsp;&emsp; 

### 2. 带额外参数使用（高级）
&emsp;&emsp;**- 下载Release版本，在ECC.exe所在位置打开命令行，输入ECC -? 或 ECC -help查看使用帮助**  
&emsp;&emsp;  
&emsp;&emsp;**- 目前提供3种基本操作（被<>包围的需要替换为自己的参数）：**  
&emsp;&emsp;&emsp;&emsp;-G：生成一个密钥对  
&emsp;&emsp;&emsp;&emsp;-Key <公钥值> -M <明文消息>：使用输入的公钥值加密输入的明文消息  
&emsp;&emsp;&emsp;&emsp;-Key <私钥值> -EM <密文消息>：使用输入的私钥值解密输入的密文消息   

&emsp;&emsp;***+ 这是唯一复用密钥对的方法，可以用其解密出传输过程中用到的AES密钥，对之前的消息进行解密***  

&emsp;&emsp;  
&emsp;&emsp;  

# 如何从源代码编译？    
### 1. 下载相关的依赖库（GMP && LibSodium）
        
&emsp;&emsp;**你可以选择手动添加这些库，确保可以顺利编译即可，当然，微软提供的vcpkg会帮你更便捷的完成这一任务：**  

&emsp;&emsp;  

&emsp;&emsp;**在powershell中依次执行如下指令：**
```
  # 选好位置，安装vcpkg:
  > cd 要安装的位置
  > git clone https://github.com/microsoft/vcpkg.git
  > ./bootstrap-vcpkg.bat
  
  # 安装相应库：
  > .\vcpkg install gmp:x64-windows (32位请去掉冒号及其后内容)
  > .\vcpkg install libsodium:x64-windows (32位请去掉冒号及其后内容)
  
  # 绑定Visual Studio（请确保已经安装好VS，并且安装了相应的英文语言包）
  # 注意！！！必须安装英文语言包！！！否则vcpkg无法识别到你的vs
  > .\vcpkg integrate install
```
&emsp;&emsp;**之后重启Visual Studio即可！**

&emsp;&emsp;（更多vcpkg的使用方法这里不再过多阐述，如有需要请自行百度。谷歌，Stack Overflow，~~CSDN~~）    

&emsp;&emsp;  

### 2. 编译运行
        
&emsp;&emsp;**请使用VS 20022及以上版本编译，否则会在编译时报“不支持的版本”错误！**  

&emsp;&emsp;**确认无误后，直接双击打开ECC.sln文件，即可进行相关修改和编译运行工作**

&emsp;&emsp; 

# 默认椭圆曲线参数

&emsp;&emsp;**(参数与BTC生成钱包时所用的椭圆曲线参数一致)**  
&emsp;&emsp;  
&emsp;&emsp;a = 0  
&emsp;&emsp;b = 7  
&emsp;&emsp;p = 115792089237316195423570985008687907853269984665640564039457584007908834671663 (取模数)  
&emsp;&emsp;n = 115792089237316195423570985008687907852837564279074904382605163141518161494337 (基点G的阶)  
&emsp;&emsp;G = (55066263022277343669578718895168534326250603453777594175500187360389116729240,  
&emsp;&emsp;&emsp;&emsp;32670510020758816978083085130507043184471273380659243275938904335757337482424)（基点G的坐标）
