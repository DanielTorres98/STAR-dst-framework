/**
 Load STAR shared libraries

 If personal == true load project-specific libraries
 */
void load(Bool_t personal=true) {
  // STAR libraries for chain, MuDST, logger etc
  gROOT->Macro("loadMuDst.C");
  gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
  loadSharedLibraries();
  gSystem->Load("StMuDstMaker");
  gSystem->Load("StMuEvent");   
  gSystem->Load("StTpcDb");
  gSystem->Load("StDetectorDbMaker");
  gSystem->Load("StDbUtilities");
  gSystem->Load("StDaqLib");
  gSystem->Load("StEmcRawMaker");
  gSystem->Load("StEmcADCtoEMaker");
  gSystem->Load("StPreEclMaker");
  gSystem->Load("StEpcMaker");
  gSystem->Load("StDbBroker");
  gSystem->Load("St_db_Maker");
  gSystem->Load("StEEmcUtil");
  gSystem->Load("StEEmcDbMaker");
  gSystem->Load("StSpinDbMaker");
  gSystem->Load("StEmcTriggerMaker");
  gSystem->Load("StTriggerUtilities");
  gSystem->Load("libfastjet.so");
  gSystem->Load("StEventMaker");
  if (personal) {  // Load our shared libraries here
    gSystem->Load("libMinuit.so");
  }
}
