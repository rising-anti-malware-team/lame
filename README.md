
<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

* [瑞星反病毒引擎SDK](#瑞星反病毒引擎sdk)
	* [**语言绑定**](#语言绑定)
		* [C/C++](#cc)
		* [JAVA](#java)
		* [Python](#python)
		* [dotNet](#dotnet)
		* [Nodejs](#nodejs)
		* [go](#go)
	* [引擎优势](#引擎优势)
		* [丰富全面的平台支持](#丰富全面的平台支持)
		* [强劲的文件深度分析/提取/解码能力](#强劲的文件深度分析提取解码能力)
			* [支持的压缩包（自解压包）](#支持的压缩包自解压包)
			* [支持的安装包:](#支持的安装包)
			* [支持的电子邮件和邮箱文件](#支持的电子邮件和邮箱文件)
			* [支持的文档/多媒体文件](#支持的文档多媒体文件)
			* [支持的磁盘/镜像/文件系统](#支持的磁盘镜像文件系统)
			* [支持的脚本发布包：](#支持的脚本发布包)
			* [支持的其他文件类型：](#支持的其他文件类型)
		* [丰富的脱壳能力](#丰富的脱壳能力)
		* [全面的模拟执行能力（反病毒虚拟机）](#全面的模拟执行能力反病毒虚拟机)
		* [丰富多样、针对性强的恶意软件识别技术](#丰富多样-针对性强的恶意软件识别技术)

<!-- /code_chunk_output -->

# 瑞星反病毒引擎SDK
---
## **语言绑定**
目前已经适配的语言：
- C/C++
- JAVA
- Python(CPytohn)
- dotNet
- Nodejs
- go

### C/C++
```c
EXTERN_C void*  lame_open_vdb(const char* vlibf);
EXTERN_C void   lame_close_vdb(void* vdb);

EXTERN_C void*  lame_create(void* vdb);
EXTERN_C void   lame_destroy(void* lame);
EXTERN_C void*  lame_fork(void* lame);

EXTERN_C long   lame_param_set(void* lame , const char*  param);
EXTERN_C long   lame_init(void* lame);
EXTERN_C long   lame_scan_file(void* lame , const char* fname , lame_scan_result* pResult);
EXTERN_C long   lame_scan_mem(void* lame , uint8_t* data , uint32_t size , lame_scan_result* pResult);


EXTERN_C long   lame_scan_file_with_callback(void* lame , const char* fname , lame_callback cb , void* user_data);
EXTERN_C long   lame_scan_mem_with_callback(void* lame , uint8_t* data , uint32_t size , lame_callback cb , void* user_data);

EXTERN_C long   lame_get_version(lame_info* info);
EXTERN_C long   lame_get_licence_info(rx_licence_info* info);
```
### JAVA
```java
public interface ScanFeedback {
	public abstract long DtEnterFile(String file_name, long depth, Object usr_data);
	public abstract void DtLeaveFile(String file_name, long depth, Object usr_data, long l);
	public abstract long DtAlarm(String file_name, ScanResult sr, Object usr_data);
}

public class Lame extends RavEngine {
	public long OpenVdb(String vdbf);
	public void CloseVdb();
	public boolean Load(long lib);
	public boolean Set(String param);
	public void Unload();
	public LicenceInfo GetLicence();
	public LameInfo GetVersion();
	public ScanResult ScanFile(String fname);
	public ScanResult ScanMem(byte [] data);
	public boolean ScanFileWithCallback(String fname,ScanFeedback cb, Object usrdata);
	public boolean ScanMemWithCallback(byte [] data,ScanFeedback cb, Object usrdata);
}
```
### Python
```python
class Lame(LameBase):
	def ScanFile(self,fname):
    	pass
	def ScanMem(self, data):
    	pass
	def Clone(self):
    	pass
class LameWithFeedback:
	def SetCallack(self,cb):
        self._callback = cb
    ...
  
```
### dotNet
```c
public class Lame : LameBase
{
	public LameScanResult ScanFile(string sFile);
	public LameScanResult ScanMem(byte[] bytes);
	public object Clone();
}

public delegate void ScanInternalFileEvent(string fname, LameScanResult result);

public class LameWithEvent : LameBase {
	public void ScanFile(string sFile);
	public void ScanMem(byte[] bytes);
	private void ScanInternalFile(string fname, IntPtr resp, IntPtr zero);
	public object Clone();
	private LameScanResult FetchResult(IntPtr resp);
}

public class VirusLib
{
	public bool lame_open_vdb(string vdbf);
	public void lame_close_vdb();
	public IntPtr vdb_handle;
}
```

### Nodejs
```javascript
lame.prototype.GetVersionInfo();
lame.prototype.GetLicenceInfo();

lame.prototype.OpenVdb(vlib);
lame.prototype.CloseVdb();

lame.prototype.ExtractFile(filename, password, enter_file, leave_file, userdata);
lame.prototype.Extract_GetSize(handle);
lame.prototype.Extract_Seek(handle, offset, method);
lame.prototype.Extract_Tell(handle);
lame.prototype.Extract_Read(handle, size);

lame.prototype.Load();
lame.prototype.SetParameters(param);
lame.prototype.Unload();
lame.prototype.ScanFile(path);
lame.prototype.ScanMem(data, size);
lame.prototype.ScanFileWithCallback(path, enter_file, leave_file, alarm, userdata);
lame.prototype.ScanMemWithCallback(data, size, enter_file, leave_file, alarm, userdata);
```

### go
```go
func (this *Lame) OpenVdb(vlib string) uintptr
func (this *Lame) CloseVdb()
func (this *Lame) Create(vdb uintptr) uintptr
func (this *Lame) Destory()
func (this *Lame) Init() bool
func (this *Lame) SetParam(param string) bool
func (this *Lame) ScanFile(path string) *DetectResult
func (this *Lame) ScanMem(data [] byte, size int) *DetectResult
func (this *Lame) ScanFileWithCallback(path string, cb ScanCallBack, userdata interface{}) bool
func (this *Lame) ScanMemWithCallback(data [] byte, size int, cb ScanCallBack, userdata interface{}) bool
func (this *Lame) GetVersion() *LameVersion
func (this *Lame) GetLicenceInfo() *LicenceInfo
func (this *Lame) ExtractFile(filename string, password string, cb ExtractCallBack, userdata interface{}) bool
func (this *Lame) Extract_GetSize(handle uintptr) int
func (this *Lame) Extract_Seek(handle uintptr, offset int, method int32) bool
func (this *Lame) Extract_Tell(handle uintptr) int32
func (this *Lame) Extract_Read(handle uintptr, buf [] byte, size uint32) uint32
```

---
## 引擎优势
### 丰富全面的平台支持
纯C++实现，无任何内联汇编，几乎可在Windows及所有符合Posix标准操作系统和CPU上编译运行。已经稳定应用于：
- Windows + x86/x64
- Linux + x86/x64
- Linux + MIPS 32/64 + 大小字节序
- Linux + ARM 32/64
- Unix + PowerPC
- 国产操作系统（Linux家族） + 国产CPU（x86/MIPS）

### 强劲的文件深度分析/提取/解码能力

支持多种压缩包、自解压包、符合文档、媒体文件、加密脚本等。

#### 支持的压缩包（自解压包）

||||||
|---|---|---|---|---|
|  7z  |  ace  |  arc   |  arj  | bz2 |
|  cab  | cabset |  gz/gzip  | lha  |  paquet  
| rar  | rar5 |sea |  tar  |  xz  |
|  zip  | apk   | jar  |  zipx   |  zoo |
|  OLE 

#### 支持的安装包:
- Instyler
- NSIS
- Inno(all version)
- SFactory
- SmartInstaller
- Wise Installer
- SIS,塞班（Symbian)系统软件安装包
- deb,Debian系Linux的软件安装包

#### 支持的电子邮件和邮箱文件
- EML/MSG, 基于文本的电子邮件存档
- MSO, 基于微软复合文档的电子邮件存档
- FixMail Mailbox（低版本）
- Outlook 4/5 Mailbox
- Outlook 2003+ (.pst)
- Netscape
      
#### 支持的文档/多媒体文件
提取文档中嵌入的其它资源，如：宏、脚本、可执行程序等
- Open Office XML  *.xml,*.odp,*.odt,*ods
- Microsoft Office Binary(v2003-), *.doc, *.xls, *.ppt, ...
- Microsoft Office XML(v2003+), *.docx ,*.docm, *.xlsx,*.xlsm,*.xlsb *.pptx,*.pptm, ...
- Adobe PDF, 包括 .pdf 和 基于XML的 .xdp
- Flash SWF, 支持 defalte 和 lzma 两种压缩算法（提取SWF中嵌入的PE文件）
- CHM，Windows帮助文件
- RTF
     
#### 支持的磁盘/镜像/文件系统
提取磁盘内的分区 以及 分区内 存放的文件。
- 虚拟化磁盘
    - VMDK（vmware)
    - VHD（microsoft）
    - QCOW（kvm/quemu）
    *目前均未支持快照*
- MBR/GPT分区表的磁盘镜像
- NTFS文件系统
- DMG,MacOS下的磁盘镜像文件
- CramFS,专门针对闪存设计的只读压缩的文件系统
- CPIO,由linux cpio命令创建的归档文件
- ISO

#### 支持的脚本发布包：
- AutoIt
- QuickBatch
- Gentee
- 易语言(老版本)

#### 支持的其他文件类型：
- VBE/JSE：微软的加密VBS脚本
- HTML Data URI
- UUE
- FAS, AutoCAD脚本快速加载文件
- Windows PE，提取资源中的文件
    
### 丰富的脱壳能力
- 针对Windows PE文件:
acprotect, armadillo, aspack, aspr, bitarts, crypt, dbpe, enigma, execrypt, exeshield, expressor, orien, packman, pcguard, pearmor, pec2, pespin, petite, pex, pklite, rlpack, svkp, upx, yoda, molebox, ahpack, cexe, depack, exe32pack, exefog, ezip, FSG, himeys, krypton, mew, nakepack, npack, nprotect, nspack, NTkrnl, obsidium, pcshrinker, pecrypt, pep, pedinisher, pelocknt, pestil, polycrypt, punisher, sdprotect, slvcodeprotector, themida, vprotect, yzpack, pe-ninja
- 针对Linux ELF文件：
upx (all targets，实验性质）

### 全面的模拟执行能力（反病毒虚拟机）

- **MS-DOS + BIOS + 8086 模拟器**  
该模拟器用于虚拟执行 MBR，DOS-COM，DOS-EXE等古老的程序。
    
- **x86平台32/64位混合式Windows模拟器**  
该模拟器用于应对现代恶意软件，支持32位和64位程序。同时还支持以32位方式执行64位程序，或以64位方式执行32位程序，来应对一些极端情况。

- **JavaScript沙盒**  
用于虚拟执行JScript/JavaScript。
  
### 丰富多样、针对性强的恶意软件识别技术
- 简单特征码匹配技术   
采用逻辑位置 + 偏移 + 特征串的方式识别病毒。
- 词法分析技术  
针对VBS/JS/PHP/JAVA/C等文本文件进行词法分析后，在语义层面对恶意脚本进行识别。
- 自定义查杀毒逻辑  
分析员通过编写查杀病毒的代码，可查杀超级复杂的数量极大的感染型病毒恶意软件家族。
- 模拟执行跟踪技术  
又称反病毒虚拟机，将目标文件（PE文件/JS脚本）在模拟器中执行，跟踪其执行时执行的指令、修改的内存、调用的函数、产生的副作用，记录日志，同时驱动特征码匹配、自定义查杀毒逻辑。
- 智能特征码匹配技术  
一种MPM全文搜索技术，类似正则表达式的病毒特征描述方式但在恶意软件识别上扩充了很多特殊的能力，适用于内容匹配和模式匹配。
- 多维度（内容平面）交叉判定技术  
将文件通过不同的处理方式，分离成不同维度的内容平面，并在这些内容平面中进行特征匹配，通过不同平面中特征匹配情况来综合判定目标文件是否为恶意软件。例如：敏感函数名 在原始内容中无法被匹配到，但在模拟执行后的函数调用日志中存在，可以推断出该敏感函数被人为加密或隐藏了，这便是一种恶意软件的逻辑特征。
- 图像匹配技术  
该技术专门针对“图标诱导”类的WinPE 恶意软件，例如，将自身的程序图标设置为MsOffice/PDF等文档图标来诱导用户双击执行的恶意软件。
- 代码基因技术  
针对不同类型的文件，提取这些文件的主干（框架）内容，并计算其指纹（我们称为代码基因），其可用于对抗轻微变型/混淆的恶意程序、恶意脚本、恶意宏。该技术支持 机器自动化 处理，无需人工提取，效率极高。
- 关键内容检验技术  
针对 Windows PE 文件（包含dotNet）,划定约15个关键内容区域，对这些区域按照特定算法计算指纹，并匹配恶意指纹库。该技术支持机器自动化处理，无需人工提取，效率高。
- 数字签名校验及匹配技术  
将目标文件的数字签名同已经收录的恶意数字签名进行比对。
- 简单哈希匹配技术  
文件尺寸＋文件内容哈希值。
- YARA技术  
融合了准行业标准恶意软件表述语言。
- 人工智能技术  
针对WinPE文件和Flash文件的人工智能恶意软件识别技术——RDM+。RDM+是瑞星自主研发的人工智能引擎，目前以“云脑”和“嵌入”两种形式存在于反病毒引擎中。“云脑”模式拥有庞大的预测模型、极快的误报处置速度、客户端无感的周期性更新，其预测过程融入在传统云引擎交互过程中，为使用者带来巨大的“零日”恶意软件识别能力的提升。“嵌入”模式则采用更为小尺寸的模型，以实现客户端部署，它为引擎提供无网络环境下的基于人工智能的恶意软件识别能力，由于模型较小，其在误报概率上会略微高于“云脑”模式。

7. 丰富的 文件格式识别能力
    - BOOT   ：  MBR引导扇区
    - MZ   ：  DOS可执行程序(exe)
    - PE   ：  Windows可执行程序
    - DEX   ：  安卓应用主程序
    - ZIP   ：  ZIP压缩包
    - CAB   ：  Microsoft制订的压缩包格式
    - ARJ   ：  ARJ压缩包
    - RAR   ：  RAR压缩包
    - RAR5   ：  RAR 5.0 压缩包
    - ARC   ：  ARC压缩包
    - ZOO   ：  Unix系统中使用旧的压缩格式
    - LZH   ：  用LHARC压缩的文件
    - TAR   ：  UNIX合并文件
    - GZIP  ：  gz 压缩包
    - UUE   ：  UUE 编码文件
    - ACE   ：  Ace 压缩档案格式
    - BZ2   ：  BZ2 压缩包
    - CHM   ：  Windows帮助文件（经过编译打包的HTML文件集合）
    - 7Z   ：  7Zip 压缩包
    - XZ   ：  XZ 压缩包
    - SIS   ：  塞班系统的软件安装包
    - APK   ：  安卓应用发布包
    - IPA   ：  苹果IOS软件发布包
    - ZIPEXE   ：  ZIP 自解压包
    - CABEXE   ：  CAB 自解压包
    - ARJEXE   ：  ARJ 自解压包
    - RAREXE   ：  RAR 自解压包
    - RAR5EXE   ：  RAR5 自解压包
    - ARCEXE   ：  ARC 自解压包
    - ZOOEXE   ：  ZOO 自解压包
    - LZHEXE   ：  LZH 自解压包
    - ZIPNE   ：  ZIP 自解压包
    - ZIPPE   ：  ZIP 自解压包
    - ACEEXE   ：  ACE 自解压包
    - NSIS   ：  NSIS 安装包
    - INNO   ：  INNO安装包（InnoSetup）
    - 7ZEXE   ：  7Zip自解压包
    - CABSETEXE   ：  CABSET自解压包
    - SEAEXE   ：  SEA 自解压包
    - WISE   ：  WISE安装包
    - WISEA   ：  WISE安装包
    - AUTOIT   ：  AUTOIT脚本编译后的可执行文件
    - ELANG   ：  易语言编译的程序
    - GENTEE   ：  GENTEE脚本编译后的程序
    - PAQUET   ：  PaquetBuilder产生的安装包
    - INSTYLER   ：  instyler SmartSetup制作的安装包
    - SFACTORY   ：  Setup Factory制作的安装包
    - QUICKBATCH   QuickBatch制作的可执行程序
    - DOC   ：  Microsoft Windows复合文档
    - MDB   ：  Microsoft Access数据库
    - HTA   ：  Windows HTML Application文件
    - REG   ：  Windows注册表文件
    - VBSENC   ：  Windows加密VBScript文件
    - JSENC   ：  Windows加密JScript文件
    - XOFFICE   ：  Microsoft Office 2003+ 文档
    - JPEG   ：  JPEG/JPG图片文件
    - SWF   ：  Flash 文件
    - SWC   ：  Flash 文件（带压缩的）
    - PDF   ：  Adobe PDF文档
    - MP3   ：  MP3 音频文件
    - LNK   ：  Windows 快捷方式文件
    - TTF   ：  True Type Font 字体文件
    - ELF   ：  Linux 可执行文件
    - CLASS   ：  Java 子节码文件
    - BMP   ：  Windows Bitmap位图文件
    - GIF   ：  GIF图片文件
    - PNG   ：  PNG图片文件
    - ICO   ：  图标文件
    - CDR   ：  CorelDraw 产生的文件
    - MIDI   ：  MIDI音频文件
    - MPEG   ：  MPEG视频文件
    - HLP   ：  Windows帮助文件
    - WPG   ：  Word Perfect产生的矢量图文件
    - CPT   ：  FineReport产生的报表文件
    - ISU   ：  安装程序产生的文件
    - WBM   ：  Macromedia Fireworks File
    - CAT   ：  Windows数字签名编录文件
    - MBX   ：  Microsoft Outlook保存电子邮件格式；Eudora邮箱
    - WAV   ：  Windows波形声形
    - WDL   ：  北京华康公司开发的一种电子读物文件格式
    - DBF   ：  dBASE文件
    - REALPLER   ：  RealPlayer媒体文件
    - RPM   ：  RedHat包管理器包（用于Linux）
    - IDB   ：  MSDev中间层文件
    - PDB   ：  Windows调试符号文件
    - AU3SCRIPT   ：  AutoIt3 脚本
    - DOSCOM   ：  DOS COM文件
    - TEXT   ：  文本文件
    - VBS   ：  VBScript文件
    - HTML   ：  网页文件
    - BAT   ：  批处理文件
    - JS   ：  JScript文件
    - NS   ：  NSIS脚本文件
    - EML   ：  电子邮件文件
    - IRC   ：  IRC脚本
    - PERL   ：  Perl脚本
    - SHELL   ：  Bash shell脚本
    - VHD   ：  微软虚拟磁盘
    - VMDK   ：  VMWare虚拟磁盘
    - RTF   ：  RTF文档
    - PST   ：  Outlook 2003 + 邮箱文件
    - UCS16LE   ：  Unicode 16LE File
    - OUTLOOKEX_V5   Outlook Express 5 邮箱文件
    - OUTLOOKEX_V4   Outlook Express 4 邮箱文件
    - FOXMAIL_V3   Foxmail 3 邮箱文件
    - NETSCAPE_V4   Netscape 4 邮箱文件
    - BASE64   ：  Base64 编码后的PE文件
    - NWS   ：  Windows Live Mail的( WLM)创建新闻组的文件
    - MHT   ：  聚合HTML文档
    - MSG   ：  邮件
    - BOX   ：  邮箱文件
    - IND   ：  邮箱索引文件
    - JAR   ：  Java归档文件
    - NTFS   ：  NTFS文件系统
    - FAT   ：  FAT文件系统
    - LOTUSPIC   ：  Lotus使用的图片文件
    - VBVDI   ：  VirtualBox 虚拟磁盘
    - SQLITE   ：  SQLite 数据库文件
    - LIBA   ：  lib文件
    - TIFF   ：  TIFF图片文件
    - FAS   ：  AutoCAD FAS文件
    - LSP   ：  AutoCAD LSP文件
    - XDP   ：  XML表达的PDF文件
    - WSF   ：  XML表达的Windows脚本文件
    - EOT   ：  Embedded Open Type 字体文件
    - ASF   ：  高级串流媒体文件
    - RIFF   ：  资源互换文件格式（ResourcesInterchange FileFormat)，媒体文件
    - QTFF   ：  QuickTime媒体文件
    - TORRENT   ：  BT种子文件
    - WMF   ：  Windows WMF 图像文件
    - JSENC   ：  加密JS脚本
    - COM   ：  DOS-COM文件
    - INI   ：  Windows的系统配置文件
    - HTM   ：  HTML网页文件
    - PPT   ：  MsOffice PowerPoint幻灯片文件
    - PHP   ：  PHP网页文件
    - MACH_O   ：  MacOS可执行文件
    - FLV   ：  FLASH VIDEO 流媒体文件
    - IPA   ：  苹果IOS应用文件
    - SMARTINSTALLER  Smart Installer制作的安装包文件
    - MSO   ：  微软ActiveMine文件
    - PS   ：  Microsoft Powershell 脚本文件
    - EPS   ：  Adobe PostScript 文件
    - WSF   ：  含有可扩展标记语言 (XML)代码的Windows 脚本文件
    - INF   ：  系统自动化脚本
    - MSCFB   ：  Microsoft Compound File Binary (CFB)
    - ODF   ：  OpenOffice文件
    - OXML   ：  OpenOffice文件（xml)
    - ISO   ：  符合ISO 9660标准的光盘镜像文件格式
    - DEB   ：  Linux软件包(debian)
    - CPIO   ：  Linux文件包（cpio命令创建）
    - CRAMFS   ：  专门针对闪存设计的只读压缩的文件系统