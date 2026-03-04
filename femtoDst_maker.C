#include <TSystem.h>

#include "TFile.h"
#include "TTree.h"
#include <stdexcept>
#include <TChain.h>

#include <fstream>
#include <iostream>
#include <map>
#include <TString.h>

using namespace std;

class StMaker;
class StMuDstMaker;
class StMuDst;
class StMuEvent;
class StMuTrack;

void femtoDst_maker(const Char_t *inputFile="list/test_list.list", const Char_t *outputFile="test_tree.root")
{
	//Load all the System libraries
	gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
	loadSharedLibraries();

	gROOT->Macro("load.C");
    gSystem->Load("StMu2AnaMaker.so");

    // STAR message manager: hide INFO (and optionally DEBUG)
    // STAR: disable ALL log messages
    gMessMgr->SetLimit("I", 0);
    gMessMgr->SetLimit("W", 0);
    gMessMgr->SetLimit("E", 0);
    gMessMgr->SetLimit("F", 0);
    gMessMgr->SetLimit("D", 0);
    gMessMgr->SetLevel(kError);
    cout << "Message Manager limits set." << endl;

    // This are for UPC, you can delete them if you are using UPC.
    gSystem->Load("StBTofUtil");
    gSystem->Load("StVpdCalibMaker");
    gSystem->Load("StBTofCalibMaker");

    StChain       *chain      = new StChain;
    StMuDstMaker  *muDstMaker = new StMuDstMaker(0, 0, "", inputFile, "", 1000);

    // Here we check in the data base of the STAR experiment for the spin information of each bunch.
    //
    St_db_Maker   *dbMk       = new St_db_Maker("db", "MySQL:StarDb", "$STAR/StarDb");
    StSpinDbMaker *spindb     = new StSpinDbMaker("spinDb");

    // This section runs the afterburner in start-less mode,
    // which is required for UPC analysis.
    //-----------------------------------------------------------------------------
    StVpdCalibMaker *vpdCalib = new StVpdCalibMaker();
    vpdCalib->setMuDstIn();
    vpdCalib->setUseVpdStart(kFALSE);  // Use "start-less" mode for UPC

    StBTofCalibMaker *btofCalib = new StBTofCalibMaker();
    btofCalib->setMuDstIn();
    btofCalib->forceTStartZero();     // Set TStart to zero
    //-----------------------------------------------------------------------------

    chain->AddMaker(vpdCalib);
    chain->AddMaker(btofCalib);

    number_of_triggers = 2;
    StMu2AnaMaker *anaMaker   = new StMu2AnaMaker("ana", muDstMaker, outputFile, number_of_triggers, spindb);
    
    // Add triggers.
    // Here we build an internal array that keeps track of the triggers used in the analysis.
    //
    // Position 0 in the array corresponds to "any trigger", meaning it will be filled if
    // any of the listed trigger IDs fires.
    //
    // The other positions correspond to specific trigger IDs. This allows us to later fill
    // histograms where bin 0 represents events passing any trigger, while the remaining
    // bins correspond to individual triggers.
    //
    // In this way we can easily compare the inclusive trigger selection with the
    // contributions from each specific trigger.

    anaMaker->addTrigger(500000, 0); // RP_UPC
    anaMaker->addTrigger(500020, 0); // RP_UPC
    anaMaker->addTrigger(500000, 1); // Trigger-specific bin
    anaMaker->addTrigger(500020, 2); // Trigger-specific bin

    // Initialize the chain
    chain->Init();
    cout << "chain->Init();" << endl;

    int nEvents = muDstMaker->chain()->GetEntries();
    cout << "Total entries = " << nEvents << endl;

    int total = 0;
    for (int i = 0; i < nEvents; i++) {
        if (i % 1000 == 0) {
            cout << "Working on eventNumber " << i << " of " << nEvents << " (" 
                 << fixed << setprecision(1) << "finished " 
                 << ((float)i / (float)nEvents) * 100.0 << "%)" << endl;
        }

        chain->Clear();
        int iret = chain->Make(i);

        if (iret) {
            cout << "Bad return code! " << iret << endl;
            break;
        }

        total++;
    }

    cout << "******************************************" << endl;
    cout << "Work done... Finishing chain." << endl;
    cout << "******************************************" << endl;
    chain->Finish();
    cout << "******************************************" << endl;
    cout << "Total number of events: " << total << endl;
    cout << "******************************************" << endl;

    delete chain;
}