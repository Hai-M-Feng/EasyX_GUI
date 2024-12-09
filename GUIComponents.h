#ifndef GUI_COMPONENTS_H
#define GUI_COMPONENTS_H

#include <iostream>
#include <graphics.h>
#include <thread>
#include <vector>
#include <conio.h>

using namespace std;

// 状态枚举变量
enum Statu {
	Normal,
	Focused,
	Hovered,
	Active,
	Error
};

class Base;

// 点类
class point {
public:
	int x, y;

	point(int x_t, int y_t) : x(x_t), y(y_t) {}
	point() : point(0, 0) {}
};

// 控件基类
class Base {
protected:
	point position;
	int Width, Height;

public:
	Base(point p, int x, int y) : position(p), Width(x), Height(y) {}
	Base() : Base(point(0, 0), 0, 0) {}
	~Base() {};

	bool hasFocused = false; // 不要更改这个变量

	virtual void render(Statu s) = 0;
	virtual void click() = 0;

	// 判断该点是否在该控件上
	bool onComponent(int x, int y) {
		return ((x > position.x && x < position.x + Width) && (y > position.y && y < position.y + Height));
	}
};

// 按钮类
class button : public Base {
private:
	void (*callback)();
	string title;

public:
	button(point p, int x, int y, string t, void (*cb)()) : Base(p, x, y), callback(cb), title(t) {}
	button() : button(point(0, 0), 0, 0, "", nullptr) {}
	~button() {};

	// 回调函数
	void click() override {
		if (callback != nullptr) {
			callback();
		}
	}

	// 渲染方法
	void render(Statu s) override {
		settextcolor(BLACK);
		settextstyle(Height - 30, 0, TEXT("等线"));

		switch (s) {
		case Normal:
			setfillcolor(WHITE);
			break;
		case Active:
			setfillcolor(0xE7E7E7);
			break;
		case Hovered:
			setfillcolor(0xFFDE84);
			break;
		default:
			setfillcolor(WHITE);
			break;
		}

		fillrectangle(position.x, position.y, position.x + Width, position.y + Height);
		setbkmode(TRANSPARENT);
		outtextxy(position.x + 15, position.y + 15, title.c_str());
	}
};

// 标签类
class textBox : public Base {
private:
	string title;

public:
	textBox(point p, int H, string t) : Base(p, 0, H), title(t) {}
	textBox() : textBox(point(0, 0), 0, "") {}
	~textBox() {};

	// 渲染方法
	void render(Statu s) override {
		settextcolor(BLACK);
		settextstyle(Height, 0, TEXT("等线"));
		setbkmode(TRANSPARENT);
		outtextxy(position.x, position.y, title.c_str());
	}

	void click() {};
};

// 文本编辑框
class inputBox : public Base {
private:
	string text;
	bool focused = false;
	int cursorPosition = 0;
	bool running = true;
	int threadnumber = 0;

	// 键盘监控线程
	void handleInput() {
		SHORT lastKeyStates[256] = { 0 }; // 用于存储上一次按键状态

		while (running && focused) {
			for (int i = ' '; i <= '~'; ++i) {
				SHORT keyState = GetAsyncKeyState(i);
				if ((keyState & 0x8000) && !(lastKeyStates[i] & 0x8000)) {
					// 按键从未按下变为按下
					text.insert(cursorPosition++, 1, static_cast<char>(i));
					render(Normal);
					FlushBatchDraw();
					lastKeyStates[i] = keyState; // 更新按键状态
					break;
				}
				else if (!(keyState & 0x8000)) {
					// 按键为释放状态，更新按键状态
					lastKeyStates[i] = keyState;
				}
			}

			// Backspace 键
			SHORT backspaceState = GetAsyncKeyState(VK_BACK);
			if ((backspaceState & 0x8000) && !(lastKeyStates[VK_BACK] & 0x8000)) {
				if (!text.empty() && cursorPosition > 0) {
					text.erase(--cursorPosition, 1);
					render(Normal);
					FlushBatchDraw();
				}
				lastKeyStates[VK_BACK] = backspaceState;
			}
			else if (!(backspaceState & 0x8000)) {
				lastKeyStates[VK_BACK] = backspaceState;
			}

			// Enter 键
			SHORT enterState = GetAsyncKeyState(VK_RETURN);
			if ((enterState & 0x8000) && !(lastKeyStates[VK_RETURN] & 0x8000)) {
				threadnumber--;
				focused = false;
				running = false;
				return;
			}
			else if (!(enterState & 0x8000)) {
				lastKeyStates[VK_RETURN] = enterState;
			}

			// 控制线程频率以减少 CPU 占用
			std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 约60帧每秒
		}

		threadnumber--;
	}


public:
	inputBox(point p, int w, int h) : Base(p, w, h), text("") {
		hasFocused = true;
	}
	inputBox() : inputBox(point(0, 0), 0, 0) {}
	~inputBox() {};

	// 渲染方法
	void render(Statu s) override {
		settextcolor(BLACK);
		settextstyle(Height - 10, 0, TEXT("等线"));

		// 设置边框颜色
		setlinecolor(focused ? BLUE : BLACK);

		// 绘制边框
		rectangle(position.x, position.y, position.x + Width, position.y + Height);

		// 填充背景色
		setfillcolor(WHITE);
		fillrectangle(position.x + 1, position.y + 1, position.x + Width - 1, position.y + Height - 1);

		// 计算文本总宽度
		int availableWidth = Width - 10; // 减去一些内边距

		// 如果文本宽度超过了输入框的可用宽度，则截断文本
		string visibleText;
		if (textwidth(text.c_str()) > availableWidth) {
			size_t i = text.length();
			do {
				--i;
				visibleText = text.substr(0, i + 1); // 包含第i个字符
			} while (i > 0 && textwidth(visibleText.c_str()) > availableWidth);

			// 确保我们不会因为字符宽度问题而多截掉一个字符
			if (textwidth(visibleText.c_str()) < availableWidth) {
				++i;
				visibleText = text.substr(0, i);
			}
		}
		else {
			visibleText = text;
		}

		// 输出文本内容
		outtextxy(position.x + 5, position.y + (Height / 2 - textheight("A") / 2), visibleText.c_str());

		// 如果处于聚焦状态，则绘制光标
		if (focused && cursorPosition <= visibleText.length()) {
			int textWidthBeforeCursor = textwidth(text.substr(0, cursorPosition).c_str());
			line(position.x + 5 + textWidthBeforeCursor, position.y,
				position.x + 5 + textWidthBeforeCursor, position.y + Height);
		}
	}

	// 被点击时的方法
	void click() override {
		focused = true;
		running = true;
		// 开启输入线程
		if (threadnumber == 0) {
			threadnumber++;
			thread input([this]() { this->handleInput(); });
			input.detach();
		}
	}

	// 结束键盘监控线程
	void stopThread() {
		focused = false;
		running = false;
	}

	// 返回编辑框内的文本
	string getString() {
		return text;
	}

	// 设置编辑框内的文本
	void setString(string text) {
		this->text = text;
		cursorPosition = text.length();
	}
};

// 窗口类
class Window {
private:
	vector<Base*> components;
	int Width, Height;
	bool stop = false;
	COLORREF color = BLACK;

	// 渲染线程
	void renderThread() const {
		MOUSEMSG mouse = { 0, 0 };
		while (!stop) {
			bool skip = false;
			BeginBatchDraw();
			setbkcolor(color);
			cleardevice();
			for (Base* temp : components) {
				if (temp->onComponent(mouse.x, mouse.y)) {
					if (mouse.uMsg == WM_LBUTTONDOWN) {
						temp->render(Active);
						temp->click();
					}
					else {
						temp->render(Hovered);
					}
				}
				else {
					if (temp->hasFocused && mouse.uMsg == WM_LBUTTONDOWN) {
						((inputBox*)temp)->stopThread();
					}
					temp->render(Normal);
				}
			}
			FlushBatchDraw();
			mouse = GetMouseMsg();
		}
	}


public:
	Window(int w, int h) : Width(w), Height(h) {}

	// 渲染一次，但是不刷新
	void renderOnce() {
		bool skip = false;
		MOUSEMSG mouse = GetMouseMsg();
		BeginBatchDraw();
		setbkcolor(color);
		cleardevice();
		for (Base* temp : components) {
			if (temp->onComponent(mouse.x, mouse.y)) {
				if (mouse.uMsg == WM_LBUTTONDOWN) {
					temp->render(Active);
					temp->click();
				}
				else {
					temp->render(Hovered);
				}
			}
			else {
				temp->render(Normal);
			}
		}
		FlushBatchDraw();
	}

	// 添加一个控件
	void addComponent(Base* temp) {
		components.push_back(temp);
	}

	// 可视化窗口
	void show() {
		initgraph(Width, Height);

		stop = false;
		thread render([this]() { this->renderThread(); });
		render.detach();
	}

	// 隐藏窗口
	void hide() {
		stop = true;
		closegraph();
	}

	// 删除窗口
	void del() {
		hide();
		for (Base* temp : components) {
			delete temp;
		}
		components.clear();
	}

	// 设置背景颜色
	void bkcolor(COLORREF c) {
		color = c;
	}
};

#endif // GUI_COMPONENTS_H