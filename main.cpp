#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>
#include <sys/stat.h>

using namespace std;

bool FileExists(string file)
{
	struct stat buffer;
	bool success = stat(file.c_str(), &buffer) == 0;
	return success;
}

string GetBackupFilename(string filename)
{
	string result;
	int number = 1;
	do {
		string substr = "_";
		substr.append(to_string(number));
		result = filename;
		result.insert(result.size() -4, substr);
		number++;
	} while (FileExists(result));
	return result;
}

bool IsInStringVector(string s, vector<string> sv)
{
	return find(sv.begin(), sv.end(), s) != sv.end();
}

string ReplaceAll(string str, const string from, const string to, int* count)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
		(*count)++;
	}
	return str;
}

string ReadFileToStringBuffer(string filename)
{
	assert(FileExists(filename));
	fstream src(filename, ios::binary | ios::out | ios::in);
	string buffer;
	buffer.assign((istreambuf_iterator<char>(src)), (istreambuf_iterator<char>()));
	src.close();
	return buffer;
}

int CountSubstrings(const string str, const string sub)
{
	if (sub.length() == 0)
		return 0;
	int count = 0;
	for (size_t offset = str.find(sub); offset != string::npos; offset = str.find(sub, offset + sub.length()))
	{
		++count;
	}
	return count;
}

int main(int argc, char** argv)
{
	string selfLocation = argv[0];
	vector<string> args;

	// Needed information
	string readFile = "";
	string writeFile = "";
	string stringToReplace = "";
	string replacementString = "";

	// Optional information
	string backupFile = "";

	// Get vector of all arguments => args
	for (int i = 1; i < argc; i++)
	{
		args.push_back(argv[i]);
	}
	for (size_t i = 0; i < args.size(); i++)
	{
		cout << args[i] << " ";
	}
	cout << endl;

	bool success = true;
	string error = "";

	// Make checks here. If something went wrong, change success to false
	if (args.size() >= 3)
	{
		// Check that the file to read exists
		readFile = args[0];
		writeFile = readFile;
		backupFile = GetBackupFilename(readFile);;

		if (!FileExists(readFile))
		{
			error = "Couldn't find file";
			success = false;
		}
		if (success)
		{
			// Checks for string to be replaced and replacement string
			stringToReplace = args[1];
			if (stringToReplace.empty())
			{
				error = "String to replace missing";
				success = false;
			}
			if (success)
			{
				error = "Replacement string missing";
				replacementString = args[2];
				if (replacementString.empty())
					success = false;
			}
		}
	}
	else
	{
		error = "Not enough arguments";
		success = false;
	}

	if (success)
	{
		string buffer = ReadFileToStringBuffer(readFile);

		// -nb => No backup
		if (!IsInStringVector("-nb", args))
		{
			ofstream dst(backupFile, ios::binary);
			dst << buffer;
			dst.close();
		}

		// -t => Test run, no changes made
		int occurrences = 0;
		if (!IsInStringVector("-t", args))
		{
			buffer = ReplaceAll(buffer, stringToReplace, replacementString, &occurrences);
		}
		else
			occurrences = CountSubstrings(buffer, stringToReplace);

		ofstream dst(writeFile);
		dst << buffer;
		dst.close();

		// -v => Verbose output for more information
		if (IsInStringVector("-v", args))
		{
			cout << "Arguments given: " << args.size() << endl;
			cout << "Arguments: ";
			for (auto it = args.begin(); it != args.end(); it++)
			{
				cout << *it << " ";
			}
			cout << endl;
			cout << "Replaced " << occurrences << " entries." << endl;
		}
	}
	else
	{
		cout << error << endl << endl;
		// Show help, because command probably wasn't invoked correctly
		cout << "Usage: " << argv[0] << " [file] [string] [replacement] <optional params>" << endl << endl;
		cout << "Optional parameters:" << endl;
		cout << "\t-t\tTest run, don't make any changes-" << endl;
		cout << "\t-nb\tNo backup. Skips making backup file before making changes." << endl;
		cout << "\t-v\tVerbose. Gives some extra information about what happens during the process." << endl;
	}
	return 0;
}
