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

#ifndef COMMONTOOLS_H_INCLUDED
#define COMMONTOOLS_H_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <dirent.h>

using namespace std;

class CommonTools
{
public:
    /**
     *  @brief  Find all the file and folder on specified directory
     *  @param  string directoryName: Path
     *  @return vector<string>*: All the file and folder name as in list of strings
     */
    vector<string>* GetFilesAndFolderListFrom(string directoryName);

    /**
     *  @brief  Removes whitespace from string
     *  @param  string text: Text to be trimmed
     *  @return string: text after trimmed
     */
    string Trim(string text);

    /**
     *  @brief  Get file stream for reading file
     *  @param  string filename: Filename
     *  @return fstream*: The file stream
     */
    fstream* GetFileStream(string filename);

    /**
     *  @brief  Converts upper case to lower from string
     *  @param  string data: Text to be lowered
     *  @return string: text lower cased
     */
    string ToLower(string data);

    /**
     *  @brief  Search substring with the given string
     *  @param  string haystack: The text where we will perform searching
     *  @param  string niddle: Substring to be searched for
     *  @param  bool isCaseSensitive: Is case sensitivity going to be ignored?
     *  @return string::size_type: Starting position of the substring
     */
    string::size_type GetSubstringPosition(string haystack, string niddle, bool isCaseSensitive = true);

    /**
     *  @brief  Cut string from the head
     *  @param  string text: Text to be splitted
     *  @param  int position: Position of the string, where it will cut
     *  @return string: String after the cut
     */
    string GetSplitedStringFrom(string text, int position);

    /**
     *  @brief  Cut string from the tail
     *  @param  string text: Text to be splitted
     *  @param  int position: Position of the string, where it will cut
     *  @return string: String after the cut
     */
    string GetSplitedStringFromReversed(string text, int position);

    /**
     *  @brief  Split string based on given character
     *  @param  string text: Text to be splitted
     *  @param  char delimeter: String to be splitted based on character
     *  @return vector<string>*: List of strings to be cut
     */
    vector<string>* GetSplittedStrings(string text, char delimeter);

    /**
     *  @brief  Write text to the given file
     *  @param  string filename: The file name where text will be written on
     *  @param  string text: Text to be written
     *  @param  bool isAppend: Is it going to be append or overwrite the file?
     *  @return none
     */
    void WriteToFile(string filename, string text, bool isAppend = false);

    /**
     *  @brief  Convert number to a string
     *  @param  double number: Number to be converted to string
     *  @return string: Converted string from the given number
     *
     *  @example 1.234 -> "1.234"
     */
    string GetStringFromNumber(double number);
};

#endif // COMMONTOOLS_H_INCLUDED
