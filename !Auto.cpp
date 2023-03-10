#include <iostream>
#include <fstream>
#include <list>
#include <vector>
using namespace std;

class Automobile
{
protected:
	string name;
	int year;
	float volume;
	double price;
public:
	Automobile(string _name, int _year, float _volume, double _price) :
		name(_name), year(_year), volume(_volume), price(_price) {}
	string GetName() const { return name; }
	unsigned int GetYear() const { return year; }
	float GetVolume() const { return volume; }
	double GetPrice() const { return price; }
	void Show(string msg) const {
		cout << msg << "=> ";
		cout << "Name: " << name << endl;
		cout << "Year: " << year << endl;
		cout << "Volume: " << volume << endl;
		cout << "Price: " << price << endl;
	}
};

class Year
{
private:
	unsigned int year;
public:
	Year(unsigned int _year) : year(_year) {}
	unsigned int GetYear() const throw(const char*)
	{
		if (year < 1950)
			throw "Incorrect year";
		else
			return year;
	}
};

class Volume
{
private:
	float volume;
public:
	Volume(float _volume) : volume(_volume) {}
	float GetVolume() const throw(const char*)
	{
		if (volume < 1.0 || volume>6.0)
			throw "Incorrect volume";
		else
			return volume;
	}
};

class Price
{
private:
	double price;
public:
	Price(double _price) : price(_price) {}
	double GetPrice() const throw(const char*)
	{
		if (price < 0.0)
			throw "Incorrect price";
		else
			return price;
	}
};

class AutomobileSerializable : public Automobile {

public:
	AutomobileSerializable(string _name = "", int _year = 1950, float _volume = 1.0,
		double _price = 0.01)
		: Automobile(_name, _year, _volume, _price) {}

	bool SaveToFile(const char* filename) {
		ofstream outfile(filename, ios::binary);
		if (!outfile.is_open()) {
			cout << "Error opening file for writing: " << filename << endl;
			return 0;
		}
		outfile.write(reinterpret_cast<const char*>(&year), sizeof(year));
		outfile.write(reinterpret_cast<const char*>(&volume), sizeof(volume));
		outfile.write(reinterpret_cast<const char*>(&price), sizeof(price));
		size_t nameLength = name.size();
		outfile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
		outfile.write(name.c_str(), nameLength);
		outfile.close();
		return 1;
	}

	bool LoadFromFile(const char* filename) {
		ifstream infile(filename, ios::binary);
		if (!infile.is_open()) {
			cout << "Error opening file for reading: " << filename << endl;
			return 0;
		}
		unsigned int yearLoaded;
		infile.read(reinterpret_cast<char*>(&yearLoaded), sizeof(yearLoaded));
		year = yearLoaded;

		float volumeLoaded;
		infile.read(reinterpret_cast<char*>(&volumeLoaded), sizeof(volumeLoaded));
		volume = volumeLoaded;

		double priceLoaded;
		infile.read(reinterpret_cast<char*>(&priceLoaded), sizeof(priceLoaded));
		price = priceLoaded;

		size_t nameLength;
		infile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
		name.resize(nameLength);
		infile.read(&name[0], nameLength);
		infile.close();
		return 1;
	}
};

class AutoShow {
private:
	list<AutomobileSerializable> listAuto;
public:
	AutoShow() {}
	void AddAuto(string name, int year, float volume, double price) {
		try {
			Year yr(year);
			Volume vol(volume);
			Price pr(price);
			listAuto.push_back(AutomobileSerializable(name, year, volume, price));
		}
		catch (exception& e) {
			cerr << "Error: " << e.what() << endl;
		}
	}
	bool DelAuto(string name, int year, float volume, double price) {
		try {
			Year yr(year);
			Volume vol(volume);
			Price pr(price);
			for (auto it = listAuto.begin(); it != listAuto.end(); ++it) {
				if (it->GetName() == name && it->GetYear() == year && it->GetVolume() == volume && it->GetPrice() == price) {
					listAuto.erase(it);
					return 1;
				}
			}
			return 0;
		}
		catch (exception& e) {
			cerr << "Error: " << e.what() << endl;
			return 0;
		}
	}
	void Show(string msg) {
		int i = 0;
		for (const auto& obj : listAuto) {
			cout << msg << i + 1 << endl;
			cout << "Name: " << obj.GetName() << endl;
			cout << "Year: " << obj.GetYear() << endl;
			cout << "Volume: " << obj.GetVolume() << endl;
			cout << "Price: " << obj.GetPrice() << endl;
			++i;
		}
	}
	AutomobileSerializable& operator[](int index) {
		auto it = listAuto.begin();
		advance(it, index);
		return *it;
	}

	double GetCostAutoShow() {
		double totalCost = 0;
		for (list<AutomobileSerializable>::const_iterator it = listAuto.begin(); it != listAuto.end(); ++it) {
			totalCost += it->GetPrice();
		}
		return totalCost;
	}

	void ConvertToVector(vector<AutomobileSerializable>& vec) {
		vec = vector<AutomobileSerializable>(listAuto.begin(), listAuto.end());
	}
};

int main()
{
	AutoShow autoShow;

	autoShow.AddAuto("Toyota", 2020, 2.5, 35000);
	autoShow.AddAuto("Honda", 2018, 2.0, 25000);
	autoShow.AddAuto("BMW", 2021, 3.0, 60000);
	autoShow.AddAuto("Mercedes-Benz", 2019, 2.5, 45000);
	autoShow.AddAuto("Ford", 2017, 2.3, 20000);
	autoShow.AddAuto("Chevrolet", 2022, 2.0, 28000);
	autoShow.AddAuto("Nissan", 2021, 2.5, 32000);


	autoShow.Show("Car ");
	cout << endl;

	autoShow[0].SaveToFile("auto1.bin");

	AutomobileSerializable auto1;
	auto1.LoadFromFile("auto1.bin");
	cout << "Read from file: \n";
	auto1.Show("");
	cout << endl;

	vector<AutomobileSerializable> vecAuto;
	autoShow.ConvertToVector(vecAuto);
	cout << "List of automobiles: \n";
	for (const auto& obj : vecAuto) {
		obj.Show("");
	}
	cout << endl;

	double totalCost = autoShow.GetCostAutoShow();
	cout << "Total cost: " << totalCost << " $\n";
}