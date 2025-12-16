#include<iostream>
#include<graphics.h>
#include<vector>
#include<conio.h>
#include<algorithm>
using namespace std;

#define BGWEIGHT 700 //宽
#define BGHIGH 900  //高

bool PointinRect(int x, int y, RECT& r) {
	return(x <= r.right && x >= r.left && y >= r.top && y <= r.bottom);
}

bool RECTcrashRect(RECT &x,RECT &y) {
	return(
		x.left <= y.right &&
		x.right >= y.left &&
		x.top <= y.bottom &&
		x.bottom >= y.top
		);
}

void welcome() {
	LPCTSTR title = _T("飞机大战");
	LPCTSTR tbegin = _T("开始游戏");
	LPCTSTR texit = _T("退出游戏");

	RECT tbeginr, texitr;
	BeginBatchDraw();//开启批量绘画
	setbkcolor(WHITE);//设置背景颜色
	cleardevice();//刷新
	settextstyle(60, 0, _T("黑体"));//设置文本及其长度，宽度
	settextcolor(BLACK);//设置文本颜色
	outtextxy(BGWEIGHT / 2 - textwidth(title) / 2, BGHIGH *0.15, title);

	settextstyle(50, 0, _T("黑体"));
	tbeginr.left = BGWEIGHT / 2 - textwidth(tbegin) / 2;
	tbeginr.right = tbeginr.left + textwidth(tbegin);
	tbeginr.top = long(BGHIGH * 0.55);
	tbeginr.bottom = tbeginr.top + textheight(tbegin);

	texitr.left = BGWEIGHT / 2 - textwidth(texit) / 2;
	texitr.right = texitr.left + textwidth(texit);
	texitr.top = long(BGHIGH * 0.65);
	texitr.bottom = texitr.top + textheight(texit);

	outtextxy(tbeginr.left, tbeginr.top, tbegin);
	outtextxy(texitr.left, texitr.top, texit);

	EndBatchDraw();

	while (true) {
		ExMessage message;
		getmessage(&message,EX_MOUSE);
		if (message.lbutton) {
			if (PointinRect(message.x, message.y, tbeginr))
				return;
			else if (PointinRect(message.x, message.y, texitr))
				exit(0);
		}
	}

}

void over(unsigned long long &kill) {
	TCHAR* str = new TCHAR[128];
	_stprintf_s(str, 128, _T("击杀数:%llu"), kill);
	settextcolor(RED);
	outtextxy(BGWEIGHT / 2 - textwidth(str), BGHIGH / 5, str);

	LPCTSTR INFO = _T("按enter返回");
	settextstyle(50, 0, _T("黑体"));
	outtextxy(BGWEIGHT - textwidth(INFO), BGHIGH - textheight(INFO),INFO);

	while (true) {
		ExMessage mess;
		getmessage(&mess,EX_KEY);
		if (mess.vkcode == 0x0D)
			return;
	}
}

//背景，敌机，英雄，子弹
class bk{
public:
	bk(IMAGE& img)
		: img(img),y(-BGHIGH)
	{

	}
	void show() {
		if (y == 0) {
			y = -BGHIGH;
		}
		y += 3;
		putimage(0, y, &img);
	}
private:
	IMAGE& img;
	int y;
};

class myplane {
public:
	bool ispeekmessage2;
	long x;
	long y;
	myplane(IMAGE& img)
		:img(img),ispeekmessage2(false)
	{
		r.left = BGWEIGHT / 2 - img.getwidth() / 2;
		r.top = BGHIGH - img.getheight();
		r.right = r.left + img.getwidth();
		r.bottom = r.top + img.getheight();
	}
	void show() {
		putimage(r.left, r.top, &img);
	}
	void control() {
		if(ispeekmessage2 == true)
		 {
			r.left = x - img.getwidth() / 2;
			r.top = y - img.getheight() / 2;
			r.right = r.left + img.getwidth();
			r.bottom = r.top + img.getheight();
		}
	}
	RECT& Getrect() { return r; }
private:
	IMAGE& img;
	RECT r;
};

class enemy {
public:
	enemy(IMAGE& img, int x)
		:img(img)
	{
		r.left = x;
		r.right = x + img.getwidth();
		r.top = -img.getheight();
		r.bottom = 0;
	}
	bool show() {
		if (r.top >= BGHIGH)
			return FALSE;
		
		r.top += 4;
		r.bottom += 4;
		putimage(r.left, r.top, &img);
		return true;
	}
	RECT& Getrect() { return r; }
private:
	IMAGE& img;
	RECT r;
};



bool addep(vector<enemy*>&eps,IMAGE & enemyplane) {
	enemy* e = new enemy(enemyplane, abs(rand()) % BGWEIGHT - enemyplane.getwidth());
	for (auto& i : eps) {
		if (RECTcrashRect(i->Getrect(), e->Getrect())) {
			delete e;
			return false;
		}
	}
	eps.push_back(e);
	return true;
}

class bullet {
public:
	bullet(IMAGE&img,RECT pr)
		:img(img)
	{
		r.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
		r.right = r.left + img.getwidth();
		r.top = pr.top - img.getheight();
		r.bottom = pr.top;
	}
	bool show() {
		if (r.bottom <= 0)
			return false;
		r.top -= 3;
		r.bottom -= 3;
		putimage(r.left, r.top, &img);
		return true;
	}
	RECT& Getrect() { return r; }
protected:
	IMAGE& img;
	RECT r;
};

class bullet2 :public bullet {
public:
	bullet2(IMAGE& img, RECT pr)
		:bullet(img,pr) {
		r.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
		r.right = r.left + img.getwidth();
		r.top = pr.bottom;
		r.bottom = r.top + img.getheight();
	}
	bool show() {
		if (r.top >= BGHIGH)
			return false;
		r.top += 5;
		r.bottom += 5;
		putimage(r.left, r.top, &img);
		return true;
	}
};


class missile {
public:
	bool islaunch;
	bool ispeekmessage;
	missile(IMAGE& img)
		:img(img),islaunch(false),ispeekmessage(false) {
		r.left = 0;
		r.right = r.left + img.getwidth();
		r.bottom = BGHIGH;
		r.top = r.bottom - img.getheight();
	}
	bool show() {
		putimage(r.left, r.top, &img);
		return true;
	}
	void reset() {
		r.left = 0;
		r.right = r.left + img.getwidth();
		r.bottom = BGHIGH;
		r.top = r.bottom - img.getheight();
	}
	void find_e_missile(vector<enemy*>& eps) {
		if (eps.empty())  return;
		if (ispeekmessage == true) {
			islaunch = true;
		}
		if (islaunch == true) {
			cout << "1" << endl;
			enemy* target = eps[0];
			r.left += (target->Getrect().left - r.left) / 20;
			r.right = r.left + img.getwidth();
			r.top += (target->Getrect().bottom - r.top) / 20;
			r.bottom = r.top + img.getheight();
			if (RECTcrashRect(r, target->Getrect())) {
				auto it = find(eps.begin(), eps.end(), target);
				if (it != eps.end()) {
					delete* it;
					eps.erase(it);
				}
				islaunch = false;
				reset();
				cout << "2" << endl;
			}
		}
	}
	RECT& Getrect() { return r; }
	
private:
	IMAGE& img;
	RECT r;
};





bool play() {
	setbkcolor(WHITE);
	cleardevice();
	bool is_play = true;

	IMAGE Myplane, enemyplane, bkimage, bulletimage,bombimage;

	loadimage(&Myplane, _T("C:\\Users\\29234\\OneDrive\\me1.png"));
	loadimage(&enemyplane, _T("C:\\Users\\29234\\OneDrive\\enemy1.png"));
	loadimage(&bkimage, _T("C:\\Users\\29234\\OneDrive\\bk2 .png"), BGWEIGHT,BGHIGH * 2);
	loadimage(&bulletimage, _T("C:\\Users\\29234\\OneDrive\\bullet1.png"));
	loadimage(&bombimage, _T("C:\\Users\\29234\\OneDrive\\bomb.png"));
	

	bk _bk = bk(bkimage);
	myplane mp = myplane(Myplane);
	missile ml = missile(bombimage);
	vector<enemy*> eps;
	vector<bullet*> bts;
	vector<bullet2*> ebts;
	int bsing = 0;
	unsigned long long kill = 0;
	for (int i = 0; i <= 3; i++) {
		addep(eps, enemyplane);
	}
	

	while (is_play) {
		Sleep(10);
		bsing++;
		if (bsing % 20==0) {
			bts.push_back(new bullet(bulletimage, mp.Getrect()));
		}
		
		if (bsing % 60 == 0) {
			for (auto& i : eps) {
				ebts.push_back(new bullet2(bulletimage, i->Getrect()));
			}
		}

		ExMessage mess;
		while (peekmessage(&mess)) {
			if (mess.message == WM_MOUSEMOVE)
			{
				mp.ispeekmessage2 = true;
				mp.x = mess.x;
				mp.y = mess.y;
			}
			else if (mess.message == WM_KEYDOWN) {
				if (mess.vkcode == VK_TAB) {
					ml.ispeekmessage = true;
				}
			}
		}

		BeginBatchDraw();
		_bk.show();
		mp.control(); 
		mp.show();
		ml.show();
		ml.find_e_missile(eps);

		auto blit = bts.begin();
		while (blit != bts.end()) {
			if (!(*blit)->show()) {
				blit = bts.erase(blit);
			}
			else
				blit++;
		}
		
		auto eblit = ebts.begin();
		while (eblit !=ebts.end()) {
			if (RECTcrashRect((*eblit)->Getrect(), mp.Getrect()) ){
				is_play = false;
				break;
			}
			else if (!(*eblit)->show()) {
				eblit = ebts.erase(eblit);
			}
			
			else
				eblit++;
		}

		auto it = eps.begin();
		while (it != eps.end()) {
			if (RECTcrashRect((*it)->Getrect(), mp.Getrect())) {
				is_play = false;
				break;
			}

			auto bit = bts.begin();
			while (bit != bts.end())
			{
				if (RECTcrashRect((*bit)->Getrect(), (*it)->Getrect())) {
					delete (*it);
					eps.erase(it);
					it = eps.begin();
					delete (*bit);
					bts.erase(bit);
					kill++;
					break;
				}
				else
					bit++;
			}

			if (!(*it)->show()) {
				delete(*it);
				eps.erase(it);
				it = eps.begin();
			}
			it++;
		}
		for (int i = 0; i < 4 - eps.size(); i++) {
			addep(eps, enemyplane);
		}
		EndBatchDraw();
		mp.ispeekmessage2 = false;
		ml.ispeekmessage = false;
	}
	over(kill);
	return true;
}



int main() {
	initgraph(BGWEIGHT, BGHIGH);
	bool Game_is_live = true;
	while (Game_is_live) {
		welcome();
		//play
		Game_is_live = play();
	}
	closegraph();
	return  0;
}