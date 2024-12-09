# GUIComponents.h 食用方法

由于这个 `EasyX` 拿来写控件一点也不 Easy

所以我构建了一个基础的 GUI 框架以及三个常用控件来简化这个过程

## 准备

首先你需要把头文件和你的程序放在一起，或者在 `VS` 中添加头文件

然后你需要加入这样一句

```cpp
#include "GUIComponents.h"
```

头文件中已经包含了

```cpp
#include <iostream>
#include <graphics.h>
#include <thread>
#include <vector>
#include <conio.h>
```

!!!!!!!!!!然后你需要在 项目-属性-高级 里面把字符集改为 `使用多字节字符集`

## 类列表

这里是头文件中包含的所有类以及对应的方法

## point 类

点类，非常简单，如下

```cpp
class point {
public:
    int x, y;

    point(int x_t, int y_t) : x(x_t), y(y_t) {}
    point() : point(0, 0) {}
};
```

## Window 类

窗口类，整合各个组件，简化渲染过程

**Window 类的方法**

- `Window(int w, int h)` 方法
  
  - 构造方法，传入宽度和高度
  
  - 没有默认构造方法，必须指定宽高

- `void renderOnce()` 方法
  
  - 渲染一次窗口（你可以在其他地方调用来刷新窗口组件）
  
  - 一般不需要使用

- `void addComponent(Base* temp)` 方法
  
  - 添加一个指向派生类控件对象的 **基类** 指针
  
  - 就是用来添加控件的

- `void show()` 方法
  
  - 展示窗口，启动窗口渲染
  
  - 他会自己创建绘画设备，不用自己创建

- `void hide()` 方法
  
  - 停止渲染，让窗口不显示，但是会保留窗口的内容
  
  - 与下面的删除方法区分

- `void del()` 方法
  
  - 删除窗口，施放所有指针指向的内存
  
  - 在结束程序之前必须调用一次，或者你也可以手动一个个释放控件

- `void bkcolor(COLORREF c)` 方法
  
  - 设置背景颜色

## textBox 类

标签类，在指定位置生成一个标签

只有构造方法能调用

```cpp
textBox(point p, int H, string t)
// 标签类(位置, 高, 内容)
textBox() : textBox(point(0, 0), 0, "") {} // 默认是这样
```

在指定的位置 `p` 生成一个高为 `H` ，内容为 `t` 的标签 <- 记得使用多字节字符集

会自动按照比例调整宽度，所以只需要指定高度

## button 类

按钮类，在指定的位置生成按钮，在按下后调用对应的回调函数

有构造方法和点击方法

```cpp
button(point p, int x, int y, string t, void (*cb)())
//按钮类(位置, 宽, 高, 标题, 回调函数)
button() : button(point(0, 0), 0, 0, "", nullptr) {} // 默认
```

你需要提供一个 **无类无参函数** 以供按钮回调

后面会有例子

## inputBox 类

编辑框类，在指定的位置生成文本编辑框

注意这个文本编辑框只实现了基础的功能

由于不能在控制台窗口输入，这里使用的是检测键盘键值的方法来输入

只能输入 `0 ~ 9` ，`A ~ Z` 以及读取退格和回车

同时由于某些原因（主要是我懒），超出编辑框长度的部分不会被渲染

- 构造方法

```cpp
inputBox(point p, int w, int h)
//编辑框类(位置, 宽, 高)
inputBox() : inputBox(point(0, 0), 0, 0) {} // 默认
```

- `string getString()` 方法，读取编辑框内的字符串，没啥好说的，原型如下

```cpp
string getString() {
    return text;
}
```

- `void setString(string text)` 方法，设置编辑框内的字符串，原型如下

```cpp
void setString(string text) {
    this->text = text;
    cursorPosition = text.length();
}
```

## 使用方法，通过一个例子来讲解

```cpp
#include <iostream>
#include <graphics.h>
#include <thread>
#include <vector>
#include <conio.h>
// 上面按照习惯写，必要的头文件已经包含了
// 包含头文件
#include "GUIComponents.h"

// 用于传递信息
bool Stop = false;
// 提供给按钮来进行操作
void exampleButtonClicked() {
    Stop = true;
}

// 主函数
int main() {
    Window window1(500, 500); // 创建一个窗口对象
    window1.bkcolor(YELLOW); // 设置背景颜色

    Base* temp; // 临时基类指针，你也可以给每个组件单独的指针来管理

    // 创建一个按钮对象
    temp = new button(point(10, 10), 400, 100, "EXAMPLE", exampleButtonClicked);
    window1.addComponent(temp); // 添加到窗口中
    // 创建一个标签对象
    temp = new textBox(point(10, 120), 100, "EXAMPLE");
    window1.addComponent(temp); // 添加到窗口中
    // 创建一个编辑框对象
    temp = new inputBox(point(10, 230), 400, 100);
    window1.addComponent(temp); // 添加到窗口中

    // 展示窗口
    window1.show();

    // 这个时候指针指向的是编辑框对象，转换为派生类指针访问来设置内容
    ((inputBox*)temp)->setString("Exapmle");

    // 用于测试按钮功能，按钮按下会改变 Stop 的值，从而让循环结束
    while (!Stop) {}

    // 访问编辑框，返回编辑框的内容
    cout << ((inputBox*)temp)->getString() << endl;

    // 别忘了清理窗口
    window1.del();

    return 0;
}
```

## 其他

我不是美术生，所以设计控件时的首要目标是让它能用，醒目，但是不保证好看

如果你觉得不好看，可以修改每个类重构后的 `render` 方法

这个 GUI 框架仍然有很多的修改空间，我也是闲暇随手写的

海明风

2024.12.9

[GitHub@Hai-M-Feng](https://github.com/Hai-M-Feng)
