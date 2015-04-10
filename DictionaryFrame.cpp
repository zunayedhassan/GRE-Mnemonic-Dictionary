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

#include "DictionaryFrame.h"

DictionaryFrame::DictionaryFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    // Setting upp application icon
	#if defined(__WXMSW__)
        // If Windows
		this->SetIcon(wxICON(progicon));
	#else
        // For other operating systems
        this->SetIcon(wxIcon(wxT("icon.ico"), wxBITMAP_TYPE_ICO));
	#endif

	wxBoxSizer *sizermain = new wxBoxSizer(wxVERTICAL);

    // Setting up toolbar buttons
	wxImage::AddHandler(new wxPNGHandler);

    wxBitmap *backIcon  = new wxBitmap(this->_path + wxT("Gnome-Go-Previous-32.png"  ),  wxBITMAP_TYPE_PNG);
	wxBitmap *nextIcon  = new wxBitmap(this->_path + wxT("Gnome-Go-Next-32.png"      ),  wxBITMAP_TYPE_PNG);
	wxBitmap *saveIcon  = new wxBitmap(this->_path + wxT("Gnome-Document-Save-32.png"),  wxBITMAP_TYPE_PNG);
	wxBitmap *aboutIcon = new wxBitmap(this->_path + wxT("Gnome-Help-About-32.png"   ),  wxBITMAP_TYPE_PNG);

	this->MainToolbar = new wxToolBar(this, wxID_ANY);
	this->MainToolbar->AddTool(wxID_BACKWARD, wxT("Previous"), *backIcon );
    this->MainToolbar->AddTool(wxID_FORWARD,  wxT("Next"),     *nextIcon );
    this->MainToolbar->AddTool(wxID_SAVE,     wxT("Save"),     *saveIcon );
    this->MainToolbar->AddTool(wxID_ABOUT,    wxT("About"),    *aboutIcon);

    this->MainToolbar->SetToolShortHelp(wxID_BACKWARD, wxT("Previous Word"                    ));
    this->MainToolbar->SetToolShortHelp(wxID_FORWARD,  wxT("Next Word"                        ));
    this->MainToolbar->SetToolShortHelp(wxID_SAVE,     wxT("Save Word Definition as Text File"));
    this->MainToolbar->SetToolShortHelp(wxID_ABOUT,    wxT("About"                            ));

	this->MainToolbar->Realize();

	sizermain->Add(this->MainToolbar, 0, wxEXPAND);

    // Adding a splitted window
	wxSplitterWindow *splittermain = new wxSplitterWindow(this, wxID_ANY);
	splittermain->SetSashGravity(0.3);
    sizermain->Add(splittermain, 1, wxEXPAND | wxALL, 0);

    wxBoxSizer *leftPanelBoxSizer = new wxBoxSizer(wxVERTICAL);

    wxPanel *leftPanel = new wxPanel(splittermain, wxID_ANY);
    leftPanel->SetSizer(leftPanelBoxSizer);

    // Search box
    wxString searchHistory[] = { };
    this->searchBox = new wxComboBox(leftPanel, wxID_FIND, wxT(""), wxPoint(0, 0), wxSize(-1, -1), 0, searchHistory, wxCB_DROPDOWN | wxTE_PROCESS_ENTER);
    searchBox->SetHint(wxT("Search"));
    leftPanelBoxSizer->Add(this->searchBox, 0, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, BORDER);

    // Word list
    this->wordList = new wxListBox(leftPanel, ID_WORDLIST);
    leftPanelBoxSizer->Add(this->wordList, 1, wxEXPAND | wxALL, BORDER);

    wxBoxSizer *rightPanelBoxSizer = new wxBoxSizer(wxVERTICAL);

    wxPanel *rightPanel = new wxPanel(splittermain, wxID_ANY);
    rightPanel->SetSizer(rightPanelBoxSizer);

    // Shows word definition in rich text format
    this->wordDefinition = new wxRichTextCtrl(rightPanel, ID_WORDDEF);
    this->wordDefinition->SetEditable(false);
    rightPanelBoxSizer->Add(this->wordDefinition, 1, wxEXPAND | wxALL, BORDER);

    splittermain->SplitVertically(leftPanel, rightPanel);

    // Statusbar
    this->statusbar = new wxStatusBar(this, wxID_ANY);
    this->statusbar->SetStatusText(wxT("Program started..."));
    sizermain->Add(this->statusbar, 0, wxEXPAND | wxLEFT | wxBottom | wxRIGHT, 0);


	this->SetSizer(sizermain);

	this->_initialize();

	// Adding Events
	this->Connect(wxID_FIND, wxEVT_TEXT_ENTER, wxKeyEventHandler(DictionaryFrame::OnWordSearched));
	this->Connect(wxID_FIND, wxEVT_TEXT, wxKeyEventHandler(DictionaryFrame::OnWordTyping));
	this->Connect(wxID_FIND, wxEVT_COMBOBOX, wxCommandEventHandler(DictionaryFrame::OnSearchBoxSelectionChanged));
	this->Connect(ID_WORDLIST, wxEVT_LISTBOX, wxCommandEventHandler(DictionaryFrame::OnListItemSelected));
	this->Connect(wxID_BACKWARD, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DictionaryFrame::OnBackToolbarButtonClicked));
	this->Connect(wxID_FORWARD, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DictionaryFrame::OnForwardToolbarButtonClicked));
	this->Connect(wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DictionaryFrame::OnSaveToolbarButtonClicked));
	this->Connect(wxID_ABOUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DictionaryFrame::OnAboutToolbarButtonClicked));
}

DictionaryFrame::~DictionaryFrame()
{
    // Freeing memory
    delete this->_listOfWords;
    delete this->_userHistory;
}

void DictionaryFrame::_initialize()
{
    sqlite3 *db;
    sqlite3_stmt *res;
    char *err_msg = 0;

    // Connecting to sqlite database
    int rc = sqlite3_open(this->_dictionaryFileName.data(), &db);

    if (rc != SQLITE_OK)
    {
        // Shows error message if failed to connect
        string errorMessage = "ERROR: Cannot open database:\n\n" + string(sqlite3_errmsg(db));

        wxMessageDialog *dial = new wxMessageDialog(NULL, errorMessage, wxT("Error"), wxOK | wxICON_ERROR);
        dial->ShowModal();

        // Exit the application along with database
        sqlite3_close(db);

        this->Close(true);
        exit(EXIT_FAILURE);
    }

    // Query for list of words as in ascending order for word list
    string sql = "SELECT word FROM dictionary ORDER BY word ASC;";
    rc = sqlite3_prepare_v2(db, sql.data(), -1, &res, 0);

    // If query dosn't work on table in dictionary database
    if (rc != SQLITE_OK)
    {
        // Show error message
        string errorMessage = "ERROR: Failed to select data.\n\n" + string(err_msg);

        wxMessageDialog *dial = new wxMessageDialog(NULL, errorMessage, wxT("Error"), wxOK | wxICON_ERROR);
        dial->ShowModal();

        sqlite3_free(err_msg);

        // Exit the application along with database
        sqlite3_close(db);

        this->Close(true);
        exit(EXIT_FAILURE);
    }

    // Read every single row from query result and add every word on word list
    while ((rc = sqlite3_step(res)) == SQLITE_ROW)
    {
        this->_listOfWords->push_back(reinterpret_cast<const char*> (sqlite3_column_text(res, 0)));
    }

    // Finalize and close the database
    sqlite3_finalize(res);
    sqlite3_close(db);

    // If database was not empty and actually sends word list, then
    if (this->_listOfWords->size() > 0)
    {
        // Saves all the word list on an array, this is required for the list box (wordlist)
        wxString wordlist[this->_listOfWords->size()];

        int index = 0;

        for (vector<string>::iterator word = this->_listOfWords->begin(); word != this->_listOfWords->end(); ++word)
        {
            wordlist[index++] = *word;
        }

        this->wordList->InsertItems(this->_listOfWords->size(), wordlist, 0);

        // Shows brief info on statusbar
        this->statusbar->SetStatusText(wxT("Total " + this->_commonTools->GetStringFromNumber(this->_listOfWords->size()) + " words loaded"));

        // Shows the first word on the dictionary on rich text ctrl
        this->_displaySelectedWordDetails(this->_listOfWords->at(0), false);
    }

    // At first both of the back and next button will be disabled, because of there is no history on user typing yet
    this->MainToolbar->EnableTool(wxID_BACKWARD, false);
    this->MainToolbar->EnableTool(wxID_FORWARD, false);
}

void DictionaryFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
    // Just quit the program
	this->Close(true);
}

void DictionaryFrame::OnWordSearched(wxKeyEvent &event)
{
    // If user search for a word from search box
    string typedWord = this->_commonTools->Trim((const char*) (this->searchBox->GetValue()).mb_str());

    // If the search box is not blank
    if (typedWord != "")
    {
        // Find that word and show that on rich text ctrl
        this->_displaySelectedWordDetails(typedWord);

        // Highlight that word on wordlist box
        this->_setSelectionOf(this->_word);

        // Add that word on history
        this->_addWordOnHistory(this->_word);

        // If possible, then enable previous/next buttons
        this->_addToUserHistory();
    }
}

void DictionaryFrame::OnWordTyping(wxKeyEvent &event)
{
    // While user typing on search box, if search box history is not empty
    if (this->searchBox->GetCount() > 0)
    {
        // Search on search box history
        int totalItem = this->searchBox->GetCount();

        for(int index = 0; index < totalItem; index++)
        {
            // If that word is already searched then select that word
            if (this->searchBox->GetString(index) == this->searchBox->GetValue())
            {
                this->searchBox->SetSelection(index);
                break;
            }
        }
    }
}

void DictionaryFrame::OnSearchBoxSelectionChanged(wxCommandEvent &event)
{
    // If user selects word from search box history
    string selectedWord = (const char *) (this->searchBox->GetString(this->searchBox->GetSelection())).mb_str();

    // Search that word on database and display the word definition on rich text ctrl
    this->_displaySelectedWordDetails(selectedWord);

    // Also, highlight on wordlist box
    this->_setSelectionOf(selectedWord);
}

void DictionaryFrame::OnListItemSelected(wxCommandEvent &event)
{
    // If user selects word from wordlist box
    string selectedWord = this->_listOfWords->at(this->wordList->GetSelection());

    // Add that word to user history
    this->_addWordOnHistory(selectedWord);

    // Search for that word and display it on rich text ctrl
    this->_displaySelectedWordDetails(selectedWord);

    // Also show that word on search box
    this->searchBox->SetValue(selectedWord);

    // Enable previous/next toolbar button if possible
    this->_addToUserHistory();
}

void DictionaryFrame::OnBackToolbarButtonClicked(wxCommandEvent &event)
{
    // If user clicked on previous/back toolbar button
    // Update user history index
    --this->_currentIndex;

    // Search for previously selected word and display that on rich text ctrl
    this->_displaySelectedWordDetails(this->_userHistory->at(this->_currentIndex));

    // Also make that word highlight on wordlist box
    this->_setSelectionOf(this->_userHistory->at(this->_currentIndex));

    // Show that word on search box
    this->searchBox->SetValue(this->_userHistory->at(this->_currentIndex));

    // If search index is at the boundary, then one of the button has to be disabled
    if (this->_currentIndex < 1)
    {
        this->MainToolbar->EnableTool(wxID_BACKWARD, false);
    }

    if (this->_userHistory->size() > 1)
    {
        this->MainToolbar->EnableTool(wxID_FORWARD, true);
    }
}

void DictionaryFrame::OnForwardToolbarButtonClicked(wxCommandEvent &event)
{
    // If user clicked on forward/next button
    ++this->_currentIndex;

    // Find that word on database and display that on rich text ctrl
    this->_displaySelectedWordDetails(this->_userHistory->at(this->_currentIndex));

    // Make it highlight on wordlist box
    this->_setSelectionOf(this->_userHistory->at(this->_currentIndex));

    // Display that word on search box too
    this->searchBox->SetValue(this->_userHistory->at(this->_currentIndex));

    // If user history index is at the boundary, then one of the previous/next button has to be disabled
    if (this->_currentIndex >= this->_userHistory->size() - 1)
    {
        this->MainToolbar->EnableTool(wxID_FORWARD, false);
    }

    if (this->_userHistory->size() >= this->_userHistory->size())
    {
        this->MainToolbar->EnableTool(wxID_BACKWARD, true);
    }
}

void DictionaryFrame::OnSaveToolbarButtonClicked(wxCommandEvent &event)
{
    // If user clicked on save button, then show a file dialog for where to be file saved
    wxFileDialog *saveFileDialog = new wxFileDialog(this, wxT("Save *.txt file"), "", "", "Text files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    // If user cancels, then do ignore the whole process
    if (saveFileDialog->ShowModal() == wxID_CANCEL)
    {
        return;
    }

    // Otherwise, save the text file on specified location
    wxString fileName = saveFileDialog->GetPath();
    this->_commonTools->WriteToFile((const char *) fileName.mb_str(), this->WordDefinitionText);

    // Update the brief info on statusbar
    this->statusbar->SetStatusText("File saved to " + fileName);
}

void DictionaryFrame::OnAboutToolbarButtonClicked(wxCommandEvent &event)
{
    // If user clicked on about toolbar button, the display information about the program
    wxAboutDialogInfo *aboutInfo = new wxAboutDialogInfo;
    aboutInfo->SetName("GRE Mnemonic Dictionary");
    aboutInfo->SetVersion("1.0");
    aboutInfo->SetDescription(_("An useful dictionary for GRE students !!!"));
    aboutInfo->SetCopyright("(C) 2015");
    aboutInfo->SetLicence("GNU GPLv3");
    aboutInfo->AddDeveloper("Mohammad Zunayed Hassan\nEmail: zunayed-hassan@live.com");
    aboutInfo->AddArtist("GNOME icon artists\nhttp://gnome.org");
    wxAboutBox(*aboutInfo);
}

void DictionaryFrame::_displaySelectedWordDetails(string lookingFor, bool isDisplayOnStatusBar)
{
    // If user wants to look for a word definition on database
    if (isDisplayOnStatusBar)
    {
        // Update the brief on statusbar, if required
        this->statusbar->SetStatusText("Looking for... \"" + lookingFor + "\" on dictionary");
    }

    sqlite3 *db;
    sqlite3_stmt *res;
    char *err_msg = 0;

    // Connect on database
    int rc = sqlite3_open(this->_dictionaryFileName.data(), &db);

    // If failed to connect
    if (rc != SQLITE_OK)
    {
        // Show error message
        string errorMessage = "ERROR: Cannot open database:\n\n" + string(sqlite3_errmsg(db));

        wxMessageDialog *dial = new wxMessageDialog(NULL, errorMessage, wxT("Error"), wxOK | wxICON_ERROR);
        dial->ShowModal();

        // Terminate the program along with database
        sqlite3_close(db);

        this->Close(true);
        exit(EXIT_FAILURE);
    }

    // Query for that word or something similar to that word user asked for
    string sql = "SELECT * FROM dictionary WHERE word LIKE '%" + lookingFor + "%';";
    rc = sqlite3_prepare_v2(db, sql.data(), -1, &res, 0);

    // If database is empty
    if (rc != SQLITE_OK)
    {
        // Show error message
        string errorMessage = "ERROR: Failed to select data.\n\n" + string(err_msg);

        wxMessageDialog *dial = new wxMessageDialog(NULL, errorMessage, wxT("Error"), wxOK | wxICON_ERROR);
        dial->ShowModal();

        // Close the program along with database
        sqlite3_free(err_msg);
        sqlite3_close(db);


        this->Close(true);
        exit(EXIT_FAILURE);
    }

    // If the word definition found as a result of query
    if ((rc = sqlite3_step(res)) == SQLITE_ROW)
    {
        // Clear previous word data
        this->wordDefinition->Clear();
        this->WordDefinitionText = "";

        // Select the first row of that result
        this->_word = reinterpret_cast<const char*> (sqlite3_column_text(res, 0));
        this->_definition = reinterpret_cast<const char*> (sqlite3_column_text(res, 1));

        // Display on rich text ctrl
        // Word Title
        this->wordDefinition->BeginSuppressUndo();
        this->wordDefinition->BeginFont(*this->titleFont);
        this->wordDefinition->BeginTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        this->wordDefinition->WriteText(this->_word);
        this->WordDefinitionText += this->_word + "\n\n";
        this->wordDefinition->EndTextColour();
        this->wordDefinition->EndFont();
        this->wordDefinition->EndSuppressUndo();

        this->wordDefinition->BeginSuppressUndo();
        this->wordDefinition->BeginFont(*this->bodyFont);
        this->wordDefinition->BeginTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));

        vector<string> *lines = this->_commonTools->GetSplittedStrings(this->_definition, '\n');
        bool isMnemonicSentenceTitleGiven = false;
        bool isExampleSetenceStarted = false;

        // For every line on word definition
        for (vector<string>::iterator line = lines->begin(); line != lines->end(); ++line)
        {
            // Correct quote (", ') related problem first
            for (int character = 0; character < line->length(); character++)
            {
                if ((*line)[character] == '”')
                {
                    (*line)[character] = '\'';
                }
            }

            // If the line is about Short Definition
            if (this->_commonTools->GetSubstringPosition(*line, "[sd] ") != -1)
            {
                (*line).replace(this->_commonTools->GetSubstringPosition(*line, "[sd] "), 5, "");
                isExampleSetenceStarted = false;

                //this->wordDefinition->Newline();
                this->wordDefinition->BeginBold();
                this->wordDefinition->WriteText("Short Definition:\n");
                this->WordDefinitionText += "Short Definition:\n";
                this->wordDefinition->EndBold();
                this->wordDefinition->BeginItalic();
                this->wordDefinition->WriteText(*line);
                this->WordDefinitionText += *line + "\n";
                this->wordDefinition->EndItalic();

                this->wordDefinition->Newline();
            }
            // Or, if the line is about Word Definition
            else if (this->_commonTools->GetSubstringPosition(*line, "[wd] ") != -1)
            {
                (*line).replace(this->_commonTools->GetSubstringPosition(*line, "[wd] "), 5, "");
                isExampleSetenceStarted = false;

                this->wordDefinition->Newline();

                this->wordDefinition->BeginBold();
                this->wordDefinition->WriteText("Definition:\n");
                this->WordDefinitionText += "Definition:\n";
                this->wordDefinition->EndBold();
                this->wordDefinition->WriteText(*line);
                this->WordDefinitionText += *line + "\n";

                this->wordDefinition->Newline();
            }
            // Or, if the line is about Synonym
            else if (this->_commonTools->GetSubstringPosition(*line, "[ws] ") != -1)
            {
                (*line).replace(this->_commonTools->GetSubstringPosition(*line, "[ws] "), 5, "");

                int totalSynonyms = 0;

                for (int character = 0; character < (*line).length(); character++)
                {
                    if (((*line)[character] == ',') || ((*line)[character] == ';'))
                    {
                        totalSynonyms++;
                    }
                }

                --totalSynonyms;

                for (int i = 1; i <= totalSynonyms; i++)
                {
                    (*line).replace(this->_commonTools->GetSubstringPosition(*line, "[ws] "), 5, "");
                }

                (*line) = this->_commonTools->Trim(*line);
                (*line)[line->length() - 1] = ' ';

                isExampleSetenceStarted = false;

                this->wordDefinition->Newline();

                this->wordDefinition->BeginBold();
                this->wordDefinition->WriteText("Synonyms:\n");
                this->WordDefinitionText += "Synonyms:\n";
                this->wordDefinition->EndBold();
                this->wordDefinition->WriteText(*line);
                this->WordDefinitionText += *line + "\n";

                this->wordDefinition->Newline();
            }
            // Or, if the line is about Example Sentence
            else if (this->_commonTools->GetSubstringPosition(*line, "[eg] ") != -1)
            {
                if (!isExampleSetenceStarted)
                {
                    isExampleSetenceStarted = true;
                    (*line).replace(this->_commonTools->GetSubstringPosition(*line, "[eg] "), 5, "");

                    this->wordDefinition->Newline();
                    this->wordDefinition->BeginBold();
                    this->wordDefinition->WriteText("Example Sentence:\n");
                    this->WordDefinitionText += "Example Sentence:\n";
                    this->wordDefinition->EndBold();
                }
                else
                {
                    (*line).replace(this->_commonTools->GetSubstringPosition(*line, "[eg] "), 5, "");
                }

                this->wordDefinition->WriteText("•  " + *line);
                this->wordDefinition->Newline();

                this->WordDefinitionText += "•  " + *line + "\n";
            }
            // Or, if the line is about Mnemonic Sentence
            else if (this->_commonTools->GetSubstringPosition(*line, "[ms] ") != -1)
            {
                if (!isMnemonicSentenceTitleGiven)
                {
                    isMnemonicSentenceTitleGiven = true;
                    (*line).replace(this->_commonTools->GetSubstringPosition(*line, "[ms] "), 5, "");

                    this->wordDefinition->Newline();

                    this->wordDefinition->BeginBold();
                    this->wordDefinition->WriteText("Mnemonics (Memory Aids) for " + this->_word + ":\n");
                    this->WordDefinitionText += "Mnemonics (Memory Aids) for " + this->_word + ":\n";
                    this->wordDefinition->EndBold();
                    this->wordDefinition->WriteText("•  " + *line);
                    this->WordDefinitionText += *line + "\n";

                    this->wordDefinition->Newline();
                }
                else
                {
                    (*line).replace(this->_commonTools->GetSubstringPosition(*line, "[ms] "), 5, "");

                    if (this->_commonTools->Trim(*line) != "")
                    {
                        (*line) = "•  " + (*line);

                        this->wordDefinition->Newline();
                        this->wordDefinition->WriteText(*line);
                        this->wordDefinition->Newline();

                        this->WordDefinitionText += *line + "\n";
                    }
                }

                isExampleSetenceStarted = false;
            }
            else
            {
                this->wordDefinition->WriteText(*line);
                this->wordDefinition->Newline();
            }
        }

        this->wordDefinition->EndTextColour();
        this->wordDefinition->EndFont();
        this->wordDefinition->EndSuppressUndo();
    }

    // Close the database
    sqlite3_finalize(res);
    sqlite3_close(db);
}

void DictionaryFrame::_setSelectionOf(string word)
{
    // Highlight on wordlist box
    int index = 0;

    for (vector<string>::iterator currentWord = this->_listOfWords->begin(); currentWord != this->_listOfWords->end(); ++currentWord)
    {
        // If current word is same on the wordlist box, then select that and stop searching further
        if (word == *currentWord)
        {
            this->wordList->SetSelection(index);
            break;
        }

        index++;
    }
}

void DictionaryFrame::_addWordOnHistory(string selectedWord)
{
    // Check if the current word is on the search box history
    // If not, then add that word on the search box history
    if (this->searchBox->FindString(selectedWord) == -1)
    {
        searchBox->Append(selectedWord);
    }
}

void DictionaryFrame::_addToUserHistory()
{
    // Add new word on user history
    this->_userHistory->push_back(this->_word);

    // Update the user history index
    this->_currentIndex = this->_userHistory->size() - 1;

    // Enable the back/previous button
    if (this->_currentIndex > 0)
    {
        this->MainToolbar->EnableTool(wxID_BACKWARD, true);
    }
}
