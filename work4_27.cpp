#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;
class Date
{
public:
	int GetMonthDay(int year, int month)
	{
		static int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
		{
			return 29;
		}
		return days[month];
		
	}
	//构造函数
	Date(int year = 0, int month = 1, int day = 1)
	{
		if (year >= 0 && month > 0 && month <= 12 && day > 0 && day <= GetMonthDay(year, month))
		{
			_year = year;
			_month = month;
			_day = day;
		}
		else
		{
			cout << "非法日期,已重置" << endl;
			_year = 1970;
			_month = 1;
			_day = 1;
		}
	}
	//拷贝构造
	Date(Date& d)
	{
		_year = d._year;
		_month = d._month;
		_day = d._day;
	}
	void Print()
	{
		cout << _year << "-" << _month << "-" << _day << endl;
	}
	//操作符重载 判断
	bool operator==(const Date&d)
	{
		return _year == d._year
			&& _month == d._month
			&& _day == d._day;
	}
	bool operator>(const Date& d)
	{
		if (_year > d._year)
			return true;
		else if (_year == d._year && _month > d._month)
			return true;
		else if (_year == d._year && _month == d._month && _day > d._day)
			return true;
		else
			return false;
	}
	bool operator<(const Date& d)
	{
		return !((*this>d) || (*this == d));
	}
	bool operator>=(const Date& d)
	{
		return !(*this < d);
	}
	bool operator<=(const Date& d)
	{
		return !(*this > d);
	}
	//日期的加减
	Date operator+(int day)
	{
		Date ret(*this);
		ret._day += day;
		//日期不合法时，当日进月
		while (ret._day > GetMonthDay(ret._year, ret._month))
		{
			ret._day -= GetMonthDay(ret._year, ret._month);
			ret._month++;
			//当月不合法时，进年
			if (ret._month == 13)
			{
				ret._year++;
				ret._month = 1;
			}
		}
		return ret;
	}
	Date& operator+=(int day)
	{
		if (day < 0)
		{
			return *this -= (-day);
		}
		_day += day;
		while (_day > GetMonthDay(_year, _month))
		{
			_day -= GetMonthDay(_year, _month);
			_month++;
			//当月不合法时，进年
			if (_month == 13)
			{
				_year++;
				_month = 1;
			}
		}
		return *this;
	}
	//复用
	/*Date operator+(int day)
	{
		Date tmp(*this);
		tmp += day;
		return tmp;
	}*/
	Date operator-(int day)
	{
		Date ret(*this);
		ret._day -= day;
		//日期不合法时，向月借天数
		while (ret._day <= 0)
		{
			--ret._month;
			//当月不合法时，向年借
			if (ret._month == 0)
			{
				ret._year--;
				ret._month = 12;
			}
			ret._day += GetMonthDay(ret._year, ret._month);
		}
		return ret;
	}
	Date& operator-=(int day)
	{
		if (day < 0)
		{
			return *this += (-day);
		}
		_day -= day;
		while (_day <= 0)
		{
			--_day;
			if (_month == 0)
			{
				--_year;
				_month = 12;
			}
			_day += GetMonthDay(_year, _month);
		}
		return *this;
	}
	Date& operator++()
	{
		return *this += 1;
	}
	Date operator++(int)
	{
		Date tmp(*this);
		*this += 1;
		return tmp;
	}
	Date& operator--()
	{
		return *this -= 1;
	}
	Date operator--(int)
	{
		Date tmp(*this);
		*this -= 1;
		return tmp;
	}
	//日期减日期
	int operator-(const Date& d)
	{
		Date tmp(*this);
		tmp -= d._day;
		tmp._month -= d._month;
		while (tmp._month <= 0)
		{
			tmp._year--;
			tmp._month += 12;
		}
		tmp._year -= d._year;
		int daysum = _day;
		while (--tmp._month > 0)
		{
			daysum = daysum + GetMonthDay(tmp._year, tmp._month);
		}
		if ((tmp._year % 4 == 0 && tmp._year % 100 != 0) || (tmp._year % 400 == 0))
			return tmp._year * 366 + daysum;
		else
			return tmp._year * 365 + daysum;
	}
private:
	int _year;
	int _month;
	int _day;
};

int main()
{
	/*Date d1(2001,4,27);*/
	/*Date d2(d1);
	Date d3(2002, 4.27);
	Date d4(2001, 4, 32);
	d1.Print();
	d2.Print();
	cout << (d1 = d2) << endl;
	cout << (d1 > d2) << endl;
	cout << (d3 > d2) << endl;
	cout << (d3 < d2) << endl;
	d4.Print();*/
	Date d1(2001, 4, 9);
	Date d2(2001, 4, 8);
	cout << d1 - d2 << endl;
	d1.Print();
	return 0;
}