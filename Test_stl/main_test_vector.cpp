#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>

using namespace std;
class A{
public:
	A(){};
	void a(){std::cout <<"Hello";}
};

void vectorofvector()
{

}
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

void vectorsize()
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

	cout << "dvec.size();"<<dvec.size();
	vector<vector<double> > ddvec;
	ddvec.push_back(dvec);
	ddvec.push_back(dvec2);

	cout <<"ddvec.size(); " <<ddvec.size();

}

void getFnameTimeStartingWith()
{
	std::ostringstream oss;
	
	std::ofstream fout("hell.csv");
	fout << "hello";
	fout.close();

	time_t curr_time;
	struct tm *curr_tm;

	curr_time = time(NULL);
	curr_tm = localtime(&curr_time);
	string str_mon;
	string str_day;
	string str_hour;
	string str_min;

	if (curr_tm->tm_mon + 1 < 10)
		str_mon = string("0") + to_string(curr_tm->tm_mon + 1);
	else
		str_mon = to_string(curr_tm->tm_mon + 1);

	if (curr_tm->tm_mday + 1 < 10)
		str_day = string("0") + to_string(curr_tm->tm_mday + 1);
	else
		str_day = to_string(curr_tm->tm_mday + 1);

	if (curr_tm->tm_hour + 1 < 10)
		str_hour = string("0") + to_string(curr_tm->tm_hour + 1);
	else
		str_hour = to_string(curr_tm->tm_hour + 1);

	if (curr_tm->tm_min + 1 < 10)
		str_min = string("0") + to_string(curr_tm->tm_min + 1);
	else
		str_min = to_string(curr_tm->tm_min + 1);


	oss << "PL" << str_mon << str_day << str_hour << str_min << ".csv";
	ofstream ffout(oss.str().c_str());
	ffout << "hello csv";

	//cout <<curr_tm->tm_mon + 1 << curr_tm->tm_mday << curr_tm->tm_hour << curr_tm->tm_min;

}

void read_record()
{

	// File pointer 
	fstream fin;

	// Open an existing file 
	fin.open("hell.csv", ios::in);

	// Get the roll number 
	// of which the data is required 
	int rollnum, roll2, count = 0;
	cout << "Enter the roll number "
		<< "of the student to display details: ";
	cin >> rollnum;

	// Read the Data from the file 
	// as String Vector 
	vector<string> row;
	string line, word, temp;

	while (fin >> temp) {

		row.clear();

		// read an entire row and 
		// store it in a string variable 'line' 
		getline(fin, line);

		// used for breaking words 
		stringstream s(line);

		// read every column data of a row and 
		// store it in a string variable, 'word' 
		while (getline(s, word, ',')) {

			// add all the column data 
			// of a row to a vector 
			row.push_back(word);
		}

		// convert string to integer for comparision 
		roll2 = stoi(row[0]);

		// Compare the roll number 
		if (roll2 == rollnum) {

			// Print the found data 
			count = 1;
			cout << "Details of Roll " << row[0] << " : \n";
			cout << "Name: " << row[1] << "\n";
			cout << "Maths: " << row[2] << "\n";
			cout << "Physics: " << row[3] << "\n";
			cout << "Chemistry: " << row[4] << "\n";
			cout << "Biology: " << row[5] << "\n";
			break;
		}
	}
	if (count == 0)
		cout << "Record not found\n";
}
int main()
{
	//read_record();
	string greeting;
	getline(cin, greeting);
	cout << "console 입력 값:" << greeting << "\n";

	vector<string> ts;
	vector<double> dbl_ts;
	ifstream ifs;
	stringstream s;
	double db;
	ifs.open("csvfile.csv");
	while (!ifs.eof()) {
		getline(ifs, greeting, '\n');
		s << greeting;
		s >> db;
		dbl_ts.push_back(db);
	}
//	ifs.close();

	/*for (auto it = ts.begin(); it != ts.end(); it++)
		cout << *it << endl;
	*/
	
	
	//for (int i = 0; i < ts.size(); i++) {
	//	s << ts[i];
	//	s >> db;
	//	dbl_ts.push_back(db);
	//}

	for (auto it = dbl_ts.begin(); it != dbl_ts.end(); it++)
		cout << *it << endl;


	return 0;

}