// -i Игнорировать регистр символов при сравнении (✓)
// -E Игнорировать различия между табуляцией и пробелами (✓)
// -Z Игнорировать разницу в завершающих пробелах (✓)
// -b Игнорировать различия в количестве пробелов (✓)
// -w Игнорировать все пробельные символы (✓)
// -B Игнорировать пустые строки (✓)
// -t Преобразовать табуляции в пробелы в выводе (✓)

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cctype>

void parse(int argc, char* argv[], std::unordered_map<std::string, bool> & keys, std::string & file1, std::string & file2)
{
	std::vector<std::string> arguments;

	for (int i = 1; i < argc; ++i) 
		arguments.push_back(argv[i]);

	int i = 0;

	while (i < arguments.size() && arguments[i].substr(0, 1) == "-")
	{
		std::string key = arguments[i]; 

		if (keys.find(key) != keys.end())
			keys[key] = true; 
		else
		{
			std::cerr << "Unknown key: " << key << std::endl;
			exit(1);
		}
		++i;
	}

	if (arguments.size() - i < 2)
	{
		std::cerr << "Usage: ziff [-i] [-E] [-Z] [-b] [-w] [-B] [-t] <file1> <file2>" << std::endl;
		exit(1);
	}

	file1 = arguments[i];
	file2 = arguments[i + 1];
}

std::vector<int> dp(const std::vector<std::string> &lines1, const std::vector<std::string> &lines2)
{
	int len1 = lines1.size(), len2 = lines2.size();
	std::vector<int> dp(len2 + 1, 0);

	for (int i = 1; i < len1 + 1; ++i)
	{
		int diag = 0;
		for (int j = 1; j < len2 + 1; ++j)
		{
			int pred_dp = dp[j];
			if (lines1[i - 1] == lines2[j - 1])
			{
				dp[j] = diag + 1;
			}
			else
			{
				dp[j] = std::max(dp[j - 1], dp[j]);
			}
			diag = pred_dp;
		}
	}
	return dp;
}

std::vector<std::string> LCS(const std::vector<std::string> &lines1, const std::vector<std::string> &lines2, std::vector<int> &left_lines1, std::vector<int> &right_lines1)
{
	int len1 = lines1.size(), len2 = lines2.size();

	if (len1 == 0 || len2 == 0) return {};

	if (len1 == 1)
	{
		if (std::find(lines2.begin(), lines2.end(), lines1[0]) != lines2.end()) return { lines1[0] };
		return {};
	}

	if (len2 == 1)
	{
		if (std::find(lines1.begin(), lines1.end(), lines2[0]) != lines1.end()) return { lines2[0] };
		return {};
	}

	int middle1 = len1 / 2;

	std::vector<std::string> reverse1 = std::vector<std::string>(lines1.begin() + middle1, lines1.end());
	std::vector<std::string> reverse2 = lines2;

	std::reverse(reverse1.begin(), reverse1.end());
	std::reverse(reverse2.begin(), reverse2.end());

	left_lines1 = dp(std::vector<std::string>(lines1.begin(), lines1.begin() + middle1), lines2);
	right_lines1 = dp(reverse1, reverse2);

	int middle2 = 0;
	for (int i = 0; i <= len2; ++i)
	{
		if (left_lines1[i] + right_lines1[len2 - i] > left_lines1[middle2] + right_lines1[len2 - middle2])
			middle2 = i;
	}

	std::vector<std::string> result;
	std::vector<std::string> left_LCS = LCS(std::vector<std::string>(lines1.begin(), lines1.begin() + middle1), std::vector<std::string>(lines2.begin(), lines2.begin() + middle2), left_lines1, right_lines1);
	std::vector<std::string> right_LCS = LCS(std::vector<std::string>(lines1.begin() + middle1, lines1.end()), std::vector<std::string>(lines2.begin() + middle2, lines2.end()), left_lines1, right_lines1);

	result.insert(result.end(), left_LCS.begin(), left_LCS.end());
	result.insert(result.end(), right_LCS.begin(), right_LCS.end());

	return result;
}

void diff(const std::vector<std::string> &lines1, const std::vector<std::string> &lines2, const std::unordered_map<std::string, bool> & keys)
{
	std::vector<int> left_lines1, right_lines1;
	std::vector<std::string> low_lines1 = lines1;
	std::vector<std::string> low_lines2 = lines2;

	if (keys.at("-i"))
	{
		for (std::string& line : low_lines1)
			std::transform(line.begin(), line.end(), line.begin(), ::tolower);

		for (std::string& line : low_lines2)
			std::transform(line.begin(), line.end(), line.begin(), ::tolower);
	}

	if (keys.at("-E"))
	{
		for (std::string& line : low_lines1)
		{
			std::string beztab;
			for (char c : line)
			{
				if (c == '\t')
					beztab.append(8, ' ');
				else
					beztab += c;
			}
			line = beztab;
		}

		for (std::string& line : low_lines2)
		{
			std::string beztab;
			for (char c : line)
			{
				if (c == '\t')
					beztab.append(8, ' ');
				else
					beztab += c;
			}
			line = beztab;
		}
	}

	if (keys.at("-Z"))
	{
		for (std::string& line : low_lines1)
		{
			int pos = line.find_last_not_of(" \t");

			if (pos != std::string::npos)
				line = line.substr(0, pos + 1);
			else
				line.clear();
		}

		for (std::string& line : low_lines2)
		{
			int pos = line.find_last_not_of(" \t");

			if (pos != std::string::npos)
				line = line.substr(0, pos + 1);
			else
				line.clear();
		}
	}

	if (keys.at("-b"))
	{
		for (std::string& line : low_lines1)
		{
			std::string indprob;
			bool podryad = false;

			for (char c : line)
			{
				if (c == ' ')
				{
					if (!podryad)
					{
						indprob += ' ';
						podryad = true;
					}
				}
				else
				{
					indprob += c;
					podryad = false;
				}
			}
			line = indprob;
		}

		for (std::string& line : low_lines2)
		{
			std::string indprob;
			bool podryad = false;

			for (char c : line)
			{
				if (c == ' ')
				{
					if (!podryad)
					{
						indprob += ' ';
						podryad = true;
					}
				}
				else
				{
					indprob += c;
					podryad = false;
				}
			}
			line = indprob;
		}
	}

	if (keys.at("-w"))
	{
		for (std::string& line : low_lines1)
		{
			std::string bezprob; 

			for (char c : line)
			{
				if (!std::isspace(c))  
					bezprob += c;
			}
			line = bezprob;
		}

		for (std::string& line : low_lines2)
		{
			std::string bezprob;

			for (char c : line)
			{
				if (!std::isspace(c)) 
					bezprob += c;
			}
			line = bezprob;
		}
	}

	std::vector<std::string> lcs = LCS(low_lines1, low_lines2, left_lines1, right_lines1);

	int i = 0, j = 0, k = 0;

	while (i < lines1.size() || j < lines2.size())
	{
		if (k < lcs.size() && i < lines1.size() && low_lines1[i] == lcs[k] && j < lines2.size() && low_lines2[j] == lcs[k])
		{
			std::string tabline = lines1[i];

			if (keys.at("-t")) 
			{
				for (char & c : tabline)
				{
					if (c == '\t')	c = ' '; 
				}
			}

			if (!(keys.at("-B") && tabline.empty()))
			{
				std::cout << "  " << tabline << std::endl;
			}
			++i;
			++j;
			++k;
		}
		else if (i < lines1.size() && (k >= lcs.size() || low_lines1[i] != lcs[k]))
		{
			std::string tabline = lines1[i];

			if (keys.at("-t"))
			{
				for (char & c : tabline)
				{
					if (c == '\t')	c = ' ';
				}
			}

			if (!(keys.at("-B") && tabline.empty()))
			{
				std::cout << "- " << tabline << std::endl;
			}
			++i;
		}
		else if (j < lines2.size() && (k >= lcs.size() || low_lines2[j] != lcs[k]))
		{
			std::string tabline = lines2[j];

			if (keys.at("-t"))
			{
				for (char & c : tabline)
				{
					if (c == '\t')	c = ' ';
				}
			}

			if (!(keys.at("-B") && tabline.empty()))
			{
				std::cout << "+ " << tabline << std::endl;
			}
			++j;
		}
	}
}

void readFile(const std::string &filename, std::vector<std::string> &lines)
{
	std::ifstream file(filename);
	std::string line;

	while (std::getline(file, line))
	{
		lines.push_back(line);
	}
}

int main(int argc, char* argv[])
{
	std::unordered_map<std::string, bool> keys = 
	{
		{"-i", false},
		{"-E", false},
		{"-Z", false},
		{"-b", false},
		{"-w", false},
		{"-B", false},
		{"-t", false}
	};
	
	std::string file1;
	std::string file2;

	parse(argc, argv, keys, file1, file2);

	std::vector<std::string> lines1, lines2;

	readFile(file1, lines1);
	readFile(file2, lines2);

	diff(lines1, lines2, keys);

	return 0;
}

// cd C:\...\...\source\repos\Project\Project

// cl /EHsc ziff.cpp

// ziff file1.txt file2.txt
