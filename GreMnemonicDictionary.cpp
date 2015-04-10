#include "GreMnemonicDictionary.h"
#include "DictionaryFrame.h"

IMPLEMENT_APP(GreMnemonicDictionary)

GreMnemonicDictionary::GreMnemonicDictionary()
{
    wxLog::SetLogLevel(0);
}

GreMnemonicDictionary::~GreMnemonicDictionary()
{
}

bool GreMnemonicDictionary::OnInit()
{
	DictionaryFrame *dictionaryFrame = new DictionaryFrame(wxT("GRE Mnemonic Dictionary"));
    dictionaryFrame->Show(true);

    return true;
}

