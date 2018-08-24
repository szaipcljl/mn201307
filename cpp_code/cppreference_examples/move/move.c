#include <iostream>
#include <utility>
#include <vector>
#include <string>

int main()
{
	std::string str = "Hello";
	std::vector<std::string> v;

	// 使用 push_back(const T&) 重载，
	// 表示我们将带来复制 str 的成本
	v.push_back(str);
	std::cout << "After copy, str is \"" << str << "\"\n";

	// 使用右值引用 push_back(T&&) 重载，
	// 表示不复制字符串；而是
	// str 的内容被移动进 vector
	// 这个开销比较低，但也意味着 str 现在可能为空。
	v.push_back(std::move(str));
	std::cout << "After move, str is \"" << str << "\"\n";

	std::cout << "The contents of the vector are \"" << v[0]
										 << "\", \"" << v[1] << "\"\n";
}

/*
 * note:
 *
 * std::move 用于指示对象 t 可以“被移动”，即允许从 t 到另一对象的有效率的资源传递。
 * 特别是， std::move 生成标识其参数 t 的亡值表达式。它准确地等价于到右值引用类型的 static_cast 。
 *
 * 参数
 * t	-	要被移动的对象
 *
 * 返回值
 * static_cast<typename std::remove_reference<T>::type&&>(t)
 */
