#include "StMu2AnaMaker.h"

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuBTofPidTraits.h"
#include "StMuDSTMaker/COMMON/StMuBTofHit.h"
#include "StMuDSTMaker/COMMON/StMuRpsCollection.h"
#include "StSpinPool/StSpinDbMaker/StSpinDbMaker.h"
#include "St_db_Maker/St_db_Maker.h"
#include "StTriggerData.h"

#include "StThreeVectorF.hh"
#include "StEvent/StDcaGeometry.h"
#include "StEvent/StRpsCollection.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TGeoManager.h"
#include <vector>
#include "TSystem.h"
#include "TRandom3.h"
#include <stdio.h>
#include "TVector3.h"

#include <bitset>
#include "StPhysicalHelixD.hh"
#include "StarClassLibrary/SystemOfUnits.h"
#include "StarClassLibrary/PhysicalConstants.h"
#include "StBTofUtil/tofPathLength.hh"
#include "StBichsel/Bichsel.h"
#include "StMessMgr.h"

#define PI 3.1415926

ClassImp(StMu2AnaMaker)
//-----------------------------------------------------------------------------
StMu2AnaMaker::StMu2AnaMaker(const char* name, StMuDstMaker *muMaker, const char* outName,
                             int number_of_triggers, StSpinDbMaker* spindb) : StMaker(name), mSpinDbMaker(spindb) 
{
	mMuDstMaker         = muMaker;
	mSpinDbMaker        = spindb;
	mMuDst              = 0;
	mMuEvent            = 0;
	mOutName            = outName;
	mOutName_hist       = outName;
	mNumber_of_triggers = number_of_triggers;

	mMaxNRunIds    = 3000;
	mRunList       = "./list/runnumber_list.list";
	runidx         = -1;

}
//----------------------------------------------------------------------------- 
StMu2AnaMaker::~StMu2AnaMaker()
{ 
	/*  */ 
}

//----------------------------------------------------------------------------- 
Int_t StMu2AnaMaker::Init() 
{
	LoadRunList();

	if(mOutName_hist!="") 
	{
		mOutFile_hist = new TFile(mOutName_hist.Data(), "RECREATE");
	}
	else
	{
		mOutFile_hist = new TFile("test.root", "RECREATE");
	}

	mOutFile_hist->cd();
	DeclareHistograms();
	BookTree();

	return kStOK;
}
//-----------------------------------------------------------------------------
/**
 * @brief Initialize the run number.
 * 
 * @param runNumber Run number of the event.
 * @return Int_t 
 */
Int_t StMu2AnaMaker::InitRun( const Int_t runNumber )
{
	return kStOK;
}
//-----------------------------------------------------------------------------
/**
 * @brief Load the run number list from a file.
 * 
 * @return Int_t 
 */
Int_t StMu2AnaMaker::LoadRunList( )
{
	ifstream indata;
	indata.open( mRunList.Data() );
	mTotalRunId.clear();

	if( indata.is_open() )
	{
		cout << "read in total run number list and map it. runlist = "<<mRunList.Data()<<endl;
		Int_t oldId;
		Int_t newId=0;
		while(indata>>oldId)
		{
			mTotalRunId[oldId] = newId;
			newId++;
		}
		cout<<"[ok]"<<endl;
	}
	else
	{
		cout << "failed to load the runnumber list " << mRunList.Data() <<"!!!"<<endl;
		return kStErr;
	}

	indata.close();

	cout<<"Run Number list: "<<mRunList<<" was loaded"<<endl;
	cout<<"--------------------------------------------"<<endl;
	return kStOK;
}
//-----------------------------------------------------------------------------
/**
 * @brief Finish the analysis.
 * 
 * @return Int_t 
 */
Int_t StMu2AnaMaker::Finish() 
{
	mOutFile_hist->Write();
	mOutFile_hist->Close();
	return kStOK;
}
//-----------------------------------------------------------------------------
/**
 * @brief Declare any histograms that might be used in the analysis.
 * 
 */
void StMu2AnaMaker::DeclareHistograms() 
{
	TH1::SetDefaultSumw2();
	h_EvtCounter     = new TH1F("h_EvtCounter", " ", 6., -0.5, 5.5);
}

//-----------------------------------------------------------------------------
/**
 * @brief Define the analysis tree and all it's variables.
 * When adding new varibales remember to edit the .h file.
 * 
 */
void StMu2AnaMaker::BookTree()
{

	mOutFile_hist->cd();
	anaTree = new TTree("mEvent", "Short Dataset description");
	anaTree->SetAutoSave(100000);
	anaTree->Branch("mrunId",              &mrunId,              "mrunId/I");
	anaTree->Branch("mfillNumber",         &mfillNumber,         "mfillNumber/F");
	anaTree->Branch("mtrigWord",           &mtrigWord,           "mtrigWord/I");
	anaTree->Branch("mfield",              &mfield,              "mfield/F");
	anaTree->Branch("mESpin",              &mESpin,              "mESpin/I");
	anaTree->Branch("mvx",                 &mvx,                 "mvx/F");
	anaTree->Branch("mvy",                 &mvy,                 "mvy/F");
	anaTree->Branch("mvz",                 &mvz,                 "mvz/F");
	anaTree->Branch("mvpdVz",              &mvpdVz,              "mvpdVz/F");
	anaTree->Branch("mnETracks",           &mnETracks,           "mnETracks/I");

	anaTree->Branch("mtrk_px",             &mtrk_px,             "mtrk_px[mnETracks]/F");
	anaTree->Branch("mtrk_py",             &mtrk_py,             "mtrk_py[mnETracks]/F");
	anaTree->Branch("mtrk_pz",             &mtrk_pz,             "mtrk_pz[mnETracks]/F");
	anaTree->Branch("mtrk_Pt",             &mtrk_Pt,             "mtrk_Pt[mnETracks]/F");
	anaTree->Branch("mtrk_P",              &mtrk_P,              "mtrk_P[mnETracks]/F");
	anaTree->Branch("mtrk_Eta",            &mtrk_Eta,            "mtrk_Eta[mnETracks]/F");
	anaTree->Branch("mtrk_Phi",            &mtrk_Phi,            "mtrk_Phi[mnETracks]/F");
	anaTree->Branch("mtrk_Charge",         &mtrk_Charge,         "mtrk_Charge[mnETracks]/I");

	anaTree->Branch("mtrk_nSigmaPion",     &mtrk_nSigmaPion,     "mtrk_nSigmaPion[mnETracks]/F");
	anaTree->Branch("mtrk_nSigmaKaon",     &mtrk_nSigmaKaon,     "mtrk_nSigmaKaon[mnETracks]/F");
	anaTree->Branch("mtrk_nSigmaProton",   &mtrk_nSigmaProton,   "mtrk_nSigmaProton[mnETracks]/F");
	anaTree->Branch("mtrk_nSigmaElectron", &mtrk_nSigmaElectron, "mtrk_nSigmaElectron[mnETracks]/F");
}
//-----------------------------------------------------------------------------
/**
 * @brief Clear the variables of the analysis tree.
 * 
 * @param opt 
 */
void StMu2AnaMaker::Clear(Option_t *opt) 
{

}
//-----------------------------------------------------------------------------
/**
 * @brief Make the analysis.
 * 
 * @return Int_t 
 */
Int_t StMu2AnaMaker::Make() 
{
	if(!mMuDstMaker) 
	{
		LOG_WARN << " No MuDstMaker! Skip! " << endm;
		return kStWarn;
	}

	mMuDst = mMuDstMaker->muDst();

	if(!mMuDst) 
	{
		LOG_WARN << " No MuDst! Skip! " << endm;
		return kStWarn;
	}

	//Load event
	//
	mMuEvent = (StMuEvent*)mMuDst->event();

	if(!mMuEvent)
	{
		cerr<<"Error opening MuDst Event, skip!"<<endl;
		return kStWarn;
	}

	bool passEvent = true;
	if (checkTriggers(0)) {
		cout << "Event filled"
		passEvent = fillEvent(mMuEvent);
		if (passEvent) fillTrack(); anaTree->Fill();
	}

	return kStOK;
}
//---------------------------------------------------------------------
/**
 * @brief Check if the trigger is activated.
 * 
 * @param trgTypeIndex 
 * @return Bool_t Return true if the trigger is activated. False otherwise.
 */
Bool_t StMu2AnaMaker::checkTriggers(int trgTypeIndex)
{
	for(auto itrg = triggers[trgTypeIndex].begin(); itrg < triggers[trgTypeIndex].end(); ++itrg)
	{
		if(mMuDst->event()->triggerIdCollection().nominal().isTrigger(*itrg))
		{
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------
/**
 * @brief Get the run index from the run number.
 * 
 * @param runId 
 * @return Int_t Run index
 */
Int_t StMu2AnaMaker::getRunIndex(Int_t runId)
{
	map<Int_t, Int_t>::iterator iter = mTotalRunId.find(runId);

	int runIndex = -1;

	if(iter != mTotalRunId.end()) 
	{
		runIndex = iter->second;
	}else
	{
		runIndex = -1;
		cout << "this run number is: " << runId << endl;
		cout << "Not found in the runnumber list!!!" << endl;
	}

	return runIndex;
}
//---------------------------------------------------------------------
/**
 * @brief Fill the tree with the event information.
 * 
 * @param mMuEvent 
 * @return Bool_t Return true if the event is good. false otherwise.
 */
Bool_t StMu2AnaMaker::fillEvent(StMuEvent* mMuEvent)
{
	Int_t runid = (int) mMuEvent->runId();
	runidx      = getRunIndex( runid );     

	Float_t fillNumber = mMuEvent->runInfo().beamFillNumber(blue);
	h_EvtCounter->Fill(1);
	for (int i=1; i<mNumber_of_triggers+1; i++) {
			// Fill event histogram depending on which trigger is activated.
			// And add it to the position i+1
			//
			if (checkTriggers(i)) {
				h_EvtCounter->Fill(i+1);
			}
	}

	Float_t vztpc  = mMuDst->event()->primaryVertexPosition().z();
	Float_t vxtpc  = mMuDst->event()->primaryVertexPosition().x();
	Float_t vytpc  = mMuDst->event()->primaryVertexPosition().y();
	Float_t vzvpd  = mMuDst->event()->vpdVz();

	Float_t bfield  = mMuDst->event()->magneticField();

	Int_t bx7 = mMuDst->event()->triggerData()->bunchId7Bit();

	int spin4 = mSpinDbMaker->spin4usingBX7(bx7);

	mESpin       = spin4;
	mfillNumber  = fillNumber;
	mtrigWord    = getTrigWord();
	mfield       = bfield;
	mrunId       = runid;
	mvx          = vxtpc;
	mvy          = vytpc;
	mvz          = vztpc;
	mvpdVz       = vzvpd;

	return true;
}
//---------------------------------------------------------------------
/**
 * @brief Fill the tree with the track information.
 * 
 * @return Int_t 
 */
Int_t StMu2AnaMaker::fillTrack( )
{

	int nTrks = mMuDst->numberOfPrimaryTracks();
	if(nTrks<=0) return 0;

	Int_t nECounts = 0;	
	
	for( int iTrk=0; iTrk<nTrks; iTrk++ )
	{
		StMuTrack* track        = (StMuTrack*) mMuDst->primaryTracks(iTrk);
	
		if (!isGoodTrack(track)) continue;

		// Dynamical variables of the track
		//
		mtrk_px[nECounts]           = track->momentum().x();
		mtrk_py[nECounts]           = track->momentum().y();
		mtrk_pz[nECounts]           = track->momentum().z();
		mtrk_Pt[nECounts]           = track->momentum().perp();
		mtrk_P[nECounts]            = track->momentum().mag();
		mtrk_Phi[nECounts]          = track->momentum().phi();		
		mtrk_Eta[nECounts]          = track->momentum().pseudoRapidity();

		// DEdx Pid.
		//
		mtrk_nSigmaPion[nECounts]     = track->nSigmaPion();
		mtrk_nSigmaKaon[nECounts]     = track->nSigmaKaon();
		mtrk_nSigmaProton[nECounts]   = track->nSigmaProton();
		mtrk_nSigmaElectron[nECounts] = track->nSigmaElectron();

		// Track charge
		//
		mtrk_Charge[nECounts]       = track->charge();

		nECounts++; //Counter for number of events.

	}//loop tracks

	mnETracks = nECounts;

	if(nECounts == 0) return 0;

	return 1;
}
//----------------------------------------------------------------------------
/**
 * @brief Checks for track level conditions to discard bad quality tracks.
 * Modify this based on your analyzis.
 * 
 * @param mutrk 
 * @return Bool_t False if it not a good track to analyze. True otherwise.
 */
Bool_t StMu2AnaMaker::isGoodTrack(StMuTrack* mutrk)
{
	Int_t    nHitsFit   = mutrk->nHitsFit();
	Int_t    nHitsDedx  = mutrk->nHitsDedx();
	Float_t  rationhits = (float)(1.0 * mutrk->nHitsFit() / mutrk->nHitsPoss());
	Float_t  pEta       = mutrk->momentum().pseudoRapidity();
	Int_t    charge     = mutrk->charge();

	if(fabs(charge)!=1)             return false;
	if(fabs(pEta)>1.0)              return false;
	if(nHitsFit<15)                 return false;
	if(nHitsDedx<10)                return false;
	if(rationhits<0.32 || rationhits>1.05) return false;

	return true;
}
//----------------------------------------------------------------------------
/**
 * @brief This function goal is to provide information of which triggers were used.
 * 
 * @return Binary code of which triggers were active. 
 */
Int_t StMu2AnaMaker::getTrigWord( ){

	Int_t trgword = (checkTriggers(1)<<0);
	for (int i = 2; i < mNumber_of_triggers + 1; i++) {
		 trgword += (checkTriggers(i)<<(i-1));
	}
	return trgword;
}
