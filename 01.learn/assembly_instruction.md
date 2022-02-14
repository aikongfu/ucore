# 汇编语言常用指令

JNC
意思是CF=0，则跳转，表示的是AX大于或等于0的时候转移到EXIT处，所以JNC也可以看成是加法没有进位，减法没有借位的时候转移。
在汇编语言中，用助记符代替机器指令的操作码，用地址符号或标号代替指令或操作数的地址。在不同的设备中，汇编语言对应着不同的机器语言指令集，通过汇编过程转换成机器指令。特定的汇编语言和特定的机器语言指令集是一一对应的，不同平台之间不可直接移植。

LJMP 的范围是16位地址，也就是64K的空间内，占用存储空间3个字节，执行周期24个时钟周期。 


inc
自增指令inc(类似c里的++i)

cmpl $0, %eax
cmpl指令将两个操作数相减，但计算结果并不保存，只是根据计算结果改变eflags寄存器中的标志位。如果两个操作数相等，则计算结果为0，eflags中的ZF位置1。
je是一个条件跳转指令，它检查eflags中的ZF位，ZF位为1则发生跳转，ZF位为0则不跳转。
比较指令和条件跳转指令是配合使用的，前者改变标志位，后者根据标志位作出判断。

leal指令用于加载有效地址（loadeffective address）。
leal指令的目的操作数必须是寄存器。实际上leal指令有时用于与加载地址无关的场景。

一、数据传输指令
　　它们在存贮器和寄存器、寄存器和输入输出端口之间传送数据.
　　1. 通用数据传送指令.
　　　　MOV　　传送字或字节.
　　　　MOVSX　先符号扩展,再传送.
　　　　MOVZX　先零扩展,再传送.
　　　　PUSH　　把字压入堆栈.
　　　　POP　　把字弹出堆栈.
　　　　PUSHA　把AX,CX,DX,BX,SP,BP,SI,DI依次压入堆栈.
　　　　POPA　　把DI,SI,BP,SP,BX,DX,CX,AX依次弹出堆栈.
　　　　PUSHAD　把EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI依次压入堆栈.
　　　　POPAD　把EDI,ESI,EBP,ESP,EBX,EDX,ECX,EAX依次弹出堆栈.
　　　　BSWAP　交换32位寄存器里字节的顺序
　　　　XCHG　　交换字或字节.( 至少有一个操作数为寄存器,段寄存器不可作为操作数)
　　　　CMPXCHG 比较并交换操作数.( 第二个操作数必须为累加器AL/AX/EAX )
　　　　XADD　　先交换再累加.( 结果在第一个操作数里 )
　　　　XLAT　　字节查表转换.
　　　　　　　　── BX 指向一张 256 字节的表的起点, AL 为表的索引值 (0-255,即
　　　　　　　　0-FFH); 返回 AL 为查表结果. ( [BX+AL]->AL )
　　2. 输入输出端口传送指令.
　　　　IN　　　I/O端口输入. ( 语法: IN 累加器, {端口号│DX} )
　　　　OUT　　I/O端口输出. ( 语法: OUT {端口号│DX},累加器 )
　　　　　输入输出端口由立即方式指定时, 其范围是 0-255; 由寄存器 DX 指定时,
　　　　　其范围是 0-65535.
　　3. 目的地址传送指令.
　　　　LEA　　装入有效地址.
　　　　　例: LEA DX,string　;把偏移地址存到DX.
　　　　LDS　　传送目标指针,把指针内容装入DS.
　　　　　例: LDS SI,string　;把段地址:偏移地址存到DS:SI.
　　　　LES　　传送目标指针,把指针内容装入ES.
　　　　　例: LES DI,string　;把段地址:偏移地址存到ES:DI.
　　　　LFS　　传送目标指针,把指针内容装入FS.
　　　　　例: LFS DI,string　;把段地址:偏移地址存到FS:DI.
　　　　LGS　　传送目标指针,把指针内容装入GS.
　　　　　例: LGS DI,string　;把段地址:偏移地址存到GS:DI.
　　　　LSS　　传送目标指针,把指针内容装入SS.
　　　　　例: LSS DI,string　;把段地址:偏移地址存到SS:DI.
　　4. 标志传送指令.
　　　　LAHF　　标志寄存器传送,把标志装入AH.
　　　　SAHF　　标志寄存器传送,把AH内容装入标志寄存器.
　　　　PUSHF　标志入栈.
　　　　POPF　　标志出栈.
　　　　PUSHD　32位标志入栈.
　　　　POPD　　32位标志出栈.
二、算术运算指令

　　　　ADD　　加法.
　　　　ADC　　带进位加法.
　　　　INC　　加 1.
　　　　AAA　　加法的ASCII码调整.
　　　　DAA　　加法的十进制调整.
　　　　SUB　　减法.
　　　　SBB　　带借位减法.
　　　　DEC　　减 1.
　　　　NEC　　求反(以 0 减之).
　　　　CMP　　比较.(两操作数作减法,仅修改标志位,不回送结果).
　　　　AAS　　减法的ASCII码调整.
　　　　DAS　　减法的十进制调整.
　　　　MUL　　无符号乘法.
　　　　IMUL　　整数乘法.
　　　　　以上两条,结果回送AH和AL(字节运算),或DX和AX(字运算),
　　　　AAM　　乘法的ASCII码调整.
　　　　DIV　　无符号除法.
　　　　IDIV　　整数除法.
　　　　　以上两条,结果回送:
　　　　　　　商回送AL,余数回送AH, (字节运算);
　　　　　或　商回送AX,余数回送DX, (字运算).
　　　　AAD　　除法的ASCII码调整.
　　　　CBW　　字节转换为字. (把AL中字节的符号扩展到AH中去)
　　　　CWD　　字转换为双字. (把AX中的字的符号扩展到DX中去)
　　　　CWDE　　字转换为双字. (把AX中的字符号扩展到EAX中去)
　　　　CDQ　　双字扩展.　　(把EAX中的字的符号扩展到EDX中去)

三、逻辑运算指令

　　　  AND　　与运算.
　　　　OR　　　或运算.
　　　　XOR　　异或运算.
　　　　NOT　　取反.
　　　　TEST　　测试.(两操作数作与运算,仅修改标志位,不回送结果).
　　　　SHL　　逻辑左移.
　　　　SAL　　算术左移.(=SHL)
　　　　SHR　　逻辑右移.
　　　　SAR　　算术右移.(=SHR)
　　　　ROL　　循环左移.
　　　　ROR　　循环右移.
　　　　RCL　　通过进位的循环左移.
　　　　RCR　　通过进位的循环右移.
　　　　　以上八种移位指令,其移位次数可达255次.
　　　　　　　移位一次时, 可直接用操作码.　如 SHL AX,1.
　　　　　　　移位>1次时, 则由寄存器CL给出移位次数.
　　　　　　　　如　MOV CL,04
　　　　　　　　　　SHL AX,CL

四、串指令

　DS:SI　源串段寄存器　:源串变址.
　　　　　　ES:DI　目标串段寄存器:目标串变址.
　　　　　　CX　　　重复次数计数器.
　　　　　　AL/AX　扫描值.
　　　　　　D标志　0表示重复操作中SI和DI应自动增量; 1表示应自动减量.
　　　　　　Z标志　用来控制扫描或比较操作的结束.
　　　　MOVS　　串传送.
　　　　　　( MOVSB　传送字符.　　MOVSW　传送字.　　MOVSD　传送双字. )
　　　　CMPS　　串比较.
　　　　　　( CMPSB　比较字符.　　CMPSW　比较字. )
　　　　SCAS　　串扫描.
　　　　　　把AL或AX的内容与目标串作比较,比较结果反映在标志位.
　　　　LODS　　装入串.
　　　　　　把源串中的元素(字或字节)逐一装入AL或AX中.
　　　　　　( LODSB　传送字符.　　LODSW　传送字.　　LODSD　传送双字. )
　　　　STOS　　保存串.
　　　　　　是LODS的逆过程.
　　　　REP　　　　　　当CX/ECX>0时重复.
　　　　REPE/REPZ　　　当ZF=1或比较结果相等,且CX/ECX>0时重复.
　　　　REPNE/REPNZ　　当ZF=0或比较结果不相等,且CX/ECX>0时重复.
　　　　REPC　　　　　当CF=1且CX/ECX>0时重复.
　　　　REPNC　　　　　当CF=0且CX/ECX>0时重复.

五、程序转移指令

　　　1>无条件转移指令 (长转移)
　　　　JMP　　无条件转移指令
　　　　CALL　　过程调用
　　　　RET/RETF过程返回.
　　2>条件转移指令 (短转移,-128到+127的距离内)
　　　　( 当且仅当(SF XOR OF)=1时,OP1<OP2 )
　　　　JA/JNBE 不小于或不等于时转移.
　　　　JAE/JNB 大于或等于转移.
　　　　JB/JNAE 小于转移.
　　　　JBE/JNA 小于或等于转移.
　　　　　以上四条,测试无符号整数运算的结果(标志C和Z).
　　　　JG/JNLE 大于转移.
　　　　JGE/JNL 大于或等于转移.
　　　　JL/JNGE 小于转移.
　　　　JLE/JNG 小于或等于转移.
　　　　　以上四条,测试带符号整数运算的结果(标志S,O和Z).
　　　　JE/JZ　等于转移.
　　　　JNE/JNZ 不等于时转移.
　　　　JC　　　有进位时转移.
　　　　JNC　　无进位时转移.
　　　　JNO　　不溢出时转移.
　　　　JNP/JPO 奇偶性为奇数时转移.
　　　　JNS　　符号位为 "0" 时转移.
　　　　JO　　　溢出转移.
　　　　JP/JPE　奇偶性为偶数时转移.
　　　　JS　　　符号位为 "1" 时转移.
　　3>循环控制指令(短转移)
　　　　LOOP　　　　　　CX不为零时循环.
　　　　LOOPE/LOOPZ　　CX不为零且标志Z=1时循环.
　　　　LOOPNE/LOOPNZ　CX不为零且标志Z=0时循环.
　　　　JCXZ　　　　　　CX为零时转移.
　　　　JECXZ　　　　　ECX为零时转移.
　　4>中断指令
　　　　INT　　中断指令
　　　　INTO　　溢出中断
　　　　IRET　　中断返回
　　5>处理器控制指令
　　　　HLT　　处理器暂停, 直到出现中断或复位信号才继续.
　　　　WAIT　　当芯片引线TEST为高电平时使CPU进入等待状态.
　　　　ESC　　转换到外处理器.
　　　　LOCK　　封锁总线.
　　　　NOP　　空操作.
　　　　STC　　置进位标志位.
　　　　CLC　　清进位标志位.
　　　　CMC　　进位标志取反.
　　　　STD　　置方向标志位.
　　　　CLD　　清方向标志位.
　　　　STI　　置中断允许位.
　　　　CLI　　清中断允许位.
六、伪指令 
       DW　　　定义字(2字节).
　　　　PROC　　定义过程.
　　　　ENDP　　过程结束.
　　　　SEGMENT 定义段.
　　　　ASSUME　建立段寄存器寻址.
　　　　ENDS　　段结束.
　　　　END　　程序结束.






 今天开始学习linux内核编程了，从没有内核编程基础开始学起。所以很多相关的知识都要了解。首先就是AT&T汇编语言。因为在linux内核源代码中，好像除了开始的bootsect.s和head.s是用intel的汇编外，别的汇编代码都是用的AT&T汇编语言，所以有必要把AT&T汇编语言了解一下。
不过，由于AT&T汇编和intel汇编大同小异，所以，了解一下一些不同的地方就可以了。
 
   以下的内容都是AT&T汇编的特点：
1、寄存器前面要加“%”，如  mov %eax,%ebx
   这里要注意的一点是，AT&T汇编中，源寄存器和目的寄存器的顺序和intel汇编刚好相反，AT&T汇编中，左边的是源寄存器，右边的是目的寄存器，在上边那个例子中，%eax是源寄存器，%ebx是目的寄存器。
 
2、立即数/常数前面要加$，如  mov $4,%ebx 把4这个数装入ebx这个寄存器。
   符号常数直接用， 如  mov  value,%eax 即把value代表的那个值装入eax寄存器。
                     mov $value,%eax 即把value的值作为地址，而把对应那个地址中的值装入eax。
 
3、b(byte):8位， w（word）：16位， l（long）：32位
   如：  movb %al,%bl     movw %ax,%bx     movl %eax,%ebx
 
4、jum/call的操作数前要加上“*"作为前缀， 远跳转ljmp，远调用lcall
   如  ljmp $section,$offset
       lcall $section,$offset
   这里$section和offset表示的就是，以section为段地址，offset为段内偏移地址。因此，ljmp $section,$offset即跳转到section:offset地址。
 
5、远返回lret
   如  lret $stack_adjust
 
6、寻址方式
   寄存器间接寻址 AT&T: (%eax)  Intel: [eax]  
   表示方式 section:disp(base,index,scale)

   计算方法 base+index*scale+disp
        即 section:[base+index*scale+disp]
   其中disp是表示偏移地址。
   如  movl -4(%ebp),%eax  把[%ebp-4]的内容装入eax
   变址寻址  AT&T: _variable(%eax)  Intel: [eax + _variable]

　　　　　  AT&T: _array(,%eax,4)  Intel: [eax*4 + _array]
　　　　     AT&T: _array(%ebx,%eax,8)  Intel: [ebx + eax*8 + _array]   
 
7、C语言中嵌入汇编
   格式: _asm_("asm statements":outputs:inputs:registers-modified)
   其中，"asm statements"是汇编语句表达式，outputs,inputs,register-modified都是可选参数，以冒号隔开，且一次以0～9编号，如outputs的寄存器是0号，inputs寄存器是1号，往后依次类推。outputs是汇编语句执行完后输出到的寄存器，inputs是输入到某个寄存器。
   例1：_asm_("pushl %%eax\n\t" "movl $0,%%eax\n\t" "popl %%eax");
   在嵌入汇编中，寄存器前面要加两个%，因为gcc在编译是，会先去掉一个%再输出成汇编格式。
   例2：{ register char _res;\
         asm("push %%fs\n\t"
         "movw %%ax,%%fs\n\t"
         "movb %%fs:%1,%%al\n\t"
         "pop %%fs"
         :"=a"(_res):"0"(seg),"m"(*(addr)));\
         _res;}
    movb %%fs:%1,%%al\n\t一句中是把以fs为段地址，以后面的第二号寄存器即后面的seg中的值为偏移地址所对应的值装入al。"=a"(_res):"0"(seg),"m"(*(addr)))一句中，"=a"(_res)表示把a寄存器中的内容给_res，"0"(seg)表示把seg中的内容给0所对应的寄存器，而0即表示使用和前一个寄存器相同的寄存器，这里即使用a寄存器，也就是说把seg中的内容个a寄存器。
   需要解释以下的是，a,b,c,d分别表示寄存器eax，ebx，ecx，edx
                  S，D分别表示寄存器esi，edi
                  r表示任意寄存器
                  0（数字0，不是o！）表示使用上一个寄存器
