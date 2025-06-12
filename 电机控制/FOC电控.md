# **FOC电控**

## **开环控制svpwm -不带反馈**

1. 设定目标速度

   - 注意速度ramp up
   - 最好使用定点小数

2. 计算目标机械角度

   - 角速度ω*Ts

3. 计算目标电气角度  这个表示合成矢量的角度θ

   - 可以通过机械角度*极对数

4. 设置目标电流 目标电压 这个表示合成矢量的模长

5. 对目标电流矢量进行dq轴分解

   - d direct 直轴

   - q quarter 交轴

   - 控制时为什么一般让d轴量为0？？

     - **表贴式永磁同步电机 (SPMSM)：** d轴电感(Ld) ≈ q轴电感(Lq)。此时电磁转矩公式简化为：`Te = (3/2) * Pp * Ψpm * Iq` (Pp:极对数, Ψpm:永磁体磁链)。**转矩只与 Iq 相关**。Id ≠ 0 只会产生d轴方向的磁阻力（不产生转矩），同时增加铜耗（I²R损耗），还可能引起不必要的弱磁效应。因此，**最大转矩电流比控制 (MTPA) 在SPMSM中就是 Id_ref = 0**。

       **内置式永磁同步电机 (IPMSM)：** Ld < Lq。此时磁阻转矩不可忽略：`Te = (3/2) * Pp * [Ψpm * Iq + (Ld - Lq) * Id * Iq]`。MTPA控制要求Id_ref为一个**负值**，以利用磁阻转矩，在相同转矩下减小总电流幅值（降低损耗）。所以Id_ref=0并非普遍适用，但在SPMSM中是最优且最常用的策略。

     - 保持额定磁链

     - 简化解耦控制

6. 将动态dq轴变换到静态的αβ坐标系上

   - 记不清是park还是clerk正还是逆变换了???
     - 逆 Park（Inverse Park）
     - 将给定的 d-q 电流或电压指令，从转子坐标系（d-q）变换到定子静止坐标系（α-β）

7. 将静态的αβ变换到三相abc坐标系上

   - abc三相相差120°

8. 将得到的三相分量通过svpwm实现

   - 五段法/七段法 -区别

   - 扇区判断

   - svpwm/spwm实现

     - spwm的实现是直接的到三相电压值，根据参考电压值的到应该设置的pwm占空比比例，进而算出三相PWM的CCR值

     - svpwm的实现是通过八个基本矢量合成得来的，根据不同扇区选取的合成矢量是不同的。需要结合具体的扇区选取合适的基本矢量，再根据需要的电压值（这里主要是使用αβ坐标下的电压值）得到不同向量的保持时间，最后将用的到基本矢量合成一个完整的pwm波周期时间。

     - ### SPWM与SVPWM的关键区别

       | 特性             | SPWM                             | SVPWM                                                        |
       | :--------------- | :------------------------------- | :----------------------------------------------------------- |
       | **实现复杂度**   | 简单（直接计算）                 | 复杂（扇区判断，矢量计算）                                   |
       | **电压利用率**   | 最大87%（理论值）                | 最大100%（通过零序分量注入）                                 |
       | **谐波性能**     | 较高（特别是低调制比时）         | 较低（开关谐波优化分布）                                     |
       | **开关损耗**     | 较高（每相独立开关）             | 较低（优化开关顺序）                                         |
       | **直流母线利用** | 最大线电压 = U_dc/√3 ≈ 0.577U_dc | 最大线电压 = U_dc/√3 ≈ 0.577U_dc（基础） 注入后可达2U_dc/√3 ≈ 0.666U_dc |
       | **波形质量**     | 正弦波                           | 马鞍波（含三次谐波注入）                                     |

9. 设置三相占空比

## **开环控制spwm -不带反馈**

1. 设定目标速度

2. 计算目标机械角度

3. 计算目标电气角度  这个表示合成矢量的角度θ

4. 设置目标电流 目标电压 这个表示合成矢量的模长
5. 对目标电流矢量进行dq轴分解

6. 将动态dq轴变换到静态的αβ坐标系上
7. 将静态的αβ变换到三相abc坐标系上
8. 将得到的三相分量通过spwm实现
   - 在实际操作中确实发现当 设置目标uq为0.5倍参考电压的时候，三相电压值基本上就快达到12v了，这也符合理论上的母线电压的0.577倍
   - 奇怪的是为什么我设置uq为6.5v时也超过12v，按照理论应该能到6.5v？？是不是我的计算精度问题？？
   - ![image-20250612224131013](./FOC%E7%94%B5%E6%8E%A7.assets/image-20250612224131013.png)
   - ![shot](./FOC%E7%94%B5%E6%8E%A7.assets/shot-1749739792143-2.png)
   - ![image-20250612224242737](./FOC%E7%94%B5%E6%8E%A7.assets/image-20250612224242737.png)
   - ![shot](./FOC%E7%94%B5%E6%8E%A7.assets/shot.png)
9. 设置三相占空比

-----

踩坑：

1. 三相电压在转换过程中会出现负值 但是现实中电机控制应该是没办法用到负电压的 所以在三相电压值上加上Vref/2的电压值 将三相电压都转换为正值
2. 由于spwm的效率问题 所以添加限幅函数是有必要的
3. 注意计算机在计算`/`时是整除 可以按照pwm周期数 适度放大 或者采用定点小数除法等 避免出现pwm的ccr值一直为0情况

## **闭环控制 -电流环/速度环/位置环**



## **开环控制 - 滑膜控制**



## 两大变换 -clerk/park

以下是 Clark(e)变换、Park变换 及其逆变换的 Markdown格式公式总结，整合了电机控制和电力电子领域的标准实现版本，并标注了误差来源与数学原理：

### 一、Clark(e)变换（3s/2s变换）

#### 1. 标准Clark变换（三相静止 → 两相静止）

将三相坐标系 $abc$ 转换为两相静止坐标系 $αβ$，并消除零序分量（当$$i_a + i_b + i_c = 0$$时适用）：
$$
\begin{bmatrix} i_\alpha \\ i_\beta  \end{bmatrix}  = \frac{2}{3}  \begin{bmatrix}  1 & -\frac{1}{2} & -\frac{1}{2} \\  0 & \frac{\sqrt{3}}{2} & -\frac{\sqrt{3}}{2}  \end{bmatrix}  \begin{bmatrix} i_a \\ i_b \\ i_c  \end{bmatrix}
$$

---

应用场景：三相电机的αβ坐标系分析

#### 2. 等幅值Clark变换（需保留幅值不变）

通过系数调整确保幅值等效：
$$
\begin{bmatrix} i_\alpha \\ i_\beta  \end{bmatrix}  = \sqrt{\frac{2}{3}}  \begin{bmatrix}  1 & -\frac{1}{2} & -\frac{1}{2} \\  0 & \frac{\sqrt{3}}{2} & -\frac{\sqrt{3}}{2}  \end{bmatrix}  \begin{bmatrix} i_a \\ i_b \\ i_c  \end{bmatrix}
$$

注释：适用于需保持幅值一致性的场景（如锁相环设计）

#### 3. Clark逆变换（两相静止 → 三相静止）

从 $αβ$ 坐标系恢复到 $abc$ 坐标系（需设定零序分量或约束条件）：
$$
\begin{bmatrix} i_a \\ i_b \\ i_c  \end{bmatrix}  =  \begin{bmatrix}  1 & 0 \\ -\frac{1}{2} & \frac{\sqrt{3}}{2} \\ -\frac{1}{2} & -\frac{\sqrt{3}}{2}  \end{bmatrix}  \begin{bmatrix} i_\alpha \\ i_\beta  \end{bmatrix}
$$

误差来源：三相不平衡时零序分量丢失

### 二、Park变换（2s/2r变换）

#### 1. 标准Park变换（两相静止 → 两相旋转）

以转子磁场同步角 $\theta$ 进行旋转坐标系 $dq$ 变换：
$$
 \begin{bmatrix} i_d \\ i_q  \end{bmatrix}  =  \begin{bmatrix}  \cos\theta & \sin\theta \\  -\sin\theta & \cos\theta  \end{bmatrix}  \begin{bmatrix} i_\alpha \\ i_\beta  \end{bmatrix} 
$$

应用场景：电机转矩与磁场解耦控制

#### 2. Park逆变换（两相旋转 → 两相静止）

从 $dq$ 坐标系返回 $αβ$ 坐标系：
$$
\begin{bmatrix} i_\alpha \\ i_\beta  \end{bmatrix}  =  \begin{bmatrix}  \cos\theta & -\sin\theta \\  \sin\theta & \cos\theta  \end{bmatrix}  \begin{bmatrix} i_d \\ i_q  \end{bmatrix}
$$

误差来源：角度估算误差可能导致解耦失效

### 三、关键误差源与优化策略

| 误差类型     | 来源说明                 | 解决方案                   |
| ------------ | ------------------------ | -------------------------- |
| 角度计算误差 | 传感器噪声/分辨率不足    | 使用EKF滤波器/高精度编码器 |
| 正交分量偏移 | 谐波干扰导致αβ偏离理想值 | 前馈补偿 + 低通滤波        |
| 数值舍入误差 | 浮点运算累积误差         | 双精度运算 + 矩阵乘法优化  |

### 四、C实现示例（参考）

```c
#include <stdio.h>
#include <math.h>

// 定义数学常量（精确到8位小数）
#define SQRT3            1.7320508075688772  // √3
#define SQRT3_OVER_2     0.8660254037844386  // √3/2
#define TWO_THIRDS       0.6666666666666666  // 2/3

// Clarke变换：三相静止（abc）→两相静止（αβ）
void clarke_transform(double a, double b, double c, double *alpha, double *beta) {
    *alpha = TWO_THIRDS * (a - 0.5*b - 0.5*c);  // 2/3*(a - b/2 - c/2)
    *beta  = TWO_THIRDS * (SQRT3_OVER_2*b - SQRT3_OVER_2*c); // 2/3*( (√3/2)b - (√3/2)c )
}

// Park变换：两相静止（αβ）→两相旋转（dq）[theta单位为弧度]
void park_transform(double alpha, double beta, double theta, double *d, double *q) {
    double cos_theta = cos(theta);
    double sin_theta = sin(theta);
    *d = alpha * cos_theta + beta * sin_theta;
    *q = -alpha * sin_theta + beta * cos_theta;
}

// Park逆变换：两相旋转（dq）→两相静止（αβ）[theta单位为弧度]
void inverse_park_transform(double d, double q, double theta, double *alpha, double *beta) {
    double cos_theta = cos(theta);
    double sin_theta = sin(theta);
    *alpha = d * cos_theta - q * sin_theta;
    *beta  = d * sin_theta + q * cos_theta;
}

// Clarke逆变换：两相静止（αβ）→三相静止（abc）（假设零序分量为0）
void inverse_clarke_transform(double alpha, double beta, double *a, double *b, double *c) {
    *a = alpha;
    *b = -0.5 * alpha + SQRT3_OVER_2 * beta;
    *c = -0.5 * alpha - SQRT3_OVER_2 * beta;
}

int main() {
    // 测试用例：abc电流(3A, -1A, -2A)，θ=30度（转为弧度）
    double a = 3.0, b = -1.0, c = -2.0;
    double alpha, beta, d, q;
    double theta_rad = 30.0 * M_PI / 180.0; // 角度转弧度

    // 1. Clarke变换测试
    clarke_transform(a, b, c, &alpha, &beta);
    printf("Clark变换结果：α=%.4f, β=%.4f\n", alpha, beta);  // 预期值：α=2.0, β≈1.732

    // 2. Park变换测试（角度30度）
    park_transform(alpha, beta, theta_rad, &d, &q);
    printf("Park变换结果：d=%.4f, q=%.4f\n", d, q);          // 预期值：d≈3.0, q≈0.0

    // 3. 逆变换测试（恢复abc值）
    double a_rev, b_rev, c_rev;
    inverse_clarke_transform(alpha, beta, &a_rev, &b_rev, &c_rev);
    printf("Clarke逆变换恢复：a=%.4f, b=%.4f, c=%.4f\n", a_rev, b_rev, c_rev); 

    return 0;
}
```

### 五、总结

- Clark变换：用于消除三相冗余信息，降维至正交静止坐标系；
- Park变换：将静止坐标映射至同步旋转坐标系，实现解耦控制；
- 逆变换：需注意零序分量恢复与角度校准问题。

## **SVPWM**

你对无感FOC过程的梳理整体很清晰，下面针对你问的 **SVPWM中五段法和七段法的区别** 给你详细解释一下：

------

### 五段法和七段法的区别

SVPWM（空间矢量脉宽调制）是一种高效的PWM调制方法，通过空间矢量理论实现对三相电机的电压矢量控制。其核心是将参考电压矢量分解为相邻两个基本矢量及零矢量的加权组合。

------

#### 1. 基本概念

- **七段法（七个矢量段）**： 在一个PWM周期内，使用**两个相邻的有效矢量 + 两个零矢量**（零矢量有两个：000和111），共7个时间段实现矢量的合成。
- **五段法（五个矢量段）**： 在一个PWM周期内，使用**两个相邻的有效矢量 + 一个零矢量**，共5个时间段实现矢量的合成。

------

#### 2. 具体区别

| 方面          | 七段法                                          | 五段法                             |
| ------------- | ----------------------------------------------- | ---------------------------------- |
| PWM周期分段数 | 7个时间段（T1, T2, T0/2, T0/2，T0为零矢量时间） | 5个时间段（T1, T2, T0）            |
| 零矢量使用    | 两个零矢量分布在PWM周期两端，均分零矢量时间     | 一个零矢量时间段集中使用           |
| 调制波形      | 波形更加平滑，谐波含量较低                      | 实现较简单，计算量略小，谐波略多   |
| 开关频率      | 开关切换次数较多                                | 开关切换次数较少，减少了开关损耗   |
| 应用场景      | 追求高性能控制的场景（比如精度、效率要求高）    | 对计算资源有限或开关损耗敏感的场合 |

------

#### 3. 具体时间段说明

- **七段法：**

  时间段按顺序分为：

  ```
  T0/2（零矢量）
  T1  （第一个有效矢量）
  T2  （第二个有效矢量）
  T0/2（零矢量）
  ```

  也就是说零矢量时间被分成两个半周期分别放在周期两端，保证PWM波形的对称性。

- **五段法：**

  时间段按顺序分为：

  ```
  T0（全部零矢量时间）
  T1
  T2
  ```

  零矢量时间段只在一边，波形对称性稍差，但实现简单。

------

#### 4. 容易混淆的点：

- **Park变换**：是**静止αβ坐标系**到**旋转dq坐标系**的变换（正变换或逆变换根据方向不同），用来把电流电压分量分解到d轴和q轴。
- **Clarke变换**：是**三相abc坐标系**到**两相静止αβ坐标系**的变换。

SVPWM中，最后把dq轴控制量通过逆Park变换得到αβ，再通过逆Clarke变换得到abc三相参考电压。

------

#### 5. 七段法例程

```c
#include <math.h>

#define SQRT3     1.73205080757f
#define PWM_PERIOD 1680  // PWM定时器周期值 (对应20kHz@84MHz时钟)
#define U_DC      24.0f  // 直流母线电压(V)

typedef struct {
    uint16_t cmp_a;
    uint16_t cmp_b;
    uint16_t cmp_c;
} PWM_Values;

// 七段法SVPWM计算
PWM_Values SVPWM_7Segment(float U_alpha, float U_beta) {
    float T1, T2, T0;
    uint8_t sector = 0;
    PWM_Values pwm = {0, 0, 0};
    
    // 1. 扇区判断
    float U_ref = sqrtf(U_alpha*U_alpha + U_beta*U_beta);
    float theta = atan2f(U_beta, U_alpha);
    
    if (theta < 0) theta += 2 * M_PI;  // 转换到0-2π范围
    sector = (uint8_t)(theta / (M_PI/3));  // 每60度一个扇区
    
    // 2. 计算矢量作用时间 (标准公式)
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);
    float k = SQRT3 * PWM_PERIOD / U_DC;
    
    T1 = k * (sin_theta * U_alpha - cos_theta * U_beta);
    T2 = k * (SQRT3/2 * U_alpha + 0.5f * U_beta);
    T0 = PWM_PERIOD - T1 - T2;
    
    // 时间过调制处理
    if (T0 < 0) {
        T1 = T1 * PWM_PERIOD / (T1 + T2);
        T2 = T2 * PWM_PERIOD / (T1 + T2);
        T0 = 0;
    }
    
    // 3. 计算PWM比较值 (七段法对称分配)
    float Ta, Tb, Tc;
    float T_half0 = T0 / 2;
    
    switch (sector) {
        case 0: // Sector I: (100)->(110)->(111)
            Ta = T_half0 + T1 + T2;
            Tb = T_half0 + T2;
            Tc = T_half0;
            break;
        case 1: // Sector II: (110)->(010)->(111)
            Ta = T_half0 + T1;
            Tb = T_half0 + T1 + T2;
            Tc = T_half0;
            break;
        case 2: // Sector III: (010)->(011)->(111)
            Ta = T_half0;
            Tb = T_half0 + T1 + T2;
            Tc = T_half0 + T2;
            break;
        case 3: // Sector IV: (011)->(001)->(111)
            Ta = T_half0;
            Tb = T_half0 + T1;
            Tc = T_half0 + T1 + T2;
            break;
        case 4: // Sector V: (001)->(101)->(111)
            Ta = T_half0 + T2;
            Tb = T_half0;
            Tc = T_half0 + T1 + T2;
            break;
        case 5: // Sector VI: (101)->(100)->(111)
            Ta = T_half0 + T1 + T2;
            Tb = T_half0;
            Tc = T_half0 + T1;
            break;
        default:
            Ta = Tb = Tc = PWM_PERIOD / 2;
    }
    
    // 4. 转换为整数PWM比较值
    pwm.cmp_a = (uint16_t)(PWM_PERIOD - Ta); // 注意极性
    pwm.cmp_b = (uint16_t)(PWM_PERIOD - Tb);
    pwm.cmp_c = (uint16_t)(PWM_PERIOD - Tc);
    
    return pwm;
}

// 定时器中断中更新PWM (示例)
void TIMx_IRQHandler(void) {
    if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET) {
        // 获取当前αβ电压
        float U_alpha = ...;  // 来自电流环输出
        float U_beta = ...;
        
        // 计算SVPWM
        PWM_Values pwm = SVPWM_7Segment(U_alpha, U_beta);
        
        // 更新PWM比较器
        TIMx->CCR1 = pwm.cmp_a;  // 相位A
        TIMx->CCR2 = pwm.cmp_b;  // 相位B
        TIMx->CCR3 = pwm.cmp_c;  // 相位C
        
        TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
    }
}
```

```txt
   七段法波形示例（扇区I）
   	 ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┐
A    │  0  │  1  │  1  │  1  │  1  │  1  │  0  │
     ├─────┼─────┼─────┼─────┼─────┼─────┼─────┤
B    │  0  │  0  │  1  │  1  │  1  │  0  │  0  │
     ├─────┼─────┼─────┼─────┼─────┼─────┼─────┤
C    │  0  │  0  │  0  │  1  │  0  │  0  │  0  │
     └─────┴─────┴─────┴─────┴─────┴─────┴─────┘
时间  T0/4  T1/2  T2/2  T0/2  T2/2  T1/2  T0/4
矢量  000   100   110   111   110   100   000
```

### 总结

| 方法   | 段数 | 零矢量分布     | 优点             | 缺点                   |
| ------ | ---- | -------------- | ---------------- | ---------------------- |
| 七段法 | 7    | 零矢量均分两端 | 谐波低，波形对称 | 实现稍复杂，开关多     |
| 五段法 | 5    | 零矢量集中一端 | 实现简单，开关少 | 谐波稍多，波形对称性差 |

------

## **SPWM**

#### 直接使用三相电压

```c
// SPWM实现 - 直接使用Ua, Ub, Uc计算占空比
void SPWM_Update(float Ua, float Ub, float Uc, float U_dc, uint16_t pwm_period,
                 uint16_t *cmp_a, uint16_t *cmp_b, uint16_t *cmp_c) 
{
    // 1. 归一化电压到[-1, 1]范围
    float norm_a = Ua / (U_dc / 2);  // 最大相电压为U_dc/2
    float norm_b = Ub / (U_dc / 2);
    float norm_c = Uc / (U_dc / 2);
    
    // 2. 限幅处理（确保在[-1, 1]范围内）
    norm_a = fmaxf(fminf(norm_a, 1.0f), -1.0f);
    norm_b = fmaxf(fminf(norm_b, 1.0f), -1.0f);
    norm_c = fmaxf(fminf(norm_c, 1.0f), -1.0f);
    
    // 3. 转换为占空比 (0-1范围)
    // 中心对齐PWM：0对应-1，0.5对应0，1对应+1
    float duty_a = 0.5f * (1.0f + norm_a);
    float duty_b = 0.5f * (1.0f + norm_b);
    float duty_c = 0.5f * (1.0f + norm_c);
    
    // 4. 计算PWM比较值
    *cmp_a = (uint16_t)(duty_a * pwm_period);
    *cmp_b = (uint16_t)(duty_b * pwm_period);
    *cmp_c = (uint16_t)(duty_c * pwm_period);
}
```

### 增强SPWM性能的技巧

```
// 添加三次谐波注入提高电压利用率（伪SPWM-SVPWM混合）
void Enhanced_SPWM(float Ua, float Ub, float Uc, float U_dc, uint16_t pwm_period,
                   uint16_t *cmp_a, uint16_t *cmp_b, uint16_t *cmp_c) 
{
    // 1. 计算零序分量（三次谐波）
    float U_offset = 0.5 * (fmaxf(fmaxf(Ua, Ub), Uc) + fminf(fminf(Ua, Ub), Uc));
    
    // 2. 注入零序分量
    Ua -= U_offset;
    Ub -= U_offset;
    Uc -= U_offset;
    
    // 3. 继续标准SPWM计算
    SPWM_Update(Ua, Ub, Uc, U_dc, pwm_period, cmp_a, cmp_b, cmp_c);
}
```

## **等幅值变换**

## **等功率变换**



