#include <vector>
#include <iostream>
using namespace std;
class A{
public:
	A(){};
	void a(){std::cout <<"Hello";}
};


void test1()
{
	std::vector<double> r(5, 10.0);

	std::vector<double> s(5, 12.0);
	auto iter_s = s.begin();
	int i = 0;
	for (auto iter = r.begin(); iter != r.end(); iter++) {
		*iter = i;
		i++;
	}

	for (auto iter = s.begin(); iter != s.end(); iter++) {
		*iter = i;
		i++;
	}


	for (auto iter = r.begin(); iter != r.end(); iter++) {
		std::cout << *iter << std::endl;
		//std::cout <<*(iter+1)<<std::endl;
		std::cout << *iter_s << std::endl;
		iter_s++;
	}

	//array식 초기화
	int arr[] = { 10,20,30 };
	std::vector<int> vec(std::begin(arr), std::end(arr));  //initialzation by allocator
	arr[2] = 100;

	std::vector<double> m(10, 1.0); //

	int n_t = 3;
	int n_s = 2;
	std::vector< std::vector<double> > vol(n_t, std::vector<double>(n_s, 0.0));
	double arr1[] = { 0.1,0.2,0.3 };
	double arr2[] = { 0.4,0.5,0.6 };

	std::vector<double> dvec(std::begin(arr1), std::end(arr1));

	vol.push_back(std::vector<double>(std::begin(arr1), std::end(arr1)));
	vol.push_back(std::vector<double>(std::begin(arr2), std::end(arr2)));

	double db[10] = { 0.1, };

	for (int i = 0; i == 5; i++)
		std::cout << i;

	std::vector<double> v;
	v.push_back(100);
	v.push_back(200);

	for (auto iter = v.begin(); iter != v.end(); iter++)
		std::cout << *iter;


	A afunc;


}

int main()
{
	double arr1[] = { 1,2,3 };
	double arr2[] = { 4,5,6 };

	std::vector<double> dvec(std::begin(arr1), std::end(arr1));
	std::vector<double> dvec2(std::begin(arr2), std::end(arr2));
	auto iter2 = dvec2.begin();

	for (auto iter = dvec.begin();iter != dvec.end(); iter++) {
		cout << *iter << " " << *iter2 << endl;
		iter2++;
	}


	return 0;
}