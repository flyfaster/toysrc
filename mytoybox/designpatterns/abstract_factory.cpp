#include <iostream>
#include <string>
#include <typeinfo>
struct creational_pattern {};

class abstract_product_a {
public:
	virtual std::string to_string() = 0;
};

class abstract_product_b {
public:
	virtual std::string to_string() = 0;
};

class abstract_factory : public creational_pattern {
public:
	virtual abstract_product_a* create_product_a() = 0;
	virtual abstract_product_b* create_product_b() = 0;
};



class product_a : public abstract_product_a {
public:
	std::string to_string() {
		return typeid(*this).name();
	}
};

class product_b : public abstract_product_b {
public:
	std::string to_string() {
		return typeid(*this).name();
	}
};

class product_a2 : public abstract_product_a {
public:
	std::string to_string() {
		return typeid(*this).name();
	}
};

class product_b2 : public abstract_product_b {
public:
	std::string to_string() {
		return typeid(*this).name();
	}
};

class concrete_factory_1 : public abstract_factory {
public:
	abstract_product_a* create_product_a() override { return new product_a; }
	abstract_product_b* create_product_b() override { return new product_b; }
};

class concrete_factory_2 : public abstract_factory {
public:
	abstract_product_a* create_product_a() override { return new product_a2; }
	abstract_product_b* create_product_b() override { return new product_b2; }
};

void use_factory(abstract_factory* factory){
	abstract_product_a* pa = factory->create_product_a();
	abstract_product_b* pb = factory->create_product_b();
	std::cout << pa->to_string() << std::endl;
	std::cout << pb->to_string() << std::endl;
}

int main(int argc, char* argv[])
{
	concrete_factory_1 cf;
	use_factory(&cf);
	concrete_factory_2 cf2;
	use_factory(&cf2);
	std::cout << argv[0] << " done\n";
	return 0;
}
