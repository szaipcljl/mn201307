#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <functional>

struct B {
	virtual void bar() { std::cout << "B::bar\n"; }
	virtual ~B() = default;
};
struct D : B
{
	D() { std::cout << "D::D\n";  }
	~D() { std::cout << "D::~D\n";  }
	void bar() override { std::cout << "D::bar\n";  }
};

// 消费 unique_ptr 的函数能以值或以右值引用接收它
std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
	p->bar();
	return p;
}

void close_file(std::FILE* fp) { std::fclose(fp); }

int main()
{
	std::cout << "unique ownership semantics demo\n";
	{
		auto p = std::make_unique<D>(); // p 是占有 D 的 unique_ptr
		auto q = pass_through(std::move(p));
		assert(!p); // 现在 p 不占有任何内容并保有空指针
		q->bar();   // 而 q 占有 D 对象
	} // ~D 调用于此

	std::cout << "Runtime polymorphism demo\n";
	{
		std::unique_ptr<B> p = std::make_unique<D>(); // p 是占有 D 的 unique_ptr
													  // 作为指向基类的指针
		p->bar(); // 虚派发

		std::vector<std::unique_ptr<B>> v;  // unique_ptr 能存储于容器
		v.push_back(std::make_unique<D>());
		v.push_back(std::move(p));
		v.emplace_back(new D);
		for(auto& p: v) p->bar(); // 虚派发
	} // ~D called 3 times

	std::cout << "Custom deleter demo\n";
	std::ofstream("demo.txt") << 'x'; // 准备要读的文件
	{
		std::unique_ptr<std::FILE, void (*)(std::FILE*) > fp(std::fopen("demo.txt", "r"),
				close_file);
		if(fp) // fopen 可以打开失败；该情况下 fp 保有空指针
			std::cout << (char)std::fgetc(fp.get()) << '\n';
	} // fclose() 调用于此，但仅若 FILE* 不是空指针
	// （即 fopen 成功）

	std::cout << "Custom lambda-expression deleter demo\n";
	{
		std::unique_ptr<D, std::function<void(D*)>> p(new D, [](D* ptr)
				{
					std::cout << "destroying from a custom deleter...\n";
					delete ptr;
				}); // p 占有 D
		p->bar();
	} // 调用上述 lambda 并销毁 D

	std::cout << "Array form of unique_ptr demo\n";
	{
		std::unique_ptr<D[]> p{new D[3]};
	} // 调用 ~D 3 次
}

/*
 * note:
 * std::unique_ptr 是通过指针占有并管理另一对象，并在 unique_ptr
 * 离开作用域时释放该对象的智能指针。
 * 定义于头文件 <memory>
 */
