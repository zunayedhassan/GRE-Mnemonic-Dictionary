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

#ifndef DICTIONARYFRAME_H_INCLUDED
#define DICTIONARYFRAME_H_INCLUDED

#include <iostream>
#include <climits>
#include <vector>
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/aboutdlg.h>

#include "sqlite3.h"
#include "CommonTools.h"

using namespace std;

class DictionaryFrame : public wxFrame
{
private:
    wxString _path = "data/";
    string _dictionaryFileName = "contents.sqlite";

    CommonTools *_commonTools = new CommonTools;

    string _word;
    string _definition;

    int _wordTitleFontSize = 20;
    int _wordDefinitionBodyFontSize = 13;

    // Saves user's searched words
    vector<string> *_userHistory = new vector<string>;
    int _currentIndex = -1;

    #ifdef _WIN32
        wxFont *titleFont = new wxFont(_wordTitleFontSize, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Cambria");
        wxFont *bodyFont  = new wxFont(_wordDefinitionBodyFontSize, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Cambria");
    #else
        wxFont *titleFont = new wxFont(_wordTitleFontSize, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);
        wxFont *bodyFont  = new wxFont(_wordDefinitionBodyFontSize, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    #endif // _WIN32

    enum
    {
        BORDER      = 3,
        ID_WORDLIST = 101,
        ID_WORDDEF  = 102
    };

    vector<string> *_listOfWords = new vector<string>;

    /**
     *  @brief Initializes program, before showing the GUI
     */
    void _initialize();

    /**
     *  @brief  Shows word definition on rich text format
     *  @param  string lookingFor: The word needed to defined
     *  @param  bool isDisplayOnStatusbar: Shows brief info on statusbar
     *  @return none
     */
    void _displaySelectedWordDetails(string lookingFor, bool isDisplayOnStatusbar = true);

    /**
     *  @brief  Selects word on word list
     *  @param  string word: The word needed to searched
     *  @return none
     */
    void _setSelectionOf(string word);

    /**
     *  @brief  Saves user searched word on history
     *  @param  string selectedWord: The word that user already searched for
     *  @return none
     */
    void _addWordOnHistory(string selectedWord);

    /**
     *  @brief  Manages user history index and enable/disable "Previous" toolbar button
     */
    void _addToUserHistory();

public:
    string WordDefinitionText = "";
    wxToolBar *MainToolbar;
    wxComboBox *searchBox;
    wxListBox *wordList;
    wxRichTextCtrl *wordDefinition;
    wxStatusBar *statusbar;

    /**
     *  @brief Constructor of DictionaryFrame class
     *  @param const wsString &title: Window Title
     */
	DictionaryFrame(const wxString& title);

	/**
	 *  @brief Destructor
     */
	~DictionaryFrame();

    /**
     *  @brief  Perform process while quiting
     *  @param  wxCommandEvent &event
     *  @return none
     */
	void OnQuit(wxCommandEvent &event);

    /**
     *  @brief  Perform process while user type word on search box and hits the "return" key
     *  @param  wxKeyEvent &event
     *  @return none
     */
	void OnWordSearched(wxKeyEvent &event);

    /**
     *  @brief  Perform process while user typing word on search box
     *  @param  wxKeyEvent &event
     *  @return none
     */
	void OnWordTyping(wxKeyEvent &event);

    /**
     *  @brief  Perform process while user selects word (previously typed) from search box
     *  @param  wxCommandEvent &event
     *  @return none
     */
	void OnSearchBoxSelectionChanged(wxCommandEvent &event);

    /**
     *  @brief  Perform process while user selects word from word list
     *  @param  wxCommandEvent &event
     *  @return none
     */
	void OnListItemSelected(wxCommandEvent &event);

    /**
     *  @brief  Perform process while user clicks on "Previous/Back" button
     *  @param  wxCommandEvent &event
     *  @return none
     */
    void OnBackToolbarButtonClicked(wxCommandEvent &event);

    /**
     *  @brief  Perform process while user clicks on "Next/Forward" button
     *  @param  wxCommandEvent &event
     *  @return none
     */
    void OnForwardToolbarButtonClicked(wxCommandEvent &event);

    /**
     *  @brief  Perform process while user clicks on "Save" button
     *  @param  wxCommandEvent &event
     *  @return none
     */
    void OnSaveToolbarButtonClicked(wxCommandEvent &event);

    /**
     *  @brief  Perform process while user clicks on "About" button
     *  @param  wxCommandEvent &event
     *  @return none
     */
    void OnAboutToolbarButtonClicked(wxCommandEvent &event);
};

#endif // DICTIONARYFRAME_H_INCLUDED
