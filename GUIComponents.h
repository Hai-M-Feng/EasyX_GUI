#ifndef GUI_COMPONENTS_H
#define GUI_COMPONENTS_H

#include <iostream>
#include <graphics.h>
#include <thread>
#include <vector>
#include <conio.h>

using namespace std;

// ״̬ö�ٱ���
enum Statu {
	Normal,
	Focused,
	Hovered,
	Active,
	Error
};

class Base;

// ����
class point {
public:
	int x, y;

	point(int x_t, int y_t) : x(x_t), y(y_t) {}
	point() : point(0, 0) {}
};

// �ؼ�����
class Base {
protected:
	point position;
	int Width, Height;

public:
	Base(point p, int x, int y) : position(p), Width(x), Height(y) {}
	Base() : Base(point(0, 0), 0, 0) {}
	~Base() {};

	bool hasFocused = false; // ��Ҫ�����������

	virtual void render(Statu s) = 0;
	virtual void click() = 0;

	// �жϸõ��Ƿ��ڸÿؼ���
	bool onComponent(int x, int y) {
		return ((x > position.x && x < position.x + Width) && (y > position.y && y < position.y + Height));
	}
};

// ��ť��
class button : public Base {
private:
	void (*callback)();
	string title;

public:
	button(point p, int x, int y, string t, void (*cb)()) : Base(p, x, y), callback(cb), title(t) {}
	button() : button(point(0, 0), 0, 0, "", nullptr) {}
	~button() {};

	// �ص�����
	void click() override {
		if (callback != nullptr) {
			callback();
		}
	}

	// ��Ⱦ����
	void render(Statu s) override {
		settextcolor(BLACK);
		settextstyle(Height - 30, 0, TEXT("����"));

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

// ��ǩ��
class textBox : public Base {
private:
	string title;

public:
	textBox(point p, int H, string t) : Base(p, 0, H), title(t) {}
	textBox() : textBox(point(0, 0), 0, "") {}
	~textBox() {};

	// ��Ⱦ����
	void render(Statu s) override {
		settextcolor(BLACK);
		settextstyle(Height, 0, TEXT("����"));
		setbkmode(TRANSPARENT);
		outtextxy(position.x, position.y, title.c_str());
	}

	void click() {};
};

// �ı��༭��
class inputBox : public Base {
private:
	string text;
	bool focused = false;
	int cursorPosition = 0;
	bool running = true;
	int threadnumber = 0;

	// ���̼���߳�
	void handleInput() {
		SHORT lastKeyStates[256] = { 0 }; // ���ڴ洢��һ�ΰ���״̬

		while (running && focused) {
			for (int i = ' '; i <= '~'; ++i) {
				SHORT keyState = GetAsyncKeyState(i);
				if ((keyState & 0x8000) && !(lastKeyStates[i] & 0x8000)) {
					// ������δ���±�Ϊ����
					text.insert(cursorPosition++, 1, static_cast<char>(i));
					render(Normal);
					FlushBatchDraw();
					lastKeyStates[i] = keyState; // ���°���״̬
					break;
				}
				else if (!(keyState & 0x8000)) {
					// ����Ϊ�ͷ�״̬�����°���״̬
					lastKeyStates[i] = keyState;
				}
			}

			// Backspace ��
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

			// Enter ��
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

			// �����߳�Ƶ���Լ��� CPU ռ��
			std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Լ60֡ÿ��
		}

		threadnumber--;
	}


public:
	inputBox(point p, int w, int h) : Base(p, w, h), text("") {
		hasFocused = true;
	}
	inputBox() : inputBox(point(0, 0), 0, 0) {}
	~inputBox() {};

	// ��Ⱦ����
	void render(Statu s) override {
		settextcolor(BLACK);
		settextstyle(Height - 10, 0, TEXT("����"));

		// ���ñ߿���ɫ
		setlinecolor(focused ? BLUE : BLACK);

		// ���Ʊ߿�
		rectangle(position.x, position.y, position.x + Width, position.y + Height);

		// ��䱳��ɫ
		setfillcolor(WHITE);
		fillrectangle(position.x + 1, position.y + 1, position.x + Width - 1, position.y + Height - 1);

		// �����ı��ܿ��
		int availableWidth = Width - 10; // ��ȥһЩ�ڱ߾�

		// ����ı���ȳ����������Ŀ��ÿ�ȣ���ض��ı�
		string visibleText;
		if (textwidth(text.c_str()) > availableWidth) {
			size_t i = text.length();
			do {
				--i;
				visibleText = text.substr(0, i + 1); // ������i���ַ�
			} while (i > 0 && textwidth(visibleText.c_str()) > availableWidth);

			// ȷ�����ǲ�����Ϊ�ַ�����������ص�һ���ַ�
			if (textwidth(visibleText.c_str()) < availableWidth) {
				++i;
				visibleText = text.substr(0, i);
			}
		}
		else {
			visibleText = text;
		}

		// ����ı�����
		outtextxy(position.x + 5, position.y + (Height / 2 - textheight("A") / 2), visibleText.c_str());

		// ������ھ۽�״̬������ƹ��
		if (focused && cursorPosition <= visibleText.length()) {
			int textWidthBeforeCursor = textwidth(text.substr(0, cursorPosition).c_str());
			line(position.x + 5 + textWidthBeforeCursor, position.y,
				position.x + 5 + textWidthBeforeCursor, position.y + Height);
		}
	}

	// �����ʱ�ķ���
	void click() override {
		focused = true;
		running = true;
		// ���������߳�
		if (threadnumber == 0) {
			threadnumber++;
			thread input([this]() { this->handleInput(); });
			input.detach();
		}
	}

	// �������̼���߳�
	void stopThread() {
		focused = false;
		running = false;
	}

	// ���ر༭���ڵ��ı�
	string getString() {
		return text;
	}

	// ���ñ༭���ڵ��ı�
	void setString(string text) {
		this->text = text;
		cursorPosition = text.length();
	}
};

// ������
class Window {
private:
	vector<Base*> components;
	int Width, Height;
	bool stop = false;
	COLORREF color = BLACK;

	// ��Ⱦ�߳�
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

	// ��Ⱦһ�Σ����ǲ�ˢ��
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

	// ���һ���ؼ�
	void addComponent(Base* temp) {
		components.push_back(temp);
	}

	// ���ӻ�����
	void show() {
		initgraph(Width, Height);

		stop = false;
		thread render([this]() { this->renderThread(); });
		render.detach();
	}

	// ���ش���
	void hide() {
		stop = true;
		closegraph();
	}

	// ɾ������
	void del() {
		hide();
		for (Base* temp : components) {
			delete temp;
		}
		components.clear();
	}

	// ���ñ�����ɫ
	void bkcolor(COLORREF c) {
		color = c;
	}
};

#endif // GUI_COMPONENTS_H