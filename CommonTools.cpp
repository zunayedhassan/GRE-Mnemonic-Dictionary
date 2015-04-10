/**
 *  Class Name:     GRE Mnemonic Dictionary
 *
 *  Version:        1.0
 *  Date:           April 10, 2015
 *
 *  Author:         Mohammad Zunayed Hassan
 *  Email:          zunayed-hassanBD@live.com
 *                  ZunayedHassanBD@gmail.com
 *
 *  Last Updated:
 *
 *  Changes History:
 */

#include "CommonTools.h"

vector<string>* CommonTools::GetFilesAndFolderListFrom(string directoryName)
{
    vector<string> *fileList = new vector<string>;

    DIR *currentDirectory = nullptr;
    struct dirent *directoryAndFileInfo = nullptr;

    currentDirectory = opendir(directoryName.data());

    if (currentDirectory)
    {
        while ((directoryAndFileInfo = readdir(currentDirectory)) != nullptr)
        {
            fileList->push_back(directoryAndFileInfo->d_name);
        }

        closedir(currentDirectory);
    }
    else
    {
        cout << "[!] ERROR: Can not the read specified directory" << endl;

        return nullptr;
    }

    return fileList;
}

string CommonTools::Trim(string text)
{
    string line = "";

    int cutFromHead = 0;

    for (int character = 0; character < text.length(); character++)
    {
        if (text[character] == ' ')
        {
            ++cutFromHead;
        }
        else
        {
            break;
        }
    }

    int cutFromTail = text.length();

    for (int character = text.length() - 1; character >= 0; character--)
    {
        if (text[character] == ' ')
        {
            --cutFromTail;
        }
        else
        {
            break;
        }
    }

    for (int character = cutFromHead; character < cutFromTail; character++)
    {
        line += text[character];
    }

    return line;
}

fstream* CommonTools::GetFileStream(string filename)
{
    fstream *file = new fstream;
    file->open(filename, ios_base::in);

    if (!file->is_open())
    {
        cout << "[!] ERROR: Unable to open the file" << endl;

        // Terminate the program
        exit(EXIT_FAILURE);
    }

    return file;
}

string CommonTools::ToLower(string data)
{
    transform(data.begin(), data.end(), data.begin(), ::tolower);
    return data;
}

string::size_type CommonTools::GetSubstringPosition(string haystack, string niddle, bool isCaseSensitive)
{
    if (!isCaseSensitive)
    {
        haystack = ToLower(haystack);
        niddle = ToLower(niddle);
    }

    string::size_type position = haystack.find(niddle);

    if ((position < 0) || (position >= haystack.length()))
    {
        return -1;
    }

    return position;
}

string CommonTools::GetSplitedStringFrom(string text, int position)
{
    string newText = "";

    if (position <= text.length() - 1)
    {
        for (int character = position; character < text.length(); character++)
        {
            newText += text[character];
        }

        return newText;
    }

    return newText;
}

string CommonTools::GetSplitedStringFromReversed(string text, int position)
{
    string newText = "";

    if (position <= text.length() - 1)
    {
        for (int character = 0; character < position; character++)
        {
            newText += text[character];
        }

        return newText;
    }

    return newText;
}


vector<string>* CommonTools::GetSplittedStrings(string text, char delimeter)
{
    text += delimeter;

    string word = "";
    vector<string> *words = new vector<string>;

    for (int character = 0; character < text.length(); character++)
    {
        if (text[character] != delimeter)
        {
            word += text[character];
        }
        else
        {
            words->push_back(word);
            word = "";
        }
    }

    return words;
}

void CommonTools::WriteToFile(string filename, string text, bool isAppend)
{
    // Write to a file
    ofstream *fout;

    if (isAppend)
    {
        fout = new ofstream(filename, ios_base::app);
    }
    else
    {
        fout = new ofstream(filename);
    }

    if (!*fout)
    {
        cout << "[!] ERROR: Can not write output file." << endl;
        exit(EXIT_FAILURE);
    }

    (*fout) << text << endl;
    fout->close();
}

string CommonTools::GetStringFromNumber(double number)
{
    ostringstream convert;
    convert << number;

    return convert.str();
}
